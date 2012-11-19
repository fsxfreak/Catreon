/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Subscribe, handle, and parse events
-Identify what functions outside of this class need
information on button pushed
********************************************************/

#include "stdafx.h"
#include <GUI/GUIEventSubscriber.hpp>
#include <memory>

GUIEventSubscriber* GUIEventSubscriber::mInstance = nullptr;
//-------------------------------------------------------------------------------------------------------
GUIEventSubscriber* GUIEventSubscriber::get()
{
    if (!mInstance)
    {
        mInstance = new GUIEventSubscriber();
    }
    //singleton pattern done right
    return mInstance;
}
//-------------------------------------------------------------------------------------------------------
void GUIEventSubscriber::unsubscribeAll()
{
    std::vector<CEGUI::Window*>::iterator itend = mButtons.end();
    for (std::vector<CEGUI::Window*>::iterator it = mButtons.begin(); it != itend; ++it)
    {
        CEGUI::WindowManager::getSingleton().destroyWindow((*it));
    }
}
//-------------------------------------------------------------------------------------------------------
void GUIEventSubscriber::subscribe(const Ogre::String& buttonName, ButtonTypes buttonType, HandleFunc func)
{
    if (buttonType == PUSH_BUTTON)
    {
        PushButtonTracked *pushButton = static_cast<PushButtonTracked*>(CEGUI::WindowManager::getSingleton().
            getWindow(buttonName));
        pushButton->replaceFunctor(func);

        mButtons.push_back(pushButton);

        pushButton->subscribeEvent(CEGUI::PushButton::EventClicked,
            CEGUI::Event::Subscriber(&GUIEventSubscriber::onPushButtonClicked, this));
        pushButton->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
            CEGUI::Event::Subscriber(&GUIEventSubscriber::onMouseEnter, this));
        pushButton->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
            CEGUI::Event::Subscriber(&GUIEventSubscriber::onMouseLeave, this));
    }
    else if (buttonType == RADIO_BUTTON)
    {
        // implementation not needed at this moment
    }
    else if (buttonType == CHECKBOX)
    {
        // implementation not needed at this moment
    }
}
//-------------------------------------------------------------------------------------------------------
bool GUIEventSubscriber::onPushButtonClicked(const CEGUI::EventArgs &mouseEvent)
{
    const CEGUI::MouseEventArgs& caller = static_cast<const CEGUI::MouseEventArgs&>(mouseEvent);
    
    CEGUI::Window *button = nullptr;

    std::vector<CEGUI::Window*>::iterator itend = mButtons.end();
    for (std::vector<CEGUI::Window*>::iterator it = mButtons.begin(); it != itend; ++it)
    {
        if ((*it)->getName() == caller.window->getName())
        {
            button = *it;
            static_cast<PushButtonTracked*>(button)->deliverAction(mouseEvent);
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool GUIEventSubscriber::onMouseEnter(const CEGUI::EventArgs &mouseEvent)
{
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool GUIEventSubscriber::onMouseLeave(const CEGUI::EventArgs &mouseEvent)
{
    return true;
}
