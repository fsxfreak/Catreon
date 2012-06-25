/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "stdafx.h"
#include "framework\AdvancedOgreFramework.hpp"
//#include "GameState.hpp"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BtOgMotionState.h"

class Object
{
protected:
    Ogre::Vector3 mPosition;
    Ogre::Vector3 mDirection;

    btScalar mMass;

    Object();
    Object(Ogre::Vector3 position, Ogre::Vector3 direction);

    virtual ~Object();

    virtual void initializePhysics() = 0;
    virtual void initializeMaterial() = 0;

public:
    virtual void accelerate(const btScalar &force) = 0; 
    virtual void decelerate(const btScalar &force) = 0;
};

#endif
