/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Create a singleton for this class
-Subscribe, handle, and parse events
********************************************************/

#include "stdafx.h"
#include <GUI/GUIEventSubscriber.hpp>

GUIEventSubscriber* GUIEventSubscriber::mInstance = nullptr;
//-------------------------------------------------------------------------------------------------------
GUIEventSubscriber* GUIEventSubscriber::get()
{
    if (!mInstance)
        mInstance = new GUIEventSubscriber();
    //singleton pattern done right
    return mInstance;
}
//-------------------------------------------------------------------------------------------------------