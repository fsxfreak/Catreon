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

const btVector3 TRIGGER_SIZE(10, 5, 10);
//-------------------------------------------------------------------------------------------------------
Road::Road(const Ogre::SceneNode* node) : mNextRoad(nullptr), mOccupied(false), mCost(0)
                                        , mTriggerNode(nullptr)
{
    mDirection = Ogre::Vector3::UNIT_Z;
    mPosition = node->getPosition() + Ogre::Vector3(0, 15, 0);
    if (!node->getUserAny().isEmpty())
    {
        mNameNextRoad = Ogre::any_cast<std::string>(node->getUserAny());
        initOther(node);
    }
}
//-------------------------------------------------------------------------------------------------------
Road::~Road()
{
    //mNextRoad is deleted with whoever created it
    mNextRoad = nullptr;
    delete mTriggerNode;
}
//-------------------------------------------------------------------------------------------------------
void Road::initOther(const Ogre::SceneNode *node)
{
    mName = node->getName();

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
void Road::obtainNextRoad()
{
    if (mNameNextRoad == std::string("nullLocator"))
    {
        mNextRoad = nullptr;
        return;
    }

    std::vector<Road*> &roadVector = getGameState()->mRoads;
    auto it = roadVector.begin();
    auto itend = roadVector.end();
    for (it; it != itend; ++it)
    {
        if ((*it)->getName() == mNameNextRoad)
        {
            replaceNextRoad((*it));
        }
    }
}
//-------------------------------------------------------------------------------------------------------
void Road::replaceNextRoad(Road *nextRoad)
{
    mNextRoad = nextRoad;
    mDirection = nextRoad->getPosition() - mPosition;
    mCost = mDirection.squaredLength();
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
Ogre::Vector3& Road::getPosition()
{
    return mPosition;
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
unsigned int Road::getCost()
{
    return mCost;
}
//-------------------------------------------------------------------------------------------------------
void Road::occupied(bool occupied)
{
    mOccupied = occupied;
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

    //This function might not be needed, because Vehicle updates Road
}