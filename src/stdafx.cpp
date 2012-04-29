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

#include "Vehicle.h"
#include "Driver.h"
#include "DemoApp.hpp"

#include "AdvancedOgreFramework.hpp"
#include "AppState.hpp"
#include "AppStateManager.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"

	#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		#define WIN32_LEAN_AND_MEAN
		#include "windows.h"
	#endif