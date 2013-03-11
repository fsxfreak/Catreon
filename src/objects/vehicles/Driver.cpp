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
void Driver::update(int milliseconds, Road *goalRoad)
{
    if (goalRoad != nullptr)
    {
        updateGoal(goalRoad);
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
void Driver::findNearestRoad(float radius)
{
    if (radius >= 32000) //A vehicle too far from any nodes will lock the program, and eventually overflow.
        return;

    //creates a tall and wide cylinder to intersect with Roads, to find the closest possible to the Vehicle
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

    getGameState()->mDynamicsWorld->removeCollisionObject(cylinder);
    delete cylinderShape;
    delete cylinder;

    Road *road = nullptr;
    if (foundRoad)
    {
        unsigned int shortestDistance = radius * radius + 1;   /* 1,000,001 = the maximum + 1 distance we could 
                                                                  possibly come up with, according to Pythagoras. */

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
            pVehicle->goTo(road, VehicleStates::FINDING_BEGIN_NODE);
        }
    }
    else
    {
        findNearestRoad(radius * 2);
    }
    chooseGoal();
    mPathToGoal = findPathToGoal(road);
}
//-------------------------------------------------------------------------------------------------------
void Driver::chooseGoal()
{
    std::vector<Road*> &roads = getGameState()->mRoads;
    int min = 1, max = roads.size();
    int randomRoad = (min + (rand() % (max - min + 1)));

    mGoal = roads.at(randomRoad - 1);
}
//-------------------------------------------------------------------------------------------------------
std::list<Node*> Driver::findPathToGoal(Road *currentRoad)
{
    //A* magic right here
    if (mGoal == nullptr)
        return std::list<Node*>();

    //unsigned long long runningCost = 0;

    Node *currentNode = &currentRoad->mNode;
    currentNode->mCost = 0;
    currentNode->mHeuristic = (currentRoad->getPosition() - mGoal->getPosition()).squaredLength();
    currentNode->mTotalCost = currentNode->mCost + currentNode->mHeuristic;

    std::list<Node*> openList;                      //the nodes that are not expanded yet
    std::list<Node*> closedList;                    //the nodes that have been expanded
                                    
    std::vector<std::list<Node*>* > partialPlans;    //the possible plans we have generated
    std::list<Node*> completePlan;                  //the correct sequence of Roads that leads to the goal

    partialPlans.push_back(new std::list<Node*>);
    partialPlans[0]->push_back(currentNode);

    openList.push_back(currentNode);

    while (!openList.empty())
    {
        unsigned long lowestTotalCost = 9999999999;
        std::list<Node*> openListCopy(openList);   //because we are adding nodes to the open list in this loop
        auto it = openListCopy.begin();
        auto itend = openListCopy.end();
        for (it; it != itend; ++it)
        {
            bool uniqueNode = true;
            auto itclosed = closedList.begin();
            auto itclosedend = closedList.end();
            for (itclosed; itclosed != itclosedend; ++itclosed)
            {
                if ((*itclosed) == (*it))
                    uniqueNode = false;
            }

            if (uniqueNode)
            {
                Node *node = (*it);

                if (node->mParent != nullptr)   //check if the node has any preceding nodes
                {
                    node->mCost = (node->mThisRoad->getPosition() - node->mParent->getPosition()).squaredLength()
                    + (node->mParent->mNode.mCost);
                }
                else    //otherwise it costs zero to get to this one
                    node->mCost = 0;
                node->mHeuristic = (node->mThisRoad->getPosition() - mGoal->getPosition()).squaredLength();
                node->mTotalCost = node->mCost + node->mHeuristic;

                if (node->mTotalCost < lowestTotalCost)  //get the node with the lowest cost
                {
                    currentNode = node;
                    lowestTotalCost = node->mTotalCost;
                }

                //put the nodes of the children of the open list on the open list
                for (int iii = 0; iii < node->mChildren.size(); ++iii)
                {   
                    if (node != &mGoal->mNode)  //if the node we have is the goal, don't put its children on
                    {
                        if (node->mChildren[iii] != nullptr)
                            openList.push_back(&node->mChildren[iii]->mNode);
                    }
                }
                openList.remove(*it);
                closedList.push_back((*it));
            }
        }

        for (int iii = 0; iii < partialPlans.size(); ++iii)
        {
            //if the node with the lowest cost is on the same path, add it to the plan
            if (&currentNode->mParent->mNode == partialPlans[iii]->back())
            {
                partialPlans[iii]->push_back(currentNode);
                break;
            }
            else    //we backtracked
            {
                auto it = partialPlans[iii]->begin();
                auto itend = partialPlans[iii]->end();
                for (it; it != itend; ++it) 
                {   
                    //take the path that matches and use that to construct a new one
                    if ((*it) == &currentNode->mParent->mNode)  
                    {
                        partialPlans.push_back(new std::list<Node*>(partialPlans[iii]->begin(), it));
                    }
                }
            }
        }
    }
    for (int iii = 0; iii < partialPlans.size(); ++iii)
    {
        unsigned long lowestCost = 9999999999;
        if (partialPlans[iii]->back() == &mGoal->mNode)
        {
            if (mGoal->mNode.mTotalCost < lowestCost)
            {
                completePlan.assign(partialPlans[iii]->begin(), partialPlans[iii]->end());
                lowestCost = mGoal->mNode.mTotalCost;
            }
        }
    }

    return completePlan;    //Will return an empty list if no path is possible.

}