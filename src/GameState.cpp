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
//-------------------------------------------------------------------------------------------------------
//inherited from Appstate, fill the scene
void GameState::createScene()
{
	mSceneMgr->createLight("Light")->setPosition(0, 75, 30);

	DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
	pDotSceneLoader->parseDotScene("CubeScene.xml", "General", mSceneMgr, mSceneMgr->getRootSceneNode());
	delete pDotSceneLoader;

	mSceneMgr->getEntity("Cube01")->setQueryFlags(CUBE_MASK);
	mSceneMgr->getEntity("Cube02")->setQueryFlags(CUBE_MASK);
	mSceneMgr->getEntity("Cube03")->setQueryFlags(CUBE_MASK);

	mOgreHeadEntity = mSceneMgr->createEntity("Cube", "ogrehead.mesh");
	mOgreHeadEntity->setQueryFlags(OGRE_HEAD_MASK);
	mOgreHeadNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CubeNode");
	mOgreHeadNode->attachObject(mOgreHeadEntity);
	mOgreHeadNode->setPosition(Ogre::Vector3(0, 0, -15));

	mOgreHeadMaterial = mOgreHeadEntity->getSubEntity(1)->getMaterial();
	mOgreHeadMaterialHigh = mOgreHeadMaterial->clone("OgreHeadMaterialHigh");
	mOgreHeadMaterialHigh->getTechnique(0)->getPass(0)->setAmbient(1, 0, 0);
	mOgreHeadMaterialHigh->getTechnique(0)->getPass(0)->setDiffuse(1, 0 ,0 , 0);
}
//-------------------------------------------------------------------------------------------------------
bool GameState::keyPressed(const OIS::KeyEvent &keyEvent)
{
	if (mbSettingsMode == true)
	{
		if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_S))
		{
			//cycle up the settings menu toggled by TAB
			OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->mTrayMgr->getWidget("ChatModeSelMenu");
			if (pMenu->getSelectionIndex() + 1 < static_cast<int>(pMenu->getNumItems()))
			{
				pMenu->selectItem(pMenu->getSelectionIndex() + 1);
			}
		}
		if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_W))
		{
			OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->mTrayMgr->getWidget("ChatModeSelMenu");
			if (pMenu->getSelectionIndex() - 1 >= 0)
			{
				pMenu->selectItem(pMenu->getSelectionIndex() - 1);
			}
		}
	}

	//pause the state
	if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_ESCAPE))
	{
		pushAppState(findByName("PauseState"));
		return true;
	}

	//toggle information
	if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_I))
	{
		if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
		{
			OgreFramework::getSingletonPtr()->mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPLEFT, 0);
			mDetailsPanel->show();
		}
		else
		{
			OgreFramework::getSingletonPtr()->mTrayMgr->removeWidgetFromTray(mDetailsPanel);
			mDetailsPanel->hide();
		}
	}

	if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_TAB))
	{
		mbSettingsMode = !mbSettingsMode;
		return true;
	}

	//on press enter
	if (mbSettingsMode && OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_RETURN) ||
		OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_NUMPADENTER))
	{

	}

	//if not in settings mode (tab), or in settings mode and key isnt O, pass the keyevent to OgreFramework
	if (!mbSettingsMode || (mbSettingsMode && !OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_O)))
	{
		OgreFramework::getSingletonPtr()->keyPressed(keyEvent);
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------
bool GameState::keyReleased(const OIS::KeyEvent &keyEvent)
{
	OgreFramework::getSingletonPtr()->keyReleased(keyEvent);
	return true;
}
//-------------------------------------------------------------------------------------------------------
bool GameState::mouseMoved(const OIS::MouseEvent &mouseEvent)
{
	if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseMove(mouseEvent))
	{
		return true;
	}
	//if right mouse down, camera look is activated
	if (mbRMouseDown)
	{
		mCamera->yaw(Ogre::Degree(mouseEvent.state.X.rel * -0.1f));
		mCamera->pitch(Ogre::Degree(mouseEvent.state.Y.rel * -0.1f));
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------
bool GameState::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
	if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseDown(mouseEvent, id))
	{
		return true;
	}

	if (id == OIS::MB_Left)
	{
		onLeftPressed(mouseEvent);
		mbLMouseDown = true;
	}
	else if (id == OIS::MB_Right)
	{
		mbRMouseDown = true;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------
bool GameState::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
	if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseMove(mouseEvent, id))
	{
		return true;
	}

	if (id == OIS::MB_Left)
	{
		mbLMouseDown = false;
	}
	else if (id == OIS::MB_Right)
	{
		mbRMouseDown = false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------
//object selection
void GameState::onLeftPressed(const OIS::MouseEvent &mouseEvent)
{
	if (mCurrentObject)
	{
		mCurrentObject->showBoundingBox(false);
		mCurrentEntity->getSubEntity(1)->setMaterial(mOgreHeadMaterial);
	}

	//quite the formatting
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
		OgreFramework::getSingletonPtr()->mMouse->getMouseState().X.abs / 
		static_cast<float>(mouseEvent.state.width), 
		OgreFramework::getSingletonPtr()->mMouse->getMouseState().Y.abs /
		static_cast<float>(mouseEvent.state.height));

	mRaySceneQuery->setRay(mouseRay);
	mRaySceneQuery->setSortByDistance(true);

	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator itRayScene;

	//get all hits from left mouse click from a ray from the camera
	for (itRayScene = result.begin(); itRayScene != result.end(); itRayScene++)
	{
		if (itRayScene->movable)
		{
			OgreFramework::getSingletonPtr()->mLog->logMessage("MovableName: " + itRayScene->movable->getName());
			mCurrentObject = mSceneMgr->getEntity(itRayScene->movable->getName())->getParentSceneNode();
			OgreFramework::getSingletonPtr()->mLog->logMessage("ObjName " + mCurrentObject->getName());
			mCurrentObject->showBoundingBox(true);
			mCurrentEntity = mSceneMgr->getEntity(itRayScene->movable->getName());
			mCurrentEntity->getSubEntity(1)->setMaterial(mOgreHeadMaterialHigh);
			break;
		}
	}
}
//-------------------------------------------------------------------------------------------------------
void GameState::itemSelected(OgreBites::SelectMenu *menu)
{
	switch (menu->getSelectionIndex())
	{
	case 0:
		mCamera->setPolygonMode(Ogre::PM_SOLID);
		break;
	case 1:
		mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
		break;
	case 2:
		mCamera->setPolygonMode(Ogre::PM_POINTS);
		break;
	}
}
//-------------------------------------------------------------------------------------------------------