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
//#include "framework\AdvancedOgreFramework.hpp"
//#include "GameState.hpp"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BtOgMotionState.h"

class Object
{
protected:
    Ogre::Vector3 mDirection;
    Ogre::Vector3 mPosition;

    std::unique_ptr<Ogre::SceneNode> mObjectNode;

    int mnSpeed;

    Object();
    Object(Ogre::Vector3 direction, Ogre::Vector3 position, int initialspeed);

    virtual ~Object() = 0;

public:
    virtual void accelerate(const Ogre::Vector3 &direction, const btScalar &force); 
    virtual void decelerate(const btScalar &force);

private:
    Object(const Object& obj);
    Object(Object &&obj);

};

#endif
