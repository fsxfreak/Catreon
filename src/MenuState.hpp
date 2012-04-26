//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "stdafx.h"

#include "AppState.hpp"

class MenuState : public AppState
{
public:
	MenuState();

	DECLARE_APPSTATE_CLASS(MenuState)

	void enter();
	void createScene();
	void exit();

	bool keyPressed(const OIS::KeyEvent &keyEvent);
	bool keyReleased(const OIS::KeyEvent &keyEvent);

	bool mouseMoved(const OIS::MouseEvent &mouseEvent);
	bool mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);

	void buttonHit(OgreBites::Button* button);

	void update(double timeSinceLastFrame);

private:
	bool mbQuit;
};

#endif