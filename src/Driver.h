#ifndef DRIVER_H
#define DRIVER_H

#include "stdafx.h"
#include "Brain.h"

#include <string>

class Driver
{
private:
	std::string mstrDestination;

	//all personality on a scale of 1 to 10
	const int mnSkill;
	int mnNervousness;
	int mnRiskTaker;

	Driver& operator=(const Driver&);
public:
	Driver(int nSkill);
	~Driver();

	void updateDestination(std::string strDest);
	void updateDecision();

	std::string getDestination();

};

#endif