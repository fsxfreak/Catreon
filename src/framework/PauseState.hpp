//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#ifndef PAUSE_STATE_HPP
#define PAUSE_STATE_HPP
#include "stdafx.h"

#include "framework\AppState.hpp"

class PauseState : public AppState
{
public:
    PauseState();

    DECLARE_APPSTATE_CLASS(PauseState)

    void enter();
    void createScene();
    void exit();

    bool keyPressed(const OIS::KeyEvent &keyEvent);
    bool keyReleased(const OIS::KeyEvent &keyEvent);

    bool mouseMoved(const OIS::MouseEvent &mouseEvent);
    bool mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id);

    void buttonHit(const CEGUI::EventArgs &mouseEvent);
    void buttonHovered(const CEGUI::EventArgs &mouseEvent);

    void update(double timeSinceLastFrame);

    Ogre::SceneManager *mSceneMgr;
    Ogre::Camera       *mCamera;
private:
    bool mbQuit;
    bool mbQuestionActive;
    irrklang::ISoundEngine *mSound;
};
#endif
