/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
- Maybe include KeyState information, in the future?
pushButtonMine->setState(KeyStates::Clicked);
********************************************************/

#include "stdafx.h"
#include <GUI/PushButtonTracked.hpp>

//-------------------------------------------------------------------------------------------------------
PushButtonTracked::PushButtonTracked(const CEGUI::String& name)
    : CEGUI::PushButton(CEGUI::String("CEGUI/PushButton"), name)
{

}
//-------------------------------------------------------------------------------------------------------
PushButtonTracked::PushButtonTracked(const CEGUI::String &name, States stateId, AppState *state)
    : CEGUI::PushButton(CEGUI::String("CEGUI/PushButton"), name)
    , mStateId(stateId), mState(state)
{
    mWindow = CEGUI::WindowManager::getSingletonPtr()->getWindow(name);
}
//-------------------------------------------------------------------------------------------------------
void PushButtonTracked::deliverClicked(const CEGUI::EventArgs &mouseEvent)
{
    switch (mStateId)
    {
    case MENUSTATE:
        static_cast<MenuState*>(mState)->buttonHit(mouseEvent);
        break;
    case PAUSESTATE:
        static_cast<PauseState*>(mState)->buttonHit(mouseEvent);
        break;
    case GAMESTATE:
        static_cast<GameState*>(mState);
        break;
    default:
        OgreFramework::getSingletonPtr()->mLog->logMessage("Tried to use PushButtonTracked without assigning a valid state");
        break;
    }
}
//-------------------------------------------------------------------------------------------------------
void PushButtonTracked::deliverHovered(const CEGUI::EventArgs &mouseEvent)
{
    switch (mStateId)
    {
    case MENUSTATE:
        static_cast<MenuState*>(mState)->buttonHovered(mouseEvent);
        break;
    case PAUSESTATE:
        static_cast<PauseState*>(mState)->buttonHovered(mouseEvent);
        break;
    case GAMESTATE:
        static_cast<GameState*>(mState);
        break;
    default:
        OgreFramework::getSingletonPtr()->mLog->logMessage("Tried to use PushButtonTracked without assigning a valid state");
        break;
    }
}
//-------------------------------------------------------------------------------------------------------
void PushButtonTracked::deliverExited(const CEGUI::EventArgs &mouseEvent)
{
    switch (mStateId)
    {
    case MENUSTATE:
        static_cast<MenuState*>(mState);
        break;
    case PAUSESTATE:
        static_cast<PauseState*>(mState);
        break;
    case GAMESTATE:
        static_cast<GameState*>(mState);
        break;
    default:
        OgreFramework::getSingletonPtr()->mLog->logMessage("Tried to use PushButtonTracked without assigning a valid state");
        break;
    }
}
//-------------------------------------------------------------------------------------------------------
CEGUI::Window* PushButtonTracked::getWindow()
{
    return mWindow;
}