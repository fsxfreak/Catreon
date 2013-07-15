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
#include <utility>

#include <memory>

struct SceneNodeDeleter
{
    void operator()(Ogre::SceneNode *node);
};

struct EntityDeleter
{
    void operator()(Ogre::Entity *entity);
};

class Traversable
{
public:
    enum RoadType;
    const static int LANE_WIDTH = 30;
    const static int ONCOMING_MEDIAN_WIDTH = 9;
    const static int DIVIDED_MEDIAN_WIDTH = 18;

protected:
    // For the graphical representation
    std::unique_ptr<Ogre::SceneNode, SceneNodeDeleter> mNode;
    std::unique_ptr<Ogre::Entity, EntityDeleter> mEntity;

    btRigidBody *mbtRigidBody;

    const Ogre::Vector3 mStartPosition;
    const Ogre::Vector3 mEndPosition;
    int mWidth;
    int mHeight;
    const Traversable::RoadType mRoadType;

    // For the pathfinding portion
    std::vector<std::unique_ptr<Road> > mRoads;

    const std::string mName;

    /* Helper function to create a plane.
    */
    std::pair<Ogre::SceneNode*, Ogre::Entity*> generatePlane(
                       const Ogre::Real &width,
                       const Ogre::Real &height,
                       const std::string &baseName,
                       const std::string &materialName,
                       const Ogre::Vector3 &position);

private:
    virtual void generateGraphical() = 0;
    virtual void generatePhysics() = 0;
    virtual void generatePathNodes() = 0;
public:
    Traversable(const Ogre::Vector3& startPosition, const Ogre::Vector3& endPosition,
                const Traversable::RoadType roadType, const std::string& name);
    virtual ~Traversable();

    std::string getName() const;

    enum RoadType
    {
        ONCOMING_TWO_LANE,
        ONCOMING_FOUR_LANE,
        ONCOMING_SIX_LANE,
        MAX_ONCOMING_LANES,

        DIVIDED_FOUR_LANE,
        DIVIDED_SIX_LANE,
        DIVIDED_EIGHT_LANE
    };

    static int getNumLanes(RoadType type);
};

#endif