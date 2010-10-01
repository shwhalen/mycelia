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

#ifndef __FRLAYOUT_HPP
#define __FRLAYOUT_HPP

#include <graph.hpp>
#include <mycelia.hpp>
#include <layout/graphlayout.hpp>

#define MAX_ITERATIONS 300
#define MAX_DELTA 100
#define COOLING_EXPONENT 1.5
#define VOLUME 1000
#define REPULSION_RADIUS 10000

class FruchtermanReingoldLayout : public GraphLayout
{
private:
    int remainingIterations;
    double springForceConstant;
    
public:
    FruchtermanReingoldLayout(Mycelia*);
    
    void layout();
    void layoutStep();
    void* layoutThreadMethod();
};

#endif
