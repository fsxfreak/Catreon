/*
Graphics and Input Handling code largely based on Advanced Ogre Framework at ogre3d.org

author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/


#include "stdafx.h"

#include "GameState.hpp"

GameState::GameState() :    mMoveSpeed(0.1f), mRotateSpeed(0.3f), mbLMouseDown(false), mbRMouseDown(false), 
                            mbQuit(false), mbSettingsMode(false), mDetailsPanel(0), 
                            physicsInitialized(false),
                            mCollisionConfiguration(new btDefaultCollisionConfiguration()),
                            mDispatcher(new btCollisionDispatcher(mCollisionConfiguration)),
                            mBroadphase(new btDbvtBroadphase()),
                            mSolver(new btSequentialImpulseConstraintSolver),
                            mDynamicsWorld(new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration))
{
    mDynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}
//-------------------------------------------------------------------------------------------------------
void GameState::enter()
{
    OgreFramework::getSingletonPtr()->mLog->logMessage("Entering Gamestate...");

    //initialize the scene
    mSceneMgr = OgreFramework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    //mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.7f, 0.7f));

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

    buildGUI();

    createScene();

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

    for (int body = 0; body < mRigidBodies.size(); ++body)
    {
        mDynamicsWorld->removeRigidBody(mRigidBodies[body]);
        delete mRigidBodies[body]->getMotionState();
        delete mRigidBodies[body];
    }

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
}
//-------------------------------------------------------------------------------------------------------
//inherited from Appstate, fill the scene
void GameState::createScene()
{
    mSceneMgr->createLight()->setPosition(Ogre::Vector3(0, 75, 0));

    DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
    pDotSceneLoader->parseDotScene("CubeScene.xml", "General", mSceneMgr, mSceneMgr->getRootSceneNode());
    delete pDotSceneLoader;

    //ground plane for testing
    planeGround.normal = Ogre::Vector3(0, 1, 0);
    planeGround.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("GroundPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    planeGround, 200000, 200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Z);
    entityGround = mSceneMgr->createEntity("Ground", "GroundPlane");
    entityGround->setMaterialName("Examples/BumpyMetal");
    nodeGround = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    nodeGround->attachObject(entityGround);
    entityGround->setCastShadows(0);
    
    btCollisionShape *plane = new btBoxShape(btVector3(btScalar(1500.), btScalar(1.), btScalar(1500.)));
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

    mDynamicsWorld->addRigidBody(groundRigidBody);
    mRigidBodies.push_back(groundRigidBody);
    
    //initialize the sphere for later creation, physics test
    //mSphereEntity = mSceneMgr->createEntity("Sphere", Ogre::SceneManager::PT_SPHERE);
    //0.1 scale = 1 unit (1 meter)

    btCollisionShape* btSphere = new btSphereShape(4);
    mCollisionShapes.push_back(btSphere);

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
        OgreFramework::getSingletonPtr()->mKb->isKeyDown(OIS::KC_NUMPADENTER))
    {
        /*
        mSphereNode->detachAllObjects();
        spherePosition = (mCamera->getPosition() + (mCamera->getDirection() * Ogre::Vector3(20, 20, 20)));

        btVector3 btSpherePosition = ogreVecToBullet(spherePosition);
		mSphereNode->setPosition(spherePosition);
		mSphereNode->attachObject(mSphereEntity);

        sphereMotionState = new BtOgMotionState(btTransform(btQuaternion(0, 0, 0, 1), btSpherePosition), mSphereNode);
        sphereMotionStates.push_back(sphereMotionState);
        btScalar massSphere = 1;
        btVector3 fallInertia(0, 0, 0);
        btSphere->calculateLocalInertia(massSphere, fallInertia);

        btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(massSphere, sphereMotionState, btSphere, fallInertia);
        sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
        mDynamicsWorld->addRigidBody(sphereRigidBody);
        

        physicsInitialized = true;
        */
        
        //default sphere has radius of 50 units
        Ogre::Entity* mSphereEntity = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
        //get a position slightly in front of the camera
        Ogre::Vector3 spherePosition = mCamera->getPosition() + (mCamera->getDerivedDirection() * Ogre::Vector3(20, 20, 20));
        Ogre::SceneNode *sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(spherePosition, Ogre::Quaternion(Ogre::Real(0)));
        //make a radius of 5 meters
        sphereNode->setScale(0.1, 0.1, 0.1);
        sphereNode->attachObject(mSphereEntity);
        //mSphereNodes.push_back(sphereNode);

        //let bullet have the position sphere is intiailized on
        btVector3 btSpherePosition = ogreVecToBullet(spherePosition);

        btTransform sphereTransform;
        sphereTransform.setIdentity();

        btScalar sphereMass(1);
        btVector3 sphereInertia(0, 0, -1);
        mCollisionShapes[1]->calculateLocalInertia(sphereMass, sphereInertia);

        sphereTransform.setOrigin(btSpherePosition);

        BtOgMotionState* sphereState = new BtOgMotionState(sphereTransform, sphereNode);
        btRigidBody::btRigidBodyConstructionInfo sphereInfo(sphereMass, sphereState, mCollisionShapes[1], sphereInertia);
        btRigidBody* spherebody = new btRigidBody(sphereInfo);

        spherebody->setLinearVelocity(ogreVecToBullet(mCamera->getDerivedDirection().normalisedCopy() * 50));

        mDynamicsWorld->addRigidBody(spherebody);
        mRigidBodies.push_back(spherebody);

        physicsInitialized = true;
        
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
    updatePhysics(timeSinceLastFrame);
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
void GameState::updatePhysics(double deltaTime)
{
    if (physicsInitialized)
    {
        mDynamicsWorld->stepSimulation(deltaTime / 500, 50);
    }
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
