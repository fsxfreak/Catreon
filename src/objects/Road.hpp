/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef ROAD_HPP
#define ROAD_HPP

#include "stdafx.h"

#include <string>

#include <BulletCollision\CollisionDispatch\btGhostObject.h>
#include <GameState.hpp>
#include <BtOgMotionState.h>

/*
    Need to intialize the Roads in backwards order in order to obtain a pointer
    to the road ahead.

    Perhaps initialize these Roads using a Street class?
*/

class Road
{
private:
    static long int mRoadsCreated;
    std::string mName;

    Road *mNextRoad;
    Ogre::Vector3 mPosition;
    Ogre::Vector3 mDirection;

    bool occupied;
    unsigned int cost;

    Road(const Road& road);

    btRigidBody *mTriggerNode; //to check if a vehicle is close enough to the center of the node
    void updateTriggerPosition(btTransform& trans);
    void initOther();
public:
    Road();
    Road(const Ogre::Vector3 &pos);
    Road(const Ogre::Vector3 &pos, Road *nextRoad);

    ~Road();

    void replaceNextRoad(Road *nextRoad);
    Road* getNextRoad();
    std::string getName();
    Ogre::Vector3& getPosition();

    void update();
};

#endif