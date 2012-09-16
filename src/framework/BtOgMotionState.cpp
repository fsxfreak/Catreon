/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO

********************************************************/

#include "stdafx.h"
#include <BtOgMotionState.h>
#include "GameState.hpp"

//-------------------------------------------------------------------------------------------------------
BtOgMotionState::BtOgMotionState(const btTransform &initialposition, Ogre::SceneNode *node)
{
    mObject = node;
    mPosition = initialposition;
}
//-------------------------------------------------------------------------------------------------------
BtOgMotionState::~BtOgMotionState()
{
}
//-------------------------------------------------------------------------------------------------------
void BtOgMotionState::setNode(Ogre::SceneNode *node)
{
    mObject = node;
}
//-------------------------------------------------------------------------------------------------------
void BtOgMotionState::getWorldTransform(btTransform &worldTransform) const
{
    worldTransform = mPosition;
}
//-------------------------------------------------------------------------------------------------------
void BtOgMotionState::setWorldTransform(const btTransform &worldTransform)
{
    if (mObject == NULL)
        return;
    btQuaternion rotation = worldTransform.getRotation();
    mObject->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    btVector3 position = worldTransform.getOrigin();
    mObject->setPosition(position.x(), position.y(), position.z());
}
//-------------------------------------------------------------------------------------------------------
Ogre::String BtOgMotionState::getName()
{
    return mObject->getName();
}
//-------------------------------------------------------------------------------------------------------