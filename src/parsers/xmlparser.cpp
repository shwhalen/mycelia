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

#include <parsers/xmlparser.hpp>

using namespace std;
using namespace boost;

XmlParser::XmlParser(Mycelia* application)
    : application(application)
{
}

void XmlParser::parse(string& filename)
{
    const basic_regex<char> colorRegex("^\\s*<color attribute=\"?([\\w\\d]+)\"? value=\"?([\\w\\d])+\"? rgba=\"?([\\d]+) ([\\d]+) ([\\d]+) ([\\d]+)\"?");
    const basic_regex<char> nodeRegex("^\\s*<node ([^>]+)>");
    const basic_regex<char> edgeRegex("^\\s*<edge ([^>]+)>");
    const basic_regex<char> keyvalueRegex("(\\w+)=\"?([^\"]+)\"?");
    
    string fileBuffer = VruiHelp::fileToString(filename);
    boost::smatch lineMatches;
    boost::smatch attributeMatches;
    string::const_iterator lineStart;
    string::const_iterator lineEnd;
    idMap.clear();
    
    // colors
    lineStart = fileBuffer.begin();
    lineEnd = fileBuffer.end();
    string colorKey;
    
    while(regex_search(lineStart, lineEnd, lineMatches, colorRegex))
    {
        colorKey = string(lineMatches[1].first, lineMatches[1].second);
        string value(lineMatches[2].first, lineMatches[2].second);
        string r(lineMatches[3].first, lineMatches[3].second);
        string g(lineMatches[4].first, lineMatches[4].second);
        string b(lineMatches[5].first, lineMatches[5].second);
        string a(lineMatches[6].first, lineMatches[6].second);
        
        vector<int> rgba(4);
        rgba[0] = VruiHelp::stringToFloat(r);
        rgba[1] = VruiHelp::stringToFloat(g);
        rgba[2] = VruiHelp::stringToFloat(b);
        rgba[3] = VruiHelp::stringToFloat(a);
        colorMap[value] = rgba;
        
        lineStart = lineMatches[0].second;
    }
    
    // nodes -- find all nodes, then add in sorted order
    lineStart = fileBuffer.begin();
    lineEnd = fileBuffer.end();
    vector<int> addList(0);
    
    while(regex_search(lineStart, lineEnd, lineMatches, nodeRegex))
    {
        string::const_iterator nodeStart = lineMatches[1].first;
        string::const_iterator nodeEnd = lineMatches[1].second;
        
        while(regex_search(nodeStart, nodeEnd, attributeMatches, keyvalueRegex))
        {
            string key(attributeMatches[1].first, attributeMatches[1].second);
            string value(attributeMatches[2].first, attributeMatches[2].second);
            
            if(key == "id")
            {
                addList.push_back(VruiHelp::stringToInt(value));
            }
            
            nodeStart = attributeMatches[0].second;
        }
        
        lineStart = lineMatches[0].second;
    }
    
    std::sort(addList.begin(), addList.end());
    
    foreach(int xmlId, addList)
    {
        int graphId = application->g->addNode();
        idMap[xmlId] = graphId;
    }
    
    // nodes -- add attributes
    lineStart = fileBuffer.begin();
    lineEnd = fileBuffer.end();
    
    while(regex_search(lineStart, lineEnd, lineMatches, nodeRegex))
    {
        string::const_iterator nodeStart = lineMatches[1].first;
        string::const_iterator nodeEnd = lineMatches[1].second;
        int xmlId = -1;
        
        // loop through attributes
        while(regex_search(nodeStart, nodeEnd, attributeMatches, keyvalueRegex))
        {
            string key(attributeMatches[1].first, attributeMatches[1].second);
            string value(attributeMatches[2].first, attributeMatches[2].second);
            
            if(key == "id")
            {
                xmlId = VruiHelp::stringToInt(value);
            }
            else if(key == colorKey)
            {
                vector<int>& rgba = colorMap[value];
                application->g->setColor(idMap[xmlId], rgba[0], rgba[1], rgba[2], rgba[3]);
            }
            else if(key == "label")
            {
                application->g->setNodeLabel(idMap[xmlId], value);
            }
            
            application->g->setAttribute(idMap[xmlId], key, value);
            nodeStart = attributeMatches[0].second;
        }
        
        lineStart = lineMatches[0].second;
    }
    
    // edges
    lineStart = fileBuffer.begin();
    lineEnd = fileBuffer.end();
    
    while(regex_search(lineStart, lineEnd, lineMatches, edgeRegex))
    {
        string::const_iterator edgeStart = lineMatches[1].first;
        string::const_iterator edgeEnd = lineMatches[1].second;
        int source = -1;
        int target = -1;
        int edgeId = -1;
        bool directed = true;
        
        // loop through attributes
        while(regex_search(edgeStart, edgeEnd, attributeMatches, keyvalueRegex))
        {
            string key(attributeMatches[1].first, attributeMatches[1].second);
            string value(attributeMatches[2].first, attributeMatches[2].second);
            
            if(key == "from")
            {
                source = VruiHelp::stringToInt(value);
            }
            else if(key == "to")
            {
                target = VruiHelp::stringToInt(value);
                edgeId = application->g->addEdge(idMap[source], idMap[target]);
            }
            else if(key == "directed" && value == "false")
            {
                directed = false; // assume directed unless directed=false in edge tag
            }
            else if(key == "label")
            {
                application->g->setEdgeLabel(edgeId, value);
            }
            
            edgeStart = attributeMatches[0].second;
        }
        
        if(!directed) application->g->addEdge(idMap[target], idMap[source]);
        lineStart = lineMatches[0].second;
    }
}