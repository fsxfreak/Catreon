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

#include <objects\Road.hpp>

const btVector3 TRIGGER_SIZE(10, 10, 10);

//-------------------------------------------------------------------------------------------------------
Road::Road() : mPosition(Ogre::Vector3(0, 0, 0)), mNextRoad(nullptr)
{
    mDirection = Ogre::Vector3::UNIT_Z;

    initPhysics();
}
//-------------------------------------------------------------------------------------------------------
Road::Road(const Ogre::Vector3 &pos) : mPosition(pos), mNextRoad(nullptr)
{
    mDirection = Ogre::Vector3::UNIT_Z;

    initPhysics();
}
//-------------------------------------------------------------------------------------------------------
Road::Road(const Ogre::Vector3 &pos, Road *nextRoad) : mPosition(pos), mNextRoad(nextRoad)
{
    mDirection = (nextRoad->getPosition() - mPosition).normalisedCopy();

    initPhysics();
}
//-------------------------------------------------------------------------------------------------------
Road::~Road()
{
    delete mTriggerNode;
}
//-------------------------------------------------------------------------------------------------------
void Road::initPhysics()
{
    mTriggerNode = new btGhostObject();
    btCollisionShape *shape = new btBoxShape(TRIGGER_SIZE);
    mTriggerNode->setCollisionShape(shape);
    btTransform transform;
    updateTriggerPosition(transform);
    mTriggerNode->setWorldTransform(transform);
    mTriggerNode->setCollisionFlags(mTriggerNode->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    getGameState()->mDynamicsWorld->addCollisionObject(mTriggerNode);
}
//-------------------------------------------------------------------------------------------------------
void Road::replaceNextRoad(Road *nextRoad)
{
    mNextRoad = nextRoad;
    mDirection = (nextRoad->getPosition() - mPosition).normalisedCopy();

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
Ogre::Vector3& Road::getPosition()
{
    return mPosition;
}