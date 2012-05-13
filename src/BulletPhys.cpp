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
#include "BulletPhys.h"

//-------------------------------------------------------------------------------------------------------
BulletPhys::BulletPhys() :  mCollisionConfiguration(new btDefaultCollisionConfiguration()),
                            mDispatcher(new btCollisionDispatcher(mCollisionConfiguration)),
                            mBroadphase(new btDbvtBroadphase()),
                            mSolver(new btSequentialImpulseConstraintSolver),
                            mDynamicsWorld(new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration))

{
    mDynamicsWorld->setGravity(btVector3(0, -10, 0));
    //ground rigid body
    btCollisionShape* shapeGround = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.0)));
    mCollisionShapes.push_back(shapeGround);
}
//-------------------------------------------------------------------------------------------------------
BulletPhys::~BulletPhys()
{
    exitPhysics();
}
//-------------------------------------------------------------------------------------------------------
void BulletPhys::exitPhysics()
{

}
//-------------------------------------------------------------------------------------------------------
btDiscreteDynamicsWorld *BulletPhys::getWorld()
{
    return mDynamicsWorld;
}
//-------------------------------------------------------------------------------------------------------
void BulletPhys::addCollisionBox(btCollisionShape* const shape)
{
    mCollisionShapes.push_back(shape);
}
//-------------------------------------------------------------------------------------------------------
void BulletPhys::clientMoveAndDisplay()
{

}
//-------------------------------------------------------------------------------------------------------
void BulletPhys::displayCallBack()
{

}
//-------------------------------------------------------------------------------------------------------
void BulletPhys::clientResetScene()
{

}
//-------------------------------------------------------------------------------------------------------