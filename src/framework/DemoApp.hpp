//code largely based on Advanced Ogre Framework at ogre3d.org
//minor formatting and name changes to fit my conventions

#ifndef DEMO_APP_HPP
#define DEMO_APP_HPP

#include "stdafx.h"

#include "framework\AdvancedOgreFramework.hpp"
#include "framework\AppStateManager.hpp"

class DemoApp
{
public:
    DemoApp();
    ~DemoApp();

    void startDemo();
private:
    AppStateManager *mAppStateManager;
};

#endif
