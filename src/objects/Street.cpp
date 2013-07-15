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
    generatePhysics();
    generatePathNodes();
}

Street::~Street()
{

}

void Street::generateGraphical()
{
    const Ogre::Vector3 roadDirection = mStartPosition - mEndPosition;

    //See Traversable.hpp RoadType enum for documentation on calculations.
    int median = mRoadType < RoadType::MAX_ONCOMING_LANES ?
        Traversable::ONCOMING_MEDIAN_WIDTH : Traversable::DIVIDED_MEDIAN_WIDTH;
    mWidth = Traversable::getNumLanes(mRoadType) * 
        Traversable::LANE_WIDTH + median;
    mHeight = roadDirection.length();

    std::pair<Ogre::SceneNode*, Ogre::Entity*> nodeAndEntity = 
        generatePlane(mWidth, mHeight, mName, "Examples/GrassFloor", roadDirection / 2);

    mNode = std::unique_ptr<Ogre::SceneNode, SceneNodeDeleter>
        (nodeAndEntity.first, SceneNodeDeleter());
    mEntity = std::unique_ptr<Ogre::Entity, EntityDeleter>
        (nodeAndEntity.second, EntityDeleter());

    Ogre::Radian roadAngle = Ogre::Vector3(0, 0, 1).angleBetween(roadDirection);
    mNode->yaw(roadAngle);
}

void Street::generatePhysics()
{
    const Ogre::Vector3 roadCenter = (mStartPosition - mEndPosition) / 2;

    btTransform tr;
    tr.setIdentity();

    tr.setOrigin(GameState::ogreVecToBullet(roadCenter));
    Ogre::Quaternion orientation = mNode->getOrientation();
    tr.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
    //Width and height are multiplied by 0.5 because btBoxShape takes half extents
    btCollisionShape *roadSectionShape = new btBoxShape(btVector3(mWidth * 0.5, 0, mHeight * 0.5));
    getGameState()->mCollisionShapes.push_back(roadSectionShape);

    BtOgMotionState *motionState = new BtOgMotionState(tr, mNode.get());
    btRigidBody::btRigidBodyConstructionInfo conInfo(0, motionState, roadSectionShape);
    mbtRigidBody = new btRigidBody(conInfo);

    getGameState()->mDynamicsWorld->addRigidBody(mbtRigidBody);

    mbtRigidBody->setFriction(200);

    getGameState()->mRigidBodies.push_back(mbtRigidBody);

}

void Street::generatePathNodes()
{
    //First, we handle the lanes heading in one direction (not the band)
    int numLanes = getNumLanes(mRoadType);
    Ogre::Vector3 roadDirection = mStartPosition - mEndPosition;
    int medianWidth = mRoadType < RoadType::MAX_ONCOMING_LANES ?
        Traversable::ONCOMING_MEDIAN_WIDTH : Traversable::DIVIDED_MEDIAN_WIDTH;
    for (int i = 0; i < numLanes / 2; ++i)
    {
        /*
            From the lane closest to the median, to the farthest
            One node will be at one end, one node at the other
        */
        //Adjust the position to be at one end of the road
        Ogre::Vector3 position = roadDirection / 2;
        position -= roadDirection.normalisedCopy() * (mHeight / 2);

        //Adjust the position to be within one lane
        Ogre::Vector3 offsetDirection = Ogre::Vector3(0, 1, 0).crossProduct(-roadDirection);
        offsetDirection.normalise();

        position += offsetDirection * i * Traversable::LANE_WIDTH 
                        + Traversable::LANE_WIDTH / 2   //Offset by lane
                  + offsetDirection * medianWidth / 2;   //Offset by median

        //Road road = new Road(position);

        //Adjust the position to be at the other end of the road
        position += roadDirection.normalisedCopy() * mHeight;

        //Road road = new Road(position);
    }

    //Now, the other flow of traffic
    for (int i = 0; i < numLanes / 2; ++i)
    {
        Ogre::Vector3 position = roadDirection / 2;
        position += roadDirection.normalisedCopy() * (mHeight / 2);

        Ogre::Vector3 offsetDirection = Ogre::Vector3(0, 1, 0).crossProduct(roadDirection);
        offsetDirection.normalise();
        
        position -= offsetDirection * i * Traversable::LANE_WIDTH
                        + Traversable::LANE_WIDTH / 2
                  + offsetDirection * medianWidth / 2;

        //Road road = new Road(position);

        position += roadDirection.normalisedCopy() * mHeight;

        //Road road = new Road(position);
    }

}
