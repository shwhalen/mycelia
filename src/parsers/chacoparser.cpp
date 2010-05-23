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

#include <parsers/chacoparser.hpp>

using namespace std;

ChacoParser::ChacoParser(Mycelia* application)
    : application(application)
{
}

void ChacoParser::parse(string& filename)
{
    ifstream in(filename.c_str());
    int nodeCount;
    int edgeCount;
    
    in >> nodeCount >> edgeCount;
    cout << nodeCount << " nodes, " << edgeCount << " edges" << endl;
    
    for(int i = 0; i < nodeCount; i++)
    {
        application->g->addNode();
    }
    
    int sourceNode = 0;
    
    while(!in.eof())
    {
        char line[256];
        in.getline(line, 256);
        
        stringstream stream(line);
        int targetNode;
        
        while(stream >> targetNode) // only works because space is delimiter
        {
            application->g->addEdge(sourceNode, targetNode);
        }
        
        sourceNode++;
    }
    
    in.close();
}