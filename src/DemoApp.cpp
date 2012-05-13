//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#include "stdafx.h"

#include "DemoApp.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"

#include <boost\thread\thread.hpp>

DemoApp::DemoApp() : mAppStateManager(0)
{
}
//-------------------------------------------------------------------------------------------------------
DemoApp::~DemoApp()
{
	delete mAppStateManager;
	delete OgreFramework::getSingletonPtr();
}
//-------------------------------------------------------------------------------------------------------
void DemoApp::startDemo()
{
	new OgreFramework();

	if (!OgreFramework::getSingletonPtr()->initOgre("Catreon", 0, 0))
		return;

	OgreFramework::getSingletonPtr()->mLog->logMessage("Catreon initialized!");

	mAppStateManager = new AppStateManager();

	MenuState::create(mAppStateManager, "MenuState");
	GameState::create(mAppStateManager, "GameState");
	PauseState::create(mAppStateManager, "PauseState");

	mAppStateManager->start(mAppStateManager->findByName("MenuState"));
}