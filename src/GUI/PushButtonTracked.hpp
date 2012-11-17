/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PUSH_BUTTON_TRACKED_HPP
#define PUSH_BUTTON_TRACKED_HPP

#include "stdafx.h"
#include <framework\AdvancedOgreFramework.hpp>
#include <framework\AppState.hpp>
#include <CEGUIOgreRenderer.h>
#include <CEGUI.h>

//function callback to needed function
typedef std::tr1::function<void (const CEGUI::EventArgs&)> HandleFunc;

class PushButtonTracked : public CEGUI::PushButton
{
private:
    HandleFunc mFunc;
public:
    void replaceFunctor(HandleFunc func);
    //only allow functions that receive a mouseEvent to refer to this pushbutton

    PushButtonTracked(const CEGUI::String& name);
    PushButtonTracked(const CEGUI::Window &window, HandleFunc func);

    void deliverAction(const CEGUI::EventArgs &mouseEvent);
};

#endif 