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
********************************************************/


#include "stdafx.h"

#include "GameState.hpp"

using namespace irrklang;

GameState* GameState::mGameSingleton = NULL;

GameState::GameState() :    mMoveSpeed(0.1f), mRotateSpeed(0.3f), mbLMouseDown(false), mbRMouseDown(false), 
                            mbQuit(false), mbSettingsMode(false), mDetailsPanel(0), 
                            mTimer(new Ogre::Timer)
{
    mGameSingleton = this;  //is that even right? - doesn't matter, works
}
//-------------------------------------------------------------------------------------------------------
void GameState::enter()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Entering Gamestate...");

    //initialize the scene
    mSceneMgr = OgreFramework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));

    mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
    mRaySceneQuery->setQueryMask(OGRE_HEAD_MASK);

    mCamera = mSceneMgr->createCamera("GameCamera");
    mCamera->setPosition(Ogre::Vector3(5, 60, 120));
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
    mBroadphase = new btDbvtBroadphase();
    mSolver = new btSequentialImpulseConstraintSolver;
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

    mDynamicsWorld->setGravity(btVector3(0, -9.81, 0));


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

    if (mSceneMgr)
        OgreFramework::getSingletonPtr()->mRoot->destroySceneManager(mSceneMgr);

    for (int body = mDynamicsWorld->getNumCollisionObjects() - 1; body >= 0; body--)
    {
        btCollisionObject *object = mDynamicsWorld->getCollisionObjectArray()[body];
        mDynamicsWorld->removeCollisionObject(object);
    }
    mRigidBodies.clear();

    auto shapesIt = mCollisionShapes.begin();
    auto shapesEnd = mCollisionShapes.end();
    for (shapesIt; shapesIt != shapesEnd; ++shapesIt)
    {
        delete *shapesIt;
    }
    mCollisionShapes.clear();

    delete mDynamicsWorld;
    delete mSolver;
    delete mCollisionConfiguration;
    delete mDispatcher;
    delete mBroadphase;

    sound->drop();
}
//-------------------------------------------------------------------------------------------------------
//inherited from Appstate, fill the scene
void GameState::createScene()
{
    Ogre::Light *directional = mSceneMgr->createLight("directionallight");
    directional->setType(Ogre::Light::LT_DIRECTIONAL);
    directional->setDirection(0, -1, 0);

    DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
    pDotSceneLoader->parseDotScene("CubeScene.xml", "General", mSceneMgr, mSceneMgr->getRootSceneNode());
    delete pDotSceneLoader;

    //scale model for reference
    mScaleModel = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mScaleEntity = mSceneMgr->createEntity("Scale", "scalingtest.mesh");
    mScaleModel->attachObject(mScaleEntity);
    mScaleModel->setPosition(-30, 4, 0);

    //ground plane for testing
    planeGround.normal = Ogre::Vector3(0, 1, 0);
    planeGround.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("GroundPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    planeGround, 200000, 200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Z);
    entityGround = mSceneMgr->createEntity("Ground", "GroundPlane");
    entityGround->setMaterialName("Examples/BumpyMetal");
    nodeGround = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    nodeGround->attachObject(entityGround);
    
    btCollisionShape *plane = new btBoxShape(btVector3(btScalar(9000.), btScalar(1.), btScalar(9000.)));
    mCollisionShapes.push_back(plane);

    btTransform planeTransform;
    planeTransform.setIdentity();
    planeTransform.setOrigin(btVector3(0, -2, 0));

    btScalar mass(0);
    btVector3 localInertia(0, 0, 0);
    plane->calculateLocalInertia(mass, localInertia);

    //initialize the plane as a rigid body
    BtOgMotionState* groundMotionState = new BtOgMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)), nodeGround);
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, plane, localInertia);
    groundRigidBody = new btRigidBody(groundRigidBodyCI);

    groundRigidBody->setFriction(8000);

    mDynamicsWorld->addRigidBody(groundRigidBody);
    mRigidBodies.push_back(groundRigidBody);

    mOgreHeadEntity = mSceneMgr->createEntity("Cube", "ogrehead.mesh");
    mOgreHeadEntity->setQueryFlags(OGRE_HEAD_MASK);
    mOgreHeadNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CubeNode");
    mOgreHeadNode->attachObject(mOgreHeadEntity);
    mOgreHeadNode->setPosition(Ogre::Vector3(0, 0, -15));

    mOgreHeadMaterial = mOgreHeadEntity->getSubEntity(1)->getMaterial();
    mOgreHeadMaterialHigh = mOgreHeadMaterial->clone("OgreHeadMaterialHigh");
    mOgreHeadMaterialHigh->getTechnique(0)->getPass(0)->setAmbient(1, 0, 0);
    mOgreHeadMaterialHigh->getTechnique(0)->getPass(0)->setDiffuse(1, 0 ,0 , 0);

    mSceneMgr->getEntity("Cube01")->setQueryFlags(CUBE_MASK);
    mSceneMgr->getEntity("Cube02")->setQueryFlags(CUBE_MASK);
    mSceneMgr->getEntity("Cube03")->setQueryFlags(CUBE_MASK);
}
//-------------------------------------------------------------------------------------------------------
bool GameState::keyPressed(const OIS::KeyEvent &keyEvent)
{
    if (mbSettingsMode == true)
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
    }

    //pause the state
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_ESCAPE))
    {
        pushAppState(findByName("PauseState"));
        return true;
    }

    //toggle information
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_I))
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
    }

    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_TAB))
    {
        mbSettingsMode = !mbSettingsMode;
        return true;
    }

    //on press enter
    //mbsettingsmode true = buffered input(non continuous)
    if (mbSettingsMode && OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_RETURN) ||
        OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_F))
    {   
        /*
        //default sphere has radius of 50 units
        Ogre::Entity* mSphereEntity = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
        mSphereEntity->setMaterialName("Examples/BumpyMetal");
        //get a position slightly in front of the camera
        Ogre::Vector3 spherePosition = mCamera->getPosition() + (mCamera->getDerivedDirection() * Ogre::Vector3(20, 20, 20));
        Ogre::SceneNode *sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(spherePosition, Ogre::Quaternion(Ogre::Real(0)));
        //make a radius of 4 meters
        sphereNode->setScale(0.1, 0.1, 0.1);
        sphereNode->attachObject(mSphereEntity);
        mSphereNodes.push_back(sphereNode);

        //let bullet have the position sphere is intiailized on
        btVector3 btSpherePosition = ogreVecToBullet(spherePosition);

        btTransform sphereTransform;
        sphereTransform.setIdentity();

        btScalar sphereMass(50);
        btVector3 sphereInertia(0, 0, 0);
        mCollisionShapes[1]->calculateLocalInertia(sphereMass, sphereInertia);

        sphereTransform.setOrigin(btSpherePosition);

        BtOgMotionState* sphereState = new BtOgMotionState(sphereTransform, sphereNode);
        btRigidBody::btRigidBodyConstructionInfo sphereInfo(sphereMass, sphereState, mCollisionShapes[1], sphereInertia);
        btRigidBody* spherebody = new btRigidBody(sphereInfo);

        spherebody->setFriction(50);

        int sphereForce = 0;
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_LSHIFT))
            sphereForce = 300;
        else
            sphereForce = 50;


        spherebody->setLinearVelocity(ogreVecToBullet(mCamera->getDerivedDirection().normalisedCopy() * sphereForce));

        mDynamicsWorld->addRigidBody(spherebody);
        mRigidBodies.push_back(spherebody);
        */
        Ball *ball = new Ball(1, mCamera->getPosition() + (mCamera->getDerivedDirection() * Ogre::Vector3(20, 20, 20))); 
    }

    //clear all spheres
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_NUMPAD0))
    {
        for (int iii = 0; iii < mSphereNodes.size(); ++iii)
        {
            if (mSphereNodes[iii] && mSphereNodes[iii] != mSceneMgr->getRootSceneNode())
            {
                mSphereNodes[iii]->detachAllObjects();
                mSceneMgr->destroySceneNode(mSphereNodes[iii]);
            }
        }
        mSphereNodes.clear();
        //iterator starts at one because the ground plane is at 0
        for (int body = mDynamicsWorld->getNumCollisionObjects() - 1; body >= 1; body--)
        {
            btCollisionObject *object = mDynamicsWorld->getCollisionObjectArray()[body];
            mDynamicsWorld->removeCollisionObject(object);
        }
        mRigidBodies.clear();
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
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseMove(mouseEvent))
    {
        return true;
    }
    //if right mouse down, camera look is activated
    if (mbRMouseDown)
    {
        mCamera->yaw(Ogre::Degree(mouseEvent.state.X.rel * -0.18f));
        mCamera->pitch(Ogre::Degree(mouseEvent.state.Y.rel * -0.18f));
    }

    return true;
}
//-------------------------------------------------------------------------------------------------------
bool GameState::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseDown(mouseEvent, id))
    {
        return true;
    }

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
    if (OgreFramework::getSingletonPtr()->mTrayMgr->injectMouseUp(mouseEvent, id))
    {
        return true;
    }

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
    if (mCurrentObject)
    {
        mCurrentObject->showBoundingBox(false);
        mCurrentEntity->getSubEntity(1)->setMaterial(mOgreHeadMaterial);
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
            OgreFramework::getSingletonPtr()->mLog->logMessage("MovableName: " + itRayScene->movable->getName());
            mCurrentObject = mSceneMgr->getEntity(itRayScene->movable->getName())->getParentSceneNode();
            OgreFramework::getSingletonPtr()->mLog->logMessage("ObjName " + mCurrentObject->getName());
            mCurrentObject->showBoundingBox(true);
            mCurrentEntity = mSceneMgr->getEntity(itRayScene->movable->getName());
            mCurrentEntity->getSubEntity(1)->setMaterial(mOgreHeadMaterialHigh);
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------------
void GameState::itemSelected(OgreBites::SelectMenu *menu)
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
}
//-------------------------------------------------------------------------------------------------------
void GameState::moveCamera()
{
    //if lshift down, move 10x faster
    if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_LSHIFT))
    {
        mCamera->moveRelative(mTranslateVector);
    }
    mCamera->moveRelative(mTranslateVector / 10);
}
//-------------------------------------------------------------------------------------------------------
void GameState::getInput()
{
    //false = unbuffered input
    if (mbSettingsMode == false)
    {
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_A))
            mTranslateVector.x = -mMoveScale;
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_D))
            mTranslateVector.x = mMoveScale;
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_W))
            mTranslateVector.z = -mMoveScale;
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_S))
            mTranslateVector.z = mMoveScale;
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_Q))
            mTranslateVector.y = -mMoveScale;
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_E))
            mTranslateVector.y = mMoveScale;
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_Z))
            mCamera->roll(Ogre::Angle(-mMoveScale));
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_X))
            mCamera->roll(Ogre::Angle(mMoveScale));
        //reset roll
        if (OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_C))
            mCamera->roll(-(mCamera->getRealOrientation().getRoll()));
    }
}
//-------------------------------------------------------------------------------------------------------
void GameState::update(double timeSinceLastFrame)
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->mTrayMgr->frameRenderingQueued(mFrameEvent);

    if (mbQuit == true)
    {
        popAppState();
        return;
    }

    if (!OgreFramework::getSingletonPtr()->mTrayMgr->isDialogVisible())
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
            if (mbSettingsMode)
            {
                mDetailsPanel->setParamValue(7, "Buffered Input");
            }
            else
                mDetailsPanel->setParamValue(7, "Un-buffered Input");
        }
    }

    mMoveScale = mMoveSpeed * timeSinceLastFrame;
    mRotateScale = mRotateSpeed * timeSinceLastFrame;

    mTranslateVector = Ogre::Vector3::ZERO;

    getInput();
    moveCamera();
    updatePhysics();
    updateSound();
}
//-------------------------------------------------------------------------------------------------------
void GameState::buildGUI()
{
    OgreFramework::getSingletonPtr()->mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
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
    OgreFramework::getSingletonPtr()->mTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "ChatModeSelMenu", "ChatMode", 200, 3, chatModes);
}
//-------------------------------------------------------------------------------------------------------
void GameState::updatePhysics()
{
    int milliseconds = getMillisecondsFromLastCall();
    mDynamicsWorld->stepSimulation(static_cast<double>(milliseconds) / 1000, 30);
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
