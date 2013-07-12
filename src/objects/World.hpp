/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2013

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/** World.hpp
    This class is responsible for generating the world

 */

#ifndef WORLD_HPP
#define WORLD_HPP

#include "stdafx.h"
#include <GameState.hpp>

#include <string>

#include <objects\Street.hpp>

class World
{
private:
    /**
        Starts the world generation by placing cities in random positions.
        Cities are at most 6000 units far from each other.
    */
    void generateWorld(int numCities = 3);

    /**
        Connects the randomly generated city locations with a highway Street
    */
    void generateHighways();

    /**
        Generates the actual city streets to be traversed around by Vehicles
    */
    void generateStreetGrid();

    // Below are extra functions to be implemented later
    void generateStreetsRadial();
    void generateStreetsFitToEnv();
    void generateEnvironment(); 

    void saveWorld(const std::string &worldName);
    void loadWorld(const std::string &worldName);

    std::vector<Ogre::Vector3> mCityPositions;
    std::vector<Traversable*> mStreets;

public:
    World(int numCities = 3);
    ~World();
};

#endif