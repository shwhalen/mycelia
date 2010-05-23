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

#include <generators/barabasigenerator.hpp>

BarabasiGenerator::BarabasiGenerator(Mycelia* application)
    : GraphGenerator(application)
{
    parameterWindow = new BarabasiWindow(application, this);
}

void BarabasiGenerator::generate() const
{
    generateNodes(INITIAL_N);
    generateEdges(INITIAL_M0, INITIAL_N);
    parameterWindow->show();
}

void BarabasiGenerator::generateNodes(int nodeCount) const
{
    application->g->clear();
    
    for(int i = 0; i < nodeCount; i++)
    {
        application->g->addNode();
    }
}

void BarabasiGenerator::generateEdges(int initialNodeCount, int maxNodeCount) const
{
    application->g->clearEdges();
    
    for(int i = 0; i < initialNodeCount; i++)
    {
        application->g->addEdge(i, (i + 1) % initialNodeCount);
    }
    
    for(int sourceNode = initialNodeCount; sourceNode < maxNodeCount; sourceNode++)
    {
        for(int candidateNode = 0; candidateNode < maxNodeCount; candidateNode++)
        {
            if(sourceNode == candidateNode) continue;
            
            float p_i = (float)application->g->getDegree(candidateNode) / application->g->getEdgeCount();
            
            if(VruiHelp::randomFloat() < p_i)
            {
                application->g->addEdge(sourceNode, candidateNode);
            }
        }
    }
}