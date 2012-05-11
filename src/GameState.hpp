//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "stdafx.h"
#include "AppState.hpp"

#include "DotSceneLoader.h"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

#include "Vehicle.h"
#include "Driver.h"

enum QueryFlags
{
	OGRE_HEAD_MASK = 1 << 0,
	CUBE_MASK = 1 << 1
};

class GameState: public AppState
{
public:
	GameState();
	
	DECLARE_APPSTATE_CLASS(GameState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void moveCamera();
	void getInput();
	void buildGUI();

	bool keyPressed(const OIS::KeyEvent &keyEvent);
	bool keyReleased(const OIS::KeyEvent &keyEvent);

	bool mouseMoved(const OIS::MouseEvent &mouseEvent);
	bool mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);

	void onLeftPressed(const OIS::MouseEvent &mouseEvent);
	void itemSelected(OgreBites::SelectMenu *menu);

	void update(double timeSinceLastFrame);

private:
	Ogre::SceneNode			*mOgreHeadNode;
	Ogre::Entity			*mOgreHeadEntity;
	Ogre::MaterialPtr		mOgreHeadMaterial;
	Ogre::MaterialPtr		mOgreHeadMaterialHigh;

	Ogre::SceneNode			*mSphereNode;
	Ogre::Entity			*mSphereEntity;

	OgreBites::ParamsPanel  *mDetailsPanel;
	bool					mbQuit;

	Ogre::Vector3			mTranslateVector;
	Ogre::Real				mMoveSpeed;
	Ogre::Degree			mRotateSpeed;
	float					mMoveScale;
	Ogre::Degree			mRotateScale;

	Ogre::RaySceneQuery		*mRaySceneQuery;
	Ogre::SceneNode			*mCurrentObject;
	Ogre::Entity			*mCurrentEntity;
	bool					mbLMouseDown;
	bool					mbRMouseDown;
	bool					mbSettingsMode;
};
#endif