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

#include <layout/edgebundler.hpp>

using namespace std;

EdgeBundler::EdgeBundler(Mycelia* application)
    : GraphLayout(application)
{
}

void EdgeBundler::allocateSegments()
{
    // calculate maximum subdivisions
    int maxSegments = 2; // endpoints
    
    for(int i = 0; i <= MAX_CYCLE; i++)
    {
        maxSegments += pow(2.0, i);
    }
    
    // allocate per-edge segment vector
    segmentVector = vector<vector<Vrui::Point> >(application->g->getEdgeCount());
    
    for(int edge = 0; edge < application->g->getEdgeCount(); edge++)
    {
        segmentVector[edge] = vector<Vrui::Point>(maxSegments);
        
        for(int segment = 0; segment < maxSegments; segment++)
        {
            segmentVector[edge][segment] = Vrui::Point(numeric_limits<float>::max(), 0, 0);
        }
    }
}

void EdgeBundler::layout()
{
    stopped = false;
    layoutThread->start(this, &EdgeBundler::layoutThreadMethod);
}

void* EdgeBundler::layoutThreadMethod()
{
    cycle = 0;
    segments = SUBDIVISIONS_0;
    stepsize = STEPSIZE_0;
    iterations = ITERATIONS_0;
    allocateSegments();
    
    while(cycle <= MAX_CYCLE && !stopped)
    {
        for(int iteration = 0; iteration < iterations; iteration++)
        {
            for(int firstEdge = 0; firstEdge < (int)application->g->getEdgeCount(); firstEdge++)
            {
                Vrui::Scalar k_p = K / Geometry::abs(application->g->getSourcePosition(firstEdge) - application->g->getTargetPosition(firstEdge));
                
                for(int segment = 1; *getSegment(firstEdge, segment) != application->g->getTargetPosition(firstEdge); segment++)
                {
                    Vrui::Point& p_prev     = *getSegment(firstEdge, segment - 1);
                    Vrui::Point& p          = *getSegment(firstEdge, segment);
                    Vrui::Point& p_next     = *getSegment(firstEdge, segment + 1);
                    
                    Vrui::Vector F_s_prev_v = p_prev - p;
                    Vrui::Vector F_s_next_v = p_next - p;
                    Vrui::Vector F_s_v      = (F_s_prev_v + F_s_next_v) * k_p;
                    Vrui::Vector F_e_v      = Vrui::Vector(0, 0, 0);
                    
                    for(int secondEdge = 0; secondEdge < application->g->getEdgeCount(); secondEdge++)
                    {
                        if(firstEdge == secondEdge) continue;
                        
                        Vrui::Point& q = *getSegment(secondEdge, segment);
                        Vrui::Vector v = q - p;
                        Vrui::Scalar mag = Geometry::mag(v);
                        
                        if(mag > 0)
                        {
                            Vrui::Vector F_e_v_delta = v / Math::pow(mag, 3); // power 2=linear, 3=quadratic
                            F_e_v += F_e_v_delta;
                        }
                    }
                    
                    Vrui::Vector force  = Vrui::Vector(0, 0, 0);
                    Vrui::Vector F_v    = F_s_v + F_e_v;
                    Vrui::Scalar mag    = Geometry::mag(F_v);
                    
                    if(mag > 0)
                    {
                        if(mag > 1) F_v = F_v.normalize();
                        force += F_v * stepsize;
                    }
                    
                    p += force;
                }
            }
            
            application->g->update();
        }
        
        cycle++;
        segments += pow(2.0, cycle);
        stepsize /= 2.0;
        iterations *= 0.66;
    }
    
    return 0;
}

inline int EdgeBundler::getIndex(int i) const
{
    return i * pow(2.0, MAX_CYCLE - cycle);
}

Vrui::Point* EdgeBundler::getSegment(int edge, int segment)
{
    int index = getIndex(segment);
    
    if(isSegmentEmpty(edge, index))
    {
        if(segment == 0)
        {
            segmentVector[edge][index] = Vrui::Point(application->g->getSourcePosition(edge));
        }
        else if(segment > segments)
        {
            segmentVector[edge][index] = Vrui::Point(application->g->getTargetPosition(edge));
        }
        else
        {
            segmentVector[edge][index] = VruiHelp::midpoint(*getSegment(edge, segment - 1), *getSegment(edge, segment + 1));
        }
    }
    
    return &segmentVector[edge][index];
}

int EdgeBundler::getSegmentCount() const
{
    return segments;
}

inline bool EdgeBundler::isSegmentEmpty(int edge, int index) const
{
    return segmentVector[edge][index][0] == numeric_limits<float>::max();
}