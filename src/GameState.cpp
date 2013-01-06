/*
Graphics and Input Handling code largely based on Advanced Ogre Framework at ogre3d.org

author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

/********************************************************
TODO
-Overhaul object creation, combine physics, graphics, and sound intialization into one class per object
-Begin treating GameState like an actual world, instead of the current test world state
-Begin creating the world in Maya
-Get sound update framework into place after object creation is completed
-Get work done on that vehicle class
-Find a way to spawn vehicles in the world according to the driver stats
-Find a way to find valid spawn points (not in some building)
-Move scene creation to an xml for use with RapidXML dotscene loader
********************************************************/

#include "stdafx.h"

#include "GameState.hpp"
#include <boost\thread.hpp>

using namespace irrklang;

GameState* GameState::mGameSingleton = nullptr;

GameState::GameState() :    mAcceleration(0.01f), mMaxMoveSpeed(0.3f), mTranslateVector(0, 0, 0),
                            mbLMouseDown(false), mbRMouseDown(false), 
                            mbQuit(false), mbSettingsMode(false), mbBackslashDown(false),
                            mTimer(new Ogre::Timer), mTimeSinceUpdate(0), 
                            sound(nullptr)
{
    mGameSingleton = this;  //is that even right? - doesn't matter, works
}
//-------------------------------------------------------------------------------------------------------
void GameState::enter()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Entering Gamestate...");

    /*DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
    pDotSceneLoader->parseDotScene("SampleDotScene.scene", "General", mSceneMgr, mSceneMgr->getRootSceneNode());
    delete pDotSceneLoader;*/

    //initialize the scene
    mSceneMgr = OgreFramework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));

    mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

    mCamera = mSceneMgr->createCamera("GameCamera");
    mCamera->setPosition(Ogre::Vector3(0, 200, -300));
    mCamera->lookAt(Ogre::Vector3(0, 0, 0));
    mCamera->setNearClipDistance(1);
    mCamera->setFarClipDistance(1000);

    mCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualWidth()) / 
        Ogre::Real(OgreFramework::getSingletonPtr()->mViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->mViewport->setCamera(mCamera);
    //object is a scene node
    mCurrentObject = 0;

    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    btVector3 worldMin(-1000, -1000, -1000);
    btVector3 worldMax(1000, 1000, 1000);
    mBroadphase = new btAxisSweep3(worldMin, worldMax);
    mSolver = new btSequentialImpulseConstraintSolver();
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

    mGhostCallback = new btGhostPairCallback();
    mDynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostCallback);

    mDynamicsWorld->setGravity(btVector3(0, -100, 0));
    mDynamicsWorld->setForceUpdateAllAabbs(false);

    Ogre::MaterialManager::getSingleton().load("DebugLines.material", "General");
    mDebugDrawer = new CDebugDraw(mSceneMgr, mDynamicsWorld);

    sound = createIrrKlangDevice();
    if (!sound)
    {
        OgreFramework::getSingletonPtr()->mLog->logMessage("Unable to create sound in GameState");
        return;
    }

    

    buildGUI();

    createScene();

    mTimer->reset();
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
    
    mDrivers.clear();
    mRoads.clear();

    for (auto it = mBalls.begin(); it != mBalls.end(); ++it)
    {
        delete *it;
    }
    mBalls.clear();

    delete mDebugDrawer;

    OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);

    for (int body = mDynamicsWorld->getNumCollisionObjects() - 1; body >= 0; body--)
    {
        btCollisionObject *object = mDynamicsWorld->getCollisionObjectArray()[body];
        mDynamicsWorld->removeCollisionObject(object);
    }
    for (auto it = mRigidBodies.begin(); it != mRigidBodies.end(); ++it)
    {
        mDynamicsWorld->removeRigidBody(*it);
        delete *it;
    }
    mRigidBodies.clear();

    auto itend = mCollisionShapes.end();
    for (auto it = mCollisionShapes.begin(); it != itend; ++it)
    {
        delete *it;
    }
    mCollisionShapes.clear();

    delete mGhostCallback;
    delete mDynamicsWorld;
    delete mSolver;
    delete mCollisionConfiguration;
    delete mDispatcher;
    delete mBroadphase;

    if (sound)
        sound->drop();

    DebugWindow::get()->reset();
}
//-------------------------------------------------------------------------------------------------------
//inherited from Appstate, fill the scene
void GameState::createScene()
{
    Ogre::SceneNode *sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    DotSceneLoader loader;
    loader.parseDotScene("catreoncity.scene", "General", mSceneMgr, sceneNode);
    sceneNode->scale(6, 1, 6);
    sceneNode->translate(0, 5, 0);
    Ogre::Node::ChildNodeIterator iterator = sceneNode->getChildIterator();

    Ogre::Light *directional = mSceneMgr->createLight("directionallight");
    directional->setType(Ogre::Light::LT_DIRECTIONAL);
    directional->setDirection(0, -0.85f, -0.3f);

    Ogre::Light *point = mSceneMgr->createLight("pointlight");
    point->setType(Ogre::Light::LT_POINT);
    point->setPosition(100, 20, -3);

    //ground plane for testing
    planeGround.normal = Ogre::Vector3(0, 1, 0);
    planeGround.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("GroundPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    planeGround, 1500, 1500, 20, 20, true, 1, 100, 100, Ogre::Vector3::UNIT_Z);
    entityGround = mSceneMgr->createEntity("gnd", "GroundPlane");
    entityGround->setMaterialName("Examples/BumpyMetal");
    nodeGround = mSceneMgr->getRootSceneNode()->createChildSceneNode("groundnode");
    nodeGround->attachObject(entityGround);

    btTransform tr;
    tr.setIdentity();

    tr.setOrigin(btVector3(0, 0, 0));
    btCollisionShape *groundShape = new btBoxShape(btVector3(1000, 0, 1000));
    mCollisionShapes.push_back(groundShape);

    BtOgMotionState *motionState = new BtOgMotionState(tr, nodeGround);
    btRigidBody::btRigidBodyConstructionInfo conInfo(0, motionState, groundShape, btVector3(0, 0, 0));
    groundRigidBody = new btRigidBody(conInfo);

    mDynamicsWorld->addRigidBody(groundRigidBody);

    groundRigidBody->setFriction(200);

    mRigidBodies.push_back(groundRigidBody);


    //test wall
    Ogre::Plane box;
    box.normal = Ogre::Vector3(1, 0, 0);
    Ogre::SceneNode *nodeBox;
    Ogre::Entity *entityBox;
    Ogre::MeshManager::getSingleton().createPlane("oox", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, box, 1000, 100);
    entityBox = mSceneMgr->createEntity("box", "oox");
    entityBox->setMaterialName("Examples/BumpyMetal");
    nodeBox = mSceneMgr->getRootSceneNode()->createChildSceneNode("boxnode");
    nodeBox->attachObject(entityBox);
    nodeBox->yaw(Ogre::Angle(90));
    nodeBox->setPosition(0, 0, 500);

    tr.setIdentity();
    tr.setOrigin(btVector3(0, 0, 500));
    btCollisionShape *boxShape = new btBoxShape(btVector3(1000, 50, 10));
    mCollisionShapes.push_back(boxShape);
    BtOgMotionState *derpstate = new BtOgMotionState(tr, nodeBox);
    btRigidBody::btRigidBodyConstructionInfo consinfo(0, derpstate, boxShape);
    btRigidBody *boxbody = new btRigidBody(consinfo);
    mDynamicsWorld->addRigidBody(boxbody);
    mRigidBodies.push_back(boxbody);

    Ogre::Vector3 pos(-150, 10, 0);
    Road *nextRoad = new Road(pos);
    mRoads.push_back(nextRoad);
    for (int iii = 0; iii < 20; ++iii)
    {
        pos.x += 30;
        Road *road = new Road(pos, nextRoad);
        mRoads.push_back(road);
        nextRoad = road;
    }
}
//-------------------------------------------------------------------------------------------------------
bool GameState::keyPressed(const OIS::KeyEvent &keyEvent)
{
    //pause the state
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_ESCAPE))
    {
        pushAppState(findByName("PauseState"));
        return true;
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_CAPITAL))
    {
        mbSettingsMode = !mbSettingsMode;
        return true;
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_BACKSLASH))
    {
        mbBackslashDown = !mbBackslashDown;
        if (!mbBackslashDown)
            mDebugDrawer->deleteSceneNode();
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_F))    //create Drivers and their vehicles
    {
        Driver *driver = new Driver();
        std::shared_ptr<Driver> driverPtr(driver);
        mDrivers.push_back(driverPtr);
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_V))    //create 20 drivers and their vehicles
    {
        for (int iii = 0; iii < 20; ++iii)
        {
            Driver *driver = new Driver();
            std::shared_ptr<Driver> driverPtr(driver);
            mDrivers.push_back(driverPtr);
        }
    }

    //clear all balls
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_NUMPAD0))
    {
        mDrivers.clear();
//      mBalls.clear();
        //iterator starts at one because the ground plane is at 0
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
    OgreFramework::getSingletonPtr()->mouseMoved(mouseEvent);
    //if right mouse down, camera look is activated
    if (mbRMouseDown)
    {
        mCamera->yaw(Ogre::Degree(mouseEvent.state.X.rel * -0.18f));

        Ogre::Degree oldpitch = mCamera->getOrientation().getPitch();
        Ogre::Degree newpitch = (Ogre::Degree(mouseEvent.state.Y.rel * -0.18f)) + oldpitch;

        if (newpitch < Ogre::Degree(88.0f) && newpitch > Ogre::Degree(-88.0f))
        {
            mCamera->pitch(Ogre::Degree(mouseEvent.state.Y.rel * -0.18f));
        }

        if (Ogre::Degree(mCamera->getOrientation().getPitch()) < Ogre::Degree(-90))
            mCamera->pitch(Ogre::Degree(1));
        else if (Ogre::Degree(mCamera->getOrientation().getPitch()) > Ogre::Degree(90))
            mCamera->pitch(Ogre::Degree(-1));
    }

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool GameState::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    OgreFramework::getSingletonPtr()->mousePressed(mouseEvent, id);

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
    OgreFramework::getSingletonPtr()->mouseReleased(mouseEvent, id);

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
    if (mbBackslashDown)
        return;

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
            //mCurrentObject = mSceneMgr->getEntity(itRayScene->movable->getName())->getParentSceneNode();
            mCurrentObject = itRayScene->movable->getParentSceneNode();
            mCurrentObject->showBoundingBox(!mCurrentObject->getShowBoundingBox());

            DebugWindow::get()->debugVehicle(mCurrentObject->getName());
            mCurrentObject = nullptr;
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------------
void GameState::moveCamera()
{
    //if lshift down, move 10x faster
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_LSHIFT))
    {
        mCamera->moveRelative(mTranslateVector * 50);
    }
    mCamera->moveRelative(mTranslateVector * 20);
}
//-------------------------------------------------------------------------------------------------------
void GameState::getInput(float timesince)
{
    bool keyPressed = false; //so we can speed up the move speed, if not, set move speed to zero
    //false = unbuffered input
    if (mbSettingsMode == false)
    {
        //left/right
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_A))
        {
            mTranslateVector.x = -mMoveScale;
            keyPressed = true;
        }

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_D))
        {
            mTranslateVector.x = mMoveScale;
            keyPressed = true;
        }

        //front/back
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_W))
        {
            mTranslateVector.z = -mMoveScale;
            keyPressed = true;
        }

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_S))
        {
            mTranslateVector.z = mMoveScale;
            keyPressed = true;
        }

        //down/up
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_Q))
        {
            mTranslateVector.y = -mMoveScale;
            keyPressed = true;
        }

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_E))
        {
            mTranslateVector.y = mMoveScale;
            keyPressed = true;
        }

        if (keyPressed)
        {
            float tempaccel = mAcceleration + 0.01f;
            if (tempaccel < mMaxMoveSpeed)
            {
                mAcceleration = tempaccel;
                mMoveScale = tempaccel * (timesince / 1000);
                mMoveScale *= 50;
            }
        }
        else
        {
            mAcceleration = 0.f;
        }
        float damping = 1.00f - (5.f * (timesince / 1000));
        mTranslateVector *= Ogre::Math::Abs(damping);
    }
}
//-------------------------------------------------------------------------------------------------------
void GameState::update(double timeSinceLastFrame)
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    //OgreFramework::getSingletonPtr()->mTrayMgr->frameRenderingQueued(mFrameEvent);
    CEGUI::System::getSingleton().injectTimePulse(timeSinceLastFrame);

    if (mbQuit == true)
    {
        popAppState();
        return;
    }

    getInput(timeSinceLastFrame);
    moveCamera();

    auto itend = mDrivers.end();
    for (auto it = mDrivers.begin(); it != itend; ++it)
    {
        (*it)->update(timeSinceLastFrame);
    }

    updatePhysics();
    updateSound();
    
    mTimeSinceUpdate += timeSinceLastFrame;
    updateRoads();
    if (mTimeSinceUpdate >= 41)
    {
        mTimeSinceUpdate = 0;
        DebugWindow::get()->update();
    }
    
}
//-------------------------------------------------------------------------------------------------------
void GameState::buildGUI()
{
    CEGUI::MouseCursor::getSingleton().show();

    CEGUI::WindowManager &windowManager = CEGUI::WindowManager::getSingleton();
    CEGUI::Window *gameRoot = windowManager.loadWindowLayout("CatreonGameState.layout");
    CEGUI::System::getSingletonPtr()->setGUISheet(gameRoot);

    //construct DebugWindow
    DebugWindow::get();
}
//-------------------------------------------------------------------------------------------------------
void GameState::updatePhysics()
{
    if (mbBackslashDown)
    {
        mDebugDrawer->Update();
        mDynamicsWorld->debugDrawWorld();
    }
    int milliseconds = getMillisecondsFromLastCall();
#ifdef _DEBUG
    mDynamicsWorld->stepSimulation(static_cast<double>(milliseconds) / 1000, 10);
#else
    mDynamicsWorld->stepSimulation(static_cast<double>(milliseconds) / 1000, 100);
#endif
}
//-------------------------------------------------------------------------------------------------------
btVector3 GameState::ogreVecToBullet(const Ogre::Vector3 &ogrevector)
{
    return btVector3(ogrevector.x, ogrevector.y, ogrevector.z);
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3 GameState::bulletVecToOgre(const btVector3 &bulletvector)
{
    return Ogre::Vector3(bulletvector.x(), bulletvector.y(), bulletvector.z());
}
//-------------------------------------------------------------------------------------------------------
btQuaternion GameState::ogreQuatToBullet(const Ogre::Quaternion &ogrequat)
{
    return btQuaternion(ogrequat.x, ogrequat.y, ogrequat.z, ogrequat.w);
}
//-------------------------------------------------------------------------------------------------------
float GameState::round(double d, int places)
{
    return static_cast<int>(d * Ogre::Math::Pow(10, places)) / static_cast<float>(Ogre::Math::Pow(10, places));
}
//-------------------------------------------------------------------------------------------------------
int GameState::getMillisecondsFromLastCall()
{
    int nTime = mTimer->getMilliseconds();

    mTimer->reset();
    return nTime;

}
//-------------------------------------------------------------------------------------------------------
void GameState::updateSound()
{
    sound->setListenerPosition(ogreVecToIrr(mCamera->getDerivedPosition()), ogreVecToIrr(mCamera->getDerivedDirection()));
}
//-------------------------------------------------------------------------------------------------------
vec3df GameState::ogreVecToIrr(const Ogre::Vector3 &ogrevector)
{
    return vec3df(ogrevector.x, ogrevector.y, ogrevector.z);
}
//-------------------------------------------------------------------------------------------------------
Ogre::Vector3 GameState::irrVecToOgre(const vec3df &irrvector)
{
    return Ogre::Vector3(irrvector.X, irrvector.Y, irrvector.Z);
}
//-------------------------------------------------------------------------------------------------------
void GameState::updateRoads()
{
    auto it = mRoads.begin();
    auto itend = mRoads.end();
    for (it; it != itend; ++it)
    {
        (*it)->update();
    }
}