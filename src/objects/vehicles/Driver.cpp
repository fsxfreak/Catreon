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
    pVehicle->goTo(findNearestRoad(), VehicleStates::FINDING_BEGIN_NODE);
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
    pVehicle->goTo(findNearestRoad(), VehicleStates::FINDING_BEGIN_NODE);
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
    mPathToGoal = findPathToGoal(findNearestRoad());
}
//-------------------------------------------------------------------------------------------------------
void Driver::updateGoal()
{
    //chooseGoal();
    Road *closest = findNearestRoad();
    if (closest != nullptr)
        mPathToGoal = findPathToGoal(closest);
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
    else if (mGoal == nullptr)
    {
        updateGoal();
    }
    //updateDecision();

    auto it = mPathToGoal.begin();
    auto itend = mPathToGoal.end();
    for (it; it != itend; ++it)
    {
       pVehicle->addToQueue((*it)->mThisRoad);
    }
    pVehicle->update(milliseconds);

}
//-------------------------------------------------------------------------------------------------------
Vehicle* Driver::getVehicle()
{
    return pVehicle;
}
//-------------------------------------------------------------------------------------------------------
Road* Driver::findNearestRoad(float radius)
{
    if (radius >= 32000) //A vehicle too far from any nodes will lock the program, and eventually overflow.
        return nullptr;

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
        unsigned int shortestDistance = radius * radius + 1;   

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
        return road;
    }
    else
    {
        findNearestRoad(radius * 2);
    }
}
//-------------------------------------------------------------------------------------------------------
/*void Driver::chooseGoal()
{
    std::vector<Road*> &roads = getGameState()->mRoads;
    int min = 1, max = roads.size();
    int randomRoad = (min + (rand() % (max - min + 1)));

    mGoal = roads.at(randomRoad - 1);
}*/
//-------------------------------------------------------------------------------------------------------
std::list<Node*> Driver::findPathToGoal(Road *currentRoad)
{
    std::list<Node*> openList;
    std::list<Node*> closedList;
    std::list<Node*> partialPlan;
    std::list<Node*> completePlan;

    Node *startingNode = &currentRoad->mNode;
    //initialize cost of starting node
    startingNode->mCost = 0;
    startingNode->mHeuristic = (startingNode->mThisRoad->getPosition() - mGoal->getPosition()).squaredLength();
    startingNode->mTotalCost = startingNode->mCost + startingNode->mHeuristic;

    //expand the starting node
    int size = startingNode->mChildren.size();
    for (int iii = 0; iii < size; ++iii)
    {
        //put the children of starting node into open list
        Node *child = &startingNode->mChildren[iii]->mNode;
        computeCost(child);
        openList.push_back(child);
    }
    openList.remove(startingNode);
    closedList.push_back(startingNode);

    while (!openList.empty())
    {
        //First, find the node on the openList with the lowest cost
        Node *nodeToExpand = nullptr;
        {
            auto itOpen = openList.begin();
            auto itOpenEnd = openList.end();
            unsigned long lowestCost = UINT_MAX;
            for (itOpen; itOpen != itOpenEnd; ++itOpen)
            {
                bool unique = true;
                auto itClosed = closedList.begin();
                auto itClosedEnd = closedList.end();
                for (itClosed; itClosed != itClosedEnd; ++itClosed)
                {
                    if (*itOpen == *itClosed) //don't expand a node we have already
                        unique = false;
                }

                if ((*itOpen)->mTotalCost < lowestCost && unique)
                {
                    nodeToExpand = *itOpen;
                }
            }
        }
        //Put all of its children on the openList
        if (nodeToExpand != &mGoal->mNode)
        {
            auto it = nodeToExpand->mChildren.begin();
            auto itend = nodeToExpand->mChildren.end();
            for (it; it != itend; ++it)
            {
                Node *child = &(*it)->mNode;

                auto itOpen = openList.begin();
                auto itOpenEnd = openList.end();
                bool onOpenList = false;
                Node *nodeAlreadyOnOpenList = nullptr;
                for (itOpen; itOpen != itOpenEnd; ++itOpen)
                {
                    if (*itOpen == child)
                    {
                        onOpenList = true;
                        nodeAlreadyOnOpenList = *itOpen;
                    }
                }

                if (!onOpenList)
                {
                    child->mParent = nodeToExpand->mThisRoad;
                    computeCost(child);
                    openList.push_back(child);
                }
                else
                {
                    /* if this node was already on the openList to be expanded,
                       we may have found a lower cost path to it.
                       Therefore, check if we indeed have a lower cost path to it.
                       If we do, we replace the parent (the lower cost path to it)
                       and recompute the cost
                    */
                    if (nodeAlreadyOnOpenList)
                    {
                        child->mParent = nodeToExpand->mThisRoad;
                        computeCost(child);
                        if (child->mCost < nodeAlreadyOnOpenList->mCost)
                        {
                            nodeAlreadyOnOpenList->mParent = nodeToExpand->mThisRoad;
                            computeCost(nodeAlreadyOnOpenList);
                        }
                    }
                }
            }
            openList.remove(nodeToExpand);
            closedList.push_back(nodeToExpand);
        }
        else    //this node is the goal, don't expand it
        {
           openList.remove(nodeToExpand);
        }
    }
    return closedList;
}
//-------------------------------------------------------------------------------------------------------
void Driver::computeCost(Node *node)
{
    Node *parent = &node->mParent->mNode;
    node->mCost = parent->mCost 
        + (node->mThisRoad->getPosition() - parent->mThisRoad->getPosition()).squaredLength();
    node->mHeuristic = (node->mThisRoad->getPosition() - mGoal->getPosition()).squaredLength();
    node->mTotalCost = node->mCost + node->mHeuristic;
}