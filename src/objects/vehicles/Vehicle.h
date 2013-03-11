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
#include <objects\Road.hpp>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BtOgMotionState.h>
#include <BulletCollision\CollisionShapes\btTriangleShape.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

#include <framework\DotSceneLoader.h>
#include <rapidxml\rapidxml.hpp>

#include <GUI\DebugWindow.hpp>

enum VehicleStates
{
    IDLE,
    FINDING_BEGIN_NODE,
    PATHFINDING,
};

class BtOgMotionState;

class Vehicle
{
    /* DebugWindow needs pretty much everything from Vehicle to display
       vital information */
    friend class DebugWindow;

private:
    static long int mVehiclesCreated;
    std::string mstrName;

    Ogre::SceneManager *mSceneManager;
    btDynamicsWorld* mDynamicsWorld;

    //main body
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;

    btCollisionShape *mbtChassisShape;
    btRigidBody *mbtCar;

    std::vector<Ogre::SceneNode*> mWheelNodes;
    std::vector<Ogre::Entity*> mWheelEntities;;

    //static btCollisionShape *mbtWheelShape;
    btRaycastVehicle::btVehicleTuning mTuning;
    btVehicleRaycaster* mVehicleRaycaster;
    btRaycastVehicle* mVehicle;

    btGhostObject *mTriggerNode;

    //state variables
    VehicleStates mState;

    bool mbIsHealthy;       //healthy as in not totaled
    bool isObstructed;

    bool isFollowingClosely;

    float mfTargetSpeed;
    float mfSpeed;

    float mSteeringValue;   //-0.6f to 0.6f
    std::list<Ogre::Vector3> mTargetPositions;

    float mDeltaTime;
    float mMillisecondsCounter; //in order to do several things like collision checking every one second

    std::string mOccupiedRoadName;
    Road *mOccupiedRoad;

    //in pounds
    //unsigned int mnCargo;
    //unsigned int mnPassengers;

    Vehicle& operator=(const Vehicle&);

    virtual void accelerate(float power = 200.f);
    virtual void brake(float power = 200.f);
    virtual void brake(const btVector3 &rayOrigin 
                       , const btCollisionWorld::ClosestRayResultCallback &rayQuery 
                       , float brakeFactor = 1.7f);
    virtual void steer(float targetSteerRadius = 0.0f);
    void updateSteering();
    void updateQueue();

    void updateTrigger();

    bool checkForVehicleAhead();
    void updateSpeed();

    virtual void initializePhysics(int cargo, int passengers);
    virtual void initializeMaterial();
    virtual void initializePreliminaries();  //avoid duplication in multiple constructors
public:
    Vehicle(int cargo, int passengers, const Ogre::Vector3 &initposition = Ogre::Vector3(0, 30, 0)
                                     , const Ogre::Vector3 &initdirection = Ogre::Vector3(0, 0, 0));
    Vehicle(int cargo, int passengers, const Ogre::Vector3 &initposition = Ogre::Vector3(0, 30, 0)
                                     , const Ogre::Quaternion &initquat = Ogre::Quaternion());
    ~Vehicle();

    float getSpeed();
    void setSpeed(float fSpeed);

    /** Precondition: pos must have a zero in the Y term.
    */
    void goTo(const Ogre::Vector3 &pos);
    void goTo(Road *road, VehicleStates state);
    void addToQueue(Road *road, VehicleStates state = VehicleStates::PATHFINDING);

    Ogre::Vector3 getPosition();
    Ogre::Vector3 getDirection();
    Ogre::Vector3 getUp();

    bool isHealthy();

    void update(float milliseconds);
};

#endif
