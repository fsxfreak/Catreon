/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Take inputs from various sources for a neural network AI
-Find a way to "know" where the car is in the city
-The driver drives the vehicle
-The vehicle doesn't have the driver

TODO now
-Use Road information (occupied, next road, cost, etc) 
to pathfind to next road
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Driver.h"

//-------------------------------------------------------------------------------------------------------
Driver::Driver(int nCargo, int nPassengers, const Ogre::Vector3 &position, const Ogre::Quaternion &quat) 
                : mGoal(nullptr), mnNervousness(0), bIsFollowingClose(0)
{
    
    pVehicle = new Vehicle(nCargo, nPassengers, position, quat);
    mnSkill = rand() % 100 + 1;
    mnRiskTaker = rand() % 100 + 1;
    findNearestRoad();
}
//-------------------------------------------------------------------------------------------------------
//random, default constructor
Driver::Driver() : mGoal(nullptr), mnNervousness(0), bIsFollowingClose(0)
{
    //create random position and random rotation
    int xpos = (rand() % 1000) - 500;
    int zpos = (rand() % 1000) - 500;
    Ogre::Vector3 pos = Ogre::Vector3(xpos, 30, zpos);

    int max = -10000, min = 10000;  //flipped values intentional, makes Vehicles more likely to point to center
    float xdir = (min + (rand() % (max - min + 1))) / 10000.f;
    float zdir = (min + (rand() % (max - min + 1))) / 10000.f;
    Ogre::Vector3 dir(pos.x * xdir, pos.y, pos.z * zdir);

    pVehicle = new Vehicle(150, 1, pos, dir);
    mnSkill = rand() % 100 + 1;
    mnRiskTaker = rand() % 100 + 1;
    findNearestRoad();
}
//-------------------------------------------------------------------------------------------------------
Driver::~Driver()
{
    if (DebugWindow::get()->getVehicle() == pVehicle)
    {
        DebugWindow::get()->debugVehicle(nullptr);
    }
    delete pVehicle;
}
//-------------------------------------------------------------------------------------------------------
void Driver::updateGoal(Road *goalRoad)
{
    mGoal = goalRoad;
}
//-------------------------------------------------------------------------------------------------------
Road* Driver::getDestination()
{
    return mGoal;
}
//-------------------------------------------------------------------------------------------------------
void Driver::updateDecision()
{
    if (mGoal == nullptr)
    {
        pVehicle->setSpeed(50); //in MPH, to be converted internally into m/s
    }
}
//-------------------------------------------------------------------------------------------------------
int Driver::getSkill()
{
    return mnSkill;
}
//-------------------------------------------------------------------------------------------------------
int Driver::getNervousness()
{
    return mnNervousness;
}
//-------------------------------------------------------------------------------------------------------
int Driver::getRiskTaker()
{
    return mnRiskTaker;
}
//-------------------------------------------------------------------------------------------------------
void Driver::setNervousness(int nNervous)
{
    mnNervousness += nNervous;
}
//-------------------------------------------------------------------------------------------------------
void Driver::update(int milliseconds, std::string goal)
{
    if (goal != "NULL")
    {
        updateGoal(goal);
    }
    updateDecision();

    pVehicle->update(milliseconds);

}
//-------------------------------------------------------------------------------------------------------
Vehicle* Driver::getVehicle()
{
    return pVehicle;
}
//-------------------------------------------------------------------------------------------------------
void Driver::findNearestRoad()
{
    const int radius = 1000;
    btGhostObject *cylinder = new btGhostObject();
    btCollisionShape *cylinderShape = new btCylinderShape(btVector3(radius, 50, radius));
    cylinder->setCollisionShape(cylinderShape);
    btVector3 pos = GameState::ogreVecToBullet(pVehicle->getPosition());
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(pos);
    cylinder->setWorldTransform(trans);
    cylinder->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    getGameState()->mDynamicsWorld->addCollisionObject(cylinder, btBroadphaseProxy::SensorTrigger
        , btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger);

    int numOverlapping = cylinder->getNumOverlappingObjects();
    bool foundRoad = false;
    std::vector<Road*> roads;
    for (int iii = 0; iii < numOverlapping; ++iii)
    {
        btCollisionObject *body = dynamic_cast<btCollisionObject*>(cylinder->getOverlappingObject(iii));
        void *object = body->getUserPointer();
        if (body->getUserPointerType() == ROAD)
        {
            foundRoad = true;
            roads.push_back(static_cast<Road*>(object));
        }
    }
    if (foundRoad)
    {
        unsigned int shortestDistance = UINT_MAX;
        Road *road = nullptr;

        Ogre::Vector3 &pos = pVehicle->getPosition();

        auto it = roads.begin();
        auto itend = roads.end();
        for (it; it != itend; ++it)
        {
            float distance = ((*it)->getPosition() - pos).squaredLength();
            if (distance < shortestDistance)
            {
                road = *it;
                shortestDistance = distance;
            }
        }
        if (road != nullptr)
        {
            pVehicle->goTo(road->getPosition());
        }
    }

    getGameState()->mDynamicsWorld->removeCollisionObject(cylinder);
    delete cylinderShape;
    delete cylinder;

}