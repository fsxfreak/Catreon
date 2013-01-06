/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Include position, pointer to next node, bool occupied, 

-Derive class to include info about lanes beside current lane
-
********************************************************/

#include "stdafx.h"

#include <sstream>
#include <iostream>

#include <objects\Road.hpp>

const btVector3 TRIGGER_SIZE(10, 11, 10);
long int Road::mRoadsCreated = 0;
//-------------------------------------------------------------------------------------------------------
Road::Road() : mPosition(Ogre::Vector3(0, 0, 0)), mNextRoad(nullptr), occupied(false), cost(1)
{
    mDirection = Ogre::Vector3::UNIT_Z;

    initOther();
}
//-------------------------------------------------------------------------------------------------------
Road::Road(const Ogre::Vector3 &pos) : mPosition(pos), mNextRoad(nullptr), occupied(false), cost(1)
{
    mDirection = Ogre::Vector3::UNIT_Z;

    initOther();
}
//-------------------------------------------------------------------------------------------------------
Road::Road(const Ogre::Vector3 &pos, Road *nextRoad) : mPosition(pos), mNextRoad(nextRoad)
                                                     , occupied(false)
{
    mDirection = nextRoad->getPosition() - mPosition;
    cost = mDirection.squaredLength();
    mDirection.normalise();

    initOther();
}
//-------------------------------------------------------------------------------------------------------
Road::~Road()
{
    //mNextRoad is deleted with whoever created it
    delete mTriggerNode;
}
//-------------------------------------------------------------------------------------------------------
void Road::initOther()
{
    std::ostringstream oss;
    oss << mRoadsCreated;
    mName = "Road_";
    mName += oss.str();

    ++mRoadsCreated;

    mTriggerNode = new btGhostObject();
    btCollisionShape *shape = new btBoxShape(TRIGGER_SIZE);
    mTriggerNode->setCollisionShape(shape);
    btTransform transform;
    updateTriggerPosition(transform);
    mTriggerNode->setWorldTransform(transform);
    mTriggerNode->setCollisionFlags(mTriggerNode->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mTriggerNode->setUserPointer(this, ROAD);
    getGameState()->mDynamicsWorld->addCollisionObject(mTriggerNode);

}
//-------------------------------------------------------------------------------------------------------
void Road::replaceNextRoad(Road *nextRoad)
{
    mNextRoad = nextRoad;
    mDirection = nextRoad->getPosition() - mPosition;
    cost = mDirection.squaredLength();
    mDirection.normalise();

    btTransform transform;
    updateTriggerPosition(transform);
    mTriggerNode->setWorldTransform(transform);
}
//-------------------------------------------------------------------------------------------------------
void Road::updateTriggerPosition(btTransform& trans)
{
    trans.setIdentity();
    trans.setOrigin(GameState::ogreVecToBullet(mPosition));
    Ogre::Quaternion rotation = Ogre::Vector3::UNIT_Z.getRotationTo(mDirection);
    trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
}
//-------------------------------------------------------------------------------------------------------
Road* Road::getNextRoad()
{
    return mNextRoad;
}
//-------------------------------------------------------------------------------------------------------
std::string Road::getName()
{
    return mName;
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3& Road::getPosition()
{
    return mPosition;
}
//-------------------------------------------------------------------------------------------------------
void Road::update()
{
    /*int numOverlapping = mTriggerNode->getNumOverlappingObjects();
    for (int iii = 0; iii < numOverlapping; ++iii)
    {
        btRigidBody *body = dynamic_cast<btRigidBody*>(mTriggerNode->getOverlappingObject(iii));
        if (body->getUserPointerType() == VEHICLE)
        {
            Vehicle *vehicle = static_cast<Vehicle*>(body->getUserPointer());
            vehicle->inRoad(mName);
        }
    }*/
}