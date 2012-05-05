/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef VEHICLE_H
#define VEHICLE_H

#include "stdafx.h"
#include "Driver.h"
#include "AdvancedOgreFramework.hpp"

class Vehicle
{
private:
	int mnSpeed;

	bool mbIsMoving;
	//healthy as in not totaled
	bool mbIsHealthy;
	bool mbIsInReverse;

	//in pounds
	unsigned int mnCargo;
	unsigned int mnPassengers;

	//AI
	Driver mcDriver;
	
	//car model
	Ogre::Entity *mVehicleEntity;
	Ogre::SceneNode *mVehicleNode;

	Vehicle& operator=(const Vehicle&);
	
public:
	Vehicle(int nCargo, int nPassengers, Ogre::Entity* entity, Ogre::SceneNode* node);
	~Vehicle();

	int getSpeed();
	void setSpeed(int nSpeed);

	bool isMoving();
	bool isHealthy();
	bool isInReverse();

	unsigned int getCargo();
	unsigned int getPassengers();

	//modify the driver through vehicle class
	Driver getDriver();

protected:
	virtual void decelerate(int nAccelForce);
	virtual void accelerate(int nDecelForce);
};


#endif
