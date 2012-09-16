/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef BT_OG_MOTIONSTATE_H
#define BT_OG_MOTIONSTATE_H

/********************************************************
TODO

********************************************************/

#include "stdafx.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "framework\AdvancedOgreFramework.hpp"


class BtOgMotionState : public btMotionState
{
public:
    BtOgMotionState(const btTransform &initialPosition, Ogre::SceneNode *node);
    virtual ~BtOgMotionState();

    Ogre::String getName();
    inline void setNode(Ogre::SceneNode *node);

    virtual void getWorldTransform(btTransform &worldTransform) const;

    virtual void setWorldTransform(const btTransform &worldTransform);
protected:
    Ogre::SceneNode *mObject;
    btTransform mPosition;
};

#endif
