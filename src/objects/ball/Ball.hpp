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
    Ogre::Vector3 mDirection;

    */
private:
    //mbtBall could be static, but we want flexibility in terms of size
    btCollisionShape *mbtBallShape;
    btRigidBody *mbtBallBody;

    float mnSize; //mnSize = actual radius in meters

    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;

    virtual void initializePhysics();
    virtual void initializeMaterial();

public:
    Ball();
    //according to copy elision, passing by value is most efficient here
    Ball(float size, float initacceleration, Ogre::Vector3 position, Ogre::Vector3 direction);
    virtual ~Ball();

    virtual void accelerate(const btScalar &force, const Ogre::Vector3 &direction);
    virtual void decelerate(const btScalar &force);
};

#endif