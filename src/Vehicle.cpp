/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
Implement acceleration/deceleration
Create derived classes of this class, Car, Truck
********************************************************/

#include "stdafx.h"
#include "Vehicle.h"
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers) 
	:	mnSpeed(0), mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1)
{
	cDriver->setCar(this);
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
Driver Vehicle::getDriver()
{
	return mcDriver;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::setModel(Ogre::Entity *entity, Ogre::SceneNode *node)
{
	mVehicleEntity = entity;
	mVehicleNode = node;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::accelerate(int nAccelForce)
{
	//accelforce, in m/s, 1 - little acceleration, 5, normal acceleration, 10, flooring
	//dirty base implementation, adding acceleration by time handling
	mnSpeed = (nAccelForce * OgreFramework::getSingletonPtr()->getTimeSinceLastFrame()) + mnSpeed;
	if (mnSpeed < 150)
		mbIsHealthy = false;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::decelerate(int nDecelForce)
{
	//accelforce, in m/s, 1 - little decleration, 5, normal deceleration, 10, flooring breaks
	//dirty base implementation, adding deceleration by time handling
	mnSpeed = (nDecelForce * OgreFramework::getSingletonPtr()->getTimeSinceLastFrame()) - mnSpeed;
	if (mnSpeed < 0)
		mbIsInReverse = true;
}
