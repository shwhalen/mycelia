/*
 * Mycelia immersive 3d network visualization tool.
 * Copyright (C) 2008-2009 Sean Whalen.
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

#ifndef __MYCELIA_HPP
#define __MYCELIA_HPP

#include <precompiled.hpp>

class ArfLayout;
class ArfWindow;
class AttributeWindow;
class BarabasiGenerator;
class ChacoParser;
class DotParser;
class EdgeBundler;
class ErdosGenerator;
class FruchtermanReingoldLayout;
class GmlParser;
class Graph;
class GraphGenerator;
class GraphLayout;
class ImageWindow;
class MyceliaDataItem;
class RpcServer;
class XmlParser;
class WattsGenerator;

#define LAYOUT_STATIC 0
#define LAYOUT_DYNAMIC 1
#define SELECTION_NONE -1
#define FONT_SIZE 96.0
#define FONT_MODIFIER 0.04
#define foreach BOOST_FOREACH
#define PYTHON "/opt/local/bin/python"

class Mycelia : public Vrui::Application, public GLObject
{
private:
    // graph
    Vrui::Scalar nodeRadius;
    Vrui::Scalar arrowHeight;
    Vrui::Scalar arrowWidth;
    Vrui::Scalar edgeThickness;
    Vrui::Scalar edgeOffset;
    
    // layout and bundling
    FruchtermanReingoldLayout* staticLayout;
    ArfLayout* dynamicLayout;
    GraphLayout* layout;
    EdgeBundler* edgeBundler;
    bool skipLayout;
    
    // gui
    GLMotif::Menu* mainMenu;
    GLMotif::PopupMenu* mainMenuPopup;
    
    // gui -- generators
    GLMotif::RadioBox* generatorRadioBox;
    GLMotif::ToggleButton* barabasiButton;
    GLMotif::ToggleButton* erdosButton;
    GLMotif::ToggleButton* wattsButton;
    
    // gui -- layout
    GLMotif::RadioBox* layoutRadioBox;
    GLMotif::ToggleButton* staticButton;
    GLMotif::ToggleButton* dynamicButton;
    
    // gui -- render options
    GLMotif::ToggleButton* bundleButton;
    GLMotif::ToggleButton* nodeInfoButton;
    GLMotif::ToggleButton* nodeLabelButton;
    GLMotif::ToggleButton* edgeLabelButton;
    GLMotif::ToggleButton* componentButton;
    
    // gui -- algorithms
    GLMotif::ToggleButton* spanningTreeButton;
    GLMotif::ToggleButton* shortestPathButton;
    
    // gui -- python plugins
    GLMotif::ToggleButton* degreeButton;
    GLMotif::ToggleButton* centralityButton;
    GLMotif::ToggleButton* adjacencyButton;
    GLMotif::ToggleButton* lanetButton;
    
    // dialogs
    GLMotif::FileSelectionDialog* fileWindow;
    AttributeWindow* nodeWindow;
    ArfWindow* layoutWindow;
    ImageWindow* imageWindow;
    AttributeWindow* statusWindow;
    
    // algorithms
    std::vector<int> predecessorVector;
    
    // fonts
    FTFont* font;
    
    // generators
    GraphGenerator* generator;
    BarabasiGenerator* barabasiGenerator;
    ErdosGenerator* erdosGenerator;
    WattsGenerator* wattsGenerator;
    
    // parsers
    ChacoParser* chacoParser;
    DotParser* dotParser;
    GmlParser* gmlParser;
    XmlParser* xmlParser;
    
    // logo
    double lastFrameTime;
    double rotationAngle;
    double rotationSpeed;
    
    // misc
    int selectedNode;
    int previousNode;
    float coneAngle;
    Vrui::Vector xVector;
    Vrui::Vector zVector;
#ifdef __RPCSERVER__
    RpcServer* server;
#endif
    
public:
    Mycelia(int, char**, char**);
    ~Mycelia();
    
    // graph functions
    void buildGraphList(MyceliaDataItem*) const;
    void drawEdge(int, int, const MyceliaDataItem*) const;
    void drawEdge(const Vrui::Point&, const Vrui::Point&, bool, bool, const MyceliaDataItem*) const;
    void drawEdges(const MyceliaDataItem*) const;
    void drawEdgeLabels(const MyceliaDataItem*) const;
    void drawLogo() const;
    void drawNode(int, const MyceliaDataItem*) const;
    void drawNodes(const MyceliaDataItem*) const;
    void drawNodeLabels(const MyceliaDataItem*) const;
    void drawShortestPath(const MyceliaDataItem*) const;
    void drawSpanningTree(const MyceliaDataItem*) const;
    bool isSelectedComponent(int) const;
    
    // layout functions
    void resumeLayout() const;
    void setLayoutType(int);
    void setSkipLayout(bool);
    void startLayout() const;
    void stopLayout() const;
    
    // vrui functions
    void display(GLContextData&) const;
    void frame();
    void initContext(GLContextData&) const;
    
    // callbacks
    void bundleCallback(GLMotif::ToggleButton::ValueChangedCallbackData*);
    void clearCallback(Misc::CallbackData*);
    void componentCallback(GLMotif::ToggleButton::ValueChangedCallbackData*);
    void fileCancelAction(GLMotif::FileSelectionDialog::CancelCallbackData*);
    void fileOpenAction(GLMotif::FileSelectionDialog::OKCallbackData*);
    void generatorCallback(GLMotif::RadioBox::ValueChangedCallbackData*);
    void nodeInfoCallback(GLMotif::ToggleButton::ValueChangedCallbackData*);
    void nodeLabelCallback(GLMotif::ToggleButton::ValueChangedCallbackData*);
    void openFileCallback(Misc::CallbackData*);
    void pythonCallback(GLMotif::RadioBox::ValueChangedCallbackData*);
    void refreshCallback(Misc::CallbackData*);
    void resetLayoutCallback(Misc::CallbackData*);
    void resetNavigationCallback(Misc::CallbackData* cbData);
    void shortestPathCallback(GLMotif::ToggleButton::ValueChangedCallbackData*);
    void spanningTreeCallback(GLMotif::ToggleButton::ValueChangedCallbackData*);
    void writeGraphCallback(Misc::CallbackData*);
    
    // node selection
    void clearSelections();
    int getPreviousNode() const;
    int getSelectedNode() const;
    void setSelectedNode(int);
    int selectNode(const Vrui::Point&) const;
    int selectNode(const Vrui::Ray&) const;
    int selectNode(Vrui::InputDevice*) const;
    
    // other
    Graph* g; // wrap this eventually
    Graph* gCopy;
    GLMotif::PopupMenu* getMainMenuPopup() { return mainMenuPopup; }
    ArfLayout* getDynamicLayout() { return dynamicLayout; }
    void setStatus(const char*) const;
};

#endif
