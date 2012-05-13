//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#ifndef OGRE_FRAMEWORK_HPP
#define OGRE_FRAMEWORK_HPP

#include "stdafx.h"

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRoot.h>

#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
 
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
 
#include <SdkTrays.h>

#include <irrKlang.h>

using namespace irrklang;

class OgreFramework : public Ogre::Singleton<OgreFramework>, OIS::KeyListener, OIS::MouseListener
{
public:
    OgreFramework();
    ~OgreFramework();

    bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);
    void updateOgre(double timeSinceLastFrame);

    bool keyPressed(const OIS::KeyEvent &keyEvent);
    bool keyReleased(const OIS::KeyEvent &keyEvent);

    bool mouseMoved(const OIS::MouseEvent &mouseEvent);
    bool mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);

    double getTimeSinceLastFrame();

    Ogre::Root                  *mRoot;
    Ogre::RenderWindow          *mRenderWindow;
    Ogre::Viewport              *mViewport;
    Ogre::Log                   *mLog;
    Ogre::Timer                 *mTimer;
    
    OIS::InputManager           *mInputMgr;
    OIS::Keyboard               *mKb;
    OIS::Mouse                  *mMouse;

    OgreBites::SdkTrayManager   *mTrayMgr;

    Ogre::String                mPluginsCfg;
    
    double mTimeSinceLastFrame;

    //sound
    ISoundEngine *soundeng;

    //physics

private:
    OgreFramework(const OgreFramework&);
    OgreFramework& operator=(const OgreFramework&);

};

#endif
