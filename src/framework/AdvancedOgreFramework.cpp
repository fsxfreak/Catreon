//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#include "stdafx.h"

#include "AdvancedOgreFramework.hpp"

CEGUI::MouseButton OgreFramework::convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
        break;
    case OIS::MB_Right:
        return CEGUI::RightButton;
        break;
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
        break;
    default:
        return CEGUI::LeftButton;
    }
}
template <>OgreFramework* Ogre::Singleton<OgreFramework>::ms_Singleton = 0;
//-------------------------------------------------------------------------------------------------------
OgreFramework::OgreFramework() :    mRoot(0), mRenderWindow(0), mViewport(0), mLog(0), mTimer(0), 
                                    mInputMgr(0), mKb(0), mMouse(0), /*mTrayMgr(0),*/
                                    mGUIRenderer(0),
                                    mTimeSinceLastFrame(0)                                  
{

}
//-------------------------------------------------------------------------------------------------------
OgreFramework::~OgreFramework()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Shutdown OGRE...");
    //if (mTrayMgr)
    //    delete mTrayMgr;
    if (mInputMgr)
        OIS::InputManager::destroyInputSystem(mInputMgr);
    if (mRoot)
        delete mRoot;
}
//-------------------------------------------------------------------------------------------------------
bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
    Ogre::LogManager *logMgr = new Ogre::LogManager();

    mLog = Ogre::LogManager::getSingleton().createLog("OgreLogFile.log", true, true, false);
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

    mViewport = mRenderWindow->addViewport(0);
    mViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.5f, 1.0f));

    mViewport->setCamera(0);

    size_t hWnd = 0;
    OIS::ParamList paramList;
    mRenderWindow->getCustomAttribute("WINDOW", &hWnd);

    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    mInputMgr = OIS::InputManager::createInputSystem(paramList);

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


    //mTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", mRenderWindow, mMouse, 0);

    mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

    CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
    CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
    CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

    mTimer = new Ogre::Timer();
    mTimer->reset();

    mRenderWindow->setActive(true);

    return true;
}
//-------------------------------------------------------------------------------------------------------
void OgreFramework::updateOgre(double timeSinceLastFrame)
{
    mTimeSinceLastFrame = timeSinceLastFrame;
    //called once per frame, update something central to ogre
}
//-------------------------------------------------------------------------------------------------------
bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEvent)
{
    //Take a screenshot of the current frame
    if (mKb->isKeyDown(OIS::KC_SYSRQ))
    {
        mRenderWindow->writeContentsToTimestampedFile("Catreon_", ".jpg");
        return true;
    }
    //Toggle logo
    /*if (mKb->isKeyDown(OIS::KC_O))
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
    }*/
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectKeyDown(keyEvent.key);
    sys.injectChar(keyEvent.text);

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEvent)
{
    CEGUI::System::getSingleton().injectKeyUp(keyEvent.key);
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool OgreFramework::mouseMoved(const OIS::MouseEvent &mouseEvent)
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectMouseMove(mouseEvent.state.X.rel, mouseEvent.state.Y.rel);

    //scroll wheel
    if (mouseEvent.state.Z.rel)
        sys.injectMouseWheelChange(mouseEvent.state.Z.rel / 120.f); //120 is the magic number it seems

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool OgreFramework::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool OgreFramework::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
    return true;
}
//-------------------------------------------------------------------------------------------------------
double OgreFramework::getTimeSinceLastFrame()
{
    return mTimeSinceLastFrame;
}
