/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Implement acceleration/deceleration = done;
-Create derived classes of this class, Car, Truck
    -meh, make class more inheritable
-Initialize physics, sound, and graphics for the vehicle object
    -only need sound left
-Get a valid spawn point somewhere and start driving
-Actually drive around itself using physics = done;
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
Vehicle::Vehicle(int cargo, int passengers, Ogre::Vector3 initposition, Ogre::Vector3 initdirection) 
    : mbIsMoving(0), mbIsHealthy(1), mfTargetSpeed(0), mbtCar(nullptr)
{
    //give a unique name to each vehicle
    std::ostringstream oss;
    oss << nVehiclesCreated;
    mstrName = "Vehicle_";
    mstrName += oss.str();

    //++nVehiclesCreated;   //iterate in initializeMaterial() instead
    
    int x = (rand() % 720) - 360;
    int z = (rand() % 720) - 360;

    initposition = Ogre::Vector3(x, 30, z);
    mNode = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(mstrName, initposition);

    initializeMaterial();
    initializePhysics(cargo, passengers);
}
//-------------------------------------------------------------------------------------------------------
Vehicle::~Vehicle()
{
    getGameState()->mSceneMgr->destroyEntity(mEntity);
    getGameState()->mSceneMgr->destroySceneNode(mNode);
    std::vector<Ogre::SceneNode*>::iterator it = mWheelNodes.begin();
    for (it; it != mWheelNodes.end(); ++it)
    {
        getGameState()->mSceneMgr->destroySceneNode(*it);
    }
	getGameState()->mSceneMgr->destroyEntity(mFL_Entity);
	getGameState()->mSceneMgr->destroyEntity(mFR_Entity);
	getGameState()->mSceneMgr->destroyEntity(mBL_Entity);
	getGameState()->mSceneMgr->destroyEntity(mBR_Entity);
	//i'm sure there is a way better way to do this;
    
    for (int iii = getGameState()->mDynamicsWorld->getNumCollisionObjects() - 1; iii >= 0; iii--)
    {
        btCollisionObject *obj = getGameState()->mDynamicsWorld->getCollisionObjectArray()[iii];
        btRigidBody *body = btRigidBody::upcast(obj);
        if (obj == mbtCar && body && body->getMotionState())
        {
            getGameState()->mDynamicsWorld->getPairCache()->cleanProxyFromPairs(body->getBroadphaseHandle()
               , getGameState()->mDynamicsWorld->getDispatcher());
            while (body->getNumConstraintRefs())
            {
                btTypedConstraint *constraint = body->getConstraintRef(0);
                getGameState()->mDynamicsWorld->removeConstraint(constraint);
                delete constraint;
            }
            delete body->getMotionState();
            getGameState()->mDynamicsWorld->removeRigidBody(body);
        }
    }
    delete mVehicleRaycaster;
    getGameState()->mDynamicsWorld->removeAction(mVehicle);
    delete mVehicle;
    
    //if (mbtChassisShape) delete mbtChassisShape;
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
    fSpeed *= 0.447f * 10;     //convert mph into m/s (multiply by 10 for scaling)
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
    //return mNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
    return mNode->getOrientation() * Ogre::Vector3::UNIT_Z;
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
void Vehicle::initializePhysics(int cargo, int passengers)
{
    if (nVehiclesCreated <= 1)
	{
        getGameState()->mCollisionShapes.push_back(mbtChassisShape);
        //getGameState()->mCollisionShapes.push_back(mbtWheelShape);
	}
    
    //main body
//  mbtChassisShape = new btBoxShape(btVector3(8.92499, 7.48537, 24.072));
    btVector3 carPosition = GameState::ogreVecToBullet(mNode->getPosition());
    btTransform chassisTransform;
    chassisTransform.setIdentity();

    //to adjust the center of mass
    btCompoundShape *compound = new btCompoundShape();
    getGameState()->mCollisionShapes.push_back(compound);
    compound->addChildShape(chassisTransform, mbtChassisShape);

    chassisTransform.setOrigin(carPosition);
    int mass = 300 + cargo + (passengers * 50);
    btVector3 localInertia(0, 0, 0);
    mbtChassisShape->calculateLocalInertia(mass, localInertia);

    BtOgMotionState *motionState = new BtOgMotionState(chassisTransform, mNode);
    btRigidBody::btRigidBodyConstructionInfo conInfo(mass, motionState, mbtChassisShape, localInertia);
    mbtCar = new btRigidBody(conInfo);

    mbtCar->setActivationState(DISABLE_DEACTIVATION);

    mVehicleRaycaster = new btDefaultVehicleRaycaster(getGameState()->mDynamicsWorld);
    mVehicle = new btRaycastVehicle(mTuning, mbtCar, mVehicleRaycaster);

    getGameState()->mDynamicsWorld->addRigidBody(mbtCar);
    getGameState()->mDynamicsWorld->addVehicle(mVehicle);
    
    mVehicle->setCoordinateSystem(0, 1, 2);

    float widthwheel = mFL_Entity->getBoundingBox().getSize().x;
    float radiuswheel = mFL_Entity->getBoundingBox().getSize().y / 2;
    btVector3 wheelDirection(0, -1, 0);
    btVector3 wheelAxle(-1, 0, 0);
    btScalar suspensionRestLength(0.2f);

    mVehicle->addWheel(btVector3(9.76f, -6, 15.37f), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, true);
    mVehicle->addWheel(btVector3(-9.76f, -6, 15.37f), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, true);
    mVehicle->addWheel(btVector3(9.76f, -6, -15.37f), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, false);
    mVehicle->addWheel(btVector3(-9.76f, -6, -15.37f), wheelDirection, wheelAxle, suspensionRestLength, radiuswheel, mTuning, false);

    for (int iii = 0; iii < mVehicle->getNumWheels(); iii++)
    {
        btWheelInfo& wheelinfo = mVehicle->getWheelInfo(iii);
        wheelinfo.m_suspensionStiffness = 75.f;
        //from 0.0 - 1.0 (not bouncy)
        float bounciness = 0.4f;
        float damping = bounciness * 2.0f * btSqrt(wheelinfo.m_suspensionStiffness);
        wheelinfo.m_wheelsDampingRelaxation = damping;
        bounciness = 0.1f;
        damping = bounciness * 2.0f * btSqrt(wheelinfo.m_suspensionStiffness);
        wheelinfo.m_wheelsDampingCompression = damping;
        wheelinfo.m_maxSuspensionTravelCm = 500.f;
        wheelinfo.m_maxSuspensionForce = 5000000.f;
        wheelinfo.m_frictionSlip = 0.8f;
        wheelinfo.m_rollInfluence = 0.1f;
        wheelinfo.getSuspensionRestLength();
    }

}
//-------------------------------------------------------------------------------------------------------
void Vehicle::initializeMaterial()
{
    mEntity = getGameState()->mSceneMgr->createEntity(mstrName, "car_bmwe46.mesh");
    mNode->attachObject(mEntity);
    mEntity->setCastShadows(true);

    std::ostringstream oss;
    oss << nVehiclesCreated;
    std::string name = "wheelFL_";
    name += oss.str();

    //child nodes do not work with current implementation, inherited rotation is compounded with bullet rotation
    mFL_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mFL_Node = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(name, Ogre::Vector3(9.76f, -6, 15.37f)); 
    mFL_Node->attachObject(mFL_Entity);
    mWheelNodes.push_back(mFL_Node);
    
    name = "wheelFR_";
    name += oss.str();

    mFR_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mFR_Node = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(name, Ogre::Vector3(-9.76f, -6, 15.37f)); 
    mFR_Node->attachObject(mFR_Entity);
    mWheelNodes.push_back(mFR_Node);

    name = "wheelBL_";
    name += oss.str();

    mBL_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mBL_Node = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(name, Ogre::Vector3(9.76f, -6, -15.37f)); 
    mBL_Node->attachObject(mBL_Entity);
    mWheelNodes.push_back(mBL_Node);

    name = "wheelBR_";
    name += oss.str();
    
    mBR_Entity = getGameState()->mSceneMgr->createEntity("wheel.mesh");
    mBR_Node = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(name, Ogre::Vector3(-9.76f, -6, -15.37f)); 
    mBR_Node->attachObject(mBR_Entity);
    mWheelNodes.push_back(mBR_Node);

    ++nVehiclesCreated;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::accelerate(float power)
{
    for (int wheel = 0; wheel <= 3; wheel++)
    {
        mVehicle->applyEngineForce(100, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::brake(float power)
{
    for (int wheel = 0; wheel <= 3; wheel++)
    {
        mVehicle->setBrake(power, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::update(int milliseconds)
{
    mfSpeed = mVehicle->getCurrentSpeedKmHour();

    for (int iii = 0; iii < 4; iii++)
    {
        btTransform wheeltrans = mVehicle->getWheelTransformWS(iii);
        mWheelNodes[iii]->_setDerivedPosition(GameState::bulletVecToOgre(wheeltrans.getOrigin()));
        mWheelNodes[iii]->setOrientation(wheeltrans.getRotation().w(), 
                                         wheeltrans.getRotation().x(),
                                         wheeltrans.getRotation().y(),
                                         wheeltrans.getRotation().z());
    }
    if (!checkForVehicleAhead()) //true if vehicle ahead, false if not
    {
        if (mfSpeed < mfTargetSpeed)
        {
            float power = (mfTargetSpeed - mfSpeed) * 25.f * (milliseconds / 1000);
            accelerate(power);
        }
        else if (mfSpeed > mfTargetSpeed)
        {
            float power = (mfSpeed - mfTargetSpeed) * 25.f * (milliseconds / 1000);
            brake();
        }
    }
    mVehicle->updateVehicle(milliseconds / 1000);
}
//-------------------------------------------------------------------------------------------------------
bool Vehicle::checkForVehicleAhead()
{
    //get ray starting position in front of the car
    btVector3 rayOrigin = GameState::ogreVecToBullet(mNode->_getDerivedPosition() + (getDirection() * 24));
    btVector3 rayFront = GameState::ogreVecToBullet(getDirection() * 300) + rayOrigin; //300 = range of driver's sight
    btCollisionWorld::ClosestRayResultCallback rayQuery(rayOrigin, rayFront);
    getGameState()->mDynamicsWorld->rayTest(rayOrigin, rayFront, rayQuery);
    getGameState()->mDebugDrawer->drawRay(rayOrigin, rayFront);

    if (rayQuery.hasHit())
    {
        btCollisionObject *obj = rayQuery.m_collisionObject;
        btRigidBody *body = btRigidBody::upcast(obj);
        BtOgMotionState *state = (BtOgMotionState*)body->getMotionState();
        if (body == getGameState()->mRigidBodies[1])
        {
            brake(1000.f);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
//-------------------------------------------------------------------------------------------------------