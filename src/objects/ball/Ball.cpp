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
Ball::Ball() : mnSize(1), Object(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0, 0, 0))
{

}
//-------------------------------------------------------------------------------------------------------
Ball::Ball(float size, Ogre::Vector3 position, Ogre::Vector3 direction = Ogre::Vector3(0, 0, 0)) 
    :   mnSize(size), 
        Object(position, direction)
{
    initializeMaterial();
    accelerate(5000.0f * mnSize, getGameState()->mCamera->getDerivedDirection().normalisedCopy());
    //just a little initial acceleration to get the ball moving(see what I did there)
}
//-------------------------------------------------------------------------------------------------------
Ball::~Ball()
{
    delete mbtBallShape;
    delete mNode;
    delete mEntity;
}
//-------------------------------------------------------------------------------------------------------
void Ball::accelerate(const btScalar &force)
{

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
    
    btVector3 btPosition = GameState::ogreVecToBullet(mPosition);
    btTransform ballTransform;
    ballTransform.setIdentity();

    btScalar ballMass(1);   //any positive value = dynamic body
    btVector3 ballInertia(0, 0, 0);
    mbtBallShape->calculateLocalInertia(ballMass, ballInertia);

    ballTransform.setOrigin(btPosition);

    BtOgMotionState *ballState = new BtOgMotionState(ballTransform, mNode);
    btRigidBody::btRigidBodyConstructionInfo ballInfo(ballMass, ballState, mbtBallShape, ballInertia);
    mbtBallBody = new btRigidBody(ballInfo);

    mbtBallBody->setFriction(900);      //some arbitrary friction number that doesn't seem to work
    mbtBallBody->setRestitution(50);    //bounciness
    mbtBallBody->setDamping(0.1, 0.1);  //rate at which ball loses speed (0.0 - 1.0)

    getGameState()->mDynamicsWorld->addRigidBody(mbtBallBody);
    getGameState()->mRigidBodies.push_back(mbtBallBody);
}
//-------------------------------------------------------------------------------------------------------
void Ball::initializeMaterial()
{
    mEntity = getGameState()->mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
    mEntity->setMaterialName("Examples/BumpyMetal");
    mNode = getGameState()->mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition, Ogre::Quaternion(0, 0, 0, 0));
    float ballsize = mnSize / 50.0f + 0.02;    //have to translate scale of PT_SPHERE
    mNode->setScale(ballsize, ballsize, ballsize);
    mNode->attachObject(mEntity);

    initializePhysics();
}
