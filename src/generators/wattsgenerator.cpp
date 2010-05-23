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

#include <generators/wattsgenerator.hpp>

WattsGenerator::WattsGenerator(Mycelia* application)
    : GraphGenerator(application)
{
    parameterWindow = new WattsWindow(application, this);
}

void WattsGenerator::generate() const
{
    generateNodes(INITIAL_N);
    generateEdges(INITIAL_N, INITIAL_BETA);
    parameterWindow->show();
}

void WattsGenerator::generateNodes(int nodeCount) const
{
    application->g->clear();
    
    for(int i = 0; i < nodeCount; i++)
    {
        application->g->addNode();
    }
}

void WattsGenerator::generateEdges(int nodeCount, float beta) const
{
    application->g->clearEdges();
    
    for(int i = 0; i < nodeCount; i++)
    {
        application->g->addEdge(i, (i + 1) % nodeCount);
    }
    
    set<int> edgesCopy = application->g->getEdges();
    
    // this is probably not correct
    foreach(int edge, edgesCopy)
    {
        const Edge& e = application->g->getEdge(edge);
        int node = e.source;
        
        if(VruiHelp::randomFloat() < beta)
        {
            application->g->deleteEdge(edge);
            int candidateNode;
            
            do
            {
                candidateNode = VruiHelp::randomFloat() * nodeCount;
            }
            while(node == candidateNode);
            
            application->g->addEdge(node, candidateNode);
        }
    }
}