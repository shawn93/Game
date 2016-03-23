#include "OgreCamera.h"
#include "Camera.h"
#include "World.h"
#include "OgreVector3.h"

PongCamera::PongCamera(Ogre::Camera *renderCamera, World *world) :
mRenderCamera(renderCamera), mWorld(world)
{
	mRenderCamera->setNearClipDistance(2);
}

void
PongCamera::Think(float time)
{
	mRenderCamera->setPosition(Ogre::Vector3(-135, 20, -75));
	mRenderCamera->lookAt(Ogre::Vector3(-100, 15, -50));

	// Any code needed here to move the camera about per frame
	//  (use mRenderCamera to get the actual render camera, of course!)
}
