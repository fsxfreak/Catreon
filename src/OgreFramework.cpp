#include "stdafx.h"
#include "OgreFramework.h"


template<> OgreFramework* Ogre::Singleton<OgreFramework>::ms_Singleton = 0;

OgreFramework::OgreFramework() :	mMoveSpeed(0.1f), mRotateSpeed(0.3f), mShutDownOgre(false), 
									mNumScreenShots(0), mRoot(0), mSceneMgr(0), mRenderWindow(0), 
									mCamera(0), mViewport(0), mLog(0), mTimer(0), mInputMgr(0), 
									mKb(0), mMouse(0), mTrayMgr(0), mFrameEvent(Ogre::FrameEvent()),
									mSoundEngine(createIrrKlangDevice())
{

}

OgreFramework::~OgreFramework()
{
	if (mInputMgr)
		OIS::InputManager::destroyInputSystem(mInputMgr);
	if (mTrayMgr)
		delete mTrayMgr;
	if (mRoot)
		delete mRoot;
	if (mSoundEngine)
		mSoundEngine->drop();
}

void OgreFramework::moveCamera()
{
	if (mKb->isKeyDown(OIS::KC_LSHIFT))
		mCamera->moveRelative(mTranslateVector);
	else
		mCamera->moveRelative(mTranslateVector / 10);
}

void OgreFramework::getInput()
{
	if (mKb->isKeyDown(OIS::KC_A))
		mTranslateVector.x = -mMoveScale;
	if (mKb->isKeyDown(OIS::KC_D))
		mTranslateVector.x = mMoveScale;
	if (mKb->isKeyDown(OIS::KC_W))
		mTranslateVector.z = -mMoveScale;
	if (mKb->isKeyDown(OIS::KC_S))
		mTranslateVector.z = mMoveScale;
}

void OgreFramework::updateOgre(double timeSinceLastFrame)
{
	mMoveScale = mMoveSpeed * static_cast<float>(timeSinceLastFrame);
	mRotateScale = mRotateSpeed * static_cast<float>(timeSinceLastFrame);

	mTranslateVector = Ogre::Vector3::ZERO;

	getInput();
	moveCamera();

	mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	mTrayMgr->frameRenderingQueued(mFrameEvent);
}

bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEvent)
{
	if (mKb->isKeyDown(OIS::KC_ESCAPE))
	{
		mShutDownOgre = true;
		return true;
	}
	if (mKb->isKeyDown(OIS::KC_SYSRQ))
	{
		mRenderWindow->writeContentsToFile("OGRE_SCREEN_");
		return true;
	}
	if (mKb->isKeyDown(OIS::KC_M))
	{
		//creates a cycling action
		static int mode = 0;
		if (mode == 2)
		{
			mCamera->setPolygonMode(Ogre::PM_SOLID);
			mode = 0;
		}
		else if (mode == 0)
		{
			mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
			mode = 1;
		}
		else if (mode == 1)
		{
			mCamera->setPolygonMode(Ogre::PM_POINTS);
			mode = 2;
		}
	}
	if (mKb->isKeyDown(OIS::KC_O))
	{
		if (mTrayMgr->isLogoVisible())
		{
			mTrayMgr->hideLogo();
			mTrayMgr->hideFrameStats();
		}
		else
		{
			mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
			mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
		}
	}
	if (mKb->isKeyDown(OIS::KC_U))
	{
		mSoundEngine->play3D("sound/gottobe.mp3", ogreToIrrVec(mCamera->getPosition()), false, false, false);
	}
	return true;
}

//place holder
bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEvent)
{
	return true;
}

bool OgreFramework::mouseMoved(const OIS::MouseEvent &mouseEvent)
{
	mCamera->yaw(Ogre::Degree(-mouseEvent.state.X.rel * 0.15f));
	mCamera->pitch(Ogre::Degree(-mouseEvent.state.Y.rel * 0.15f));

	return true;
}

//place holder
bool OgreFramework::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
	return true;
}
//place holder
bool OgreFramework::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
	return true;
}

//translate an ogre vector into an irrklang vector
vec3df OgreFramework::ogreToIrrVec(Ogre::Vector3 ogrevec)
{
	vec3df irrvec;
	irrvec.X = ogrevec.x;
	irrvec.Y = ogrevec.y;
	irrvec.Z = ogrevec.z;

	return irrvec;
}
//translate an irrklang vector into an ogre vector
Ogre::Vector3 irrToOgreVec(vec3df irrvec)
{
	Ogre::Vector3 ogrevec;
	ogrevec.x = irrvec.X;
	ogrevec.y = irrvec.Y;
	ogrevec.z = irrvec.Z;

	return ogrevec;
}

bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
	Ogre::LogManager* logMgr = new Ogre::LogManager();
	mLog = Ogre::LogManager::getSingleton().createLog("Ogrelog.log", true, true, false);
	mLog->setDebugOutputEnabled(true);
	
#ifdef _DEBUG
	mPluginsCfg = "plugins_d.cfg";
#else
	mPluginsCfg = "plugins.cfg";
#endif

	mRoot = new Ogre::Root(mPluginsCfg);

	if (!mRoot->showConfigDialog())
		return false;
	mRenderWindow = mRoot->initialise(true, wndTitle);

	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.5f, 0.8f));

	mCamera = mSceneMgr->createCamera("DefaultCam");
	mCamera->setPosition(Ogre::Vector3(0, 0, 0));
	mCamera->lookAt(Ogre::Vector3(0, 0, 0));
	mCamera->setNearClipDistance(1);



	mViewport = mRenderWindow->addViewport(mCamera);
	mViewport->setBackgroundColour(Ogre::ColourValue(0.4f, 0.7f, 0.6f));

	mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

	mViewport->setCamera(mCamera);

	size_t hWnd = 0;
	OIS::ParamList paramlist;
	mRenderWindow->getCustomAttribute("WINDOW", &hWnd);

	paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	mInputMgr = OIS::InputManager::createInputSystem(paramlist);

	mKb = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));

	mMouse->getMouseState().height = mRenderWindow->getHeight();
	mMouse->getMouseState().width = mRenderWindow->getWidth();

	if (pKeyListener == 0)
		mKb->setEventCallback(this);
	else
		mKb->setEventCallback(pKeyListener);

	if (pMouseListener == 0)
		mMouse->setEventCallback(this);
	else
		mMouse->setEventCallback(pMouseListener);

	Ogre::String secName, typeName, archName;
	Ogre::ConfigFile cf;
#ifdef _DEBUG
	cf.load("resources_d.cfg");
#else
	cf.load("resources.cfg");
#endif

	Ogre::ConfigFile::SectionIterator secit = cf.getSectionIterator();
	while (secit.hasMoreElements())
	{
		secName = secit.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = secit.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;

		for (it = settings->begin(); it != settings->end(); ++it)
		{
			typeName = it->first;
			archName = it->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mTimer = new Ogre::Timer();
	mTimer->reset();

	mTrayMgr = new OgreBites::SdkTrayManager("TrayMgr", mRenderWindow, mMouse, this);
	mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	mTrayMgr->hideCursor();

	mRenderWindow->setActive(true);

	//check the sound engine
	if (!mSoundEngine)
		return 1;

	

	return true;


}

