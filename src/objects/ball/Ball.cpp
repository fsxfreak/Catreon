/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-All done!
********************************************************/

#include "stdafx.h"
#include "objects\ball\Ball.hpp"

//-------------------------------------------------------------------------------------------------------
Ball::Ball() : mnSize(1)
{
    initializeMaterial();
    accelerate(50.0f * mnSize, getGameState()->mCamera->getDerivedDirection().normalisedCopy());
}
//-------------------------------------------------------------------------------------------------------
Ball::Ball(float size, float initacceleration, Ogre::Vector3 position, Ogre::Vector3 direction = Ogre::Vector3(0, 0, 0)) 
    :   mnSize(size)
{
    initializeMaterial();
    accelerate(initacceleration * mnSize, getGameState()->mCamera->getDerivedDirection().normalisedCopy());
    //just a little initial acceleration to get the ball moving(see what I did there)
}
//-------------------------------------------------------------------------------------------------------
Ball::~Ball()
{
    if (mbtBallShape)    
        delete mbtBallShape;
    getGameState()->mSceneMgr->destroyEntity(mEntity);
    getGameState()->mSceneMgr->destroySceneNode(mNode);
}
//-------------------------------------------------------------------------------------------------------
void Ball::accelerate(const btScalar &force, const Ogre::Vector3 &direction)
{

    btVector3 forcedir = GameState::ogreVecToBullet(direction * force);
    mbtBallBody->applyCentralForce(forcedir);
}
//-------------------------------------------------------------------------------------------------------
void Ball::decelerate(const btScalar &force)
{
    accelerate(-force, Ogre::Vector3(0, 0, 0)); //something to shut the compiler up
}
//-------------------------------------------------------------------------------------------------------
void Ball::initializePhysics()
{
    mbtBallShape = new btSphereShape(mnSize);
    getGameState()->mCollisionShapes.push_back(mbtBallShape);
    
    btVector3 btPosition = GameState::ogreVecToBullet(mPosition);
    btTransform ballTransform;
    ballTransform.setIdentity();

    btScalar ballMass(30);   //any positive value = dynamic body
    btVector3 ballInertia(0, 0, 0);
    mbtBallShape->calculateLocalInertia(ballMass, ballInertia);

    ballTransform.setOrigin(btPosition);

    BtOgMotionState *ballState = new BtOgMotionState(ballTransform, mNode);
    btRigidBody::btRigidBodyConstructionInfo ballInfo(ballMass, ballState, mbtBallShape, ballInertia);
    mbtBallBody = new btRigidBody(ballInfo);

    mbtBallBody->setFriction(100);      //some arbitrary friction number that doesn't seem to work
    mbtBallBody->setRestitution(50);    //bounciness
    mbtBallBody->setDamping(0.1f, 0.1f);  //rate at which ball loses speed (0.0 - 1.0)

    getGameState()->mDynamicsWorld->addRigidBody(mbtBallBody);
    getGameState()->mRigidBodies.push_back(mbtBallBody);
}
//-------------------------------------------------------------------------------------------------------
void Ball::initializeMaterial()
{
    mEntity = getGameState()->mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
    mEntity->setMaterialName("Examples/Rockwall");
    mNode = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition, Ogre::Quaternion(0, 0, 0, 0));
    float ballsize = mnSize / 50.0f + 0.02;    //have to translate scale of PT_SPHERE
    mNode->setScale(ballsize, ballsize, ballsize);
    mNode->attachObject(mEntity);

    initializePhysics();
}
