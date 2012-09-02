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
    -Use a compound shape - box with four wheel instead
    -Raycast shape instead?
    -add four wheel nodes as a child of the car
    -should just use btRaycastVehicle
-Make a simple car already = done;
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Vehicle.h"

#include <sstream>

long int Vehicle::nVehiclesCreated = 0;

btCollisionShape *Vehicle::mbtChassisShape = new btBoxShape(btVector3(8.92499, 7.48537, 24.072));
//box as the main body
//the middle is a bit mis-aligned
btCollisionShape *Vehicle::mbtWheelShape = new btCylinderShapeX(btVector3(1.02, 4.07, 3.88));
//yay magic numbers
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers, Ogre::Vector3 position, Ogre::Vector3 direction) 
    : mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1), mfTargetSpeed(0), mLastPosition(position), mbtCar(nullptr),
      Object(position, direction)
{
    //give a unique name to each vehicle
    std::ostringstream oss;
    oss << nVehiclesCreated;
    mstrName = "Vehicle_";
    mstrName += oss.str();

    ++nVehiclesCreated;
        
    mNode = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(mstrName, mPosition);

    initializeMaterial();
    initializePhysics();
}
//-------------------------------------------------------------------------------------------------------
Vehicle::~Vehicle()
{
    getGameState()->mSceneMgr->destroyEntity(mEntity);
    getGameState()->mSceneMgr->destroySceneNode(mNode);
	getGameState()->mSceneMgr->destroySceneNode(mFL_Node);
	getGameState()->mSceneMgr->destroySceneNode(mFR_Node);
	getGameState()->mSceneMgr->destroySceneNode(mBL_Node);
	getGameState()->mSceneMgr->destroySceneNode(mBR_Node);
	getGameState()->mSceneMgr->destroyEntity(mFL_Entity);
	getGameState()->mSceneMgr->destroyEntity(mFR_Entity);
	getGameState()->mSceneMgr->destroyEntity(mBL_Entity);
	getGameState()->mSceneMgr->destroyEntity(mBR_Entity);
	//i'm sure there is a way better way to do this;

    delete mbtCar;
    if (mbtChassisShape) delete mbtChassisShape;
    if (mbtWheelShape) delete mbtWheelShape;
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
    if (nVehiclesCreated <= 1)
	{
        getGameState()->mCollisionShapes.push_back(mbtChassisShape);
        getGameState()->mCollisionShapes.push_back(mbtWheelShape);
	}
    
    //main body
//  mbtChassisShape = new btBoxShape(btVector3(8.92499, 7.48537, 24.072));
    btTransform chassisTransform;
    chassisTransform.setIdentity();
    chassisTransform.setOrigin(GameState::ogreVecToBullet(mNode->getPosition()));

    mbtCar = createRigidBody(1200, chassisTransform, mbtCar, mbtChassisShape);
    mbtCar->setActivationState(DISABLE_DEACTIVATION);

    mVehicleRaycaster = new btDefaultVehicleRaycaster(getGameState()->mDynamicsWorld);
    mVehicle = new btRaycastVehicle(mTuning, mbtCar, mVehicleRaycaster);
    mVehicle->setCoordinateSystem(0, 1, 2);

    float connectionHeight = 1.2f;
    float wheelWidth = mFL_Entity->getBoundingBox().getSize().x;
    float wheelRadius = mFL_Entity->getBoundingBox().getSize().y / 2;
    btVector3 wheelDirection(0, -1, 0);
    btVector3 wheelAxle(-1, 0, 0);
    btScalar suspensionRestLength(0.6);

    mVehicle->addWheel(btVector3(9.76, -4.37, 15.37), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, mTuning, true);
    mVehicle->addWheel(btVector3(-9.76, -4.37, 15.37), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, mTuning, true);
    mVehicle->addWheel(btVector3(9.76, -4.37, -15.37), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, mTuning, false);
    mVehicle->addWheel(btVector3(-9.76, -4.37, -15.37), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, mTuning, false);

    getGameState()->mDynamicsWorld->addVehicle(mVehicle);

    
    //mbtCar->setFriction(0);
    //mbtCar->setRestitution(1);
    //mbtCar->setDamping(0.1, 0.8);

}
//-------------------------------------------------------------------------------------------------------
void Vehicle::initializeMaterial()
{
    mEntity = getGameState()->mSceneMgr->createEntity(mstrName, "car_bmwe46.mesh");
    mNode->attachObject(mEntity);

    mFL_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mFL_Node= mNode->createChildSceneNode(Ogre::Vector3(9.76, -4.37, 15.37));
    mFL_Node->attachObject(mFL_Entity);
    
    mFR_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mFR_Node = mNode->createChildSceneNode(Ogre::Vector3(-9.76, -4.37, 15.37));
    mFR_Node->attachObject(mFR_Entity);

    mBL_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mBL_Node = mNode->createChildSceneNode(Ogre::Vector3(9.76, -4.37, -15.37));
    mBL_Node->attachObject(mBL_Entity);
    
    mBR_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mBR_Node = mNode->createChildSceneNode(Ogre::Vector3(-9.76, -4.37, -15.37));
    mBR_Node->attachObject(mBR_Entity);
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::accelerate()
{
    /*btScalar force = abs(mfTargetSpeed - mfSpeed) * 500;
    btVector3 forcedir = GameState::ogreVecToBullet(getDirection()).normalized();
    forcedir *= force;
    mbtCar->applyCentralForce(-forcedir);*/
    for (int wheel = 0; wheel <= 3; ++wheel)
    {
        mVehicle->applyEngineForce(200, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::decelerate()
{
    /*btScalar force = abs(mfTargetSpeed - mfSpeed) * 500;
    btVector3 forcedir = GameState::ogreVecToBullet(getDirection()).normalized();
    forcedir *= force;
    mbtCar->applyCentralForce(forcedir);*/
}
//-------------------------------------------------------------------------------------------------------
btRigidBody* Vehicle::createRigidBody(float mass, const btTransform &trans, btRigidBody *rigidBody, btCollisionShape *chassisShape)
{
    btVector3 localInertia(0, 0, 0);
    chassisShape->calculateLocalInertia(mass, localInertia);

    BtOgMotionState *motionState = new BtOgMotionState(trans, mNode);
    btRigidBody::btRigidBodyConstructionInfo conInfo(mass, motionState, mbtChassisShape, localInertia);
    rigidBody = new btRigidBody(conInfo);

    getGameState()->mDynamicsWorld->addRigidBody(rigidBody);
    return rigidBody;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::update()
{
    if (mLastPosition.y - mNode->getPosition().y > 1)
    {
        //car is falling from the sky
        return;
    }

    accelerate();
    mVehicle->updateVehicle(1);
    //btVector3 speedvector = (GameState::ogreVecToBullet(mNode->getPosition()) - mLastPosition);
    //mfSpeed = speedvector.length();
    /*mfSpeed = mbtCar->getLinearVelocity().length();

    //anything > 0 = forward, < 0 = backward
    Ogre::Real directionTraveling = mNode->getPosition().dotProduct(mLastPosition);

    if (directionTraveling > 0)
        mbIsInReverse = 0;
    else if (directionTraveling < 0)
        mbIsInReverse = 1;

    if (mfSpeed < mfTargetSpeed || mbIsInReverse)
    {
        accelerate();
    }
    else if (mfSpeed > mfTargetSpeed && !mbIsInReverse)
    {
        decelerate();
    }*/
    mLastPosition = mNode->getPosition();
}
//-------------------------------------------------------------------------------------------------------