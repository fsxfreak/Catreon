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
********************************************************/

#include "stdafx.h"
#include "objects\vehicles\Driver.h"

//-------------------------------------------------------------------------------------------------------
Driver::Driver(int nSkill, int nRiskTaker) : mnSkill(nSkill), mnRiskTaker(nRiskTaker)
{

}
//-------------------------------------------------------------------------------------------------------
//random, default constructor
Driver::Driver() : mnNervousness(25)
{
    mnSkill = rand() % 100;
    mnRiskTaker = rand() % 100;
}
//-------------------------------------------------------------------------------------------------------
Driver::~Driver()
{

}
//-------------------------------------------------------------------------------------------------------
void Driver::setCar(Vehicle *vehicle)
{
    pVehicle = vehicle;
}
//-------------------------------------------------------------------------------------------------------
void Driver::updateDestination(std::string strDest)
{
    mstrDestination = strDest;
}
//-------------------------------------------------------------------------------------------------------
std::string Driver::getDestination()
{
    return mstrDestination;
}
//-------------------------------------------------------------------------------------------------------
void Driver::updateDecision()
{

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
