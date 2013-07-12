/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2013

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO

********************************************************/

#include "stdafx.h"
#include <objects\Street.hpp>

Street::Street(const Ogre::Vector3& startPosition, const Ogre::Vector3& endPosition
             , const Traversable::RoadType roadType, const std::string& name)
    : Traversable(startPosition, endPosition, roadType, name)
{
    generateGraphical();
    generatePathNodes();
}

Street::~Street()
{

}

void Street::generateGraphical()
{
    Ogre::Vector3 roadDirection = mStartPosition - mEndPosition;

    //See Traversable.hpp RoadType enum for documentation on calculations.
    int median = mRoadType < RoadType::MAX_ONCOMING_LANES ?
        Traversable::ONCOMING_MEDIAN_WIDTH : Traversable::DIVIDED_MEDIAN_WIDTH;
    Ogre::Real width = Traversable::getNumLanes(mRoadType) * 
        Traversable::LANE_WIDTH + median;
    Ogre::Real height = roadDirection.length();

    std::pair<Ogre::SceneNode*, Ogre::Entity*> nodeAndEntity = 
        generatePlane(width, height, mName, "Examples/GrassFloor", roadDirection / 2);

    mNode = std::unique_ptr<Ogre::SceneNode, SceneNodeDeleter>
        (nodeAndEntity.first, SceneNodeDeleter());
    mEntity = std::unique_ptr<Ogre::Entity, EntityDeleter>
        (nodeAndEntity.second, EntityDeleter());

    Ogre::Radian roadAngle = Ogre::Vector3(0, 0, 1).angleBetween(roadDirection);
    mNode->yaw(roadAngle);
}

void Street::generatePhysics()
{

}

void Street::generatePathNodes()
{

}
