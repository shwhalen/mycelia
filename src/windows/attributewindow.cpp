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

#include <windows/attributewindow.hpp>

using namespace std;

AttributeWindow::AttributeWindow(Mycelia* application, const char* title, int size)
    : Window(application)
{
    window = new GLMotif::PopupWindow("AttributeWindow", Vrui::getWidgetManager(), title);
    
    GLMotif::RowColumn* dialog = new GLMotif::RowColumn("AttributeDialog", window, false);
    dialog->setNumMinorWidgets(2);
    
    labelVector.resize(size);
    fieldVector.resize(size);
    
    for(int i = 0; i < size; i++)
    {
        labelVector[i] = new GLMotif::Label("Label", dialog, "");
        fieldVector[i] = new GLMotif::TextField("Value", dialog, 30);
    }
    
    dialog->manageChild();
}

void AttributeWindow::clear() const
{
    for(int i = 0; i < (int)labelVector.size(); i++)
    {
        labelVector[i]->setString("");
        fieldVector[i]->setString("");
    }
    
    Vrui::requestUpdate();
}

void AttributeWindow::update(const char* label, const char* value) const
{
    Attributes a;
    a.push_back(pair<string, string>(label, value));
    update(a);
}

void AttributeWindow::update(const Attributes& attributes) const
{
    clear();
    int i = 0;
    
    for(Attributes::const_iterator entry = attributes.begin(); entry != attributes.end(); entry++)
    {
        if(i > (int)labelVector.size()) return;
        
        labelVector[i]->setString(entry->first.c_str());
        fieldVector[i]->setString(entry->second.c_str());
        i++;
    }
    
    Vrui::requestUpdate();
}