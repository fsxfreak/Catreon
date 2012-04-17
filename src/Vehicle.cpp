#include "stdafx.h"
#include <assert.h>
#include "Vehicle.h"
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers, int nDriverSkill) 
	:	mnSpeed(0), mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1),
		mcDriver(nDriverSkill)
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
		bool mbIsInReverse = 1;
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
int Vehicle::getCargo()
{
	return mnCargo;
}
//-------------------------------------------------------------------------------------------------------
int Vehicle::getPassengers()
{
	return mnPassengers;
}
//-------------------------------------------------------------------------------------------------------
Driver Vehicle::getDriver()
{
	return mcDriver;
}