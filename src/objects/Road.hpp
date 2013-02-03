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
    std::string mName;
    std::string mNameNextRoad;

    Road *mNextRoad;
    Ogre::Vector3 mPosition;
    Ogre::Vector3 mDirection;

    bool mOccupied;
    unsigned int mCost;

    Road(const Road& road);

    btGhostObject *mTriggerNode; //to check if a vehicle is close enough to the center of the node
    void updateTriggerPosition(btTransform& trans);
    void initOther(const Ogre::SceneNode *node);
public:
    Road(const Ogre::SceneNode *node);

    ~Road();

    //called after all Road nodes have been generated, so we can build links to next nodes
    //requires the name of the next road to be set
    void obtainNextRoad();
    void replaceNextRoad(Road *nextRoad);

    Ogre::Vector3& getPosition();
    Road* getNextRoad();
    std::string getName();
    unsigned int getCost();
    
    void occupied(bool occupied);

    bool isOccupied();

    void update();
};

#endif