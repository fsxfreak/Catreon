/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef FORKING_ROAD_HPP
#define FORKING_ROAD_HPP

#include "stdafx.h"

#include <GameState.hpp>
#include <objects\Road.hpp>

class ForkingRoad : public Road
{
private:
    std::string mNameNextForkRoad;
    Road *mNextForkRoad;
public:
    ForkingRoad(const Ogre::SceneNode *node);
    ~ForkingRoad();

    void obtainNextForkRoad();  //gets the fork road
    void replaceNextForkRoad(Road *nextForkRoad);
    
    Road* getNextForkRoad();
    unsigned int getCostForkRoad();
};


#endif
