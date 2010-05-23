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

#ifndef __EDGEBUNDLER_HPP
#define __EDGEBUNDLER_HPP

#include <graph.hpp>
#include <mycelia.hpp>
#include <vruihelp.hpp>
#include <layout/graphlayout.hpp>

#define SUBDIVISIONS_0      1
#define STEPSIZE_0          0.04
#define ITERATIONS_0        50
#define MAX_CYCLE           5
#define K                   1.5     // higher = less bundling

class EdgeBundler : public GraphLayout
{
private:
    int segments;
    double stepsize;
    int iterations;
    int cycle;
    std::vector<std::vector<Vrui::Point> > segmentVector;
    
public:
    EdgeBundler(Mycelia*);
    
    void allocateSegments();
    void layout();
    void* layoutThreadMethod();
    int getIndex(int) const;
    Vrui::Point* getSegment(int, int);
    int getSegmentCount() const;
    bool isSegmentEmpty(int, int) const;
};

#endif