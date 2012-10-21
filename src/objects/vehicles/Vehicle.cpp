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


long int Vehicle::mVehiclesCreated = 0;
btCollisionShape *Vehicle::mbtChassisShape = new btBoxShape(btVector3(8, 7, 23));
//box as the main body
//the middle is a bit mis-aligned
//btCollisionShape *Vehicle::mbtWheelShape = new btCylinderShapeX(btVector3(1.02, 4.07, 3.88));
//yay magic numbers
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(int cargo, int passengers, Ogre::Vector3 initposition, Ogre::Vector3 initdirection) 
    : mbIsMoving(0), mbIsHealthy(1), mfTargetSpeed(0), mbtCar(nullptr), mSteeringValue(0.f), 
      mSceneManager(getGameState()->mSceneMgr), mDynamicsWorld(getGameState()->mDynamicsWorld)
{
    //give a unique name to each vehicle
    std::ostringstream oss;
    oss << mVehiclesCreated;
    mstrName = "Vehicle_";
    mstrName += oss.str();

    //++mVehiclesCreated;   //iterate in initializeMaterial() instead
    
    int x = (rand() % 1000) - 500;
    int z = (rand() % 1000) - 500;
    int yawangle  = (rand () % 720) - 360;

    initposition = Ogre::Vector3(x, 30, z);
    mNode = mSceneManager->getRootSceneNode()->createChildSceneNode(mstrName, initposition);
    mNode->yaw(Ogre::Angle(yawangle));

    initializeMaterial(yawangle);
    initializePhysics(cargo, passengers, yawangle);
}
//-------------------------------------------------------------------------------------------------------
Vehicle::~Vehicle()
{
    mSceneManager->destroyEntity(mEntity);
    mSceneManager->destroySceneNode(mNode);
    std::vector<Ogre::SceneNode*>::iterator it = mWheelNodes.begin();
    for (it; it != mWheelNodes.end(); ++it)
    {
        mSceneManager->destroySceneNode(*it);
    }
	std::vector<Ogre::Entity*>::iterator itr = mWheelEntities.begin();
    for (itr; itr != mWheelEntities.end(); ++itr)
    {
        mSceneManager->destroyEntity(*itr);
    }
    
    for (int iii = mDynamicsWorld->getNumCollisionObjects() - 1; iii >= 0; iii--)
    {
        btCollisionObject *obj = mDynamicsWorld->getCollisionObjectArray()[iii];
        btRigidBody *body = btRigidBody::upcast(obj);
        if (obj == mbtCar && body && body->getMotionState())
        {
            mDynamicsWorld->getPairCache()->cleanProxyFromPairs(body->getBroadphaseHandle()
               , mDynamicsWorld->getDispatcher());
            while (body->getNumConstraintRefs())
            {
                btTypedConstraint *constraint = body->getConstraintRef(0);
                mDynamicsWorld->removeConstraint(constraint);
                delete constraint;
            }
            delete body->getMotionState();
            mDynamicsWorld->removeRigidBody(body);
        }
    }
    delete mVehicleRaycaster;
    mDynamicsWorld->removeAction(mVehicle);
    delete mVehicle;
    
    //if (mbtChassisShape) delete mbtChassisShape;
    //if (mbtWheelShape) delete mbtWheelShape;
}
//-------------------------------------------------------------------------------------------------------
float Vehicle::getSpeed()
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
void Vehicle::initializePhysics(int cargo, int passengers, float yawangle)
{
    if (mVehiclesCreated <= 1)
	{
        getGameState()->mCollisionShapes.push_back(mbtChassisShape);
	}
    
    //main body
//  mbtChassisShape = new btBoxShape(btVector3(8.92499, 7.48537, 24.072));
    btVector3 carPosition = GameState::ogreVecToBullet(mNode->getPosition());
    btTransform chassisTransform;
    chassisTransform.setIdentity();
    btQuaternion rotation;
    rotation.setEulerZYX(0, yawangle, 0);
   
    //to adjust the center of mass
    btCompoundShape *compound = new btCompoundShape();
    getGameState()->mCollisionShapes.push_back(compound);
    compound->addChildShape(chassisTransform, mbtChassisShape);

    chassisTransform.setOrigin(carPosition);
    chassisTransform.setRotation(rotation);
    float mass = 300.f + cargo + (passengers * 50.f);
    btVector3 localInertia(0, 0, 0);
    mbtChassisShape->calculateLocalInertia(mass, localInertia);

    BtOgMotionState *motionState = new BtOgMotionState(chassisTransform, mNode);
    btRigidBody::btRigidBodyConstructionInfo conInfo(mass, motionState, mbtChassisShape, localInertia);
    mbtCar = new btRigidBody(conInfo);

    mbtCar->setActivationState(DISABLE_DEACTIVATION);

    mVehicleRaycaster = new btDefaultVehicleRaycaster(mDynamicsWorld);
    mVehicle = new btRaycastVehicle(mTuning, mbtCar, mVehicleRaycaster);

    mDynamicsWorld->addRigidBody(mbtCar);
    mDynamicsWorld->addVehicle(mVehicle);
    
    mVehicle->setCoordinateSystem(0, 1, 2);

    float widthwheel = mWheelEntities[0]->getBoundingBox().getSize().x;
    float radiuswheel = mWheelEntities[0]->getBoundingBox().getSize().y / 2;
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
    }

}
//-------------------------------------------------------------------------------------------------------
void Vehicle::initializeMaterial(float fyawangle)
{
    mEntity = mSceneManager->createEntity(mstrName, "car_bmwe46.mesh");
    mNode->attachObject(mEntity);
    mEntity->setCastShadows(true);

    Ogre::Radian yawangle = Ogre::Angle(fyawangle);

    //my crowning achievement, lol
    auto createWheel = [&](Ogre::Entity *ent, Ogre::SceneNode *node, const Ogre::Vector3 &pos, const std::string &name) {
        ent = mSceneManager->createEntity("wheel.mesh");
        node = mSceneManager->getRootSceneNode()->createChildSceneNode(name, pos);
        node->yaw(yawangle);
        node->attachObject(ent);
        mWheelNodes.push_back(node);
        mWheelEntities.push_back(ent);
    };

    std::ostringstream oss;
    oss << mVehiclesCreated;

    std::string name = "wheelFL_";
    name += oss.str();
    Ogre::Entity *fl_ent;
    Ogre::SceneNode *fl_node;
    createWheel(fl_ent, fl_node, Ogre::Vector3(9.76f, -6, 15.37f), name);
    
    name = "wheelFR_";
    name += oss.str();
    Ogre::Entity *fr_ent;
    Ogre::SceneNode *fr_node;
    createWheel(fr_ent, fr_node, Ogre::Vector3(-9.76f, -6, 15.37f), name);

    name = "wheelBL_";
    name += oss.str();
    Ogre::Entity *bl_ent;
    Ogre::SceneNode *bl_node;
    createWheel(bl_ent, bl_node, Ogre::Vector3(9.76f, -6, -15.37f), name);

    name = "wheelBR_";
    name += oss.str();
    Ogre::Entity *br_ent;
    Ogre::SceneNode *br_node;
    createWheel(br_ent, br_node, Ogre::Vector3(-9.76f, -6, -15.37f), name);

    ++mVehiclesCreated;
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::accelerate(float power)
{
    for (int wheel = 0; wheel <= 3; wheel++)
    {
        mVehicle->applyEngineForce(power, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::brake(float power)
{
    accelerate(0);
    for (int wheel = 0; wheel <= 3; wheel++)
    {
        mVehicle->setBrake(power, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::brake(const btVector3 &rayOrigin, const btCollisionWorld::ClosestRayResultCallback &rayQuery, float brakeFactor)
{
    accelerate(0);
    btVector3 hitDistance = rayQuery.m_hitPointWorld - rayOrigin;
    hitDistance.setY(0);
    if (hitDistance.length2() < 10000)
    {
        float brakeForce = (250 / Ogre::Math::Log(hitDistance.length2())) * brakeFactor;
        brake(brakeForce);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::steer(float targetSteerRadius)
{
    if (targetSteerRadius > 0.8f)
        targetSteerRadius = 0.8f;
    else if (targetSteerRadius < -0.8f)
        targetSteerRadius = -0.8f;
    
    targetSteerRadius *= -1.f;  //apparently negative value = right, so fixing that

    if (targetSteerRadius > mSteeringValue) //then turn the wheel right
    {
        float steerOffset = (targetSteerRadius - mSteeringValue) / 2.f;   //will most likely have floating point errors
        mSteeringValue += steerOffset * (mDeltaTime / 1000);               //but precision is not needed
    }
    else if (targetSteerRadius < mSteeringValue) //then turn the wheel left
    {
        float steerOffset = (mSteeringValue - targetSteerRadius) / 2.f;
        mSteeringValue -= steerOffset * (mDeltaTime / 1000);
    }

    for (int wheel = 0; wheel <= 1; wheel++)
    {
        mVehicle->setSteeringValue(mSteeringValue, wheel);
    }
}
//-------------------------------------------------------------------------------------------------------
void Vehicle::update(float milliseconds)
{
    mDeltaTime = milliseconds;
    mfSpeed = mVehicle->getCurrentSpeedKmHour();
    for (int iii = 0; iii < 4; iii++)
    {
        btTransform wheeltrans = mVehicle->getWheelTransformWS(iii);
        mWheelNodes[iii]->setPosition(wheeltrans.getOrigin().x(), wheeltrans.getOrigin().y(), wheeltrans.getOrigin().z());
        mWheelNodes[iii]->setOrientation(wheeltrans.getRotation().w(), 
                                         wheeltrans.getRotation().x(),
                                         wheeltrans.getRotation().y(),
                                         wheeltrans.getRotation().z());
    }

    /*if (!checkForVehicleAhead()) //true if vehicle ahead, false if not
    {
        if (mfSpeed < mfTargetSpeed)
        {
            float power = (mfTargetSpeed - mfSpeed) * 50.f;
            accelerate(power);
        }
        else if (mfSpeed > mfTargetSpeed)
        {
            float power = (mfSpeed - mfTargetSpeed) * 50.f;
            brake(power);
        }
    }*/
    mVehicle->updateVehicle((int)milliseconds / 1000);
}
//-------------------------------------------------------------------------------------------------------
bool Vehicle::checkForVehicleAhead()
{
    //get ray starting position in front of the car
    btVector3 rayOrigin = GameState::ogreVecToBullet(mNode->_getDerivedPosition() + (getDirection() * 20));
    btVector3 rayFront = GameState::ogreVecToBullet(getDirection() * 300) + rayOrigin; //300 = range of driver's sight
    btCollisionWorld::ClosestRayResultCallback rayQueryFront(rayOrigin, rayFront);

    btVector3 right = GameState::ogreVecToBullet(getDirection().crossProduct(Ogre::Vector3(0, 1, 0)));
    right.normalize();

    btVector3 rayRight = rayFront + (right * 80) - GameState::ogreVecToBullet(getDirection() * 45);
    btCollisionWorld::ClosestRayResultCallback rayQueryRight(rayOrigin, rayRight);

    btVector3 rayLeft = rayFront - (right * 80) - GameState::ogreVecToBullet(getDirection() * 45);
    btCollisionWorld::ClosestRayResultCallback rayQueryLeft(rayOrigin, rayLeft);

    CDebugDraw *debugDrawer = getGameState()->mDebugDrawer;

    btVector3 rayBackOrigin;
    btVector3 rayBack;
    btCollisionWorld::ClosestRayResultCallback rayQueryBack(rayQueryLeft);  //a hack
    if (mfSpeed < 0.f)
    {
        rayBackOrigin = rayOrigin - GameState::ogreVecToBullet(getDirection() * 20);
        rayBack = -GameState::ogreVecToBullet(getDirection() * 65) + rayBackOrigin;
        rayQueryBack.m_rayFromWorld = rayBackOrigin;
        rayQueryBack.m_rayToWorld = rayBack;
        mDynamicsWorld->rayTest(rayBackOrigin, rayBack, rayQueryBack);
//#ifdef _DEBUG
        debugDrawer->drawRay(rayBackOrigin, rayBack);
//#endif
    }

    mDynamicsWorld->rayTest(rayOrigin, rayFront, rayQueryFront);
    mDynamicsWorld->rayTest(rayOrigin, rayRight, rayQueryRight);
    mDynamicsWorld->rayTest(rayOrigin, rayLeft, rayQueryLeft);

//#ifdef _DEBUG
    debugDrawer->drawRay(rayOrigin, rayFront);
    debugDrawer->drawRay(rayOrigin, rayRight);
    debugDrawer->drawRay(rayOrigin, rayLeft);
//#endif

    bool frontHit = rayQueryFront.hasHit();
    bool rightHit = rayQueryRight.hasHit();
    bool leftHit = rayQueryLeft.hasHit();

    btVector3 hitDistance = rayQueryFront.m_hitPointWorld - rayOrigin;
    hitDistance.setY(0);
    btScalar distanceToHit = hitDistance.length2();

    if (frontHit || rightHit || leftHit) //let the logic begin
    {
        return 1;
    }
    return 0;
}
//-------------------------------------------------------------------------------------------------------