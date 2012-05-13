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
    delete mCollisionConfiguration;
    delete mDispatcher;
    delete mBroadphase;
    delete mSolver;
    delete mDynamicsWorld;

    exitPhysics();
}
//-------------------------------------------------------------------------------------------------------
void BulletPhys::exitPhysics()
{

}
//-------------------------------------------------------------------------------------------------------
void BulletPhys::addCollisionShape(btCollisionShape* const shape)
{
    mCollisionShapes.push_back(shape);
}
//-------------------------------------------------------------------------------------------------------
btVector3 BulletPhys::ogreVecToBullet(const Ogre::Vector3 &ogrevector)
{
    return btVector3(ogrevector.x, ogrevector.y, ogrevector.z);
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3 BulletPhys::bulletVecToOgre(const btVector3 &bulletvector)
{
    return Ogre::Vector3(bulletvector.x(), bulletvector.y(), bulletvector.z());
}
//-------------------------------------------------------------------------------------------------------
btRigidBody* createTestSphere(const btCollisionShape *sphere, const btVector3 &spherePosition)
{

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