#include "stdafx.h"

#include <OgreException.h>
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>

#include "objects\vehicles\Vehicle.h"
#include "objects\vehicles\Driver.h"
#include "objects\Object.hpp"

#include "framework\DemoApp.hpp"

#include "framework\AdvancedOgreFramework.hpp"
#include "framework\AppState.hpp"
#include "framework\AppStateManager.hpp"
#include "framework\MenuState.hpp"
#include "GameState.hpp"


    #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        #define WIN32_LEAN_AND_MEAN
        #include "windows.h"
    #endif
