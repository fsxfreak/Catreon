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

class GUIEventSubscriber
{
private:
    GUIEventSubscriber() {};
    GUIEventSubscriber(const GUIEventSubscriber&) {};
    GUIEventSubscriber& operator=(const GUIEventSubscriber&) {};

    static GUIEventSubscriber* mInstance;
    void update();
public:
    static GUIEventSubscriber* get();
    void subscribe(const Ogre::String& buttonName, void *subscriberClass);

};

#endif