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

#include <dataItem.hpp>
#include <tools/graphbuilder.hpp>

using namespace std;

/*
 * static initializer
 */
GraphBuilderFactory* GraphBuilder::factory = 0;

/*
 * factory
 */
GraphBuilderFactory::GraphBuilderFactory(Vrui::ToolManager& toolManager, Mycelia* application)
    : ToolFactory("GraphBuilder", toolManager), application(application)
{
    layout.setNumButtons(0, 1); // 0th device requires 1 button
    
    GraphBuilder::factory = this;
}

GraphBuilderFactory::~GraphBuilderFactory()
{
    GraphBuilder::factory = 0;
}

Vrui::Tool* GraphBuilderFactory::createTool(const Vrui::ToolInputAssignment& inputAssignment) const
{
    return new GraphBuilder(this, inputAssignment);
}

void GraphBuilderFactory::destroyTool(Vrui::Tool* tool) const
{
    delete tool;
}

const char* GraphBuilderFactory::getName() const
{
    return "Graph Builder";
}

/*
 * tool
 */
GraphBuilder::GraphBuilder(const Vrui::ToolFactory* factory, const Vrui::ToolInputAssignment& inputAssignment)
    : Vrui::Tool(factory, inputAssignment)
{
    dragging = false;
    currentPosition = Vrui::Point(0, 0, 0);
    fromNode = SELECTION_NONE;
}

void GraphBuilder::buttonCallback(int buttonIndex, Vrui::InputDevice::ButtonCallbackData* cbData)
{
    Vrui::InputDevice* device = getButtonDevice(0);
    int selectedNode = factory->application->selectNode(device);
    
    if(cbData->newButtonState)
    {
        factory->application->stopLayout();
        dragging = true;
        
        if(selectedNode == SELECTION_NONE)
        {
            fromNode = factory->application->g->addNode(currentPosition);
        }
        else
        {
            fromNode = selectedNode;
        }
        
        // for visual feedback
        factory->application->clearSelections();
        factory->application->setSelectedNode(fromNode);
        fromPosition = currentPosition;
    }
    else
    {
        dragging = false;
        int toNode;
        
        if(selectedNode == SELECTION_NONE)
        {
            toNode = factory->application->g->addNode(currentPosition);
        }
        else
        {
            toNode = selectedNode;
        }
        
        if(fromNode != toNode) factory->application->g->addEdge(fromNode, toNode);
        factory->application->setSelectedNode(toNode);
        factory->application->resumeLayout();
    }
}

void GraphBuilder::display(GLContextData& contextData) const
{
    if(dragging)
    {
        bool drawArrow = true;
        bool bidirectional = false;
        MyceliaDataItem* dataItem = contextData.retrieveDataItem<MyceliaDataItem>(this);
        glPushMatrix();
        glMultMatrix(Vrui::getNavigationTransformation());
        factory->application->drawEdge(fromPosition, currentPosition, drawArrow, bidirectional, dataItem);
        glPopMatrix();
    }
}

void GraphBuilder::frame()
{
    Vrui::InputDevice* device = input.getSlotDevice(0);
    currentPosition = getPosition(device);
}

const Vrui::ToolFactory* GraphBuilder::getFactory(void) const
{
    return factory;
}

Vrui::Point GraphBuilder::getPosition(Vrui::InputDevice* device) const
{
    Vrui::Point pos;
    
    if(device->is6DOFDevice())
    {
        pos = Vrui::getInverseNavigationTransformation().transform(device->getPosition());
    }
    else
    {
        // calculate ray equation
        Vrui::Ray deviceRay(device->getPosition(), device->getRayDirection());
        
        // find the closest intersection with any screen
        std::pair<Vrui::VRScreen*, Vrui::Scalar> screenPair = Vrui::findScreen(deviceRay);
        
        if(screenPair.first != 0)
        {
            // get the intersection position in model coordinates
            Vrui::Point intersectionPoint = deviceRay(screenPair.second);
            pos = Vrui::getInverseNavigationTransformation().transform(intersectionPoint);
        }
    }
    
    return pos;
}

void GraphBuilder::initContext(GLContextData& contextData) const
{
        MyceliaDataItem* dataItem = new MyceliaDataItem();

        Vrui::Scalar arrowWidth = factory->application->getArrowWidth();
        Vrui::Scalar arrowHeight = factory->application->getArrowHeight();

        glNewList(dataItem->arrowList, GL_COMPILE);
        gluCylinder(dataItem->quadric, arrowWidth, 0.0, arrowHeight, 10, 1);
        gluQuadricOrientation(dataItem->quadric, GLU_INSIDE);
        gluDisk(dataItem->quadric, 0.0, arrowWidth, 10, 1);
        gluQuadricOrientation(dataItem->quadric, GLU_OUTSIDE);
        glEndList();

        contextData.addDataItem(this, dataItem);
}

