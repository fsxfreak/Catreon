/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2013

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/** Street.hpp

 */

#ifndef STREET_HPP
#define STREET_HPP

#include "stdafx.h"
#include <GameState.hpp>
#include <objects\Traversable.hpp>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BtOgMotionState.h"

class Street : public Traversable
{
private:
    virtual void generateGraphical();
    virtual void generatePhysics();
    virtual void generatePathNodes();
public:
    Street(const Ogre::Vector3& startPosition, const Ogre::Vector3& endPosition,
           const Traversable::RoadType roadType, const std::string& name);
    ~Street();
};

#endif