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
#include <random>

#include <objects\World.hpp>

World::World(int numCities)
{
    generateWorld(numCities);
}

World::~World()
{

}

void World::generateWorld(int numCities)
{
    srand(time(0));

    const int min = 750;
    const int max = 3000;

    for (int i = 0; i < numCities; ++i)
    {
        int x = min + (rand() % (max - min + 1));
        int z = min + (rand() % (max - min + 1));

        mCityPositions.push_back(Ogre::Vector3(x, 0, z));
    }

    generateHighways();
}

void World::generateHighways()
{

}

void World::generateStreetGrid()
{

}

