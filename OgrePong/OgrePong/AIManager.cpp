#include "AIManager.h"
#include "World.h"

// Include Ogre classes
#include "Ogre.h"
#include "OgreSceneNode.h"

AIManager::AIManager(World *world) : mWorld(world)
{
	PaddleSpeed = mWorld->getPaddleSpeed();
}

AIManager::~AIManager()
{
    // Clean up after yourself ...
}

void
AIManager::Control(float time, Ogre::SceneNode *mBallNode, Ogre::SceneNode *mLeftPaddleNode)
{
	if (mBallNode->getPosition().y <= mLeftPaddleNode->getPosition().y) {
		// AIpaddle goes down
		if (mLeftPaddleNode->getPosition().y >= -36) {				// Don't go across bottom wall
			mLeftPaddleNode->translate(0, -time * PaddleSpeed, 0);
		}
	} 
	else {
		// AIpaddle goes up
		if (mLeftPaddleNode->getPosition().y <= 35.3) {				// Don't go across top wall
			mLeftPaddleNode->translate(0, time * PaddleSpeed, 0);
		}
	}
}

void 
AIManager::Think(float time) 
{
	mBallNode = mWorld->getBallNode();
	setGraphics = mWorld->getSetGraphics();
	mLeftPaddleNode = mWorld->getLeftPaddleNode();
	beginnerDifficulty = mWorld->getBeginnerDifficulty();
	intermediateDifficulty = mWorld->getIntermediateDifficulty();
	expertDifficulty = mWorld->getExpertDifficulty();
	Paused = mWorld->getPaused();

	if (setGraphics) {
		if (beginnerDifficulty) {
			if (mBallNode->getPosition().x >= 36) {
				Control(time, mBallNode, mLeftPaddleNode);
			}
		}
		else if (intermediateDifficulty) {
			PaddleSpeed = 40;
			if (mBallNode->getPosition().x >= 24) {
				Control(time, mBallNode, mLeftPaddleNode);
			}
		}
		else if (expertDifficulty) {
			PaddleSpeed = 60;
			if (mBallNode->getPosition().x >= 5) {
				Control(time, mBallNode, mLeftPaddleNode);
			}
		}
	}
}

