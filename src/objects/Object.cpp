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
Object::Object() :  mDirection(0, 0, 0), mPosition(0, 0, 0), mnSpeed(0)
{
    //mObjectNode(new Ogre::SceneNode(mSceneMgr->getRootSceneNode()->
                                        //createChildSceneNode(mPosition, Ogre::Quaternion(0, 0, 0, 0))));
}
//-------------------------------------------------------------------------------------------------------
Object::~Object()
{
    //pure virtual, no implementation
}
//-------------------------------------------------------------------------------------------------------
Object::Object(Ogre::Vector3 direction, Ogre::Vector3 position, int initialspeed) : 
                                        mDirection(direction), mPosition(position), mnSpeed(initialspeed) 
{

}
//-------------------------------------------------------------------------------------------------------
void Object::accelerate(const Ogre::Vector3 &direction, const btScalar &force)
{

}
//-------------------------------------------------------------------------------------------------------
void Object::decelerate(const btScalar &force)
{

}