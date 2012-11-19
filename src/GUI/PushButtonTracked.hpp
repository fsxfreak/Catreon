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
#include <CEGUIOgreRenderer.h>
#include <CEGUI.h>

#include <GameState.hpp>
#include <framework\PauseState.hpp>
#include <framework\MenuState.hpp>
#include <framework\AppState.hpp>

enum States
{
    MENUSTATE,
    PAUSESTATE,
    GAMESTATE
};

class PushButtonTracked : public CEGUI::PushButton
{
private:
    AppState *mState;
    States mStateId;
    CEGUI::Window *mWindow;
public:
    //only allow functions that receive a mouseEvent to refer to this pushbutton
    PushButtonTracked(const CEGUI::String& name);
    PushButtonTracked(const CEGUI::String &name, States stateId, AppState *state);

    void deliverAction(const CEGUI::EventArgs &mouseEvent);
    CEGUI::Window* getWindow();
};

#endif 