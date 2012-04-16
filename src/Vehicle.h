#ifndef VEHICLE_H
#define VEHICLE_H

#include "stdafx.h"
#include "Driver.h"

class Vehicle
{
private:
	int mnSpeed;
	bool mbIsMoving;
	//healthy as in not totaled
	bool mbIsHealthy;
	bool mbIsInReverse;

	//in pounds
	int mnCargo;
	int mnPassengers;

	//AI
	Driver mcDriver;
	
public:
	Vehicle(int nCargo, int nPassengers, int nDriverSkill);
	~Vehicle();

	int getSpeed();
	void setSpeed(int nSpeed);
	void addSpeed(int nSpeed);
	void decreaseSpeed(int nSpeed);

	bool isMoving();
	bool isHealthy();
	bool isInReverse();

	//modify the driver through vehicle class
	Driver getDriver();
};


#endif
