/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
********************************************************/

#include "stdafx.h"

#include <GUI\DebugWindow.hpp>
#include <string>

DebugWindow* DebugWindow::mInstance = nullptr;
//-------------------------------------------------------------------------------------------------------
DebugWindow::DebugWindow() : lastTimeSinceLastFrame(0)
{
    mVehicle = nullptr;
    CEGUI::Window* rootWindow = CEGUI::WindowManager::getSingletonPtr()->getWindow("Game/DebugWindow");
    
    int numChildren = rootWindow->getChildCount();
    for (int iii = 0; iii < numChildren; ++iii)
    {
        mWindows.push_back(rootWindow->getChildAtIdx(iii));
        mWindows[iii]->setFont("DejaVuSans-6");
    }
}
//-------------------------------------------------------------------------------------------------------
DebugWindow* DebugWindow::get()
{
    if (!mInstance)
    {
        mInstance = new DebugWindow();
    }
    //singleton pattern done right
    return mInstance;
}
//-------------------------------------------------------------------------------------------------------
void DebugWindow::debugVehicle(Vehicle *vehicle)
{
    mVehicle = vehicle;
}
//-------------------------------------------------------------------------------------------------------
void DebugWindow::debugVehicle(const Ogre::String& name)
{
    //need to extract the vehicle pointer from the name
    auto drivers = GameState::getSingleton()->mDrivers;
    auto it = drivers.begin();
    auto itend = drivers.end();
    for (it; it != itend; ++it)
    {
        if ((*it)->getVehicle()->mstrName == name)
        {
            mVehicle = (*it)->getVehicle();
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------------
Vehicle* DebugWindow::getVehicle()
{
    return mVehicle;
}
//-------------------------------------------------------------------------------------------------------
void DebugWindow::update()
{
    //update the fps (included here because this is DebugWindow,
    //and is easier due to Window structure in the .layout
    float time = OgreFramework::getSingletonPtr()->mTimeSinceLastFrame * 0.9 + lastTimeSinceLastFrame * 0.1;
    lastTimeSinceLastFrame = time;
    int fps = (1 / time) * 1000;
    mWindows[0]->setText("FPS: " + Ogre::StringConverter::toString(fps));
                              
    if (mVehicle)
    {
        mWindows[1]->setText("Name: " + mVehicle->mstrName);
        mWindows[2]->setText("Speed: " + Ogre::StringConverter::toString(GameState::round(mVehicle->mfSpeed, 2)));
        Ogre::Vector3 pos = mVehicle->mNode->getPosition();
        mWindows[3]->setText("Position: " + Ogre::StringConverter::toString(static_cast<int>(pos.x)) 
                                    + " " + Ogre::StringConverter::toString(static_cast<int>(pos.y)) 
                                    + " " + Ogre::StringConverter::toString(static_cast<int>(pos.z)));
        Ogre::Vector3 dir = mVehicle->getDirection();
        mWindows[4]->setText("Direction: " + Ogre::StringConverter::toString(GameState::round(dir.x, 2))
                                     + " " + Ogre::StringConverter::toString(GameState::round(dir.y, 2))
                                     + " " + Ogre::StringConverter::toString(GameState::round(dir.z, 2)));
        mWindows[5]->setText("Road: " + mVehicle->mOccupiedRoadName);
    }
}
//-------------------------------------------------------------------------------------------------------
void DebugWindow::reset()
{
    std::vector<CEGUI::Window*>::iterator it = mWindows.begin();
    auto itend = mWindows.end();
    for (it; it != itend; ++it)
    {
        CEGUI::WindowManager::getSingletonPtr()->destroyWindow((*it));
    }
    //delete mVehicle;  ---  Driver handles this
    mVehicle = nullptr;
    mWindows.clear();
    delete mInstance;
    mInstance = nullptr;
}