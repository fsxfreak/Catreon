//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions


#include "stdafx.h"

#include "GameState.hpp"

GameState::GameState() :	mMoveSpeed(0.1f), mRotateSpeed(0.3f), mbLMouseDown(false), mbRMouseDown(false), 
							mbQuit(false), mbSettingsMode(false), mDetailsPanel(0)
{

}
//-------------------------------------------------------------------------------------------------------
void GameState::enter()
{
	OgreFramework::getSingletonPtr()->mLog->logMessage("Entering Gamestate...");

	//initialize the scene
	mSceneMgr = OgreFramework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.7f, 0.7f));

	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	mRaySceneQuery->setQueryMask(OGRE_HEAD_MASK);

	mCamera = mSceneMgr->createCamera("GameCamera");
	mCamera->setPosition(Ogre::Vector3(5, 60, 120));
	mCamera->lookAt(Ogre::Vector3(0, 0, 0));
	mCamera->setNearClipDistance(5);

	mCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualWidth()) / 
		Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualHeight()));

	OgreFramework::getSingletonPtr()->mViewport->setCamera(mCamera);
	//object is a scene node
	mCurrentObject = 0;

	buildGUI();

	createScene();

}
//-------------------------------------------------------------------------------------------------------
bool GameState::pause()
{
	OgreFramework::getSingletonPtr()->mLog->logMessage("Pausing GameState...");

	return true;
}
//-------------------------------------------------------------------------------------------------------
//rebuild GUI, ser camera to active
void GameState::resume()
{
	OgreFramework::getSingletonPtr()->mLog->logMessage("Resuming GameState...");

	buildGUI();

	OgreFramework::getSingletonPtr()->mViewport->setCamera(mCamera);
	mbQuit = false;
}
//-------------------------------------------------------------------------------------------------------
void GameState::exit()
{
	OgreFramework::getSingletonPtr()->mLog->logMessage("Exiting GameState...");

	mSceneMgr->destroyCamera(mCamera);
	mSceneMgr->destroyQuery(mRaySceneQuery);

	if (mSceneMgr)
		OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);
}
