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

#ifndef __GRAPHBUILDER_HPP
#define __GRAPHBUILDER_HPP

#include <graph.hpp>
#include <mycelia.hpp>

class Mycelia;

class GraphBuilderFactory : public Vrui::ToolFactory
{
    friend class GraphBuilder;
    
private:
    Mycelia* application;
    
public:
    GraphBuilderFactory(Vrui::ToolManager&, Mycelia*);
    ~GraphBuilderFactory();
    
    Vrui::Tool* createTool(const Vrui::ToolInputAssignment& inputAssignment) const;
    void destroyTool(Vrui::Tool* tool) const;
    const char* getName() const;
};

class GraphBuilder : public Vrui::Tool
{
    friend class GraphBuilderFactory;
    
private:
    static GraphBuilderFactory* factory;
    Vrui::ONTransform initial;
    bool dragging;
    
    Vrui::Point currentPosition;
    Vrui::Point fromPosition;
    int fromNode; // stored internally to enable multiple tools
    
public:
    GraphBuilder(const Vrui::ToolFactory*, const Vrui::ToolInputAssignment&);
    
    void buttonCallback(int, Vrui::InputDevice::ButtonCallbackData*);
    void display(GLContextData&) const;
    void frame();
    const Vrui::ToolFactory* getFactory() const;
    Vrui::Point getPosition(Vrui::InputDevice* device) const;
};

#endif
