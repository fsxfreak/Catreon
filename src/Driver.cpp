#include "stdafx.h"
#include "Driver.h"

//-------------------------------------------------------------------------------------------------------
Driver::Driver(int nSkill) : mnSkill(nSkill)
{

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