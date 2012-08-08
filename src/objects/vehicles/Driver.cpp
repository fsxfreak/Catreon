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
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Driver.h"

//-------------------------------------------------------------------------------------------------------
Driver::Driver(int nSkill, int nRiskTaker, 
                int nCargo, int nPassengers, Ogre::Vector3 position) 
                : mstrGoal(""), mnNervousness(0), bIsFollowingClose(0)
{
    pVehicle = new Vehicle(nCargo, nPassengers, position);
    mnSkill = rand() % 100 + 1;
    mnRiskTaker = rand() % 100 + 1;
}
//-------------------------------------------------------------------------------------------------------
//random, default constructor
Driver::Driver() : mnNervousness(0), bIsFollowingClose(0)
{
    pVehicle = new Vehicle(150, 1);
    mnSkill = rand() % 100 + 1;
    mnRiskTaker = rand() % 100 + 1;
}
//-------------------------------------------------------------------------------------------------------
Driver::~Driver()
{

}
//-------------------------------------------------------------------------------------------------------
void Driver::setCar(Vehicle *vehicle)
{
    pVehicle = vehicle;
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
        pVehicle->setSpeed(90); //in MPH, to be converted internally into m/s
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
void Driver::update(std::string goal)
{
    if (goal != "NULL")
    {
        updateGoal(goal);
    }
    updateDecision();

    pVehicle->update();

}