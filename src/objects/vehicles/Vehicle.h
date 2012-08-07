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

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BtOgMotionState.h"

#include "MaterialParser.hpp"

class Vehicle : public Object
{
private:
    /*  inherited members           <---- is there any reason to have these?
    Ogre::Vector3 mPosition;
    Ogre::Vector3 mDirection;
    */  
    static long int nVehiclesCreated;
    std::string mstrName;

    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;

    btCollisionShape *mbtChassisShape;
    btRigidBody *mbtCarBody;

    //state variables
    bool mbIsMoving;
    bool mbIsHealthy;       //healthy as in not totaled
    bool mbIsInReverse;

    bool isFollowingClosely;

    int mnTargetSpeed;
    int mnSpeed;

    //in pounds
    unsigned int mnCargo;
    unsigned int mnPassengers;

    Vehicle& operator=(const Vehicle&);

    virtual void accelerate(const btScalar &force, const Ogre::Vector3 &direction);
    virtual void decelerate(const btScalar &force);

    void createRigidBody(float mass, const btTransform &trans);
    
public:
    Vehicle(int nCargo, int nPassengers, Ogre::Vector3 position = Ogre::Vector3(0, 0, 0), 
                                        Ogre::Vector3 direction = Ogre::Vector3(0, 0, 0));
    virtual ~Vehicle();

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

    void update();
};

#endif
