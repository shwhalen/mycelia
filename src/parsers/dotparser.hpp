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

#ifndef __DOTPARSER_HPP
#define __DOTPARSER_HPP

#include <graph.hpp>
#include <mycelia.hpp>
#include <vruihelp.hpp>

class DotParser
{
private:
    Mycelia* application;
    std::map<std::string, int> nodeMap;
    
public:
    DotParser(Mycelia*);
    
    void parse(std::string&);
};

#endif
