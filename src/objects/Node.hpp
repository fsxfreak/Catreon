/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef NODE_HPP
#define NODE_HPP

#include "stdafx.h"

#include <GameState.hpp>
#include <objects\Road.hpp>

class Road;

class Node
{
public:
    Road *mThisRoad;    //guaranteed to be initialized to a valid pointer
    Road *mParent;      //may not be initialized due to being a start of a road
    std::vector<Road*> mChildren;

    unsigned long mCost;        //cost to reach this node + those who came before
    unsigned long mHeuristic;   //heuristic to the goal
    unsigned long mTotalCost;   //cost to reach this node + heuristic + those who came before

    Node(Road *road);
    ~Node();
};

#endif