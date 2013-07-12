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
#include <objects\Traversable.hpp>

#include <string>

void SceneNodeDeleter::operator()(Ogre::SceneNode *node) 
{
    //We know that scenenodes will really only exist in the gamestate
    getGameState()->mSceneMgr->destroySceneNode(node);
}

void EntityDeleter::operator()(Ogre::Entity *entity)
{
    getGameState()->mSceneMgr->destroyEntity(entity);
}

Traversable::Traversable(const Ogre::Vector3& startPosition
                       , const Ogre::Vector3& endPosition
                       , const Traversable::RoadType roadType
                       , const std::string& name)
    : mNode(nullptr), mEntity(nullptr)
    , mStartPosition(startPosition), mEndPosition(endPosition)
    , mRoadType(roadType)
    , mName(name)
{

}

Traversable::~Traversable()
{

}

std::pair<Ogre::SceneNode*, Ogre::Entity*> 
     Traversable::generatePlane(const Ogre::Real &width,
                                const Ogre::Real &height,
                                const std::string &baseName,
                                const std::string &materialName,
                                const Ogre::Vector3 &position)
{
    Ogre::Plane plane(Ogre::Vector3(0, 1, 0), 0);
    std::string planeMeshName = baseName + "_plane";
    
    Ogre::MeshPtr planePtr = Ogre::MeshManager::getSingletonPtr()->createPlane(
        planeMeshName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane,
        width, height,
        1, 1, true, 1U, 1.0F, 1.0F, 
        Ogre::Vector3::UNIT_Z);
    planePtr->buildEdgeList();
    planePtr->prepareForShadowVolume();

    Ogre::Entity* entity = getGameState()->mSceneMgr->
        createEntity(baseName + "_entity", planeMeshName);
    entity->setMaterialName(materialName);

    Ogre::SceneNode* node = getGameState()->mSceneMgr->getRootSceneNode()->
        createChildSceneNode(baseName, position);
    node->attachObject(entity);

    return std::pair<Ogre::SceneNode*, Ogre::Entity*>(node, entity);
}

std::string Traversable::getName() const
{
    return mName;
}

int Traversable::getNumLanes(RoadType type)
{
    switch (type)
    {
    case ONCOMING_TWO_LANE:
        return 2;
    case ONCOMING_FOUR_LANE:
        return 4;
    case ONCOMING_SIX_LANE:
        return 6;
    case DIVIDED_FOUR_LANE:
        return 4;
    case DIVIDED_SIX_LANE:
        return 6;
    case DIVIDED_EIGHT_LANE:
        return 8;
    default:
        return 0;
    }
}
