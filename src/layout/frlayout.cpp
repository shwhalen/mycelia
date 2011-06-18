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

#include <layout/frlayout.hpp>

using namespace std;

FruchtermanReingoldLayout::FruchtermanReingoldLayout(Mycelia* application)
    : GraphLayout(application)
{
}

void FruchtermanReingoldLayout::layout()
{
    stopped = false;
    layoutThread->start(this, &FruchtermanReingoldLayout::layoutThreadMethod);
}

void* FruchtermanReingoldLayout::layoutThreadMethod()
{
    springForceConstant = Math::pow(VOLUME / application->g->getNodeCount(), 1.0 / 3.0);
    
    for(remainingIterations = MAX_ITERATIONS; remainingIterations > 0 && !stopped; remainingIterations--)
    {
        layoutStep();
    }
    
    application->resetNavigationCallback(0);
    return 0;
}

void FruchtermanReingoldLayout::layoutStep()
{
    // temperature affects rate of movement, starts at 1 and moves gradually to 0
    double temperature = MAX_DELTA * Math::pow(remainingIterations / (double)MAX_ITERATIONS, COOLING_EXPONENT);
    vector<Vrui::Vector> forceVector(application->g->getNodeCount());
    
    // calculate repulsion between all nodes
    foreach(int source, application->g->getNodes())
    {
        if(!application->isSelectedComponent(source))
        {
            continue;
        }
        
        foreach(int target, application->g->getNodes())
        {
            if(!application->isSelectedComponent(target) || source == target)
            {
                continue;
            }
            
            // calculate repulsive force as k^2 / distance (or variant)
            Vrui::Vector v = application->g->getNodePosition(source) - application->g->getNodePosition(target);
            Vrui::Scalar mag = Geometry::mag(v);
            
            if(mag > 0) v = v.normalize();
            else mag = 0.001;
            
            Vrui::Scalar repulsiveForce = springForceConstant * springForceConstant * (1 / mag - mag * mag / REPULSION_RADIUS);
            Vrui::Scalar weightedForce = repulsiveForce * application->g->getNodeDegree(source);
            
            forceVector[source] += v * weightedForce;
            forceVector[target] -= v * weightedForce;
            
            // attract connected nodes as distance^2 / k
            if(application->g->hasEdge(source, target))
            {
                Vrui::Scalar attractiveForce = mag * mag / springForceConstant;
                float edgeWeights = 0;
                
                foreach(int edge, application->g->getEdges(source, target))
                {
                    edgeWeights += application->g->getEdgeWeight(edge);
                }
                
                attractiveForce *= edgeWeights;
                forceVector[source] -= v * attractiveForce;
                forceVector[target] += v * attractiveForce;
            }
        }
    }
    
    // dampen motion and update position
    for(int node = 0; node < application->g->getNodeCount(); node++)
    {
        if(!application->isSelectedComponent(node))
        {
            continue;
        }
        
        Vrui::Scalar mag = forceVector[node].mag();
        
        if(mag > temperature)
        {
            forceVector[node] *= temperature / mag;
        }
        
        application->g->updateNodePosition(node, forceVector[node]);
    }
}