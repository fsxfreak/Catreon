//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#include "stdafx.h"

#include "AppStateManager.hpp"

#include <OgreWindowEventUtilities.h>

AppStateManager::AppStateManager() : mbShutdown(false)
{
}
//-------------------------------------------------------------------------------------------------------
AppStateManager::~AppStateManager()
{
	state_info si;

	while (!mActiveStateStack.empty())
	{
		mActiveStateStack.back()->exit();
		mActiveStateStack.pop_back();
	}

	while (!mStates.empty())
	{
		si = mStates.back();
		si.state->destroy();
		mStates.pop_back();
	}
}
//-------------------------------------------------------------------------------------------------------
void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
	try
	{
		state_info new_state_info;
		new_state_info.name = stateName;
		new_state_info.state = state;
		mStates.push_back(new_state_info);
	}
	catch(std::exception &e)
	{
		delete state;
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new AppState\n" +
			Ogre::String(e.what()), "AppStateManager.cpp (33)");
	}
}
//-------------------------------------------------------------------------------------------------------
AppState* AppStateManager::findByName(Ogre::String stateName)
{
	std::vector<state_info>::iterator it;

	for (it = mStates.begin(); it != mStates.end(); it++)
	{
		if (it->name == stateName)
			return it->state;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------
//starts the game loop
//calls ogreframework to render
void AppStateManager::start(AppState* state)
{
	changeAppState(state);

	int timeSinceLastFrame = 1;
	int startTime = 0;

	while (!mbShutdown)
	{
		if (OgreFramework::getSingletonPtr()->mRenderWindow->isClosed())
			mbShutdown = true;

		Ogre::WindowEventUtilities::messagePump();

		if(OgreFramework::getSingletonPtr()->mRenderWindow->isActive())
		{
			startTime = OgreFramework::getSingletonPtr()->mTimer->getMillisecondsCPU();

			OgreFramework::getSingletonPtr()->mKb->capture();
			OgreFramework::getSingletonPtr()->mMouse->capture();

			mActiveStateStack.back()->update(timeSinceLastFrame);

			OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			OgreFramework::getSingletonPtr()->mRoot->renderOneFrame();

			timeSinceLastFrame = OgreFramework::getSingletonPtr()->mTimer->getMillisecondsCPU() - startTime;
		}
		else
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
		}
	}

	OgreFramework::getSingletonPtr()->mLog->logMessage("Quitting main loop...");
}
//-------------------------------------------------------------------------------------------------------
//Exits the current state (if there is any) and starts the new specified one 
void AppStateManager::changeAppState(AppState* state)
{
	if (!mActiveStateStack.empty())
	{
		mActiveStateStack.back()->exit();
		mActiveStateStack.pop_back();
	}

	mActiveStateStack.push_back(state);
	init(state);
	mActiveStateStack.back()->enter();
}
//-------------------------------------------------------------------------------------------------------
//Puts an new state on the top of the stack and starts it 
bool AppStateManager::pushAppState(AppState* state)
{
	if (!mActiveStateStack.empty())
	{
		if (!mActiveStateStack.back()->pause())
		{
			return false;
		}
	}

	mActiveStateStack.push_back(state);
	init(state);
	mActiveStateStack.back()->enter();

	return true;
}
//-------------------------------------------------------------------------------------------------------
//Removes the top most state and resumes the one below if there is one, otherwise shutdown the application 
void AppStateManager::popAppState()
{
	if (!mActiveStateStack.empty())
	{
		init(mActiveStateStack.back());
		mActiveStateStack.back()->resume();
	}
	else
	{
		shutdown();
	}
}
//-------------------------------------------------------------------------------------------------------
//Exits all existing app state on the stack and enters the given new state 
void AppStateManager::popAllAndPushAppState(AppState* state)
{
	while (!mActiveStateStack.empty())
	{
		mActiveStateStack.back()->exit();
		mActiveStateStack.pop_back();
	}

	pushAppState(state);
}
//-------------------------------------------------------------------------------------------------------
//pauses the app state
void AppStateManager::pauseAppState()
{
	if (!mActiveStateStack.empty())
	{
		mActiveStateStack.back()->pause();
	}

	if (mActiveStateStack.size() > 2)
	{
		init(mActiveStateStack.at(mActiveStateStack.size() - 2));
		mActiveStateStack.at(mActiveStateStack.size() - 2)->resume();
	}
}
//-------------------------------------------------------------------------------------------------------
void AppStateManager::shutdown()
{
	mbShutdown = true;
}
//-------------------------------------------------------------------------------------------------------
void AppStateManager::init(AppState* state)
{
	OgreFramework::getSingletonPtr()->mKb->setEventCallback(state);
	OgreFramework::getSingletonPtr()->mMouse->setEventCallback(state);
	OgreFramework::getSingletonPtr()->mTrayMgr->setListener(state);

	OgreFramework::getSingletonPtr()->mRenderWindow->resetStatistics();
}