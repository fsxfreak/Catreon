#ifndef STDAFX_H
#define STDAFX_H

#include <OgreException.h>
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>

#include "AdvancedOgreFramework.hpp"
#include "AppState.hpp"
#include "AppStateManager.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"

#include "DotSceneLoader.h"

#include "Vehicle.h"
#include "Driver.h"
#include "demo.h"
#include "OgreFramework.h"

#include "irrKlang.h"


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#endif
