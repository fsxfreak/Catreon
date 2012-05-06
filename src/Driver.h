/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DRIVER_H
#define DRIVER_H

#include "stdafx.h"
#include <string>
#include "Vehicle.h"

class Driver
{
private:
	std::string mstrDestination;

	//all personality on a scale of 1 to 100
	int mnSkill;
	int mnNervousness;
	int mnRiskTaker;

	Vehicle *cVehicle;

	Driver& operator=(const Driver& cDriver);
public:
	Driver(int nSkill, int nRiskTaker);
	Driver();
	~Driver();

	void setCar(Vehicle *cVehicle);

	void updateDestination(std::string strDest);
	void updateDecision();

	int getSkill();
	int getNervousness();
	int getRiskTaker();

	void setNervousness(int nNervous);

	std::string getDestination();

};


#endif
