/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef GUI_EVENT_SUBSCRIBER_HPP
#define GUI_EVENT_SUBSCRIBER_HPP

#include "stdafx.h"
#include <framework\AdvancedOgreFramework.hpp>
#include <framework\AppState.hpp>
#include <CEGUIOgreRenderer.h>
#include <CEGUI.h>

#include <GUI/PushButtonTracked.hpp>

class PushButtonTracked;

enum ButtonTypes
{
    PUSH_BUTTON,
    RADIO_BUTTON,
    CHECKBOX
};

class GUIEventSubscriber
{
private:
    GUIEventSubscriber() {};
    GUIEventSubscriber(const GUIEventSubscriber&) {};
    GUIEventSubscriber& operator=(const GUIEventSubscriber&) {};

    static GUIEventSubscriber* mInstance;

    std::vector<CEGUI::Window*> mButtons;
    
    bool onPushButtonClicked(const CEGUI::EventArgs &mouseEvent);
    bool onMouseEnter(const CEGUI::EventArgs &mouseEvent);
    bool onMouseLeave(const CEGUI::EventArgs &mouseEvent);
public:
    static GUIEventSubscriber* get();
    void subscribe(const CEGUI::String& buttonName, ButtonTypes buttonType, States stateId, AppState *state);
    void unsubscribeAll();
    //need the class containing the function to call the member function pointer

};

#endif