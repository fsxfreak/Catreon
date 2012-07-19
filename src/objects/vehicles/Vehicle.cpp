/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Implement acceleration/deceleration
-Create derived classes of this class, Car, Truck
-Initialize physics, sound, and graphics for the vehicle object
-Get a valid spawn point somewhere and start driving
-Actually drive around itself using physics
-Make a simple car already
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Vehicle.h"

long int Vehicle::nVehiclesCreated = 0;

//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers, Ogre::Vector3 position = Ogre::Vector3(0, 0, 0),
    Ogre::Vector3 direction = Ogre::Vector3(0, 0, 0)) 
    : mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1),
      Object(position, direction)
{
    mNode = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode("Vehicle_"+nVehiclesCreated, position); 
}
//-------------------------------------------------------------------------------------------------------
Vehicle::~Vehicle()
{

}
//-------------------------------------------------------------------------------------------------------
int Vehicle::getSpeed()
{
    return mnSpeed;
}
//-------------------------------------------------------------------------------------------------------
//will be depreciated
void Vehicle::setSpeed(int nSpeed)
{
    mnSpeed = nSpeed;

    if (mnSpeed > 0)
    {    
        mbIsMoving = 1;
        mbIsInReverse = 0;
    }
    else if (mnSpeed == 0)
    {
        mbIsMoving = 0;
    }
    else
    {
        mbIsInReverse = 1;
    }
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3 Vehicle::getPosition()
{
    return mNode->getPosition();
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3 Vehicle::getDirection()
{
    //Converts quaternions into a direction vector for easier direction checking of AI driving on roads
    Ogre::Quaternion temp = mNode->getOrientation();
    Ogre::Vector3 dir(0, 0, 0);
    if (temp.z == 1.0f)
    {
        Ogre::Radian radangle =  2 * Ogre::Math::ACos(temp.w);
        dir.x = Ogre::Math::Cos(radangle);
        dir.y = 0;
        dir.z = Ogre::Math::Sin(radangle);
    }

    return dir;
}
//-------------------------------------------------------------------------------------------------------
bool Vehicle::isMoving()
{
    return mbIsMoving;
}
//-------------------------------------------------------------------------------------------------------
bool Vehicle::isHealthy()
{
    return mbIsHealthy;
}
//-------------------------------------------------------------------------------------------------------
bool Vehicle::isInReverse()
{
    if (mnSpeed < 0)
        mbIsInReverse = 1;

    return mbIsInReverse;
}
//-------------------------------------------------------------------------------------------------------
unsigned int Vehicle::getCargo()
{
    return mnCargo;
}
//-------------------------------------------------------------------------------------------------------
unsigned int Vehicle::getPassengers()
{
    return mnPassengers;
}
//-------------------------------------------------------------------------------------------------------
//will be replaced by actual physics
void Vehicle::accelerate(int nAccelForce)
{
    //accelforce, in m/s, 1 - little acceleration, 5, normal acceleration, 10, flooring
    //dirty base implementation, adding acceleration by time handling
    mnSpeed = (nAccelForce * OgreFramework::getSingletonPtr()->getTimeSinceLastFrame()) + mnSpeed;
    if (mnSpeed < 150)
        mbIsHealthy = false;
}
//-------------------------------------------------------------------------------------------------------
//will be replaced by actual physics
void Vehicle::decelerate(int nDecelForce)
{
    //accelforce, in m/s, 1 - little decleration, 5, normal deceleration, 10, flooring breaks
    //dirty base implementation, adding deceleration by time handling
    mnSpeed = (nDecelForce * OgreFramework::getSingletonPtr()->getTimeSinceLastFrame()) - mnSpeed;
    if (mnSpeed < 0)
        mbIsInReverse = true;

}
//-------------------------------------------------------------------------------------------------------

