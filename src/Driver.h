#ifndef DRIVER_H
#define DRIVER_H

#include "stdafx.h"

#include <string>

class Driver
{
private:
	std::string mstrDestination;

	//all personality on a scale of 1 to 100
	int mnSkill;
	int mnNervousness;
	int mnRiskTaker;

	Driver& operator=(const Driver&);
public:
	Driver(int nSkill, int nRiskTaker);
	Driver();
	~Driver();

	void updateDestination(std::string strDest);
	void updateDecision();

	int getSkill();
	int getNervousness();
	int getRiskTaker();

	void setNervousness(int nNervous);

	std::string getDestination();

};

#endif