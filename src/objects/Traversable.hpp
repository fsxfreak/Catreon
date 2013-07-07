/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2013

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/** Traversable.hpp
    This is a base class

 */

#ifndef TRAVERSABLE_HPP
#define TRAVERSABLE_HPP

#include "stdafx.h"
#include <GameState.hpp>
#include <objects\Road.hpp>

#include <string>
#include <vector>

struct SceneNodeDeleter
{
    void operator()(Ogre::SceneNode *node) 
    {
        //We know that scenenodes will really only exist in the gamestate
        getGameState()->mSceneMgr->destroySceneNode(node);
    }
};

struct EntityDeleter
{
    void operator()(Ogre::Entity *entity) 
    {
        //We know that entities will really only exist in the gamestate
        getGameState()->mSceneMgr->destroyEntity(entity);
    }
};

class Traversable
{
protected:
    // For the graphical representation
    std::unique_ptr<Ogre::SceneNode, SceneNodeDeleter> mNode;
    std::unique_ptr<Ogre::Entity, EntityDeleter> mEntity;

    // For the pathfinding portion
    std::vector<std::unique_ptr<Road> > mRoads;

    const std::string mName;

private:
    virtual void generateGraphical() = 0;
    virtual void generatePathNodes() = 0;
public:
    Traversable(const Ogre::Vector3& startPosition, const Ogre::Vector3& endPosition,
                const int numLanes, const std::string& name);
    virtual ~Traversable();

    std::string getName() const;
};

#endif