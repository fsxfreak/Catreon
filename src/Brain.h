#ifndef BRAIN_H
#define BRAIN_H
#include "stdafx.h"

class Brain
{
private:

	Brain& operator=(const Brain&);
public:
	Brain();
	~Brain();

	void updateDecisions();

};


#endif