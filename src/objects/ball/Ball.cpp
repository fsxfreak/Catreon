/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Make it so that initializing Ball initializes everything 
to do with the ball
=Create the entity (PT_SPHERE), give entity a material
=Using position passed in as a param, create a scenenode there
=Scale node accordingly to size, attach entity to node
=Physics time, initialize collison shape (btSphereShape)
=Give bullet the position acquired from param
=Calculate mass and inertia
=use BtOgMotionState, give it the bttransform and the node
=construct a btRigidBody with info
********************************************************/

#include "stdafx.h"
#include "objects\ball\Ball.hpp"

//-------------------------------------------------------------------------------------------------------
Ball::Ball() : mnSize(1), Object(Ogre::Vector3(0, 0, 0), Ogre::Quaternion(0, 0, 0, 0)), scenemgr(0)
{

}
//-------------------------------------------------------------------------------------------------------
Ball::Ball(Ogre::SceneManager *scenemgr, int size, Ogre::Vector3 position, 
                                Ogre::Quaternion direction = Ogre::Quaternion(0, 0, 0, 0)) 
    :   mSceneMgr(scenemgr),
        mnSize(size), 
        Object(position, direction)
{
    initializeMaterial();
    initializePhysics();
}
//-------------------------------------------------------------------------------------------------------
Ball::~Ball()
{
    delete mbtBall;
}
//-------------------------------------------------------------------------------------------------------
void Ball::accelerate(const btScalar &force)
{

}
//-------------------------------------------------------------------------------------------------------
void Ball::decelerate(const btScalar &force)
{

}
//-------------------------------------------------------------------------------------------------------
void Ball::initializePhysics()
{

}
//-------------------------------------------------------------------------------------------------------
void Ball::initializeMaterial()
{
    mEntity = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
    mEntity->setMaterialName("Examples/BumpyMetal");
}