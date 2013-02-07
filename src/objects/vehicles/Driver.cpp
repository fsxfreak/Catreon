/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Take inputs from various sources for a neural network AI
-Find a way to "know" where the car is in the city
-The driver drives the vehicle
-The vehicle doesn't have the driver

TODO now
-Use Road information (occupied, next road, cost, etc) 
to pathfind to next road
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Driver.h"

//-------------------------------------------------------------------------------------------------------
Driver::Driver(int nCargo, int nPassengers, const Ogre::Vector3 &position, const Ogre::Vector3 &direction) 
                : mstrGoal(""), mnNervousness(0), bIsFollowingClose(0)
{
    pVehicle = new Vehicle(nCargo, nPassengers, position, direction);
    mnSkill = rand() % 100 + 1;
    mnRiskTaker = rand() % 100 + 1;
}
//-------------------------------------------------------------------------------------------------------
//random, default constructor
Driver::Driver() : mnNervousness(0), bIsFollowingClose(0)
{
    int x = (rand() % 1000) - 500;
    int z = (rand() % 1000) - 500;
    int yawAngle  = (rand () % 720) - 360;

    Ogre::Vector3 initPosition = Ogre::Vector3(x, 30, z);

    pVehicle = new Vehicle(150, 1, initPosition, Ogre::Vector3(0, 0, 0), yawAngle);
    mnSkill = rand() % 100 + 1;
    mnRiskTaker = rand() % 100 + 1;
}
//-------------------------------------------------------------------------------------------------------
Driver::~Driver()
{
    if (DebugWindow::get()->getVehicle() == pVehicle)
    {
        DebugWindow::get()->debugVehicle(nullptr);
    }
    delete pVehicle;
}
//-------------------------------------------------------------------------------------------------------
void Driver::updateGoal(std::string strGoal)
{
    mstrGoal = strGoal;
}
//-------------------------------------------------------------------------------------------------------
std::string Driver::getDestination()
{
    return mstrGoal;
}
//-------------------------------------------------------------------------------------------------------
void Driver::updateDecision()
{
    if (mstrGoal == "")
    {
        pVehicle->setSpeed(50); //in MPH, to be converted internally into m/s
    }
}
//-------------------------------------------------------------------------------------------------------
int Driver::getSkill()
{
    return mnSkill;
}
//-------------------------------------------------------------------------------------------------------
int Driver::getNervousness()
{
    return mnNervousness;
}
//-------------------------------------------------------------------------------------------------------
int Driver::getRiskTaker()
{
    return mnRiskTaker;
}
//-------------------------------------------------------------------------------------------------------
void Driver::setNervousness(int nNervous)
{
    mnNervousness += nNervous;
}
//-------------------------------------------------------------------------------------------------------
void Driver::update(int milliseconds, std::string goal)
{
    if (goal != "NULL")
    {
        updateGoal(goal);
    }
    updateDecision();

    pVehicle->update(milliseconds);

}
//-------------------------------------------------------------------------------------------------------
Vehicle* Driver::getVehicle()
{
    return pVehicle;
}