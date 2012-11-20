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

    OgreFramework::getSingletonPtr()->mViewport->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
    OgreFramework::getSingletonPtr()->mViewport->setCamera(mCamera);

    mSound = irrklang::createIrrKlangDevice();

    createScene();
}
//-------------------------------------------------------------------------------------------------------
//menu scene, actually the GUI
void MenuState::createScene()
{
    CEGUI::WindowManager &windowManager = CEGUI::WindowManager::getSingleton();
    CEGUI::Window *menuRoot = windowManager.loadWindowLayout("CatreonMenuState.layout");
    CEGUI::System::getSingleton().setGUISheet(menuRoot);

    GUIEventSubscriber::get()->subscribe("EnterButton", ButtonTypes::PUSH_BUTTON, States::MENUSTATE, this);
    GUIEventSubscriber::get()->subscribe("ExitButton", ButtonTypes::PUSH_BUTTON, States::MENUSTATE, this);
}
//-------------------------------------------------------------------------------------------------------
void MenuState::exit()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Leaving MenuState...");

    GUIEventSubscriber::get()->unsubscribeAll();
    CEGUI::WindowManager::getSingleton().destroyAllWindows();
    CEGUI::MouseCursor::getSingleton().hide();

    mSound->drop();

    mSceneMgr->destroyCamera(mCamera);
    if (mSceneMgr)
        OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::keyPressed(const OIS::KeyEvent &keyEvent)
{
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_ESCAPE))
    {
        mbQuit = true;
        return true;
    }
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_RETURN))
    {
        changeAppState(findByName("GameState"));
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
    OgreFramework::getSingletonPtr()->mouseMoved(mouseEvent);

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    OgreFramework::getSingletonPtr()->mousePressed(mouseEvent, id);
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool MenuState::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    OgreFramework::getSingletonPtr()->mouseReleased(mouseEvent, id);
    return true;
}
//-------------------------------------------------------------------------------------------------------
void MenuState::update(double timeSinceLastFrame)
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    CEGUI::System::getSingleton().injectTimePulse(timeSinceLastFrame);

    if (mbQuit == true)
    {
        shutdown();
        return;
    }
}
//-------------------------------------------------------------------------------------------------------
void MenuState::buttonHit(const CEGUI::EventArgs &mouseEvent)
{
    const CEGUI::MouseEventArgs& caller = static_cast<const CEGUI::MouseEventArgs&>(mouseEvent);
    if (caller.window->getName() == "EnterButton")
    {
        changeAppState(findByName("GameState"));
    }
    if (caller.window->getName() == "ExitButton")
    {
        mbQuit = true;
    }
}
//-------------------------------------------------------------------------------------------------------
void MenuState::buttonHovered(const CEGUI::EventArgs &mouseEvent)
{
    mSound->play2D("../media/sound/gui_rollover.wav");
}