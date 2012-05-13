/*
Graphics and Input Handling code largely based on Advanced Ogre Framework at ogre3d.org

author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "stdafx.h"
#include "AppState.hpp"

#include "DotSceneLoader.h"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

#include "Vehicle.h"
#include "Driver.h"

#include "BulletPhys.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BtOgMotionState.h"

enum QueryFlags
{
    OGRE_HEAD_MASK = 1 << 0,
    CUBE_MASK = 1 << 1
};

class GameState: public AppState
{
public:
    GameState();
    
    DECLARE_APPSTATE_CLASS(GameState)

    void enter();
    void createScene();
    void exit();
    bool pause();
    void resume();

    void moveCamera();
    void getInput();
    void buildGUI();

    bool keyPressed(const OIS::KeyEvent &keyEvent);
    bool keyReleased(const OIS::KeyEvent &keyEvent);

    bool mouseMoved(const OIS::MouseEvent &mouseEvent);
    bool mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);

    void onLeftPressed(const OIS::MouseEvent &mouseEvent);
    void itemSelected(OgreBites::SelectMenu *menu);

    void update(double timeSinceLastFrame);

    void updatePhysics(double deltaTime);

private:

    Ogre::SceneNode         *mOgreHeadNode;
    Ogre::Entity            *mOgreHeadEntity;
    Ogre::MaterialPtr       mOgreHeadMaterial;
    Ogre::MaterialPtr       mOgreHeadMaterialHigh;

    Ogre::SceneNode         *nodeGround;
    Ogre::Entity            *entityGround;
    Ogre::Plane             planeGround;
    BtOgMotionState         *groundMotionState;
    btRigidBody             *groundRigidBody;

    Ogre::SceneNode         *mSphereNode;
    Ogre::Entity            *mSphereEntity;
    Ogre::Vector3           spherePosition;
    btCollisionShape        *btSphere;
    std::deque<BtOgMotionState*> sphereMotionStates;
    btRigidBody             *sphereRigidBody;
    BtOgMotionState         *sphereMotionState; 
                            
    OgreBites::ParamsPanel  *mDetailsPanel;
    bool                    mbQuit;

    Ogre::Vector3           mTranslateVector;
    Ogre::Real              mMoveSpeed;
    Ogre::Degree            mRotateSpeed;
    float                   mMoveScale;
    Ogre::Degree            mRotateScale;

    Ogre::RaySceneQuery     *mRaySceneQuery;
    Ogre::SceneNode         *mCurrentObject;
    Ogre::Entity            *mCurrentEntity;
    bool                    mbLMouseDown;
    bool                    mbRMouseDown;
    bool                    mbSettingsMode;

    //physics engine
    BulletPhys              *bullet;
    bool                    physicsInitialized;
};
#endif
