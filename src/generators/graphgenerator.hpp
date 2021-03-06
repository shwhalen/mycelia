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

#ifndef __GRAPHGENERATOR_HPP
#define __GRAPHGENERATOR_HPP

#include <mycelia.hpp>
#include <windows/window.hpp>

class GraphGenerator
{
protected:
    Mycelia* application;
    Window* parameterWindow;
    
public:
    GraphGenerator(Mycelia* application) : application(application) { parameterWindow = NULL; }
    
    void hide()
    {
        if(parameterWindow != NULL)
        {
            parameterWindow->hide();
        }
    }
    
    virtual void generate() const = 0;
};

#endif