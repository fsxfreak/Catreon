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

Traversable::Traversable(const Ogre::Vector3& startPosition
                       , const Ogre::Vector3& endPosition
                       , const int numLanes
                       , const std::string& name)
    : mNode(nullptr), mEntity(nullptr)
    , mName(name)
{

}

Traversable::~Traversable()
{

}