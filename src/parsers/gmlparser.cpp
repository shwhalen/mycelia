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

#include <parsers/gmlparser.hpp>

using namespace std;

GmlParser::GmlParser(Mycelia* application)
    : application(application)
{
}

void GmlParser::parse(string& filename)
{
    ifstream in(filename.c_str());
    
    while(!in.eof())
    {
        char line[256];
        in.getline(line, 256);
        
        stringstream stream(line);
        string token;
        stream >> token;
        
        if(token == "id")
        {
            application->g->addNode();
        }
        else if(token == "source")
        {
            int source;
            int target;
            
            stream >> source;
            in.getline(line, 256);
            stringstream stream2(line);
            stream2 >> token >> target;
            
            if(token == "target")
            {
                application->g->addEdge(source, target);
                cout << source << " -> " << target << endl;
            }
        }
    }
}