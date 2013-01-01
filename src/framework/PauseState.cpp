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

    mSound = irrklang::createIrrKlangDevice();

    /*OgreFramework::getSingletonPtr()->mTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->mTrayMgr->showCursor();
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "BackToGameBtn", "Return to Catreon", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "BackToMenuBtn", "Return to Menu", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton(OgreBites::TL_CENTER, "ExitBtn", "Exit Catreon", 250);
    OgreFramework::getSingletonPtr()->mTrayMgr->createLabel(OgreBites::TL_TOP, "PauseLbl", "Pause mode", 250);*/

    mbQuit = false;

    createScene();
}
//-------------------------------------------------------------------------------------------------------
void PauseState::createScene()
{
    CEGUI::WindowManager &windowManager = CEGUI::WindowManager::getSingleton();
    CEGUI::Window *pauseRoot = windowManager.loadWindowLayout("CatreonPauseState.layout");
    CEGUI::System::getSingleton().setGUISheet(pauseRoot);

    GUIEventSubscriber::get()->subscribe("Pause/ExitButton", ButtonTypes::PUSH_BUTTON, States::PAUSESTATE, this);
    GUIEventSubscriber::get()->subscribe("Pause/ReturnButton", ButtonTypes::PUSH_BUTTON, States::PAUSESTATE, this);
    GUIEventSubscriber::get()->subscribe("Pause/ReturnToMenu", ButtonTypes::PUSH_BUTTON, States::PAUSESTATE, this);
}
//-------------------------------------------------------------------------------------------------------
void PauseState::exit()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Leaving PauseState...");

    GUIEventSubscriber::get()->unsubscribeAll();
    CEGUI::WindowManager::getSingleton().destroyAllWindows();

    mSceneMgr->destroyCamera(mCamera);
    if (mSceneMgr)
        OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);

    mSound->drop();

    /*OgreFramework::getSingletonPtr()->mTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->mTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->mTrayMgr->setListener(0);*/
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::keyPressed(const OIS::KeyEvent &keyEvent)
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
bool PauseState::keyReleased(const OIS::KeyEvent &keyEvent)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEvent);
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::mouseMoved(const OIS::MouseEvent &mouseEvent)
{
    OgreFramework::getSingletonPtr()->mouseMoved(mouseEvent);
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    OgreFramework::getSingletonPtr()->mousePressed(mouseEvent, id);
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool PauseState::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    OgreFramework::getSingletonPtr()->mouseReleased(mouseEvent, id);
    return true;
}
//-------------------------------------------------------------------------------------------------------
void PauseState::update(double timeSinceLastFrame)
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    //OgreFramework::getSingletonPtr()->mTrayMgr->frameRenderingQueued(mFrameEvent);
    CEGUI::System::getSingleton().injectTimePulse(timeSinceLastFrame);

    if (mbQuit == true)
    {
        shutdown();
        return;
    }

}
//-------------------------------------------------------------------------------------------------------
void PauseState::buttonHit(const CEGUI::EventArgs &mouseEvent)
{
    const CEGUI::MouseEventArgs& caller = static_cast<const CEGUI::MouseEventArgs&>(mouseEvent);
    if (caller.window->getName() == "Pause/ExitButton")
    {
        mbQuit = true;
    }
    else if (caller.window->getName() == "Pause/ReturnButton")
    {
        popAllAndPushAppState(findByName("GameState"));
        mbQuit = true;
    }
    else if (caller.window->getName() == "Pause/ReturnToMenu")
    {
        popAllAndPushAppState(findByName("MenuState"));
    }
}
//-------------------------------------------------------------------------------------------------------
void PauseState::buttonHovered(const CEGUI::EventArgs &mouseEvent)
{
    mSound->play2D("../media/sound/gui_rollover.wav");
    //probably should replace this with Sound->play(Snd::Hovered);
}
