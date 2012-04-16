#ifndef DRIVER_H
#define DRIVER_H

#include "stdafx.h"

#include <string>

class Driver
{
private:
	std::string mstrDestination;

	//driving skill, scale of 1 to 10
	const int mnSkill;

public:
	Driver(int nSkill);
	~Driver();

	void updateDestination(std::string strDest);

	std::string getDestination();
};

#endif