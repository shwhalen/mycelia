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

#ifndef __WATTSGENERATOR_HPP
#define __WATTSGENERATOR_HPP

#include <graph.hpp>
#include <mycelia.hpp>
#include <vruihelp.hpp>
#include <generators/graphgenerator.hpp>

#define INITIAL_N 20
#define INITIAL_BETA 0.2

class WattsGenerator : public GraphGenerator
{
    class WattsWindow : public Window
    {
    private:
        GLMotif::Slider* nodeCountSlider;
        GLMotif::Slider* betaSlider;
        
        GLMotif::TextField* nodeCountField;
        GLMotif::TextField* betaField;
        
        WattsGenerator* generator;
        
    public:
        WattsWindow(Mycelia* application, WattsGenerator* generator) : Window(application), generator(generator)
        {
            window = new GLMotif::PopupWindow("WattsWindow", Vrui::getWidgetManager(), "Watts-Strogatz Graph Parameters");
            
            GLMotif::RowColumn* dialog = new GLMotif::RowColumn("WattsDialog", window, false);
            dialog->setNumMinorWidgets(3);
            
            ParamPair p = VruiHelp::createParameter("Nodes", 10, 50, INITIAL_N, dialog);
            nodeCountField = p.first;
            nodeCountSlider = p.second;
            nodeCountSlider->getValueChangedCallbacks().add(this, &WattsGenerator::WattsWindow::sliderCallback);
            
            p = VruiHelp::createParameter("Replacement Probability", 0.0, 1.0, INITIAL_BETA, dialog);
            betaField = p.first;
            betaSlider = p.second;
            betaSlider->getValueChangedCallbacks().add(this, &WattsGenerator::WattsWindow::sliderCallback);
            
            dialog->manageChild();
        }
        
        void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
        {
            int nodeCount = nodeCountSlider->getValue();
            float beta = betaSlider->getValue();
            application->stopLayout();
            
            if(cbData->slider == betaSlider)
            {
                betaField->setValue(beta);
                generator->generateEdges(nodeCount, beta);
            }
            else if(cbData->slider == nodeCountSlider)
            {
                nodeCountField->setValue(nodeCount);
                generator->generateNodes(nodeCount);
                generator->generateEdges(nodeCount, beta);
            }
            
            application->resumeLayout();
        }
    };
    
public:
    WattsGenerator(Mycelia* application);
    
    void generate() const;
    void generateNodes(int) const;
    void generateEdges(int, float) const;
};

#endif