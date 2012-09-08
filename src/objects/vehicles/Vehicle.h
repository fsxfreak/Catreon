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
#include "GameState.hpp"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BtOgMotionState.h"

#include "MaterialParser.hpp"

class Vehicle
{
private:
    static long int nVehiclesCreated;
    std::string mstrName;

    //main body
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;

    static btCollisionShape *mbtChassisShape;
    btRigidBody *mbtCar;

    //wheels
    //FL = front left, BL = back left
    Ogre::SceneNode *mFL_Node;
    Ogre::Entity *mFL_Entity;
    Ogre::SceneNode *mFR_Node;
    Ogre::Entity *mFR_Entity;
    Ogre::SceneNode *mBL_Node;
    Ogre::Entity *mBL_Entity;
    Ogre::SceneNode *mBR_Node;
    Ogre::Entity *mBR_Entity;

    std::vector<Ogre::SceneNode*> mWheelNodes;

    //static btCollisionShape *mbtWheelShape;
    btRaycastVehicle::btVehicleTuning mTuning;
    btVehicleRaycaster* mVehicleRaycaster;
    btRaycastVehicle* mVehicle;

    //state variables
    bool mbIsMoving;
    bool mbIsHealthy;       //healthy as in not totaled
    bool mbIsInReverse;

    bool isFollowingClosely;

    float mfTargetSpeed;
    float mfSpeed;

    //in pounds
    //unsigned int mnCargo;
    //unsigned int mnPassengers;

    Vehicle& operator=(const Vehicle&);

    Ogre::Vector3 mLastPosition;

    virtual void accelerate();
    virtual void decelerate();

    btRigidBody* createRigidBody(float mass, const btTransform &trans, btRigidBody *rigidBody, btCollisionShape *chassisShape);
    
public:
    Vehicle(int cargo, int passengers, Ogre::Vector3 initposition = Ogre::Vector3(0, 40, 0), 
                                         Ogre::Vector3 initdirection = Ogre::Vector3(0, 0, 0));
    ~Vehicle();

    int getSpeed();
    void setSpeed(float fSpeed);

    Ogre::Vector3 getPosition();
    Ogre::Vector3 getDirection();

    bool isMoving();
    bool isHealthy();
    bool isInReverse();

    virtual void initializePhysics(int cargo, int passengers);
    virtual void initializeMaterial();

    void update(int milliseconds);
};

#endif
