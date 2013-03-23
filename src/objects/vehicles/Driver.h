/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DRIVER_H
#define DRIVER_H

#include "stdafx.h"
#include <string>
#include "objects\vehicles\Vehicle.h"
#include <objects\Road.hpp>
#include <GameState.hpp>

class Vehicle;

class Driver
{
private:
    Road *mGoal;

    //all personality on a scale of 1 to 100
    int mnSkill;
    int mnNervousness;
    int mnRiskTaker;

    bool bIsFollowingClose;

    Vehicle* pVehicle;
    std::list<Node*> mPathToGoal;

    Road* findNearestRoad(float radius = 1000);
    void chooseGoal();
    std::list<Node*> findPathToGoal(Road *currentRoad);
    void computeCost(Node *to);

    Driver& operator=(const Driver& cDriver);
public:
    Driver(int nCargo, int nPassengers, const Ogre::Vector3 &position, const Ogre::Quaternion &quat);
    Driver();
    ~Driver();

    void updateGoal(Road *goalRoad);
    void updateGoal();
    void updateDecision();
    void update(int milliseconds, Road *goalRoad = nullptr);

    int getSkill();
    int getNervousness();
    int getRiskTaker();

    void setNervousness(int nNervous);

    Road* getDestination();
    Vehicle* getVehicle();

};


#endif
