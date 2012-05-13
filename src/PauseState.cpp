//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#include "stdafx.h"

#include "PauseState.hpp"

PauseState::PauseState() : mbQuit(false), mbQuestionActive(false)
{
    mFrameEvent = Ogre::FrameEvent();
}
//-------------------------------------------------------------------------------------------------------
void PauseState::enter()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Entering PauseState...");

    mSceneMgr = OgreFramework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC, "PauseSceneMgr");
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.7f, 0.7f));

    mCamera = mSceneMgr->createCamera("PauseCam");
    mCamera->setPosition(Ogre::Vector3(0, 25, -50));
    mCamera->lookAt(Ogre::Vector3(0, 0, 0));
    mCamera->setNearClipDistance(5);

    mCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualWidth()) / 
        Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->mViewport->setCamera(mCamera);

    OgreFramework::getSingletonPtr()->mTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->mTrayMgr->showCursor();
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "BackToGameBtn", "Return to Catreon", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "BackToMenuBtn", "Return to Menu", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "ExitBtn", "Exit Catreon", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createLabel(OgreBites::TL_TOP, "PauseLbl", "Pause mode", 250);

    mbQuit = false;

    createScene();
}
//-------------------------------------------------------------------------------------------------------
void PauseState::createScene()
{
    //only gui elements, so not needed now
}
//-------------------------------------------------------------------------------------------------------
void PauseState::exit()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Leaving PauseState...");

    mSceneMgr->destroyCamera(mCamera);
    if (mSceneMgr)
        OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);

    OgreFramework::getSingletonPtr()->mTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->mTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->mTrayMgr->setListener(0);
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::keyPressed(const OIS::KeyEvent &keyEvent)
{
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_ESCAPE) && !mbQuestionActive)
    {
        mbQuit = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEvent);

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::keyReleased(const OIS::KeyEvent &keyEvent)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEvent);

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::mouseMoved(const OIS::MouseEvent &mouseEvent)
{
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseMove(mouseEvent))
        return true;

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseDown(mouseEvent, id))
        return true;

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseUp(mouseEvent, id))
        return true;

    return true;
}
//-------------------------------------------------------------------------------------------------------
void PauseState::update(double timeSinceLastFrame)
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->mTrayMgr->frameRenderingQueued(mFrameEvent);

    if (mbQuit == true)
    {
        popAppState();
        return;
    }
}
//-------------------------------------------------------------------------------------------------------
void PauseState::buttonHit(OgreBites::Button *button)
{
    if (button->getName() == "ExitBtn")
    {
        OgreFramework::getSingletonPtr()->mTrayMgr->showYesNoDialog("Exit Catreon", "Are you sure you want to quit?");
        mbQuestionActive = true;
    }
    else if (button->getName() == "BackToGameBtn")
    {
        popAllAndPushAppState(findByName("GameState"));
        mbQuit = true;
    }
    else if (button->getName() == "BackToMenuBtn")
    {
        popAllAndPushAppState(findByName("MenuState"));
    }
}
//-------------------------------------------------------------------------------------------------------
void PauseState::yesNoDialogClosed(const Ogre::DisplayString &question, bool yesHit)
{
    if (yesHit == true)
        shutdown();
    else
        OgreFramework::getSingletonPtr()->mTrayMgr->closeDialog();

    mbQuestionActive = false;
}
