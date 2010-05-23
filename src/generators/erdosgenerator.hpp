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

#ifndef __ERDOS_HPP
#define __ERDOS_HPP

#include <graph.hpp>
#include <mycelia.hpp>
#include <vruihelp.hpp>
#include <generators/graphgenerator.hpp>

#define INITIAL_P 0.05
#define INITIAL_N 20

class ErdosGenerator : public GraphGenerator
{
    class ErdosWindow : public Window
    {
    private:
        GLMotif::Slider* probabilitySlider;
        GLMotif::Slider* nodeCountSlider;
        
        GLMotif::TextField* probabilityField;
        GLMotif::TextField* nodeCountField;
        
        ErdosGenerator* generator;
        
    public:
        ErdosWindow(Mycelia* application, ErdosGenerator* generator) : Window(application), generator(generator)
        {
            window = new GLMotif::PopupWindow("ErdosWindow", Vrui::getWidgetManager(), "Erdos-Renyi Graph Parameters");
            
            GLMotif::RowColumn* dialog = new GLMotif::RowColumn("ErdosDialog", window, false);
            dialog->setNumMinorWidgets(3);
            
            ParamPair p = VruiHelp::createParameter("Edge Probability", 0, 1, INITIAL_P, dialog);
            probabilityField = p.first;
            probabilitySlider = p.second;
            probabilitySlider->getValueChangedCallbacks().add(this, &ErdosGenerator::ErdosWindow::sliderCallback);
            
            p = VruiHelp::createParameter("Nodes", 0, 50, INITIAL_N, dialog);
            nodeCountField = p.first;
            nodeCountSlider = p.second;
            nodeCountSlider->getValueChangedCallbacks().add(this, &ErdosGenerator::ErdosWindow::sliderCallback);
            
            dialog->manageChild();
        }
        
        void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
        {
            float p = probabilitySlider->getValue();
            int nodeCount = nodeCountSlider->getValue();
            application->stopLayout();
            
            if(cbData->slider == probabilitySlider)
            {
                probabilityField->setValue(p);
                generator->generateEdges(p);
            }
            else if(cbData->slider == nodeCountSlider)
            {
                nodeCountField->setValue(nodeCount);
                generator->generateNodes(nodeCount);
                generator->generateEdges(p);
            }
            
            application->resumeLayout();
        }
    };
    
public:
    ErdosGenerator(Mycelia*);
    
    void generate() const;
    void generateNodes(int) const;
    void generateEdges(float) const;
};

#endif