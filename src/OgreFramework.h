#ifndef OGRE_FRAMEWORK_H
#define OGRE_FRAMEWORK_H

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
 

class OgreFramework : public Ogre::Singleton<OgreFramework>, OIS::KeyListener, OIS::MouseListener, OgreBites::SdkTrayListener
{
public:
	OgreFramework();
	~OgreFramework();

	bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);
	void updateOgre(double timeSinceLastFrame);
	void moveCamera();
	void getInput();

	bool isOgreToBeShutDown() const	{ return mShutDownOgre; }

	bool keyPressed(const OIS::KeyEvent &keyEvent);
	bool keyReleased(const OIS::KeyEvent &KeyEvent);

	bool mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
	bool mouseMoved(const OIS::MouseEvent &mouseEvent);

	Ogre::Root*			mRoot;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mRenderWindow;
	Ogre::Camera*		mCamera;
	Ogre::Viewport*		mViewport;
	Ogre::Log*			mLog;
	Ogre::Timer*		mTimer;

	OIS::InputManager*	mInputMgr;
	OIS::Keyboard*		mKb;
	OIS::Mouse*			mMouse;

	Ogre::String		mPluginsCfg;

	//handle cameras
	Ogre::SceneNode *cameraNode;

private:
	OgreFramework(const OgreFramework&);
	OgreFramework& operator=(const OgreFramework&);

	OgreBites::SdkTrayManager* mTrayMgr;
	Ogre::FrameEvent mFrameEvent;
	int mNumScreenShots;

	bool mShutDownOgre;

	Ogre::Vector3 mTranslateVector;
	Ogre::Real mMoveSpeed;
	Ogre::Degree mRotateSpeed;
	float mMoveScale;
	Ogre::Degree mRotateScale;
};

#endif

