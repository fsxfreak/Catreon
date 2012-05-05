/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/**************************
TODO
Implement acceleration/deceleration

***************************/

#include "stdafx.h"
#include <assert.h>
#include "Vehicle.h"
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers) 
	:	mnSpeed(0), mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1), 
		mnAccel(calculateAccel()), mnDecel(calculateDecel())
{

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
void Vehicle::accelerate(int nTargetSpeed)
{
	//dirty base implementation, adding acceleration by time handling
	mnSpeed += nTargetSpeed;
	if (mnSpeed > 150)
		mbIsHealthy = 0;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::decelerate(int nTargetSpeed)
{
	//dirty base implementation, adding deceleration by time handling
	mnSpeed -= nTargetSpeed;
	if (mnSpeed < 0)
		mbIsInReverse = 1;
}
//-------------------------------------------------------------------------------------------------------
int Vehicle::calculateAccel()
{
	//trying to figure out car physics, this will do for now
	int accelForce = rand();
}
//-------------------------------------------------------------------------------------------------------
int Vehicle::calculateDecel()
{
	//trying to figure out car physics, this will do for now
	int decelForce = rand();
}