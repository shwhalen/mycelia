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

#include <windows/imagewindow.hpp>

using namespace std;

ImageWindow::ImageWindow(Mycelia* application)
    : Window(application)
{
    window = new GLMotif::PopupWindow("ImageWindowPopup", Vrui::getWidgetManager(), "Plugin Output");
    
    widget = new ImageWidget(window);
    widget->manageChild();
}

ImageWindow::ImageWidget::ImageWidget(GLMotif::Container* parent)
    : GLMotif::Widget("ImageWidget", parent, false)
{
    width = 0;
    height = 0;
    version = 0;
}

GLMotif::Vector ImageWindow::ImageWidget::calcNaturalSize() const
{
    return GLMotif::Vector(width, height, 0);
}

void ImageWindow::ImageWidget::draw(GLContextData& contextData) const
{
    DataItem* dataItem = contextData.retrieveDataItem<DataItem>(this);
    const GLMotif::Vector& size = getExterior().size;
    const GLMotif::Vector& origin = getExterior().origin;
    
    if(dataItem->version != version)
    {
        glGenTextures(1, &dataItem->imageId);
        glBindTexture(GL_TEXTURE_2D, dataItem->imageId);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        image.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        dataItem->version = version;
    }
    
    glBindTexture(GL_TEXTURE_2D, dataItem->imageId);
    glEnable(GL_TEXTURE_2D);
    
    glPushMatrix();
    glTranslatef(origin[0], origin[1], 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(1, 0); glVertex3f(size[0], 0, 0);
    glTexCoord2f(1, 1); glVertex3f(size[0], size[1], 0);
    glTexCoord2f(0, 1); glVertex3f(0, size[1], 0);
    glEnd();
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ImageWindow::ImageWidget::initContext(GLContextData& contextData) const
{
    DataItem* dataItem = new DataItem;
    contextData.addDataItem(this, dataItem);
}

void ImageWindow::ImageWidget::load(string scriptFilename)
{
    string cmdline = string(PYTHON) + " " + scriptFilename;
    int retval = system(cmdline.c_str());
    
    if(retval == 0)
    {
        image = Images::readImageFile("/tmp/output.png");
        width = image.getWidth() / image.getHeight() * Vrui::getDisplaySize();
        height =  Vrui::getDisplaySize();
        version++;
        
        getParent()->requestResize(this, calcNaturalSize());
    }
}