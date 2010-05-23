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

#include <layout/arfwindow.hpp>

ArfWindow::ArfWindow(Mycelia* application)
    : Window(application), layout(application->getDynamicLayout())
{
    window = new GLMotif::PopupWindow("AttributeWindow", Vrui::getWidgetManager(), "Layout Parameters");
    
    GLMotif::RowColumn* dialog = new GLMotif::RowColumn("ArfDialog", window, false);
    dialog->setNumMinorWidgets(3);
    
    // damping
    ParamPair p = VruiHelp::createParameter("Damping", -10, -1, layout->dampingConstant, dialog);
    dampingField = p.first;
    dampingSlider = p.second;
    dampingSlider->getValueChangedCallbacks().add(this, &ArfWindow::sliderCallback);
    
    // step size
    p = VruiHelp::createParameter("Step Size", 0.001, 0.05, layout->deltaTime, dialog);
    stepsizeField = p.first;
    stepsizeSlider = p.second;
    stepsizeSlider->getValueChangedCallbacks().add(this, &ArfWindow::sliderCallback);
    
    // beta
    p = VruiHelp::createParameter("Repulsion Falloff", -0.6, -0.1, layout->beta, dialog);
    betaField = p.first;
    betaSlider = p.second;
    betaSlider->getValueChangedCallbacks().add(this, &ArfWindow::sliderCallback);
    
    // strongly connected constant slider
    p = VruiHelp::createParameter("Strongly Connected Const", -5, -1, layout->stronglyConnectedSpringConstant, dialog);
    stronglyConnectedConstantField = p.first;
    stronglyConnectedConstantSlider = p.second;
    stronglyConnectedConstantSlider->getValueChangedCallbacks().add(this, &ArfWindow::sliderCallback);
    
    // connected constant slider
    p = VruiHelp::createParameter("Connected Const", -5, -1, layout->connectedSpringConstant, dialog);
    connectedConstantField = p.first;
    connectedConstantSlider = p.second;
    connectedConstantSlider->getValueChangedCallbacks().add(this, &ArfWindow::sliderCallback);
    
    // unconnected constant slider
    p = VruiHelp::createParameter("Unconnected Const", -5, -1, layout->connectedSpringConstant, dialog);
    unconnectedConstantField = p.first;
    unconnectedConstantSlider = p.second;
    unconnectedConstantSlider->getValueChangedCallbacks().add(this, &ArfWindow::sliderCallback);
    
    GLMotif::Button* navButton = new GLMotif::Button("NavButton", dialog, "Center Graph");
    navButton->getSelectCallbacks().add(application, &Mycelia::resetNavigationCallback);
    
    dialog->manageChild();
}

void ArfWindow::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
    float f = cbData->slider->getValue();
    
    if(cbData->slider == dampingSlider)
    {
        layout->dampingConstant = f;
        dampingField->setValue(f);
    }
    else if(cbData->slider == stepsizeSlider)
    {
        layout->deltaTime = f;
        stepsizeField->setValue(f);
    }
    else if(cbData->slider == betaSlider)
    {
        layout->beta = f;
        betaField->setValue(1.0 + f);
    }
    else if(cbData->slider == stronglyConnectedConstantSlider)
    {
        layout->stronglyConnectedSpringConstant = f;
        stronglyConnectedConstantField->setValue(f);
    }
    else if(cbData->slider == connectedConstantSlider)
    {
        layout->connectedSpringConstant = f;
        connectedConstantField->setValue(f);
    }
    else if(cbData->slider == unconnectedConstantSlider)
    {
        layout->unconnectedSpringConstant = f;
        unconnectedConstantField->setValue(f);
    }
}