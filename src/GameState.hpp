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
#include "framework\AppState.hpp"

#include "framework\DotSceneLoader.h"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "BtOgMotionState.h"
#include <framework\DebugDraw.hpp>

#include <objects\ball\Ball.hpp>
#include <objects\vehicles\Vehicle.h>
#include <objects\vehicles\Driver.h>
#include <objects\Road.hpp>

#include <irrKlang.h>

#include <CEGUI.h>
#include <CEGUIOgreRenderer.h>
#include <GUI\DebugWindow.hpp>

using namespace irrklang;

enum QueryFlags
{
    OGRE_HEAD_MASK = 1 << 0,
    CUBE_MASK = 1 << 1
};

//forward declares
class Ball;
class Vehicle;
class Driver;
class BtOgMotionState;
class CDebugDraw;
class DebugWindow;
class Road;

//EVERYTHING SHALL BE PUBLIC -throws enscapulation out the window-
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
    void getInput(float timesince);
    void buildGUI();

    bool keyPressed(const OIS::KeyEvent &keyEvent);
    bool keyReleased(const OIS::KeyEvent &keyEvent);

    bool mouseMoved(const OIS::MouseEvent &mouseEvent);
    bool mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);

    void onLeftPressed(const OIS::MouseEvent &mouseEvent);
    //void itemSelected(OgreBites::SelectMenu *menu);

    void update(double timeSinceLastFrame);
    int getMillisecondsFromLastCall();

    void updatePhysics();

    static btVector3 ogreVecToBullet(const Ogre::Vector3 &ogrevector);
    static Ogre::Vector3 bulletVecToOgre(const btVector3 &bulletvector);
    static btQuaternion ogreQuatToBullet(const Ogre::Quaternion &ogrequat);

    void updateSound();
    //void updateRoads();

    static vec3df ogreVecToIrr(const Ogre::Vector3 &ogrevector);
    static Ogre::Vector3 irrVecToOgre(const vec3df &irrvector);

    static float round(double d, int places);

    Ogre::SceneManager *mSceneMgr;
    Ogre::Camera       *mCamera;

    static GameState* getSingleton() { return mGameSingleton; }

//private: (public for singleton access)
    static GameState                        *mGameSingleton;
    Ogre::Timer                             *mTimer;

    //scale model for testing
    Ogre::SceneNode                         *mScaleModel;
    Ogre::Entity                            *mScaleEntity;

    Ogre::SceneNode                         *mOgreHeadNode;
    Ogre::Entity                            *mOgreHeadEntity;
    Ogre::MaterialPtr                       mOgreHeadMaterial;
    Ogre::MaterialPtr                       mOgreHeadMaterialHigh;

    Ogre::SceneNode                         *nodeGround;
    Ogre::Entity                            *entityGround;
    Ogre::Plane                             planeGround;
    BtOgMotionState                         *groundMotionState;
    btRigidBody                             *groundRigidBody;

    Ogre::Vector3                           spherePosition;
    std::deque<Ogre::SceneNode*>            mSphereNodes;
                            
    //OgreBites::ParamsPanel                *mDetailsPanel;
    bool                                    mbQuit;

    //camera speed scales and vectors
    Ogre::Vector3                           mTranslateVector;
    float                                   mAcceleration;
    float                                   mMoveScale;
    float                                   mMaxMoveSpeed;

    Ogre::RaySceneQuery                     *mRaySceneQuery;
    Ogre::SceneNode                         *mCurrentObject;    //currently selected object
    Ogre::Entity                            *mCurrentEntity;
    bool                                    mbLMouseDown;
    bool                                    mbRMouseDown;
    bool                                    mbSettingsMode;
    bool                                    mbBackslashDown;

    //physics engine

    btDefaultCollisionConfiguration         *mCollisionConfiguration;
    btCollisionDispatcher                   *mDispatcher;
    btBroadphaseInterface                   *mBroadphase;
    btSequentialImpulseConstraintSolver     *mSolver;
    btDiscreteDynamicsWorld                 *mDynamicsWorld;
    btOverlappingPairCallback               *mGhostCallback;

    std::vector<btCollisionShape*>          mCollisionShapes;
    std::vector<btRigidBody*>               mRigidBodies;
    std::vector<Ball*>                      mBalls;
    std::vector<std::shared_ptr<Driver> >   mDrivers;

    btVector3                               *mVertices;
    btTriangleIndexVertexArray              *mIndexVertexArray;

    CDebugDraw                              *mDebugDrawer;

    //sound engine
    ISoundEngine                            *sound;

    //GUI
    float                                   mTimeSinceUpdate;

};

//just a little of what I like to call function typedef
//singleton design thanks to Jabberwocky at Ogre3d
inline GameState* getGameState()
{
    return GameState::getSingleton();
}
#endif
