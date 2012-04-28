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
void GameState::moveCamera()
{
	//if lshift down, move 10x faster
	if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_LSHIFT))
	{
		mCamera->moveRelative(mTranslateVector);
	}
	mCamera->moveRelative(mTranslateVector / 10);
}
//-------------------------------------------------------------------------------------------------------
void GameState::getInput()
{
	if (mbSettingsMode == false)
	{
		if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_A))
			mTranslateVector.x = -mMoveScale;
		if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_D))
			mTranslateVector.x = mMoveScale;
		if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_W))
			mTranslateVector.z = -mMoveScale;
		if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_S))
			mTranslateVector.z = mMoveScale;
	}
}
//-------------------------------------------------------------------------------------------------------
void GameState::update(double timeSinceLastFrame)
{
	mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	OgreFramework::getSingletonPtr()->mTrayMgr->frameRenderingQueued(mFrameEvent);

	if (mbQuit == true)
	{
		popAppState();
		return():
	}

	if (!OgreFramework::getSingletonPtr()->mTrayMgr->isDialogVisible())
	{
		if (mDetailsPanel->isVisible())
		{
			mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
			mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
			mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
			mDetailsPanel->setParamValue(3, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
			mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
			mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
			mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
			if (mbSettingsMode)
			{
				mDetailsPanel->setParamValue(7, "Buffered Input");
			}
			else
				mDetailsPanel->setParamValue(7, "Un-buffered Input");
		}
	}

	mMoveScale = mMoveSpeed * timeSinceLastFrame;
	mRotateScale = mRotateSpeed * timeSinceLastFrame;

	mTranslateVector = Ogre::Vector3::ZERO;

	getInput();
	moveCamera();
}
//-------------------------------------------------------------------------------------------------------
void GameState::buildGUI()
{
	OgreFramework::getSingletonPtr()->mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	OgreFramework::getSingletonPtr()->mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	OgreFramework::getSingletonPtr()->mTrayMgr->createLabel(OgreBites::TL_TOP, "GameLbl", "Game mode", 250);
	OgreFramework::getSingletonPtr()->mTrayMgr->showCursor();

	Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("Mode");

	mDetailsPanel = OgreFramework::getSingletonPtr()->mTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 200, items);
	mDetailsPanel->show();

	Ogre::String infoText = "[TAB] - Switch input mode\n\n[W] - Forward / Mode up\n[S] - Backwards/ Mode down\n[A] - Left\n";
    infoText.append("[D] - Right\n\nPress [SHIFT] to move faster\n\n[O] - Toggle FPS / logo\n");
	infoText.append("[Print] - Take screenshot\n\n[ESC] - Exit");

	OgreFramework::getSingletonPtr()->mTrayMgr->createTextBox(OgreBites::TL_RIGHT, "InfoPanel", infoText, 300, 220);

	Ogre::StringVector chatModes;
	chatModes.push_back("Solid mode");
	chatModes.push_back("Wireframe mode");
	chatModes.push_back("Point mode");
	OgreFramework::getSingletonPtr()->mTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "ChatModeSelMenu", "ChatMode", 200, 3, chatModes);
}
//-------------------------------------------------------------------------------------------------------