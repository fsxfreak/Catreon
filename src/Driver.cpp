/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "stdafx.h"
#include "Driver.h"

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
