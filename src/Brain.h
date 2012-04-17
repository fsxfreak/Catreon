#ifndef BRAIN_H
#define BRAIN_H
#include "stdafx.h"

class Brain
{
private:

	Brain(const Brain&);
	Brain& operator=(const Brain&);
public:
	Brain();
	~Brain();

	void updateDecisions();

};


#endif