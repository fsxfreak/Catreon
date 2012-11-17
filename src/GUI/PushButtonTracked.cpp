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
PushButtonTracked::PushButtonTracked(const CEGUI::Window &window, HandleFunc func)
    : mFunc(nullptr), CEGUI::PushButton(CEGUI::String("CEGUI/PushButton"), window.getName())
{
}
//-------------------------------------------------------------------------------------------------------
void PushButtonTracked::replaceFunctor(HandleFunc binded)
{
    mFunc = new HandleFunc(binded);
}
//-------------------------------------------------------------------------------------------------------
void PushButtonTracked::deliverAction(const CEGUI::EventArgs &mouseEvent)
{
    (*mFunc)(mouseEvent);
}
