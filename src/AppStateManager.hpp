//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions


#ifndef APP_STATE_MANAGER_HPP
#define APP_STATE_MANAGER_HPP

#include "stdafx.h"

#include "AppState.hpp"

class AppStateManager : public AppStateListener
{
public:
	struct state_info
	{
		Ogre::String name;
		AppState* state;
	}

	AppStateManager();
	~AppStateManager();

	void manageAppState(Ogre::String stateName, AppState *state);

	AppState* findByName(Ogre::String stateName);

	void start(AppState *state);
	void changeAppState(AppState *state);
	bool pushAppState(AppState *state);
	void popAppState();
	void pauseAppState();
	void shutdown();
    void popAllAndPushAppState(AppState *state);
	
protected:
	void init(AppState *state);

	std::vector<AppState*>	mActiveStateStack;
	std::vector<state_info>	mStates;

	bool mbShutdown;
};


#endif
