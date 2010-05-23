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

#ifndef __BARABASI_HPP
#define __BARABASI_HPP

#include <graph.hpp>
#include <mycelia.hpp>
#include <vruihelp.hpp>
#include <generators/graphgenerator.hpp>

#define INITIAL_M0 5
#define INITIAL_N 20

class BarabasiGenerator : public GraphGenerator
{
    class BarabasiWindow : public Window
    {
    private:
        GLMotif::Slider* initialNodesSlider;
        GLMotif::Slider* maximumNodesSlider;
        
        GLMotif::TextField* initialNodesField;
        GLMotif::TextField* maximumNodesField;
        
        BarabasiGenerator* generator;
        
    public:
        BarabasiWindow(Mycelia* application, BarabasiGenerator* generator) : Window(application), generator(generator)
        {
            window = new GLMotif::PopupWindow("BarabasiWindow", Vrui::getWidgetManager(), "Barabasi-Albert Graph Parameters");
            
            GLMotif::RowColumn* dialog = new GLMotif::RowColumn("BarabasiDialog", window, false);
            dialog->setNumMinorWidgets(3);
            
            ParamPair p = VruiHelp::createParameter("Initial Nodes", 1, 20, INITIAL_M0, dialog);
            initialNodesField = p.first;
            initialNodesSlider = p.second;
            initialNodesSlider->getValueChangedCallbacks().add(this, &BarabasiGenerator::BarabasiWindow::sliderCallback);
            
            p = VruiHelp::createParameter("Maximum Nodes", 20, 50, INITIAL_N, dialog);
            maximumNodesField = p.first;
            maximumNodesSlider = p.second;
            maximumNodesSlider->getValueChangedCallbacks().add(this, &BarabasiGenerator::BarabasiWindow::sliderCallback);
            
            dialog->manageChild();
        }
        
        void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
        {
            int initialNodeCount = initialNodesSlider->getValue();
            int maximumNodeCount = maximumNodesSlider->getValue();
            application->stopLayout();
            
            initialNodesField->setValue(initialNodeCount);
            maximumNodesField->setValue(maximumNodeCount);
            generator->generateNodes(maximumNodeCount);
            generator->generateEdges(initialNodeCount, maximumNodeCount);
            
            application->resumeLayout();
        }
    };
    
public:
    BarabasiGenerator(Mycelia*);
    
    void hide() const;
    void generate() const;
    void generateNodes(int) const;
    void generateEdges(int, int) const;
};

#endif