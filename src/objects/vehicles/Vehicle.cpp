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

btCollisionShape *Vehicle::mbtChassisShape = new btBoxShape(btVector3(8.92499, 7.48537, 24.072));
//box as the main body
//the middle is a bit mis-aligned
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers, Ogre::Vector3 position, Ogre::Vector3 direction) 
    : mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1), mfTargetSpeed(0), mLastPosition(position),
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
    return mfSpeed;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::setSpeed(float fSpeed)
{
    //Usually, the target speed will be the speed limit
    //The vehicle will attempt to speed up to the target speed when not obstructed
    fSpeed *= 0.447 * 10;     //convert mph into m/s (multiply by 10 for scaling)
    mfTargetSpeed = fSpeed;
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
    return mNode->getOrientation() * -Ogre::Vector3::UNIT_Z;
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
    return (mfSpeed < 0) ? (mbIsInReverse = 1, true) : (mbIsInReverse = 0, false);
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
    //Ogre::AxisAlignedBox axisboxsize = mEntity->getBoundingBox();
    //Ogre::Vector3 boxsize = axisboxsize.getSize();
    //17.84998, 14.970741, 48.143997
    //box as the main body
    //the middle is a bit mis-aligned
    //mbtChassisShape = new btBoxShape(btVector3(boxsize.x / 2, boxsize.y / 2, boxsize.z / 2));
    if (nVehiclesCreated <= 1)
        getGameState()->mCollisionShapes.push_back(mbtChassisShape);

    btTransform chassisTransform;
    chassisTransform.setIdentity();
    chassisTransform.setOrigin(GameState::ogreVecToBullet(mNode->getPosition()));

    createRigidBody(1000, chassisTransform); 

    mbtCarBody->setFriction(0);
    mbtCarBody->setRestitution(1);
    mbtCarBody->setDamping(0.1, 0.8);

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
void Vehicle::accelerate()
{
    btScalar force = abs(mfTargetSpeed - mfSpeed) * 500;
    btVector3 forcedir = GameState::ogreVecToBullet(getDirection()).normalized();
    forcedir *= force;
    mbtCarBody->applyCentralForce(-forcedir);
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::decelerate()
{
    btScalar force = abs(mfTargetSpeed - mfSpeed) * 500;
    btVector3 forcedir = GameState::ogreVecToBullet(getDirection()).normalized();
    forcedir *= force;
    mbtCarBody->applyCentralForce(forcedir);
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
    //btVector3 speedvector = (GameState::ogreVecToBullet(mNode->getPosition()) - mLastPosition);
    //mfSpeed = speedvector.length();
    mfSpeed = mbtCarBody->getLinearVelocity().length();

    Ogre::Real test = getDirection().dotProduct(mLastPosition.normalisedCopy());
    if (test > 0)
        mbIsInReverse = 0;
    else if (test < 0)
        mbIsInReverse = 1;

    if (mfSpeed < mfTargetSpeed || mbIsInReverse)
    {
        accelerate();
    }
    else if (mfSpeed > mfTargetSpeed && !mbIsInReverse)
    {
        decelerate();
    }
}
//-------------------------------------------------------------------------------------------------------