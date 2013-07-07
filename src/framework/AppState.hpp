//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#ifndef APP_STATE_HPP
#define APP_STATE_HPP

#include "stdafx.h"
#include "framework\AdvancedOgreFramework.hpp"

class AppState;

class AppStateListener
{
public:
    AppStateListener() {};
    virtual ~AppStateListener() {};

    virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;
    
    virtual AppState*   findByName(Ogre::String stateName) = 0;
    virtual void        changeAppState(AppState *state) = 0;
    virtual bool        pushAppState(AppState *state) = 0;
    virtual void        popAppState() = 0;
    virtual void        pauseAppState() = 0;
    virtual void        shutdown() = 0;
    virtual void        popAllAndPushAppState(AppState* state) = 0;
};
//-------------------------------------------------------------------------------------------------------
class AppState : public OIS::KeyListener, public OIS::MouseListener
{
public:
    static void create(AppStateListener *parent, const Ogre::String name) {};

    void destroy() { delete this; }

    virtual void enter() = 0; 
    virtual void exit() = 0;
    virtual bool pause() { return true; }
    virtual void resume() {}
    virtual void update(double timeSinceLastFrame) = 0;

    Ogre::SceneManager *mSceneMgr;

protected:
    AppState() {};

    AppState*    findByName(Ogre::String stateName)         { return mParent->findByName(stateName); }
    void        changeAppState(AppState *state)             { mParent->changeAppState(state); }
    bool        pushAppState(AppState *state)               { return mParent->pushAppState(state); }
    void        popAppState()                               { mParent->popAppState(); }
    void        shutdown()                                  { mParent->shutdown(); }
    void        popAllAndPushAppState(AppState *state)      { mParent->popAllAndPushAppState(state); }

    AppStateListener *mParent;

    Ogre::FrameEvent mFrameEvent;
};
//-------------------------------------------------------------------------------------------------------
#define DECLARE_APPSTATE_CLASS(T)                                        \
static void create(AppStateListener* parent, const Ogre::String name)    \
{                                                                        \
    T* myAppState = new T();                                             \
    myAppState->mParent = parent;                                        \
    parent->manageAppState(name, myAppState);                            \
}

#endif
