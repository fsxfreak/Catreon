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
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class BulletPhys
{
private:
    btDefaultCollisionConfiguration         *mCollisionConfiguration;
    btCollisionDispatcher                   *mDispatcher;
    btBroadphaseInterface                   *mBroadphase;
    btSequentialImpulseConstraintSolver     *mSolver;
    btDiscreteDynamicsWorld                 *mDynamicsWorld;

	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

public:
    BulletPhys();
    virtual ~BulletPhys();

    void initPhysics();
    void exitPhysics();

    void addCollisionBox(btCollisionShape* const shape);

    virtual void clientMoveAndDisplay();

    virtual void displayCallBack();
    virtual void clientResetScene();

};