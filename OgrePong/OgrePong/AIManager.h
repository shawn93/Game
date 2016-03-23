#ifndef __AIManager_h_
#define __AIManager_h_

namespace Ogre {
	class SceneNode;
}

class World;

class AIManager 
{

public:
    // You may want to add parameters to the AI Manager constructor
	AIManager(World *world);
	~AIManager();

    // Go through all of the AIs, and call think on each one
	void Control(float time, Ogre::SceneNode *mBallNode, Ogre::SceneNode *mLeftPaddleNode);

    void Think(float time);


protected:
	World *mWorld;
	float BallSpeed;
	Ogre::SceneNode *mBallNode;
	Ogre::SceneNode *mLeftPaddleNode;
	float PaddleSpeed;

	bool setGraphics;
	bool beginnerDifficulty;
	bool intermediateDifficulty;
	bool expertDifficulty;
	bool Paused;
};

#endif