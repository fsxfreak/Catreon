//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#include "stdafx.h"

#include "MenuState.hpp"

MenuState::MenuState() : mbQuit(false)
{
    mFrameEvent = Ogre::FrameEvent();
}
//-------------------------------------------------------------------------------------------------------
//create scene manager, camera, gui, call createscene to fill scene
void MenuState::enter()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Entering MenuState...");

    mSceneMgr = OgreFramework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC, "MenuSceneMgr");
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    mCamera = mSceneMgr->createCamera("MenuCam");
    mCamera->setPosition(Ogre::Vector3(0, 25, -50));
    mCamera->lookAt(0, 0, 0);
    mCamera->setNearClipDistance(1);

    mCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualWidth()) / 
        Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->mViewport->setCamera(mCamera);

    OgreFramework::getSingletonPtr()->mTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    OgreFramework::getSingletonPtr()->mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    OgreFramework::getSingletonPtr()->mTrayMgr->showCursor();
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "EnterBtn", "Enter Catreon", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "ExitBtn", "Exit Catreon", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createLabel(OgreBites::TL_TOP, "MenuLbl", "Menu", 250);

    createScene();
}
//-------------------------------------------------------------------------------------------------------
//menu scene
void MenuState::createScene()
{

}
//-------------------------------------------------------------------------------------------------------
void MenuState::exit()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Leaving MenuState...");

    mSceneMgr->destroyCamera(mCamera);
    if (mSceneMgr)
        OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);

    OgreFramework::getSingletonPtr()->mTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->mTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->mTrayMgr->setListener(0);
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::keyPressed(const OIS::KeyEvent &keyEvent)
{
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_ESCAPE))
    {
        mbQuit = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEvent);
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::keyReleased(const OIS::KeyEvent &keyEvent)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEvent);
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::mouseMoved(const OIS::MouseEvent &mouseEvent)
{
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseMove(mouseEvent))
        return true;

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseDown(mouseEvent, id))
        return true;

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseUp(mouseEvent, id))
        return true;

    return true;
}
//-------------------------------------------------------------------------------------------------------
void MenuState::update(double timeSinceLastFrame)
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->mTrayMgr->frameRenderingQueued(mFrameEvent);

    if (mbQuit == true)
    {
        shutdown();
        return;
    }
}
//-------------------------------------------------------------------------------------------------------
//triggered whenever a UI button is clicked
void MenuState::buttonHit(OgreBites::Button *button)
{
    if (button->getName() == "ExitBtn")
        mbQuit = true;
    else if (button->getName() == "EnterBtn")
    {
        changeAppState(findByName("GameState"));
    }
}
