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

#include <layout/arflayout.hpp>

using namespace std;

ArfLayout::ArfLayout(Mycelia* application)
    : GraphLayout(application)
{
    dynamic = true;
    
    dampingConstant = -3;
    beta = -0.45;
    deltaTime = 0.01;
    layoutRadius = 2;
    
    connectedSpringConstant = -2;
    stronglyConnectedSpringConstant = -2;
    unconnectedSpringConstant = -1;
    
    connectedSpringLength = 1;
    stronglyConnectedSpringLength = 1;
    unconnectedSpringLength = 1;
}

inline double ArfLayout::getSpringConstant(int edgeCount) const
{
    switch(edgeCount)
    {
    case 2:
        return stronglyConnectedSpringConstant;
        break;
        
    case 1:
        return connectedSpringConstant;
        break;
        
    default:
        return unconnectedSpringConstant;
        break;
    }
}

inline double ArfLayout::getSpringLength(int edgeCount) const
{
    switch(edgeCount)
    {
    case 2:
        return stronglyConnectedSpringLength;
        break;
        
    case 1:
        return connectedSpringLength;
        break;
        
    default:
        return unconnectedSpringLength;
        break;
    }
}

void ArfLayout::layout()
{
    stopped = false;
    layoutThread->start(this, &ArfLayout::layoutThreadMethod);
}

void* ArfLayout::layoutThreadMethod()
{
    // there is an inherent conflict between having a responsive dynamic layout and
    // threads that can add/remove nodes/edges at any time (graph tool/rpc server).
    // if using in the cave: comment out the lock/unlock.
    // if using rpc server: uncomment.
    while(!stopped)
    {
        application->g->lock();
        layoutStep();
        application->g->unlock();
    }
    
    return 0;
}

void ArfLayout::layoutStep()
{
    int nodeCount = application->g->getNodeCount();
    vector<Vrui::Vector> velocityVector(nodeCount);
    vector<Vrui::Vector> positionVector(nodeCount);
    
    double totalWeight = 0;
    
    foreach(int edge, application->g->getEdges())
    {
        totalWeight += application->g->getEdgeWeight(edge);
    }
    
    foreach(int source, application->g->getNodes())
    {
        if(!application->isSelectedComponent(source) || source == application->getSelectedNode())
        {
            continue;
        }
        
        double mass = application->g->getSize(source); // treat size as mass
        Vrui::Vector velocity = application->g->getVelocity(source);
        Vrui::Vector dampingForce = dampingConstant * velocity;
        
        //Vrui::Vector springForce(0);
        //Vrui::Vector repulsionForce(0);
        
        foreach(int target, application->g->getNodes())
        {
            if(stopped)
            {
                return;
            }
            
            if(!application->isSelectedComponent(target) || source == target)
            {
                continue;
            }
            
            Vrui::Vector v = application->g->getPosition(source) - application->g->getPosition(target);
            Vrui::Scalar mag = Geometry::mag(v);
            
            int edgeCount = (int)application->g->hasEdge(source, target) + (int)application->g->hasEdge(target, source);
            double springConstant = getSpringConstant(edgeCount);
            double springLength = getSpringLength(edgeCount);
            
            double constA = springConstant * (mag - springLength) / mag;
            Vrui::Vector springForce = constA * v;
            
            double constB = layoutRadius * sqrt(nodeCount) / pow(mag, 1 + beta);
            Vrui::Vector repulsionForce = constB * v;
            
            double edgeWeight = 0;
            
            if(application->g->hasEdge(source, target))
            {
                foreach(int edge, application->g->getEdges(source, target))
                {
                    edgeWeight += application->g->getEdgeWeight(edge);
                }
                
                edgeWeight /= totalWeight;
            }
            
            mass *= (1 + edgeWeight);
            velocityVector[source] = VruiHelp::rk4(velocityVector[source], (dampingForce + springForce + repulsionForce) / mass, deltaTime);
            positionVector[source] = VruiHelp::rk4(positionVector[source], velocity, deltaTime);
        }
        
        //velocityVector[source] += deltaTime * (dampingForce + springForce + repulsionForce) / mass;
        //positionVector[source] += deltaTime * velocity;
    }
    
    foreach(int node, application->g->getNodes())
    {
        application->g->updateVelocity(node, velocityVector[node]);
        application->g->updatePosition(node, positionVector[node]);
    }
}