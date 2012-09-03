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
-Create derived classes of this class
-Initialize physics, sound, and graphics for the vehicle object
********************************************************/

#include "stdafx.h"
#include "objects\Object.hpp"
//-------------------------------------------------------------------------------------------------------
Object::Object(Ogre::Vector3 position = Ogre::Vector3(0, 15, 0), Ogre::Vector3 direction = Ogre::Vector3(0, 0, 0))
    : mPosition(position), mDirection(direction)
{
    //mObjectNode = GameState::mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition, mDirection);
}
//-------------------------------------------------------------------------------------------------------
Object::~Object()
{

}
//-------------------------------------------------------------------------------------------------------
void Object::accelerate()
{
    //pure virtual
}
//-------------------------------------------------------------------------------------------------------
void Object::decelerate()
{
    //pure virtual
}
//-------------------------------------------------------------------------------------------------------
void Object::initializePhysics()
{
    //pure virtual
}
//-------------------------------------------------------------------------------------------------------
void Object::initializeMaterial()
{
    //pure virtual
}
