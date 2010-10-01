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

#ifndef __XMLPARSER_HPP
#define __XMLPARSER_HPP

#include <graph.hpp>
#include <mycelia.hpp>

class XmlParser
{
private:
    Mycelia* application;
    
    // stores a value/color mapping for nodes containing a colorAttributeName
    std::map<std::string, std::vector<int> > colorMap;
    
    // maps optional node id from xml to internal node id
    std::map<int, int> idMap;
    
public:
    XmlParser(Mycelia*);
    
    void parse(std::string&);
};

#endif
