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

#ifndef __ARFWINDOW_HPP
#define __ARFWINDOW_HPP

#include <mycelia.hpp>
#include <layout/arflayout.hpp>
#include <windows/window.hpp>

class ArfLayout;

class ArfWindow : public Window
{
private:
    ArfLayout* layout;
    
    GLMotif::Slider* dampingSlider;
    GLMotif::Slider* stepsizeSlider;
    GLMotif::Slider* betaSlider;
    GLMotif::Slider* stronglyConnectedConstantSlider;
    GLMotif::Slider* connectedConstantSlider;
    GLMotif::Slider* unconnectedConstantSlider;
    GLMotif::Slider* stronglyConnectedLengthSlider;
    GLMotif::Slider* connectedLengthSlider;
    GLMotif::Slider* unconnectedLengthSlider;
    
    GLMotif::TextField* dampingField;
    GLMotif::TextField* stepsizeField;
    GLMotif::TextField* betaField;
    GLMotif::TextField* stronglyConnectedConstantField;
    GLMotif::TextField* connectedConstantField;
    GLMotif::TextField* unconnectedConstantField;
    GLMotif::TextField* stronglyConnectedLengthField;
    GLMotif::TextField* connectedLengthField;
    GLMotif::TextField* unconnectedLengthField;
    
public:
    ArfWindow(Mycelia*);
    
    void sliderCallback(GLMotif::Slider::ValueChangedCallbackData*);
};

#endif