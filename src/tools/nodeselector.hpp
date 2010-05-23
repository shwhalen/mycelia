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

#ifndef __NODESELECTOR_HPP
#define __NODESELECTOR_HPP

#include <graph.hpp>
#include <mycelia.hpp>

class Mycelia;

class NodeSelectorFactory : public Vrui::ToolFactory
{
    friend class NodeSelector;
    
private:
    Mycelia* application;
    
public:
    NodeSelectorFactory(Vrui::ToolManager&, Mycelia*);
    ~NodeSelectorFactory();
    
    Vrui::Tool* createTool(const Vrui::ToolInputAssignment& inputAssignment) const;
    void destroyTool(Vrui::Tool* tool) const;
    const char* getName() const;
};

class NodeSelector : public Vrui::Tool//, public Vrui::Application::Tool<Mycelia>
{
    friend class NodeSelectorFactory;
    
private:
    static NodeSelectorFactory* factory;
    Vrui::ONTransform initial;
    bool dragging;
    
public:
    NodeSelector(const Vrui::ToolFactory*, const Vrui::ToolInputAssignment&);
    
    void buttonCallback(int, int, Vrui::InputDevice::ButtonCallbackData*);
    void frame();
    const Vrui::ToolFactory* getFactory() const;
};

#endif