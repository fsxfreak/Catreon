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
#include "framework\AdvancedOgreFramework.hpp"
#include "objects\Object.hpp"

class Vehicle : public Object
{
private:
    bool mbIsMoving;
    //healthy as in not totaled
    bool mbIsHealthy;
    bool mbIsInReverse;

    int mnSpeed;

    //in pounds
    unsigned int mnCargo;
    unsigned int mnPassengers;

    Vehicle& operator=(const Vehicle&);
    
public:
    Vehicle(int nCargo, int nPassengers, Ogre::Vector3 position, Ogre::Vector3 direction);
    ~Vehicle();

    int getSpeed();
    void setSpeed(int nSpeed);

    bool isMoving();
    bool isHealthy();
    bool isInReverse();

    unsigned int getCargo();
    unsigned int getPassengers();

    virtual void decelerate(int nDecelForce);
    virtual void accelerate(int nAccelForce);

};


#endif
