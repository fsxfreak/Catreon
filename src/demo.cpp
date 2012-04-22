#include "stdafx.h"
#include "demo.h"
#include <iostream>

OgreApp::OgreApp() : mNodePenguin(0), mEntityPenguin(0)
{

}

OgreApp::~OgreApp()
{
	delete OgreFramework::getSingletonPtr();
}

void OgreApp::startDemo()
{
	new OgreFramework();

	//graphics and sound initialized
	if (!OgreFramework::getSingletonPtr()->initOgre("OgreApp", this, 0))
		return;


	mShutDown = false;

	OgreFramework::getSingletonPtr()->mLog->logMessage("App initialized");

	createScene();
	runDemo();
}

bool OgreApp::keyPressed(const OIS::KeyEvent &keyEvent)
{
	OgreFramework::getSingletonPtr()->keyPressed(keyEvent);
	

	return true;
}

bool OgreApp::keyReleased(const OIS::KeyEvent &keyEvent)
{
	OgreFramework::getSingletonPtr()->keyReleased(keyEvent);
	return true;
}

void OgreApp::createScene()
{
	OgreFramework::getSingletonPtr()->mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	OgreFramework::getSingletonPtr()->mSceneMgr->createLight("Light")->setPosition(75, 75, 75);

	mEntityPenguin = OgreFramework::getSingletonPtr()->mSceneMgr->createEntity("penguin.mesh");
	mNodePenguin = OgreFramework::getSingletonPtr()->mSceneMgr->getRootSceneNode()->createChildSceneNode("PenguinNode");
	mNodePenguin->attachObject(mEntityPenguin);

	mEntityTerrain = OgreFramework::getSingletonPtr()->mSceneMgr->createEntity("city2.mesh");
	
	mNodeTerrain = OgreFramework::getSingletonPtr()->mSceneMgr->getRootSceneNode()->createChildSceneNode("TerrainNode");
	mNodeTerrain->attachObject(mEntityTerrain);

	mNodeTerrain->setPosition(0, -200, 0);

	Vehicle mVehicleTest(200, 2);
	mVehicleTest.setSpeed(50);
	mVehicleTest.getDriver().updateDestination("San Francisco");

	std::cout << "speed: " << mVehicleTest.getSpeed() << " destination: " << mVehicleTest.getDriver().getDestination() << std::endl;
	
}

void OgreApp::runDemo()
{
	srand(GetTickCount());

	OgreFramework::getSingletonPtr()->mLog->logMessage("Start main loop.");

	double timeSinceLastFrame = 0;
	double frameStartTime = 0;

	OgreFramework::getSingletonPtr()->mRenderWindow->resetStatistics();

	while (!mShutDown && !OgreFramework::getSingletonPtr()->isOgreToBeShutDown())
	{
		if (OgreFramework::getSingletonPtr()->mRenderWindow->isClosed())
		{
			mShutDown = true;
			OgreFramework::getSingletonPtr()->mLog->logMessage("Shutdown message received");
		}

		Ogre::WindowEventUtilities::messagePump();

		if (OgreFramework::getSingletonPtr()->mRenderWindow->isActive())
		{
			frameStartTime = OgreFramework::getSingletonPtr()->mTimer->getMillisecondsCPU();

			OgreFramework::getSingletonPtr()->mKb->capture();
			OgreFramework::getSingletonPtr()->mMouse->capture();

			OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			OgreFramework::getSingletonPtr()->mRoot->renderOneFrame();
			
			timeSinceLastFrame = OgreFramework::getSingletonPtr()->mTimer->getMillisecondsCPU() - frameStartTime;
		}
		else	//window not active
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			Sleep(1000);
#else
			sleep(1);
#endif
		}
	}

	OgreFramework::getSingletonPtr()->mLog->logMessage("Ogre shutting down");
}

//main function//////////////////////////////////////////////////////////////////////////////////////////
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        try {
            OgreApp app;
			app.startDemo();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif