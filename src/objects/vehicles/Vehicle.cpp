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
    -should just use btRaycastVehicle6
-Make a simple car already = done;
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Vehicle.h"

#include <sstream>

long int Vehicle::nVehiclesCreated = 0;

btCollisionShape *Vehicle::mbtChassisShape = new btBoxShape(btVector3(8, 7, 23));
//box as the main body
//the middle is a bit mis-aligned
//btCollisionShape *Vehicle::mbtWheelShape = new btCylinderShapeX(btVector3(1.02, 4.07, 3.88));
//yay magic numbers
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int nCargo, int nPassengers, Ogre::Vector3 initposition, Ogre::Vector3 initdirection) 
    : mbIsMoving(0), mbIsHealthy(1), mnCargo(200), mnPassengers(1), mfTargetSpeed(0), mLastPosition(initposition), mbtCar(nullptr)
{
    //give a unique name to each vehicle
    std::ostringstream oss;
    oss << nVehiclesCreated;
    mstrName = "Vehicle_";
    mstrName += oss.str();

    ++nVehiclesCreated;
   
    initposition = Ogre::Vector3(0, 30, 0);
    mNode = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(mstrName, initposition);

    initializeMaterial();
    initializePhysics();
}
//-------------------------------------------------------------------------------------------------------
Vehicle::~Vehicle()
{
    getGameState()->mSceneMgr->destroyEntity(mEntity);
    getGameState()->mSceneMgr->destroySceneNode(mNode);
    mWheelNodes.clear();
	getGameState()->mSceneMgr->destroyEntity(mFL_Entity);
	getGameState()->mSceneMgr->destroyEntity(mFR_Entity);
	getGameState()->mSceneMgr->destroyEntity(mBL_Entity);
	getGameState()->mSceneMgr->destroyEntity(mBR_Entity);
	//i'm sure there is a way better way to do this;

    delete mbtCar;
    if (mbtChassisShape) delete mbtChassisShape;
    //if (mbtWheelShape) delete mbtWheelShape;
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
        //getGameState()->mCollisionShapes.push_back(mbtWheelShape);
	}
    
    //main body
//  mbtChassisShape = new btBoxShape(btVector3(8.92499, 7.48537, 24.072));
    btTransform chassisTransform;
    chassisTransform.setIdentity();
    //chassisTransform.setOrigin(GameState::ogreVecToBullet(mNode->getPosition()));
    chassisTransform.setOrigin(btVector3(0, 0, 0));

    mbtCar = createRigidBody(800, chassisTransform, mbtCar, mbtChassisShape);
    mbtCar->setActivationState(DISABLE_DEACTIVATION);

    mTuning.m_maxSuspensionTravelCm = 500.0f;
    mTuning.m_suspensionCompression = 4.4f;
    mTuning.m_suspensionDamping = 2.3f;
    mTuning.m_frictionSlip = 1000.0f;
    mTuning.m_suspensionStiffness = 200.0f;

    mVehicleRaycaster = new btDefaultVehicleRaycaster(getGameState()->mDynamicsWorld);
    mVehicle = new btRaycastVehicle(mTuning, mbtCar, mVehicleRaycaster);

    getGameState()->mDynamicsWorld->addRigidBody(mbtCar);
    getGameState()->mDynamicsWorld->addVehicle(mVehicle);
    
    mVehicle->setCoordinateSystem(0, 1, 2);

    float connectionHeight = 0.2f;
    float widthwheel = mFL_Entity->getBoundingBox().getSize().x;
    float radiuswheel = mFL_Entity->getBoundingBox().getSize().y / 2;
    btVector3 wheelDirection(0, -1, 0);
    btVector3 wheelAxle(-1, 0, 0);
    btScalar suspensionRestLength(0.6);

    mVehicle->addWheel(btVector3(9.76, -6, 15.37), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, true);
    mVehicle->addWheel(btVector3(-9.76, -6, 15.37), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, true);
    mVehicle->addWheel(btVector3(9.76, -6, -15.37), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, false);
    mVehicle->addWheel(btVector3(-9.76, -6, -15.37), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, false);

    for (int iii = 0; iii < mVehicle->getNumWheels(); iii++)
    {
        btWheelInfo& wheelinfo = mVehicle->getWheelInfo(iii);
        wheelinfo.m_suspensionStiffness = 20.0f;
        wheelinfo.m_wheelsDampingRelaxation = 2.3f;
        wheelinfo.m_wheelsDampingCompression = 1.0f;
        wheelinfo.m_maxSuspensionTravelCm = 500.0f;
        wheelinfo.m_maxSuspensionForce = 500.0f;
        wheelinfo.m_frictionSlip = 2.3f;
        wheelinfo.m_rollInfluence = 0;
    }

}
//-------------------------------------------------------------------------------------------------------
void Vehicle::initializeMaterial()
{
    mEntity = getGameState()->mSceneMgr->createEntity(mstrName, "car_bmwe46.mesh");
    mNode->attachObject(mEntity);

    mFL_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mFL_Node= mNode->createChildSceneNode(Ogre::Vector3(9.76, -6, 15.37));
    mFL_Node->attachObject(mFL_Entity);
    mWheelNodes.push_back(mFL_Node);
    
    mFR_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mFR_Node = mNode->createChildSceneNode(Ogre::Vector3(-9.76, -6, 15.37));
    mFR_Node->attachObject(mFR_Entity);
    mWheelNodes.push_back(mFR_Node);

    mBL_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mBL_Node = mNode->createChildSceneNode(Ogre::Vector3(9.76, -6, -15.37));
    mBL_Node->attachObject(mBL_Entity);
    mWheelNodes.push_back(mBL_Node);
    
    mBR_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mBR_Node = mNode->createChildSceneNode(Ogre::Vector3(-9.76, -6, -15.37));
    mBR_Node->attachObject(mBR_Entity);
    mWheelNodes.push_back(mBR_Node);
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
        mVehicle->applyEngineForce(1000, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::decelerate()
{
    /*btScalar force = abs(mfTargetSpeed - mfSpeed) * 500;
    btVector3 forcedir = GameState::ogreVecToBullet(getDirection()).normalized();
    forcedir *= force;
    mbtCar->applyCentralForce(forcedir);*/
    for (int wheel = 0; wheel <= 3; ++wheel)
    {
        mVehicle->setBrake(100000000, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
btRigidBody* Vehicle::createRigidBody(float mass, const btTransform &trans, btRigidBody *rigidBody, btCollisionShape *chassisShape)
{
    btVector3 localInertia(0, 0, 0);
    chassisShape->calculateLocalInertia(mass, localInertia);

    BtOgMotionState *motionState = new BtOgMotionState(trans, mNode);
    btRigidBody::btRigidBodyConstructionInfo conInfo(mass, motionState, chassisShape, localInertia);
    conInfo.m_friction = 0.6;
    conInfo.m_restitution = 0.6;
    conInfo.m_linearDamping = 0.2;
    conInfo.m_angularDamping = 0.2;
    rigidBody = new btRigidBody(conInfo);

    return rigidBody;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::update()
{
    accelerate();
    mVehicle->updateVehicle(1);

    for (int iii = 0; iii < 4; iii++)
    {
        mVehicle->updateWheelTransform(iii, true);
        btTransform wheeltrans = mVehicle->getWheelTransformWS(iii);
        mWheelNodes[iii]->setOrientation(wheeltrans.getRotation().w(), 
                                         wheeltrans.getRotation().x(),
                                         wheeltrans.getRotation().y(),
                                         wheeltrans.getRotation().z());
        mVehicle->setPitchControl(20);
        mWheelNodes[iii]->setPosition(GameState::bulletVecToOgre(wheeltrans.getOrigin()));
    }

    mfSpeed = mVehicle->getCurrentSpeedKmHour();

    if (mfSpeed > 0)
        mbIsInReverse = 0;
    else if (mfSpeed < 0)
        mbIsInReverse = 1;

    if (mfSpeed < mfTargetSpeed || mbIsInReverse)
    {
        accelerate();
    }
    else if (mfSpeed > mfTargetSpeed && !mbIsInReverse)
    {
        decelerate();
    }
    mLastPosition = mNode->getPosition();
}
//-------------------------------------------------------------------------------------------------------