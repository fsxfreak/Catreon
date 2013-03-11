/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "stdafx.h"

#include <objects\Node.hpp>

//-------------------------------------------------------------------------------------------------------
Node::Node(Road *road) : mThisRoad(road), mParent(nullptr), mChildren(0, nullptr), mCost(0), mHeuristic(0)
                       , mTotalCost(0)
{

}
//-------------------------------------------------------------------------------------------------------
Node::~Node()
{
    mParent = nullptr;
}
//-------------------------------------------------------------------------------------------------------