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

#include <objects\Traversable.hpp>
#include <objects\Node.hpp>

/*
    Need to intialize the Roads in backwards order in order to obtain a pointer
    to the road ahead.

    Perhaps initialize these Roads using a Street class?
*/

class Node;

class Road
{
protected:
    std::string mName;
    
    std::unique_ptr<Ogre::SceneNode, SceneNodeDeleter> mNode;

    Ogre::Vector3 mPosition;
    Ogre::Vector3 mDirection;

    bool mOccupied;
    
    Road(const Road& road);

    btGhostObject *mTriggerNode; //to check if a vehicle is close enough to the center of the node
    void updateTriggerPosition(btTransform& trans);
    void initOther(const Ogre::SceneNode *node);
public:
    Road(const std::string& name, const Ogre::Vector3 &pos,
         const Ogre::Vector3 &dir);
    ~Road();

    void setNextRoad(Road *nextRoad);

    Ogre::Vector3& getPosition();
    Ogre::Vector3& getDirection();
    Road* getNextRoad();
    std::string getName();
    
    Node mNode;

    void occupied(bool occupied);
    bool isOccupied();

    void update();
};


#endif