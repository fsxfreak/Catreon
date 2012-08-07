/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Implement acceleration/deceleration
-Create derived classes of this class, Car, Truck
-Initialize physics, sound, and graphics for the vehicle object
-Get a valid spawn point somewhere and start driving
-Actually drive around itself using physics
-Make a simple car already = done;
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Vehicle.h"

#include <sstream>

long int Vehicle::nVehiclesCreated = 0;

//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers, Ogre::Vector3 position, Ogre::Vector3 direction) 
    : mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1), mnTargetSpeed(0),
      Object(position, direction)
{
    //give a unique name to each vehicle
    std::ostringstream oss;
    oss << nVehiclesCreated;
    mstrName = "Vehicle_";
    mstrName += oss.str();

    ++nVehiclesCreated;
        
    mNode = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(mstrName, position);

    initializeMaterial();
    initializePhysics();
}
//-------------------------------------------------------------------------------------------------------
Vehicle::~Vehicle()
{
    getGameState()->mSceneMgr->destroyEntity(mEntity);
    getGameState()->mSceneMgr->destroySceneNode(mNode);

    delete mbtCarBody;
    delete mbtChassisShape;
}
//-------------------------------------------------------------------------------------------------------
int Vehicle::getSpeed()
{
    return mnTargetSpeed;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::setSpeed(int nSpeed)
{
    //Usually, the target speed will be the speed limit
    //The vehicle will attempt to speed up to the target speed when not obstructed
    mnTargetSpeed = nSpeed;
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3 Vehicle::getPosition()
{
    return mNode->getPosition();
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3 Vehicle::getDirection()
{
    //Converts quaternions into a direction vector for easier direction checking of AI driving on roads
    Ogre::Quaternion tempquat = mNode->_getDerivedOrientation();

    Ogre::Vector3 dirvec = tempquat * Ogre::Vector3(0, 0, -1);

    return dirvec;
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
    return (mnSpeed < 0) ? (mbIsInReverse = 1, true) : (mbIsInReverse = 0, false);
}
//-------------------------------------------------------------------------------------------------------
unsigned int Vehicle::getCargo()
{
    return mnCargo;
}
//-------------------------------------------------------------------------------------------------------
unsigned int Vehicle::getPassengers()
{
    return mnPassengers;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::initializePhysics()
{
    Ogre::AxisAlignedBox axisboxsize = mEntity->getBoundingBox();
    Ogre::Vector3 boxsize = axisboxsize.getSize();

    //box as the main body
    //the middle is a bit mis-aligned
    mbtChassisShape = new btBoxShape(btVector3(boxsize.x / 2, boxsize.y / 2, boxsize.z / 2));
    getGameState()->mCollisionShapes.push_back(mbtChassisShape);

    btTransform chassisTransform;
    chassisTransform.setIdentity();
    chassisTransform.setOrigin(GameState::ogreVecToBullet(mNode->getPosition()));

    createRigidBody(1500, chassisTransform); 

    mbtCarBody->setRestitution(1);
    mbtCarBody->setDamping(0.5, 0.5);

}
//-------------------------------------------------------------------------------------------------------
void Vehicle::initializeMaterial()
{
    mEntity = getGameState()->mSceneMgr->createEntity(mstrName, "car_bmwe46.mesh");

    /*if (!parseMaterial(mEntity, "car_bmwe46.material"))
    {
        OgreFramework::getSingletonPtr()->mLog->logMessage(
            "Failed to parse material file: car_bmwe46.material");
        return;
    }*/
    
    mNode->attachObject(mEntity);
}
//-------------------------------------------------------------------------------------------------------
//will be replaced by actual physics
void Vehicle::accelerate(const btScalar &force, const Ogre::Vector3 &direction)
{
    //called internally to reach the target speed set by setSpeed();

}
//-------------------------------------------------------------------------------------------------------
//will be replaced by actual physics
void Vehicle::decelerate(const btScalar &force)
{
    //accelforce, in m/s, 1 - little decleration, 5, normal deceleration, 10, flooring breaks
    //dirty base implementation, adding deceleration by time handling
    /*mnSpeed = (nDecelForce * OgreFramework::getSingletonPtr()->getTimeSinceLastFrame()) - mnSpeed;
    if (mnSpeed < 0)
        mbIsInReverse = true;*/

}
//-------------------------------------------------------------------------------------------------------
void Vehicle::createRigidBody(float mass, const btTransform &trans)
{
    btVector3 localInertia(0, 0, 0);
    mbtChassisShape->calculateLocalInertia(mass, localInertia);

    BtOgMotionState *motionState = new BtOgMotionState(trans, mNode);
    btRigidBody::btRigidBodyConstructionInfo conInfo(mass, motionState, mbtChassisShape, localInertia);
    mbtCarBody = new btRigidBody(conInfo);

    getGameState()->mDynamicsWorld->addRigidBody(mbtCarBody);
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::update()
{
    mnSpeed = mbtCarBody->getLinearVelocity().length();
    if (mnSpeed < mnTargetSpeed)
    {
        mbtCarBody->setLinearVelocity(mbtCarBody->getLinearVelocity() * 1.1);
    }
    else if (mnSpeed > mnTargetSpeed)
    {
        mbtCarBody->setLinearVelocity(mbtCarBody->getLinearVelocity() * 0.9);
    }
}
//-------------------------------------------------------------------------------------------------------