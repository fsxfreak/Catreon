/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef BALL_HPP
#define BALL_HPP

#include "stdafx.h"
#include "objects\Object.hpp"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BtOgMotionState.h"

#include "GameState.hpp"

class Ball : public Object
{
protected:
    /* inherited protected members

    Ogre::Vector3 mPosition;
    Ogre::Quaternion mDirection;

    int mnSpeed;
    btScalar mMass;

    */
private:
    //mbtBall could be static, but we want flexibility in terms of size
    btCollisionShape* mbtBall;

    int mnSize;

    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;

    Ogre::SceneManager *mSceneMgr;

public:
    Ball();
    Ball(Ogre::SceneManager *scenemgr, int size, Ogre::Vector3 position, Ogre::Quaternion direction);
    virtual ~Ball();

    virtual void accelerate(const btScalar &force);
    virtual void decelerate(const btScalar &force);

    virtual void initializePhysics();
    virtual void initializeMaterial();
};

#endif