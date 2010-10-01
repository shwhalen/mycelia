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

#ifndef __IMAGEWINDOW_HPP
#define __IMAGEWINDOW_HPP

#include <mycelia.hpp>
#include <vruihelp.hpp>
#include <windows/window.hpp>

class ImageWindow : public Window
{
private:
    class ImageWidget : public GLMotif::Widget, public GLObject
    {
        struct DataItem : public GLObject::DataItem
        {
            GLuint imageId;
            int version;
            
            DataItem()
            {
                glGenTextures(1, &imageId);
                version = 0;
            }
            
            ~DataItem()
            {
                glDeleteTextures(1, &imageId);
            }
        };
        
    private:
        Images::RGBImage image;
        float width;
        float height;
        int version;
        
    public:
        ImageWidget(GLMotif::Container*);
        
        virtual GLMotif::Vector calcNaturalSize() const;
        void draw(GLContextData& contextData) const;
        void initContext(GLContextData& contextData) const;
        void load(std::string);
    };
    
    ImageWidget* widget;
    
public:
    ImageWindow(Mycelia*);
    
    void load(std::string scriptFilename) { widget->load(scriptFilename); }
};

#endif