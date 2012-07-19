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
#include "objects\vehicles\Vehicle.h"

class Driver
{
private:
    std::string mstrGoal;

    //all personality on a scale of 1 to 100
    int mnSkill;
    int mnNervousness;
    int mnRiskTaker;

    Vehicle *pVehicle;

    Driver& operator=(const Driver& cDriver);
public:
    Driver(int nSkill, int nRiskTaker, int nCargo, int nPassengers, Ogre::Vector3 position);
    Driver();
    ~Driver();

    void setCar(Vehicle *vehicle);

    void updateGoal(std::string strDest);
    void updateDecision();

    int getSkill();
    int getNervousness();
    int getRiskTaker();

    void setNervousness(int nNervous);

    std::string getDestination();

};


#endif
