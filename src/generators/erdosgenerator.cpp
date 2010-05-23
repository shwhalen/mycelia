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

#include <generators/erdosgenerator.hpp>

using namespace std;

ErdosGenerator::ErdosGenerator(Mycelia* application)
    : GraphGenerator(application)
{
    parameterWindow = new ErdosWindow(application, this);
}

void ErdosGenerator::generate() const
{
    generateNodes(INITIAL_N);
    generateEdges(INITIAL_P);
    parameterWindow->show();
}

void ErdosGenerator::generateNodes(int nodeCount) const
{
    application->g->clear();
    
    for(int i = 0; i < nodeCount; i++)
    {
        application->g->addNode();
    }
}

void ErdosGenerator::generateEdges(float p) const
{
    application->g->clearEdges();
    
    foreach(int sourceNode, application->g->getNodes())
    {
        foreach(int candidateNode, application->g->getNodes())
        {
            if(sourceNode == candidateNode) continue;
            
            if(VruiHelp::randomFloat() < p)
            {
                application->g->addEdge(sourceNode, candidateNode);
            }
        }
    }
}