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

#ifndef __DATAITEM_HPP
#define __DATAITEM_HPP

#include <mycelia.hpp>

class MyceliaDataItem : public GLObject::DataItem
{
public:
    GLMaterial* defaultNodeMaterial;
    GLMaterial* defaultEdgeMaterial;
    GLMaterial* previousMaterial;
    GLMaterial* selectedMaterial;
    GLUquadric* quadric;
    GLuint arrowList;
    GLuint graphList;
    GLuint nodeList;
    int graphListVersion;
    
    MyceliaDataItem()
    {
        defaultNodeMaterial = new GLMaterial(GLMaterial::Color(1.0, 1.0, 1.0));
        defaultEdgeMaterial = new GLMaterial(GLMaterial::Color(0.3, 0.3, 0.3));
        previousMaterial = new GLMaterial(GLMaterial::Color(1.0, 0.5, 1.0));
        selectedMaterial = new GLMaterial(GLMaterial::Color(1.0, 0.0, 1.0));
        quadric = gluNewQuadric();
        arrowList = glGenLists(1);
        graphList = glGenLists(1);
        nodeList = glGenLists(1);
        graphListVersion = 0;
    }
    
    ~MyceliaDataItem()
    {
        delete defaultNodeMaterial;
        delete defaultEdgeMaterial;
        delete previousMaterial;
        delete selectedMaterial;
        gluDeleteQuadric(quadric);
        glDeleteLists(arrowList, 1);
        glDeleteLists(graphList, 1);
        glDeleteLists(nodeList, 1);
    }
};

#endif
