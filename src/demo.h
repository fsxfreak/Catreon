#ifndef DEMO_H
#define DEMO_H

#include "stdafx.h"
#include "AdvancedOgreFramework.hpp"
#include "Vehicle.h"

class OgreApp : public OIS::KeyListener
{
public:
	OgreApp();
	~OgreApp();

	void startDemo();

	bool keyPressed(const OIS::KeyEvent &keyEvent);
	bool keyReleased(const OIS::KeyEvent &keyEvent);

private:

	void createScene();
	void runDemo();

	Ogre::SceneNode* mNodePenguin;
	Ogre::Entity* mEntityPenguin;

	Ogre::SceneNode* mNodeTerrain;
	Ogre::Entity* mEntityTerrain;

	
	
	bool mShutDown;
};

#endif