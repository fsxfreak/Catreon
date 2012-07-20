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
#include "GameState.hpp"

class Vehicle : public Object
{
private:
    /*  inherited members
    Ogre::Vector3 mPosition;
    Ogre::Vector3 mDirection;
    */
    static long int nVehiclesCreated;

    Ogre::SceneNode *mNode;
    
    //state variables
    bool mbIsMoving;
    bool mbIsHealthy;       //healthy as in not totaled
    bool mbIsInReverse;

    bool isFollowingClosely;

    int mnSpeed;

    //in pounds
    unsigned int mnCargo;
    unsigned int mnPassengers;

    Vehicle& operator=(const Vehicle&);
    
public:
    Vehicle(int nCargo, int nPassengers, Ogre::Vector3 position = Ogre::Vector3(0, 0, 0), 
                                        Ogre::Vector3 direction = Ogre::Vector3(0, 0, 0));
    ~Vehicle();

    int getSpeed();
    void setSpeed(int nSpeed);

    Ogre::Vector3 getPosition();
    Ogre::Vector3 getDirection();

    bool isMoving();
    bool isHealthy();
    bool isInReverse();

    unsigned int getCargo();
    unsigned int getPassengers();

    virtual void initializePhysics();
    virtual void initializeMaterial();

    virtual void accelerate(const btScalar &force, const Ogre::Vector3 &direction);
    virtual void decelerate(const btScalar &force);
};

#endif
