/*
 * Mycelia immersive 3d network visualization tool.
 * Copyright (C) 2008-2010 Sean Whalen.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <dataitem.hpp>
#include <graph.hpp>
#include <mycelia.hpp>
#include <vruihelp.hpp>
#include <generators/barabasigenerator.hpp>
#include <generators/erdosgenerator.hpp>
#include <generators/graphgenerator.hpp>
#include <generators/wattsgenerator.hpp>
#include <layout/arflayout.hpp>
#include <layout/arfwindow.hpp>
#include <layout/edgebundler.hpp>
#include <layout/frlayout.hpp>
#include <layout/graphlayout.hpp>
#include <parsers/chacoparser.hpp>
#include <parsers/dotparser.hpp>
#include <parsers/gmlparser.hpp>
#include <parsers/xmlparser.hpp>
#include <tools/graphbuilder.hpp>
#include <tools/nodeselector.hpp>
#include <windows/attributewindow.hpp>
#include <windows/imagewindow.hpp>
#ifdef __RPCSERVER__
#include "rpcserver.hpp"
#endif

using namespace std;

/*
 * link to gpu layout function if cuda is enabled
 */
#ifdef __CUDA__
#include <vector_types.h>
extern "C" { void gpuLayout(float4*, int*, int); }
#endif

Mycelia::Mycelia(int argc, char** argv, char** appDefaults)
    : Vrui::Application(argc, argv, appDefaults)
{
    // node layout / edge bundler
    dynamicLayout = new ArfLayout(this);
    staticLayout = new FruchtermanReingoldLayout(this);
    edgeBundler = new EdgeBundler(this);
    skipLayout = false;
    
    // node selection tool factory
    NodeSelectorFactory* selectorFactory = new NodeSelectorFactory(*Vrui::getToolManager(), this);
    Vrui::getToolManager()->addClass(selectorFactory, 0);
    
    // graph builder tool factory
    GraphBuilderFactory* builderFactory = new GraphBuilderFactory(*Vrui::getToolManager(), this);
    Vrui::getToolManager()->addClass(builderFactory, 0);
    
    // file submenu
    GLMotif::Popup* filePopup = new GLMotif::Popup("FilePopup", Vrui::getWidgetManager());
    GLMotif::SubMenu* fileSubMenu = new GLMotif::SubMenu("FileSubMenu", filePopup, false);
    
    GLMotif::Button* openFileButton = new GLMotif::Button("OpenFileButton", fileSubMenu, "Open...");
    openFileButton->getSelectCallbacks().add(this, &Mycelia::openFileCallback);
    
    GLMotif::Button* writeGraphButton = new GLMotif::Button("WriteGraphButton", fileSubMenu, "Save");
    writeGraphButton->getSelectCallbacks().add(this, &Mycelia::writeGraphCallback);
    
    // graph generators submenu
    GLMotif::Popup* generatorPopup = new GLMotif::Popup("GeneratorMenu", Vrui::getWidgetManager());
    generatorRadioBox = new GLMotif::RadioBox("GeneratorRadioBox", generatorPopup, false);
    generatorRadioBox->setSelectionMode(GLMotif::RadioBox::ATMOST_ONE);
    generatorRadioBox->getValueChangedCallbacks().add(this, &Mycelia::generatorCallback);
    
    erdosButton = new GLMotif::ToggleButton("ErdosButton", generatorRadioBox, "Random (Erdos-Renyi)");
    barabasiButton = new GLMotif::ToggleButton("BarabasiButton", generatorRadioBox, "Scale Free (Barabasi-Albert)");
    wattsButton = new GLMotif::ToggleButton("WattsButton", generatorRadioBox, "Small World (Watts-Strogatz)");
    
    // layout submenu
    GLMotif::Popup* layoutPopup = new GLMotif::Popup("LayoutPopup", Vrui::getWidgetManager());
    layoutRadioBox = new GLMotif::RadioBox("LayoutSubMenu", layoutPopup, false);
    layoutRadioBox->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);
    layoutRadioBox->getValueChangedCallbacks().add(this, &Mycelia::resetLayoutCallback);
    
    staticButton = new GLMotif::ToggleButton("StaticButton", layoutRadioBox, "Static");
    dynamicButton = new GLMotif::ToggleButton("DynamicButton", layoutRadioBox, "Dynamic");
    layout = staticLayout;
    
    // render submenu
    GLMotif::Popup* renderPopup = new GLMotif::Popup("RenderPopup", Vrui::getWidgetManager());
    GLMotif::SubMenu* renderSubMenu = new GLMotif::SubMenu("RenderSubMenu", renderPopup, false);
    
    bundleButton = new GLMotif::ToggleButton("BundleButton", renderSubMenu, "Bundle Edges");
    bundleButton->getValueChangedCallbacks().add(this, &Mycelia::bundleCallback);
    
    nodeInfoButton = new GLMotif::ToggleButton("NodeInfoButton", renderSubMenu, "Show Node Information");
    nodeInfoButton->getValueChangedCallbacks().add(this, &Mycelia::nodeInfoCallback);
    
    nodeLabelButton = new GLMotif::ToggleButton("NodeLabelButton", renderSubMenu, "Show Node Labels");
    nodeLabelButton->setToggle(true);
    nodeLabelButton->getValueChangedCallbacks().add(this, &Mycelia::nodeLabelCallback);
    
    edgeLabelButton = new GLMotif::ToggleButton("EdgeLabelButton", renderSubMenu, "Show Edge Labels");
    edgeLabelButton->setToggle(true);
    edgeLabelButton->getValueChangedCallbacks().add(this, &Mycelia::nodeLabelCallback); // same callback
    
    componentButton = new GLMotif::ToggleButton("ComponentButton", renderSubMenu, "Show Only Selected Subgraph");
    componentButton->getValueChangedCallbacks().add(this, &Mycelia::componentCallback);
    
    // algorithms submenu
    GLMotif::Popup* algorithmsPopup = new GLMotif::Popup("AlgorithmsPopup", Vrui::getWidgetManager());
    GLMotif::SubMenu* algorithmsSubMenu = new GLMotif::SubMenu("AlgorithmsSubMenu", algorithmsPopup, false);
    
    shortestPathButton = new GLMotif::ToggleButton("ShortestPathButton", algorithmsSubMenu, "Shortest Path");
    shortestPathButton->getValueChangedCallbacks().add(this, &Mycelia::shortestPathCallback);
    
    spanningTreeButton = new GLMotif::ToggleButton("SpanningTreeButton", algorithmsSubMenu, "Spanning Tree");
    spanningTreeButton->getValueChangedCallbacks().add(this, &Mycelia::spanningTreeCallback);
    
    // plots submenu
    GLMotif::Popup* pythonPopup = new GLMotif::Popup("PythonPopup", Vrui::getWidgetManager());
    GLMotif::RadioBox* pythonSubMenu = new GLMotif::RadioBox("PythonSubMenu", pythonPopup, false);
    pythonSubMenu->setSelectionMode(GLMotif::RadioBox::ATMOST_ONE);
    pythonSubMenu->getValueChangedCallbacks().add(this, &Mycelia::pythonCallback);
    
    degreeButton = new GLMotif::ToggleButton("DegreeButton", pythonSubMenu, "Node Degree Distribution");
    centralityButton = new GLMotif::ToggleButton("CentralityButton", pythonSubMenu, "Node Betweenness Centrality");
    adjacencyButton = new GLMotif::ToggleButton("AdjacencyButton", pythonSubMenu, "Adjacency Matrix");
    lanetButton = new GLMotif::ToggleButton("LaNetButton", pythonSubMenu, "k-Core Hierarchical Layout");
    
    // main menu
    mainMenuPopup = new GLMotif::PopupMenu("MainMenuPopup", Vrui::getWidgetManager());
    mainMenuPopup->setTitle("Mycelia Network Visualizer");
    mainMenu = new GLMotif::Menu("MainMenu", mainMenuPopup, false);
    
    GLMotif::CascadeButton* fileCascade = new GLMotif::CascadeButton("FileCascade", mainMenu, "File");
    fileCascade->setPopup(filePopup);
    
    GLMotif::CascadeButton* generatorCascade = new GLMotif::CascadeButton("GeneratorCascade", mainMenu, "Generators");
    generatorCascade->setPopup(generatorPopup);
    
    GLMotif::CascadeButton* layoutCascade = new GLMotif::CascadeButton("LayoutCascade", mainMenu, "Layout");
    layoutCascade->setPopup(layoutPopup);
    
    GLMotif::CascadeButton* renderCascade = new GLMotif::CascadeButton("RenderCascade", mainMenu, "Rendering Options");
    renderCascade->setPopup(renderPopup);
    
    GLMotif::CascadeButton* algorithmsCascade = new GLMotif::CascadeButton("AlgorithmsCascade", mainMenu,  "Algorithms");
    algorithmsCascade->setPopup(algorithmsPopup);
    
    GLMotif::CascadeButton* pythonCascade = new GLMotif::CascadeButton("PythonCascade", mainMenu, "Python Plugins");
    pythonCascade->setPopup(pythonPopup);
    
    GLMotif::Button* clearButton = new GLMotif::Button("ClearButton", mainMenu, "Clear Screen");
    clearButton->getSelectCallbacks().add(this, &Mycelia::clearCallback);
    
    GLMotif::Button* navButton = new GLMotif::Button("NavButton", mainMenu, "Center Graph");
    navButton->getSelectCallbacks().add(this, &Mycelia::resetNavigationCallback);
    
    GLMotif::Button* layoutButton = new GLMotif::Button("LayoutButton", mainMenu, "Reset Layout");
    layoutButton->getSelectCallbacks().add(this, &Mycelia::resetLayoutCallback);
    
    fileSubMenu->manageChild();
    generatorRadioBox->manageChild();
    layoutRadioBox->manageChild();
    renderSubMenu->manageChild();
    algorithmsSubMenu->manageChild();
    pythonSubMenu->manageChild();
    mainMenu->manageChild();
    Vrui::setMainMenu(mainMenuPopup);
    
    // windows
    char cwd[1024];
    string dataDirectory = string(getcwd(cwd, 1024)) + "/data";
    
    fileWindow = new GLMotif::FileSelectionDialog(mainMenu->getManager(), "Open file...",
            dataDirectory.c_str(), ".xml;.dot;.chaco;.gml");
    fileWindow->getOKCallbacks().add(this, &Mycelia::fileOpenAction);
    fileWindow->getCancelCallbacks().add(this, &Mycelia::fileCancelAction);
    
    nodeWindow = new AttributeWindow(this, "Node Attributes", 5);
    nodeWindow->hide();
    
    layoutWindow = new ArfWindow(this);
    layoutWindow->hide();
    
    imageWindow = new ImageWindow(this);
    imageWindow->hide();
    
    statusWindow = new AttributeWindow(this, "Status", 1);
    statusWindow->hide();
    
    // fonts
    font = new FTGLTextureFont("fonts/Sansation_Light.ttf");
    font->FaceSize(FONT_SIZE);
    
    // generators
    barabasiGenerator = new BarabasiGenerator(this);
    erdosGenerator = new ErdosGenerator(this);
    wattsGenerator = new WattsGenerator(this);
    generator = barabasiGenerator;
    
    // logo
    lastFrameTime = Vrui::getApplicationTime();
    rotationAngle = 0;
    rotationSpeed = 40.0;
    
    // parsers
    chacoParser = new ChacoParser(this);
    dotParser = new DotParser(this);
    gmlParser = new GmlParser(this);
    xmlParser = new XmlParser(this);
    
    // misc
    selectedNode = SELECTION_NONE;
    previousNode = SELECTION_NONE;
    coneAngle = 0.005;
    xVector = Vrui::Vector(1, 0, 0);
    zVector = Vrui::Vector(0, 0, 1);
#ifdef __RPCSERVER__
    server = new RpcServer(this);
#endif
    
    // graph
    g = new Graph(this);
    gCopy = new Graph(this);
    
    // establishes initial node+edge sizes if graph builder is used first
    resetNavigationCallback(0);
}

Mycelia::~Mycelia()
{
    stopLayout();
}

void Mycelia::buildGraphList(MyceliaDataItem* dataItem) const
{
    // update version first in case of preemption
    dataItem->graphListVersion = gCopy->getVersion();
    
    glNewList(dataItem->nodeList, GL_COMPILE);
    gluSphere(dataItem->quadric, nodeRadius, 20, 20);
    glEndList();
    
    glNewList(dataItem->arrowList, GL_COMPILE);
    gluCylinder(dataItem->quadric, arrowWidth, 0.0, arrowHeight, 10, 1);
    
    gluQuadricOrientation(dataItem->quadric, GLU_INSIDE);
    gluDisk(dataItem->quadric, 0.0, arrowWidth, 10, 1);
    gluQuadricOrientation(dataItem->quadric, GLU_OUTSIDE);
    glEndList();
    
    glNewList(dataItem->graphList, GL_COMPILE);
    drawNodes(dataItem);
    drawEdges(dataItem);
    glEndList();
}

void Mycelia::drawEdge(int source, int target, const MyceliaDataItem* dataItem) const
{
    drawEdge(gCopy->getNodePosition(source), gCopy->getNodePosition(target), true, gCopy->isBidirectional(source, target), dataItem);
}

void Mycelia::drawEdge(const Vrui::Point& source, const Vrui::Point& target, bool drawArrow, bool isBidirectional, const MyceliaDataItem* dataItem) const
{
    // computer graphics 2nd ed, p.413
    const Vrui::Vector edgeVector = target - source;
    const Vrui::Vector normalVector = Geometry::cross(edgeVector, zVector);
    const Vrui::Scalar length = Geometry::mag(edgeVector);
    
    // calculate space for directional arrow(s)
    double sourceOffset = 0;
    double targetOffset = drawArrow ? length - edgeOffset : length;
    
    if(isBidirectional && drawArrow)
    {
        sourceOffset = edgeOffset;
        targetOffset = length - 2 * edgeOffset;
    }
    
    glPushMatrix();
    
    // translate to point 1 and rotate towards point 2
    glTranslatef(source[0], source[1], source[2]);
    glRotatef(-VruiHelp::degrees(VruiHelp::angle(edgeVector, zVector)), normalVector[0], normalVector[1], normalVector[2]);
    
    // draw edge, leaving room for arrow
    glTranslatef(0, 0, sourceOffset);
    gluCylinder(dataItem->quadric, edgeThickness, edgeThickness, targetOffset, 10, 1);
    
    if(drawArrow)
    {
        // move near point 2 and draw arrow
        glTranslatef(0, 0, targetOffset);
        glCallList(dataItem->arrowList);
    }
    
    glPopMatrix();
}

void Mycelia::drawEdges(const MyceliaDataItem* dataItem) const
{
    /*
    we don't draw an edge if one was already drawn between two nodes.
    this saves lots of time for very dense graphs.
    todo: the current approach to tracking drawn edges is a quick hack.
    */
    glMaterial(GLMaterialEnums::FRONT_AND_BACK, *gCopy->getMaterial(MATERIAL_EDGE_DEFAULT));
    bool drawn[1000][1000] = {{false}};
    
    foreach(int edge, gCopy->getEdges())
    {
        const Edge& e = gCopy->getEdge(edge);
        
        if(!isSelectedComponent(e.source) || drawn[e.source][e.target])
        {
            continue;
        }
        
        if(bundleButton->getToggle())
        {
            for(int segment = 0; segment <= edgeBundler->getSegmentCount(); segment++)
            {
                const Vrui::Point& p = *edgeBundler->getSegment(edge, segment);
                const Vrui::Point& q = *edgeBundler->getSegment(edge, segment + 1);
                drawEdge(p, q, false, false, dataItem);
            }
        }
        else
        {
            drawEdge(e.source, e.target, dataItem);
            drawn[e.source][e.target] = true;
        }
    }
}

void Mycelia::drawEdgeLabels(const MyceliaDataItem* dataItem) const
{
    if(!edgeLabelButton->getToggle()) return;
    
    Vrui::Rotation inverseRotation = Vrui::getInverseNavigationTransformation().getRotation();
    inverseRotation *= Vrui::Rotation(xVector, Vrui::Scalar(M_PI / 2));
    float scale = nodeRadius * FONT_MODIFIER;
    
    foreach(int edge, gCopy->getEdges())
    {
        if(!isSelectedComponent(gCopy->getEdge(edge).source))
        {
            continue;
        }
        
        const string& label = gCopy->getEdgeLabel(edge);
        
        if(label.size() > 0)
        {
            const Vrui::Point& p = VruiHelp::midpoint(gCopy->getSourceNodePosition(edge), gCopy->getTargetNodePosition(edge));
            
            glPushMatrix();
            glTranslatef(p[0] + nodeRadius, p[1] + nodeRadius, p[2] + nodeRadius);
            glRotate(inverseRotation);
            glScalef(scale, scale, scale);
            font->Render(label.c_str());
            glPopMatrix();
        }
    }
}

void Mycelia::drawLogo() const
{
    Vrui::setNavigationTransformation(Vrui::Point::origin, 30);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glTranslatef(-6.5, 0, 0);
    glRotate(rotationAngle, Vrui::Vector(1, 1, 1));
    
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1, 1, 1); glVertex3f(1.5, 1.5, 1.5);
    glColor3f(1, 0, 0); glVertex3f(-1.5, -1.5, 1.5);
    glColor3f(0, 1, 0); glVertex3f(-1.5, 1.5, -1.5);
    glColor3f(0, 0, 1); glVertex3f(1.5, -1.5, -1.5);
    glColor3f(1, 1, 1); glVertex3f(1.5, 1.5, 1.5);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glTranslatef(-4, 0, -1.5);
    glRotate(90.0, Vrui::Vector(1, 0, 0));
    glScalef(FONT_MODIFIER, FONT_MODIFIER, FONT_MODIFIER);
    font->Render("mycelia.");
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    Vrui::requestUpdate();
}

void Mycelia::drawNode(int node, const MyceliaDataItem* dataItem) const
{
    const Vrui::Point& p = gCopy->getNodePosition(node);
    const float size = gCopy->getNodeSize(node);
    
    if(node == selectedNode)
        glMaterial(GLMaterialEnums::FRONT_AND_BACK, *gCopy->getMaterial(MATERIAL_SELECTED));
    else if(node == previousNode)
        glMaterial(GLMaterialEnums::FRONT_AND_BACK, *gCopy->getMaterial(MATERIAL_SELECTED_PREVIOUS));
    else
        glMaterial(GLMaterialEnums::FRONT_AND_BACK, *gCopy->getNodeMaterial(node));
        
    glPushMatrix();
    glTranslatef(p[0], p[1], p[2]);
    glScalef(size, size, size);
    glCallList(dataItem->nodeList);
    glPopMatrix();
}

void Mycelia::drawNodes(const MyceliaDataItem* dataItem) const
{
    foreach(int node, gCopy->getNodes())
    {
        if(!isSelectedComponent(node))
        {
            continue;
        }
        
        drawNode(node, dataItem);
    }
}

void Mycelia::drawNodeLabels(const MyceliaDataItem* dataItem) const
{
    if(!nodeLabelButton->getToggle()) return;
    
    Vrui::Rotation inverseRotation = Vrui::getInverseNavigationTransformation().getRotation();
    inverseRotation *= Vrui::Rotation(xVector, Vrui::Scalar(M_PI / 2));
    float scale = nodeRadius * FONT_MODIFIER;
    
    foreach(int node, gCopy->getNodes())
    {
        if(!isSelectedComponent(node))
        {
            continue;
        }
        
        const Vrui::Point& p = gCopy->getNodePosition(node);
        const string& label = gCopy->getNodeLabel(node);
        
        if(label.size() > 0)
        {
            glPushMatrix();
            glTranslatef(p[0] + 1.1 * nodeRadius, p[1] + 1.1 * nodeRadius, p[2] + 1.1 * nodeRadius);
            glRotate(inverseRotation);
            glScalef(scale, scale, scale);
            
            // draw a shadow for readability
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(1, 0, -1);
            font->Render(label.c_str());
            glPopMatrix();
            
            glColor3f(1, 1, 1);
            font->Render(label.c_str());
            glPopMatrix();
        }
    }
}

void Mycelia::drawShortestPath(const MyceliaDataItem* dataItem) const
{
    glMaterial(GLMaterialEnums::FRONT_AND_BACK, *gCopy->getMaterial(MATERIAL_SELECTED));
    
    for(int i = selectedNode; i != previousNode; i = predecessorVector[i])
    {
        if(i == predecessorVector[i]) break;
        
        drawNode(i, dataItem);
        drawEdge(i, predecessorVector[i], dataItem);
    }
}

void Mycelia::drawSpanningTree(const MyceliaDataItem* dataItem) const
{
    glMaterial(GLMaterialEnums::FRONT_AND_BACK, *gCopy->getMaterial(MATERIAL_SELECTED));
    
    for(int i = 0; i < (int)predecessorVector.size(); i++)
    {
        drawNode(i, dataItem);
        drawEdge(i, predecessorVector[i], dataItem);
    }
}

void Mycelia::display(GLContextData& contextData) const
{
    if(gCopy->getNodeCount() == 0)
    {
        drawLogo();
        return;
    }
    
    MyceliaDataItem* dataItem = contextData.retrieveDataItem<MyceliaDataItem>(this);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    
    // re-create display list if it's been updated
    if(dataItem->graphListVersion != gCopy->getVersion())
    {
        buildGraphList(dataItem);
    }
    
    if(spanningTreeButton->getToggle())
    {
        drawSpanningTree(dataItem);
    }
    else
    {
        glCallList(dataItem->graphList);
        
        glDisable(GL_LIGHTING);
        drawNodeLabels(dataItem);
        drawEdgeLabels(dataItem);
        glEnable(GL_LIGHTING);
        
        if(shortestPathButton->getToggle())
        {
            drawShortestPath(dataItem);
        }
    }
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
}

void Mycelia::frame()
{
    double newFrameTime = Vrui::getApplicationTime();
    rotationAngle += (newFrameTime - lastFrameTime) * rotationSpeed;
    rotationAngle = Math::mod(rotationAngle, Vrui::Scalar(360));
    lastFrameTime = newFrameTime;
    
    g->lock();
    *gCopy = *g;
    g->unlock();
}

void Mycelia::initContext(GLContextData& contextData) const
{
    MyceliaDataItem* dataItem = new MyceliaDataItem();
    contextData.addDataItem(this, dataItem);
}

bool Mycelia::isSelectedComponent(int node) const
{
    if(componentButton->getToggle())
    {
        return gCopy->getNodeComponent(node) == gCopy->getNodeComponent(selectedNode);
    }
    
    return true;
}

void Mycelia::setStatus(const char* status) const
{
    statusWindow->update("", status);
    
    if(strcmp(status, "") == 0)
    {
        statusWindow->hide();
    }
    else
    {
        statusWindow->show(true);
    }
}

/*
 * layout
 */
void Mycelia::resumeLayout() const
{
    if(layout->isDynamic() && !skipLayout)
    {
        startLayout();
    }
}

void Mycelia::setLayoutType(int type)
{
    if(type == LAYOUT_DYNAMIC)
    {
        edgeBundler->stop();
        bundleButton->setToggle(false);
        
        layoutRadioBox->setSelectedToggle(1);
        layout = dynamicLayout;
        layoutWindow->show();
    }
    else if(type == LAYOUT_STATIC)
    {
        layoutRadioBox->setSelectedToggle(0);
        layout = staticLayout;
        layoutWindow->hide();
    }
}

void Mycelia::setSkipLayout(bool skipLayout)
{
    this->skipLayout = skipLayout;
}

void Mycelia::startLayout() const
{
    layout->layout();
}

void Mycelia::stopLayout() const
{
    edgeBundler->stop();
    staticLayout->stop();
    dynamicLayout->stop();
}

/*
 * callbacks
 */
void Mycelia::bundleCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
    if(g->getNodeCount() == 0) return;
    
    if(cbData->set)
    {
        stopLayout();
        edgeBundler->layout();
    }
    else
    {
        edgeBundler->stop();
        g->update();
        resumeLayout();
    }
}

void Mycelia::clearCallback(Misc::CallbackData* cbData)
{
    g->clear();
    
    // clear menu toggles
    bundleButton->setToggle(false);
    componentButton->setToggle(false);
    centralityButton->setToggle(false);
    degreeButton->setToggle(false);
    adjacencyButton->setToggle(false);
    lanetButton->setToggle(false);
    nodeInfoButton->setToggle(false);
    shortestPathButton->setToggle(false);
    spanningTreeButton->setToggle(false);
    barabasiButton->setToggle(false);
    erdosButton->setToggle(false);
    wattsButton->setToggle(false);
    
    // hide windows
    VruiHelp::hide(fileWindow);
    imageWindow->hide();
    nodeWindow->clear();
    nodeWindow->hide();
    statusWindow->clear();
    statusWindow->hide();
    generator->hide();
}

void Mycelia::componentCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
    if(cbData->set)
    {
        g->setComponents();
    }
    
    resetLayoutCallback(0);
}

void Mycelia::fileCancelAction(GLMotif::FileSelectionDialog::CancelCallbackData* cbData)
{
    VruiHelp::hide(fileWindow);
}

void Mycelia::fileOpenAction(GLMotif::FileSelectionDialog::OKCallbackData* cbData)
{
    clearCallback(0);
    
    // set to true if parser detects nodes with explicit positions
    skipLayout = false;
    
    // call appropriate parser
    string& filename = cbData->selectedFileName;
    
    if(VruiHelp::endsWith(filename, ".dot"))
    {
        dotParser->parse(filename);
    }
    else if(VruiHelp::endsWith(filename, ".xml"))
    {
        xmlParser->parse(filename);
    }
    else if(VruiHelp::endsWith(filename, ".chaco"))
    {
        chacoParser->parse(filename);
    }
    else if(VruiHelp::endsWith(filename, ".gml"))
    {
        gmlParser->parse(filename);
    }
    
    // reset navigation here in case skipLayout is true
    resetNavigationCallback(0);
    resetLayoutCallback(0);
}

void Mycelia::generatorCallback(GLMotif::RadioBox::ValueChangedCallbackData* cbData)
{
    g->clear();
    
    setLayoutType(LAYOUT_DYNAMIC);
    generator->hide();
    
    if(cbData->newSelectedToggle == barabasiButton)
    {
        generator = barabasiGenerator;
    }
    else if(cbData->newSelectedToggle == erdosButton)
    {
        generator = erdosGenerator;
    }
    else if(cbData->newSelectedToggle == wattsButton)
    {
        generator = wattsGenerator;
    }
    
    generator->generate();
    resumeLayout();
}

void Mycelia::nodeInfoCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
    if(cbData->set)
    {
        nodeWindow->show();
    }
    else
    {
        nodeWindow->hide();
    }
}

void Mycelia::nodeLabelCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
    g->update();
}

void Mycelia::openFileCallback(Misc::CallbackData* cbData)
{
    VruiHelp::show(fileWindow, mainMenu);
}

void Mycelia::pythonCallback(GLMotif::RadioBox::ValueChangedCallbackData* cbData)
{
    if(g->getNodeCount() == 0) return;
    
    imageWindow->hide();
    
    if(cbData->newSelectedToggle == centralityButton)
    {
        vector<double> bc = gCopy->getBetweennessCentrality();
        ofstream out("/tmp/input.txt");
        
        foreach(int node, gCopy->getNodes())
        {
            out << bc[node] << endl;
        }
        
        out.close();
        imageWindow->load("python/plugins/bc.py");
    }
    else if(cbData->newSelectedToggle == degreeButton)
    {
        ofstream out("/tmp/input.txt");
        
        foreach(int node, gCopy->getNodes())
        {
            out << gCopy->getNodeDegree(node) << endl;
        }
        
        out.close();
        imageWindow->load("python/plugins/degree.py");
    }
    else if(cbData->newSelectedToggle == adjacencyButton)
    {
        bool a[1000][1000] = {{false}}; // temporary
        
        foreach(int source, gCopy->getNodes())
        {
            foreach(int target, gCopy->getNodes())
            {
                if(gCopy->hasEdge(source, target))
                {
                    a[source][target] = true;
                }
            }
        }
        
        ofstream out("/tmp/input.txt");
        
        for(int source = 0; source < gCopy->getNodeCount(); source++)
        {
            for(int target = 0; target < gCopy->getNodeCount(); target++)
            {
                out << a[source][target] << " ";
            }
            
            out << endl;
        }
        
        out.close();
        imageWindow->load("python/plugins/adjmatrix.py");
    }
    else if(cbData->newSelectedToggle == lanetButton)
    {
        ofstream out("/tmp/input.txt");
        
        foreach(int source, gCopy->getNodes())
        {
            foreach(int target, gCopy->getNodes())
            {
                if(gCopy->hasEdge(source, target))
                {
                    out << source << " " << target << endl;
                }
            }
        }
        
        out.close();
        imageWindow->load("python/plugins/lanet.py");
    }
    
    if(cbData->newSelectedToggle) imageWindow->show();
}

void Mycelia::resetLayoutCallback(Misc::CallbackData* cbData)
{
    stopLayout();
    bundleButton->setToggle(false);
    
    // allow changing layout before graph is loaded
    if(staticButton->getToggle())
    {
        setLayoutType(LAYOUT_STATIC);
    }
    else
    {
        setLayoutType(LAYOUT_DYNAMIC);
    }
    
    // abort layout if no nodes, or positions hard coded in data
    int size = g->getNodeCount();
    
    if(skipLayout || size == 0)
    {
        return;
    }
    
    // reset layout state
    g->randomizePositions(100);
    g->resetVelocities();
    
#ifndef __CUDA__
    startLayout();
#else
    // positions
    float4* positions_h = new float4[size];
    
    foreach(int node, g->getNodes())
    {
        const Vrui::Point& p = g->getNodePosition(node);
        float4 q;
        q.x = p[0];
        q.y = p[1];
        q.z = p[2];
        q.w = g->getNodeDegree(node);
        positions_h[node] = q;
    }
    
    // adjacency matrix
    int* adjacencies_h = new int[size * size];
    
    for(int row = 0; row < size; row++)
    {
        for(int col = 0; col < size; col++)
        {
            adjacencies_h[row * size + col] = g->hasEdge(row, col);
        }
    }
    
    // layout
    gpuLayout(positions_h, adjacencies_h, size);
    
    // update positions
    foreach(int node, g->getNodes())
    {
        const float4& q = positions_h[node];
        g->setNodePosition(node, Vrui::Point(q.x, q.y, q.z));
    }
    
    // free memory
    delete[] positions_h;
    delete[] adjacencies_h;
    
    resetNavigationCallback(0);
#endif
}

void Mycelia::resetNavigationCallback(Misc::CallbackData* cbData)
{
    stopLayout();
    
    pair<Vrui::Point, Vrui::Scalar> p = g->locate();
    Vrui::Point& center = p.first;
    Vrui::Scalar& radius = p.second;
    
    nodeRadius = radius / 150;
    arrowHeight = nodeRadius / 2;
    arrowWidth = arrowHeight / 2;
    edgeThickness = nodeRadius / 10;
    edgeOffset = nodeRadius + arrowHeight;
    
    Vrui::setNavigationTransformation(center, radius);
    
    g->update();
    resumeLayout();
}

void Mycelia::shortestPathCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
    if(shortestPathButton->getToggle())
    {
        if(previousNode != SELECTION_NONE && selectedNode != SELECTION_NONE)
        {
            predecessorVector = g->getShortestPath();
            Vrui::requestUpdate();
        }
        else
        {
            shortestPathButton->setToggle(false);
        }
    }
}

void Mycelia::spanningTreeCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
    if(cbData->set)
    {
        predecessorVector = g->getSpanningTree();
        Vrui::requestUpdate();
    }
}

void Mycelia::writeGraphCallback(Misc::CallbackData* cbData)
{
    g->write("data/graphdump.dot");
}

/*
 * node selection
 */
void Mycelia::clearSelections()
{
    previousNode = selectedNode = SELECTION_NONE;
    g->update();
}

int Mycelia::getPreviousNode() const
{
    return previousNode;
}

int Mycelia::getSelectedNode() const
{
    return selectedNode;
}

void Mycelia::setSelectedNode(int node)
{
    if(!g->isValidNode(node))
    {
        cout << "invalid node selected: " << node << endl;
        return;
    }
    
    if(nodeInfoButton->getToggle())
    {
        nodeWindow->update(g->getNodeAttributes(node));
    }
    
    previousNode = selectedNode;
    selectedNode = node;
    
    shortestPathCallback(0);
    g->update();
#ifdef __RPCSERVER__
    server->callback(node);
#endif
}

int Mycelia::selectNode(const Vrui::Point& clickPosition) const
{
    int result = SELECTION_NONE;
    float minDist2 = Math::sqr(nodeRadius);
    
    foreach(int node, g->getNodes())
    {
        float dist2 = Geometry::sqrDist(clickPosition, g->getNodePosition(node));
        
        if(dist2 < minDist2)
        {
            result = node;
            minDist2 = dist2;
        }
    }
    
    return result;
}

int Mycelia::selectNode(const Vrui::Ray& ray) const
{
    int result = SELECTION_NONE;
    float coneAngle2 = Math::sqr(coneAngle);
    float lambdaMin = numeric_limits<float>::max();
    
    foreach(int node, g->getNodes())
    {
        Vrui::Vector sp = g->getNodePosition(node) - ray.getOrigin();
        float x = sp * ray.getDirection();
        
        if(x >= 0 && x < lambdaMin)
        {
            float y2 = Geometry::sqr(Geometry::cross(sp, ray.getDirection()));
            
            if(y2 / Math::sqr(x) <= coneAngle2)
            {
                result = node;
                lambdaMin = x;
            }
        }
    }
    
    return result;
}

int Mycelia::selectNode(Vrui::InputDevice* device) const
{
    int result;
    
    if(device->is6DOFDevice())
    {
        Vrui::Point devicePosition(Vrui::getNavigationTransformation().inverseTransform(device->getPosition()));
        result = selectNode(devicePosition);
    }
    else
    {
        Vrui::Ray deviceRay(device->getPosition(), device->getRayDirection());
        deviceRay.transform(Vrui::getInverseNavigationTransformation());
        deviceRay.normalizeDirection();
        result = selectNode(deviceRay);
    }
    
    return result;
}

Vrui::Scalar Mycelia::getArrowWidth() const
{
    return arrowWidth;
}

Vrui::Scalar Mycelia::getArrowHeight() const
{
    return arrowHeight;
}

int main(int argc, char** argv)
{
    char** appDefaults = 0;
    Mycelia app(argc, argv, appDefaults);
    app.run();
    
    return 0;
}
