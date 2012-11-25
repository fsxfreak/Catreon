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
        CEGUI::WindowManager::getSingletonPtr()->destroyWindow((*it));
    }
    mButtons.clear();
}
//-------------------------------------------------------------------------------------------------------
void GUIEventSubscriber::subscribe(const CEGUI::String& buttonName, ButtonTypes buttonType, States stateId, AppState *state)
{
    if (buttonType == PUSH_BUTTON)
    {
        PushButtonTracked *pushButton = new PushButtonTracked(buttonName, stateId, state);

        mButtons.push_back(pushButton);

        //CEGUI::WindowManager::getSingleton().getWindow(buttonName)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUIEventSubscriber::onPushButtonClicked, this));
        pushButton->getWindow()->subscribeEvent(CEGUI::PushButton::EventClicked,
            CEGUI::Event::Subscriber(&GUIEventSubscriber::onPushButtonClicked, this));
        pushButton->getWindow()->subscribeEvent(CEGUI::PushButton::EventMouseEnters,
            CEGUI::Event::Subscriber(&GUIEventSubscriber::onMouseEnter, this));
        pushButton->getWindow()->subscribeEvent(CEGUI::PushButton::EventMouseLeaves,
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
CEGUI::Window* GUIEventSubscriber::getButtonFromArgs(const CEGUI::EventArgs &mouseEvent)
{
    const CEGUI::MouseEventArgs& caller = static_cast<const CEGUI::MouseEventArgs&>(mouseEvent);

    std::vector<CEGUI::Window*>::iterator itend = mButtons.end();
    for (std::vector<CEGUI::Window*>::iterator it = mButtons.begin(); it != itend; ++it)
    {
        if ((*it)->getName() == caller.window->getName())
        {
            return *it;
        }
    }
    return nullptr;
}
//-------------------------------------------------------------------------------------------------------
bool GUIEventSubscriber::onPushButtonClicked(const CEGUI::EventArgs &mouseEvent)
{
    CEGUI::Window *button = getButtonFromArgs(mouseEvent);
    if (button != nullptr)
    {
        static_cast<PushButtonTracked*>(getButtonFromArgs(mouseEvent))->deliverClicked(mouseEvent);
    }
    else
        return false;

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool GUIEventSubscriber::onMouseEnter(const CEGUI::EventArgs &mouseEvent)
{
    CEGUI::Window *button = getButtonFromArgs(mouseEvent);
    if (button != nullptr)
        static_cast<PushButtonTracked*>(button)->deliverHovered(mouseEvent);
    else
        return false;

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool GUIEventSubscriber::onMouseLeave(const CEGUI::EventArgs &mouseEvent)
{
    CEGUI::Window *button = getButtonFromArgs(mouseEvent);
    if (button != nullptr)
        static_cast<PushButtonTracked*>(button)->deliverExited(mouseEvent);
    else
        return false;
    return true;
}
