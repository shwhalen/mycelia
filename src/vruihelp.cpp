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

#include <vruihelp.hpp>

using namespace std;

namespace VruiHelp
{
Vrui::Scalar angle(const Vrui::Vector& u, const Vrui::Vector& v)
{
    return acos((u * v) / (Geometry::mag(u) * Geometry::mag(v)));
}

Vrui::Scalar degrees(Vrui::Scalar radians)
{
    return radians * 180.0 / M_PI;
}

Vrui::Point midpoint(const Vrui::Point& p, const Vrui::Vector& v)
{
    return p + v * 0.5;
}

Vrui::Point midpoint(const Vrui::Point& p, const Vrui::Point& q)
{
    return midpoint(p, q - p);
}

Vrui::Vector rk4(const Vrui::Vector& p, const Vrui::Vector& v, double dt)
{
    Vrui::Vector k1 = dt * v;
    Vrui::Vector k2 = dt * (p + 0.5 * k1);
    Vrui::Vector k3 = dt * (p + 0.5 * k2);
    Vrui::Vector k4 = dt * (p + k3);
    return p + (k1 + Vrui::Scalar(2) * k2 + Vrui::Scalar(2) * k3 + k4) / Vrui::Scalar(6);
}

bool contains(string& s, string& pattern)
{
    return s.find(pattern) != string::npos;
}

bool endsWith(string& s, const char* extension)
{
    int index = s.find_last_of('.');
    
    if(index == (int)string::npos) return false;
    else return s.substr(index) == string(extension);
}

bool startsWith(string& s, string& prefix)
{
    return s.substr(0, prefix.size()).compare(prefix) == 0;
}

string intToString(int i)
{
    stringstream stream;
    stream << i;
    return stream.str();
}

string longToString(long l)
{
    stringstream stream;
    stream << l;
    return stream.str();
}

float stringToFloat(string& s)
{
    istringstream stream(s);
    float f;
    stream >> f;
    return f;
}

int stringToInt(string& s)
{
    istringstream stream(s);
    int i;
    stream >> i;
    return i;
}

string fileToString(string& filename)
{
    ifstream fstream(filename.c_str());
    stringstream stream;
    
    stream << fstream.rdbuf();
    fstream.close();
    
    return stream.str();
}

float randomFloat()
{
    return rand() / (float(RAND_MAX) + 1);
}

void show(GLMotif::Widget* w)
{
    Vrui::popupPrimaryWidget(w);
}

void show(GLMotif::Widget* w, const GLMotif::Widget* x)
{
    Vrui::getWidgetManager()->popupPrimaryWidget(w, Vrui::getWidgetManager()->calcWidgetTransformation(x));
}

void hide(GLMotif::Widget* w)
{
    Vrui::popdownPrimaryWidget(w);
}

ParamPair createParameter(const char* label, float min, float max, float initial, GLMotif::Container* parent)
{
    new GLMotif::Label("", parent, label);
    GLMotif::TextField* f = new GLMotif::TextField("", parent, 5);
    f->setValue(initial);
    
    GLMotif::Slider* s = new GLMotif::Slider("", parent, GLMotif::Slider::HORIZONTAL,
            Vrui::getWidgetManager()->getStyleSheet()->fontHeight * 5.0);
    s->setValueRange(min, max, abs(max - min) / 100);
    s->setValue(initial);
    
    return ParamPair(f, s);
}
}