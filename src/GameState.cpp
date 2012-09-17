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

using namespace irrklang;

GameState* GameState::mGameSingleton = nullptr;

GameState::GameState() :    mMoveSpeed(0.01f), mMaxMoveSpeed(0.5f), mTranslateVector(0, 0, 0),
                            mbLMouseDown(false), mbRMouseDown(false), 
                            mbQuit(false), mbSettingsMode(false), mbBackslashDown(false),
                            mTimer(new Ogre::Timer),
                            sound(0)
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
    //mRaySceneQuery->setQueryMask(OGRE_HEAD_MASK);

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

    mDynamicsWorld->setGravity(btVector3(0, -100, 0));

    Ogre::MaterialManager::getSingleton().load("DebugLines.material", "General");
    mDebugDrawer = new CDebugDraw(mSceneMgr, mDynamicsWorld);

    sound = createIrrKlangDevice();
    if (!sound)
        return;

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
    
    std::vector<Driver*>::iterator it = mDrivers.begin();
    for (it; it != mDrivers.end(); ++it)
    {
        delete *it;
    }
    mDrivers.clear();
    mBalls.clear();

    if (mSceneMgr)
        OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);

    for (int body = mDynamicsWorld->getNumCollisionObjects() - 1; body >= 0; body--)
    {
        btCollisionObject *object = mDynamicsWorld->getCollisionObjectArray()[body];
        mDynamicsWorld->removeCollisionObject(object);
    }
    mRigidBodies.clear();

    mCollisionShapes.clear();

    delete mDynamicsWorld;
    delete mSolver;
    delete mCollisionConfiguration;
    delete mDispatcher;
    delete mBroadphase;

    if (sound)
        sound->drop();
}
//-------------------------------------------------------------------------------------------------------
//inherited from Appstate, fill the scene
void GameState::createScene()
{
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

    /*btCollisionShape *groundShape;
    const float TRIANGLE_SIZE = 20.0f;
    //triangle mesh ground
    int vertStride = sizeof(btVector3);
    int indexStride = 3 * sizeof(int);
    const int NUM_VERTS_X = 200;
    const int NUM_VERTS_Y = 200;
    const int totalVerts = NUM_VERTS_X * NUM_VERTS_Y;
    const int totalTriangles = 2 * (NUM_VERTS_X - 1) * (NUM_VERTS_Y - 1);
    
    mVertices = new btVector3[totalVerts];
    int *gIndices = new int[totalTriangles * 3];

    for (int iii = 0; iii < NUM_VERTS_X; iii++)
    {
        for (int jjj = 0; jjj < NUM_VERTS_Y; jjj++)
        {
            float w1 = 0.2f;
            float height = 0.0f;    //flat land

            mVertices[iii + jjj * NUM_VERTS_X].setValue
                ( (iii - NUM_VERTS_X * 0.5f) * TRIANGLE_SIZE
                , height
                , (jjj - NUM_VERTS_Y * 0.5f) * TRIANGLE_SIZE);
        }
    }

    int index = 0;
    for (int iii = 0; iii < NUM_VERTS_X - 1; iii++)
    {
        for (int jjj = 0; jjj < NUM_VERTS_Y - 1; jjj++)
        {
            gIndices[index++] = jjj * NUM_VERTS_X + iii;
            gIndices[index++] = jjj * NUM_VERTS_X + iii + 1;
            gIndices[index++] = (jjj + 1) * NUM_VERTS_X + iii + 1;

            gIndices[index++] = jjj * NUM_VERTS_X + iii;
            gIndices[index++] = (jjj + 1) * NUM_VERTS_X + iii + 1;
            gIndices[index++] = (jjj + 1) * NUM_VERTS_X + iii;
        }
    }

    mIndexVertexArray = new btTriangleIndexVertexArray(totalTriangles, gIndices, indexStride, totalVerts, (btScalar*) &mVertices[0].x(), vertStride);
    bool useQuantizedAabbCompression = true;
    groundShape = new btBvhTriangleMeshShape(mIndexVertexArray, useQuantizedAabbCompression);*/

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
}
//-------------------------------------------------------------------------------------------------------
bool GameState::keyPressed(const OIS::KeyEvent &keyEvent)
{
    /*if (mbSettingsMode == true)
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
    }*/

    //pause the state
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_ESCAPE))
    {
        pushAppState(findByName("PauseState"));
        return true;
    }

    //toggle information
    /*if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_I))
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
    }*/

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_CAPITAL))
    {
        mbSettingsMode = !mbSettingsMode;
        return true;
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_BACKSLASH))
    {
        mbBackslashDown = !mbBackslashDown;
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_F))    //create Drivers and their vehicles
    {
        Driver *driver = new Driver();
        mDrivers.push_back(driver);
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_V))
    {
//      Ball *ball = new Ball(50, 5000.0, mCamera->getDerivedPosition() + (mCamera->getDerivedDirection() * Ogre::Vector3(20, 20, 20)), Ogre::Vector3(0, 0, 0));
//      mBalls.push_back(ball);
    }

    //clear all balls
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_NUMPAD0))
    {
        std::vector<Driver*>::iterator it = mDrivers.begin();
        for (it; it != mDrivers.end(); ++it)
        {
            delete *it;
        }
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
    /*if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseMove(mouseEvent))
    {
        return true;
    }*/
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
    /*if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseDown(mouseEvent, id))
    {
        return true;
    }*/
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
    /*if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseUp(mouseEvent, id))
    {
        return true;
    }*/
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

    if (mCurrentObject)
    {
        mCurrentObject->showBoundingBox(false);
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
            //OgreFramework::getSingletonPtr()->mLog->logMessage("MovableName: " + itRayScene->movable->getName());
            mCurrentObject = mSceneMgr->getEntity(itRayScene->movable->getName())->getParentSceneNode();
            //OgreFramework::getSingletonPtr()->mLog->logMessage("ObjName " + mCurrentObject->getName());
            mCurrentObject->showBoundingBox(true);
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------------
/*void GameState::itemSelected(OgreBites::SelectMenu *menu)
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
}*/
//-------------------------------------------------------------------------------------------------------
void GameState::moveCamera()
{
    //if lshift down, move 10x faster
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_LSHIFT))
    {
        mCamera->moveRelative(mTranslateVector * 5);
    }
    mCamera->moveRelative(mTranslateVector);
}
//-------------------------------------------------------------------------------------------------------
void GameState::getInput(float timesince)
{
    bool keyPressed = false; //so we can speed up the move speed, if not, set move speed to zero
    mMoveScale = mMoveSpeed * timesince;
    float damping = 0.95f;
    //false = unbuffered input
    if (mbSettingsMode == false)
    {
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_A))
        {
            mTranslateVector.x = -mMoveScale;
            keyPressed = true;
        }
        else
            mTranslateVector.x *= damping;

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_D))
        {
            mTranslateVector.x = mMoveScale;
            keyPressed = true;
        }
        else 
            mTranslateVector.x *= damping;

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_W))
        {
            mTranslateVector.z = -mMoveScale;
            keyPressed = true;
        }
        else
            mTranslateVector.z *= damping;

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_S))
        {
            mTranslateVector.z = mMoveScale;
            keyPressed = true;
        }
        else
            mTranslateVector.z *= damping;

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_Q))
        {
            mTranslateVector.y = -mMoveScale;
            keyPressed = true;
        }
        else
            mTranslateVector.y *= damping;

        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_E))
        {
            mTranslateVector.y = mMoveScale;
            keyPressed = true;
        }
        else
            mTranslateVector.y *= damping;

        if (keyPressed)
        {
            float tempspeed = mMoveSpeed * (10.f * (mMaxMoveSpeed - mMoveSpeed));
            if (tempspeed < mMaxMoveSpeed)
            {
                mMoveSpeed = tempspeed;
            }
        }
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

    /*if (!OgreFramework::getSingletonPtr()->mTrayMgr->isDialogVisible())
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
            mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mMoveSpeed));
            if (mCurrentObject)
            {
                mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCurrentObject->getPosition()));
            }
            if (mbSettingsMode)
            {
                mDetailsPanel->setParamValue(7, "Buffered Input");
            }
            else
                mDetailsPanel->setParamValue(7, "Un-buffered Input");
        }
    }*/

    

    //mTranslateVector = Ogre::Vector3::ZERO;   //works against my camera smoothing

    getInput(timeSinceLastFrame);
    moveCamera();
    updatePhysics();
    updateSound();
    
    for (std::vector<Driver*>::iterator it = mDrivers.begin(); it != mDrivers.end(); ++it)
    {
        (*it)->update(timeSinceLastFrame);
    }
}
//-------------------------------------------------------------------------------------------------------
void GameState::buildGUI()
{
    /*OgreFramework::getSingletonPtr()->mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
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

    Ogre::StringVector chatModes;
    chatModes.push_back("Solid mode");
    chatModes.push_back("Wireframe mode");
    chatModes.push_back("Point mode");
    OgreFramework::getSingletonPtr()->mTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "ChatModeSelMenu", "ChatMode", 200, 3, chatModes);*/
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
    mDynamicsWorld->stepSimulation(static_cast<double>(milliseconds) / 1000, 1000);
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
