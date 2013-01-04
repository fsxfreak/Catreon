/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DEBUG_WINDOW_HPP
#define DEBUG_WINDOW_HPP

#include "stdafx.h"
#include <memory>

#include <CEGUIOgreRenderer.h>
#include <CEGUI.h>

#include <objects\vehicles\Vehicle.h>
#include <GameState.hpp>

class DebugWindow
{
private:
    DebugWindow();
    ~DebugWindow() {};
    DebugWindow(const DebugWindow&) {};
    DebugWindow& operator=(const DebugWindow&) {};

    static DebugWindow *mInstance;

    Vehicle *mVehicle;

    /* to group these windows in a more organized fashion in the memory
       The order of the windows is as follows:
       Game/FPSText, Game/VehicleName, Game/VehicleSpeed, Game/VehiclePosition,
       Game/VehicleDirection */
    std::vector<CEGUI::Window*> mWindows;

    float lastTimeSinceLastFrame;
public:
    static DebugWindow* get();

    void debugVehicle(Vehicle *vehicle);
    void debugVehicle(const Ogre::String& name);

    Vehicle* getVehicle();

    void update();
    void reset();
};

#endif