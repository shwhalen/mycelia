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

#include <tools/nodeselector.hpp>

using namespace std;

/*
 * static initializer
 */
NodeSelectorFactory* NodeSelector::factory = 0;

/*
 * factory
 */
NodeSelectorFactory::NodeSelectorFactory(Vrui::ToolManager& toolManager, Mycelia* application)
    : ToolFactory("NodeSelector", toolManager), application(application)
{
    layout.setNumDevices(1);    // custom tools require one input device
    layout.setNumButtons(0, 1); // 0th device requires 1 button
    
    NodeSelector::factory = this;
}

NodeSelectorFactory::~NodeSelectorFactory()
{
    NodeSelector::factory = 0;
}

Vrui::Tool* NodeSelectorFactory::createTool(const Vrui::ToolInputAssignment& inputAssignment) const
{
    return new NodeSelector(this, inputAssignment);
}

void NodeSelectorFactory::destroyTool(Vrui::Tool* tool) const
{
    delete tool;
}

const char* NodeSelectorFactory::getName() const
{
    return "Node Selector";
}

/*
 * tool
 */
NodeSelector::NodeSelector(const Vrui::ToolFactory* factory, const Vrui::ToolInputAssignment& inputAssignment)
    : Vrui::Tool(factory, inputAssignment), dragging(false)
{
}

void NodeSelector::buttonCallback(int deviceIndex, int buttonIndex, Vrui::InputDevice::ButtonCallbackData* cbData)
{
    Vrui::InputDevice* device = input.getDevice(0);
    
    if(cbData->newButtonState)
    {
        int selectedNode = factory->application->selectNode(device);
        
        if(selectedNode != SELECTION_NONE)
        {
            factory->application->setSelectedNode(selectedNode);
            
            // transform 'stores' initial device translation (using inverse to later compute incremental change), as well as node starting point
            initial = Vrui::ONTransform(Vrui::getDeviceTransformation(device).getTranslation(), Vrui::getDeviceTransformation(device).getRotation());
            initial.doInvert();
            initial *= Vrui::ONTransform(factory->application->g->getPosition(selectedNode) - Vrui::Point::origin, Geometry::Rotation<double, 3>());
            
            dragging = true;
        }
    }
    else
    {
        dragging = false;
    }
}

void NodeSelector::frame()
{
    Vrui::InputDevice* device = input.getDevice(0);
    
    if(dragging)
    {
        Vrui::ONTransform current(Vrui::getDeviceTransformation(device).getTranslation(), Vrui::getDeviceTransformation(device).getRotation());
        current *= initial; // 'subtracts' starting transform to find increment, adds to node start position
        factory->application->g->setPosition(factory->application->getSelectedNode(), current.getOrigin());
    }
}

const Vrui::ToolFactory* NodeSelector::getFactory(void) const
{
    return factory;
}
