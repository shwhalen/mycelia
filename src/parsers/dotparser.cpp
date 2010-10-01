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

#include <parsers/dotparser.hpp>

using namespace std;
using namespace boost;

DotParser::DotParser(Mycelia* application)
    : application(application)
{
}

void DotParser::parse(string& filename)
{
    const basic_regex<char> edgeRegex("^\\s*([\\w\\d]+)\\s*-[-|>]\\s*([\\w\\d]+)[^-{};]*;");
    const basic_regex<char> nodeRegex("^\\s*([\\w\\d]+)[^-{};]*;");
    const basic_regex<char> positionRegex("pos=\"([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)\"");
    const basic_regex<char> labelRegex("label=\"([^\"]+)\"");
    
    string fileBuffer = VruiHelp::fileToString(filename);
    string::const_iterator lineStart;
    string::const_iterator lineEnd;
    boost::smatch lineMatches;
    boost::smatch positionMatches;
    boost::smatch labelMatches;
    nodeMap.clear();
    
    // nodes
    lineStart = fileBuffer.begin();
    lineEnd = fileBuffer.end();
    
    while(regex_search(lineStart, lineEnd, lineMatches, nodeRegex))
    {
        string nodeId(lineMatches[1].first,  lineMatches[1].second);
        nodeMap[nodeId] = application->g->addNode();
        
        string::const_iterator posStart = lineMatches[1].second; // end of node name
        string::const_iterator posEnd = lineMatches[0].second;; // end of line
        
        if(regex_search(posStart, posEnd, positionMatches, positionRegex))
        {
            string sx(positionMatches[1].first, positionMatches[1].second);
            string sy(positionMatches[2].first, positionMatches[2].second);
            string sz(positionMatches[3].first, positionMatches[3].second);
            
            float x = VruiHelp::stringToFloat(sx);
            float y = VruiHelp::stringToFloat(sy);
            float z = VruiHelp::stringToFloat(sz);
            
            application->g->setPosition(nodeMap[nodeId], Vrui::Point(x, y, z));
            application->setSkipLayout(true);
        }
        
        string::const_iterator labelStart = lineMatches[1].second; // end of node name
        string::const_iterator labelEnd = lineMatches[0].second;; // end of line
        
        if(regex_search(labelStart, labelEnd, labelMatches, labelRegex))
        {
            string label(labelMatches[1].first, labelMatches[1].second);
            application->g->setNodeLabel(nodeMap[nodeId], label);
        }
        
        lineStart = lineMatches[0].second;
    }
    
    // edges
    lineStart = fileBuffer.begin();
    lineEnd = fileBuffer.end();
    
    while(regex_search(lineStart, lineEnd, lineMatches, edgeRegex))
    {
        string source(lineMatches[1].first, lineMatches[1].second);
        string target(lineMatches[2].first, lineMatches[2].second);
        
        if(nodeMap.find(source) == nodeMap.end())
        {
            nodeMap[source] = application->g->addNode(source);
        }
        
        if(nodeMap.find(target) == nodeMap.end())
        {
            nodeMap[target] = application->g->addNode(target);
        }
        
        int edgeId = application->g->addEdge(nodeMap[source], nodeMap[target]);
        string::const_iterator labelStart = lineMatches[1].second; // end of transition
        string::const_iterator labelEnd = lineMatches[0].second;; // end of line
        
        if(regex_search(labelStart, labelEnd, labelMatches, labelRegex))
        {
            string label(labelMatches[1].first, labelMatches[1].second);
            application->g->setEdgeLabel(edgeId, label);
        }
        
        lineStart = lineMatches[0].second;
    }
}