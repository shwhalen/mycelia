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

#ifndef __VRUIHELP_HPP
#define __VRUIHELP_HPP

#include <mycelia.hpp>

typedef std::pair<GLMotif::TextField*, GLMotif::Slider*> ParamPair;

namespace VruiHelp
{
Vrui::Scalar angle(const Vrui::Vector&, const Vrui::Vector&);
Vrui::Scalar degrees(Vrui::Scalar);
Vrui::Point midpoint(const Vrui::Point&, const Vrui::Vector&);
Vrui::Point midpoint(const Vrui::Point&, const Vrui::Point&);
Vrui::Vector rk4(const Vrui::Vector&, const Vrui::Vector&, double);
bool contains(std::string&, std::string&);
bool endsWith(std::string&, const char*);
bool startsWith(std::string&, std::string&);
std::string intToString(int);
std::string longToString(long);
float stringToFloat(std::string&);
int stringToInt(std::string&);
std::string fileToString(std::string&);
float randomFloat();
void show(GLMotif::Widget*);
void show(GLMotif::Widget*, const GLMotif::Widget*);
void hide(GLMotif::Widget*);
ParamPair createParameter(const char*, float, float, float, GLMotif::Container*);
}

#endif