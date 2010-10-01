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

// module by Chris Ellison

#ifndef __ARFLAYOUT_HPP
#define __ARFLAYOUT_HPP

#include <graph.hpp>
#include <mycelia.hpp>
#include <layout/graphlayout.hpp>

class ArfLayout : public GraphLayout
{
    friend class ArfWindow;
    
private:
    double dampingConstant;
    double beta;
    double deltaTime;
    double layoutRadius;
    
    double connectedSpringConstant;
    double stronglyConnectedSpringConstant;
    double unconnectedSpringConstant;
    
    double connectedSpringLength;
    double stronglyConnectedSpringLength;
    double unconnectedSpringLength;
    
public:
    ArfLayout(Mycelia*);
    
    double getSpringConstant(int) const;
    double getSpringLength(int) const;
    void layout();
    void layoutStep();
    void* layoutThreadMethod();
};

#endif