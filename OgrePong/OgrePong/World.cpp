// My header file.  This should go first!
#include "World.h"

// Ogre header files
#include "Ogre.h"
#include "OgreMath.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreFontManager.h"
#include "OgreTextAreaOverlayElement.h"

// IOS (Input system) header files

#include <ois/ois.h>

// Other input files for my project
#include "Camera.h"
#include "InputHandler.h"

// For srand, rand
#include <stdlib.h>
#include <time.h>

using namespace std;

World::World(Ogre::SceneManager *sceneManager, InputHandler *input)   : mSceneManager(sceneManager), mInputHandler(input)
{

	choseDifficulty = false;
	beginnerDifficulty = false;
	intermediateDifficulty = false;
	expertDifficulty = false;
	setGraphics = false;
	Start = true;
	HitTopWall = false;
	HitBottomWall = false;
	HitByAI = false;
	HitByPlayer = false;
	AIScored = false;
	PlayerScored = false;
	Paused = false;
	AIWon = false;
	PlayerWon = false;
	GameOver = false;

	BallSpeed = 36.0;
	PaddleSpeed = 36.0;
	random = 0;
	numHits = 0;
	AIScore = 0;
	PlayerScore = 0;

	// Global illumination for now.  Adding individual light sources will make you scene look more realistic
	mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));

	// Yeah, this should be done automatically for all fonts referenced in an overlay file.
	//  But there is a bug in the OGRE code so we need to do it manually.
	Ogre::ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();
	while (iter.hasMoreElements()) 
	{ 
		iter.getNext()->load(); 
	}

	//Show ScoreBoard
	Scoreboard();
}

float
World::getBallSpeed() {
	return BallSpeed;
}

float
World::getPaddleSpeed() {
	return PaddleSpeed;
}

void 
World::Think(float mTime)
{
	const float RADIANS_PER_SECOND = 0.5;

	if (!choseDifficulty) {
		startMenu();
		overly1->hide();
		overly2->hide();
	} 
	else {
		if (!GameOver) {
			if (!setGraphics) {
				Models();
				Scoreboard();
				setGraphics = true;
				menuDifficultyOverly->hide();
				menuBeginnerOverly->hide();
				menuIntermediateOverly->hide();
				menuExpertOverly->hide();
			}

			// Set Ball Speed for Intermediate Difficulty
			if (getIntermediateDifficulty()) {
				BallSpeed = 40;
				PaddleSpeed = 40;
			}

			// Set Ball Speed for Expert Difficulty
			if (getExpertDifficulty()) {
				BallSpeed = 65;
				PaddleSpeed = 65;
			}

			/* Initialize random seed */
			srand (time(NULL));

			Move(mTime);

			//Right Paddle move up
			if (mInputHandler->IsKeyDown(OIS::KC_UP))
			{
				if (mRightPaddleNode->getPosition().y <= 36) // Don't cross the Top Wall
					mRightPaddleNode->translate(0, mTime * PaddleSpeed, 0);
			}

			//Right Paddle move down
			else if (mInputHandler->IsKeyDown(OIS::KC_DOWN))
			{
				if (mRightPaddleNode->getPosition().y >= -36) // Don't cross the Bottom Wall
					mRightPaddleNode->translate(0, -mTime * PaddleSpeed, 0);
			}

			updateScoreboard();	
		} 
		else {
			endGame();
			DetachAllObjects();
		}
	} 
}

Ogre::SceneNode *World::getBallNode() {
	return mBallNode;
}

Ogre::SceneNode *World::getLeftPaddleNode() {
	return mLeftPaddleNode;
}

void 
World::Models(void) {

	// Create ball
	Ogre::Entity *Ball = mSceneManager->createEntity("Ball.mesh");

	// Create Paddles
	Ogre::Entity *LeftPaddle = mSceneManager->createEntity("Paddle.mesh");
	Ogre::Entity *RightPaddle = mSceneManager->createEntity("Paddle.mesh");

	// Create Walls
	Ogre::Entity *TopWall = mSceneManager->createEntity("Wall.mesh");
	Ogre::Entity *BottomWall = mSceneManager->createEntity("Wall.mesh");
	Ogre::Entity *MiddleWall = mSceneManager->createEntity("MiddleWall.mesh");

	// Set Walls
	mTopWallNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-41, 41, 0));
	mTopWallNode->attachObject(TopWall);
	mBottomWallNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-40.5, -40, 0));
	mBottomWallNode->attachObject(BottomWall);
	mMiddleWallNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0.5, -3, 0));
	mMiddleWallNode->attachObject(MiddleWall);

	// Paddles
	mLeftPaddleNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(55, 0, 0));
	mLeftPaddleNode->attachObject(LeftPaddle);
	mRightPaddleNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-55, 0, 0));
	mRightPaddleNode->attachObject(RightPaddle);

	// Ball
	mBallNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, 0));
	mBallNode->attachObject(Ball);

	mBallNode->scale(1.65, 1.65, 0);		// Scales the Ball
	mTopWallNode->scale(14, 1, 0);			// Scales Top Wall
	mBottomWallNode->scale(14, 1, 0);		// Scales Bottom Wall
	mMiddleWallNode->scale(0.75, 12, 0);	// Scales Middle Wall

	mLeftPaddleNode->scale(1, 1.65, 0);		// Scales Left Paddle
	mRightPaddleNode->scale(1, 1.65, 0);	// Scales Right Paddle
}

void
World::Move(float time) {

	// Change difficulty when number of hits > 10
	if (numHits > 10) {
		BallSpeed = BallSpeed * 1.2;
		numHits = 0;
	}

	// Game Start! Serve the ball
	serveBall(time);

	// Ball hits top wall 
	if (mBallNode->getPosition().y >= 38) {
		// Changes ball status
		HitTopWall = true;
		HitBottomWall = false;
	}

	// Ball hits bottom wall 
	else if (mBallNode->getPosition().y <= -38) {
		// Changes ball status
		HitBottomWall = true;
		HitTopWall = false;
	}

	/* Translate ball to 4 kinds of directions */

	// Translate ball to SouthEast direction 
	if (HitTopWall && HitByAI) {
		mBallNode->translate(-time * BallSpeed, 0, 0);
		mBallNode->translate(0, -time * BallSpeed, 0);
	}
	// Translate ball to NorthEast direction 
	if (HitBottomWall && HitByAI) {
		mBallNode->translate(-time * BallSpeed, 0, 0);
		mBallNode->translate(0, time * BallSpeed, 0);
	}
	// Translate ball to SouthWest direction 
	if (HitTopWall && HitByPlayer) {
		mBallNode->translate(time * BallSpeed, 0, 0);
		mBallNode->translate(0, -time * BallSpeed, 0);
	}
	// Translate ball to NorthWest direction */
	if (HitBottomWall && HitByPlayer) {
		mBallNode->translate(time * BallSpeed, 0, 0);
		mBallNode->translate(0, time * BallSpeed, 0);
	}

	// Ball touches left paddle 
	if (mBallNode->getPosition().x >= 55 && mBallNode->getPosition().x <= 56) {
		if (mBallNode->getPosition().y >= mLeftPaddleNode->getPosition().y - 6 && mBallNode->getPosition().y <= mLeftPaddleNode->getPosition().y + 6) {
			HitByAI = true;
			HitByPlayer = false;
			numHits++;
		}
	}

	// Ball touches right paddle 
	if (mBallNode->getPosition().x <= -55 && mBallNode->getPosition().x >= -56) {
		if (mBallNode->getPosition().y >= mRightPaddleNode->getPosition().y - 6 && mBallNode->getPosition().y <= mRightPaddleNode->getPosition().y + 6) {
			HitByPlayer = true;
			HitByAI = false;
			numHits++;
		}
	}
}

void
World::Scoreboard(void) {

	// Scoreboard's Overlay
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	overly1 = om.getByName("Player1");
	overly1->show();

	overly2 = om.getByName("Player2");
	overly2->show();
}

void
World::updateScoreboard(void) {
	
	// User scored
	if (mBallNode->getPosition().x >= 60) {

		Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
		Ogre::TextAreaOverlayElement *te = (Ogre::TextAreaOverlayElement *) om.getOverlayElement("Scoreboard/Panel2/Score2");
		
		// Update score for user player
		int score = Ogre::StringConverter::parseInt(te->getCaption());
		score++;
		te->setCaption(Ogre::StringConverter::toString(score));

		// Reset status
		HitTopWall = false;
		HitBottomWall = false;
		HitByPlayer = false;
		HitByAI = false;
		Start = true;
		PlayerScored = true;
		AIScored = false;
		BallSpeed = 27;
		PlayerScore++;

		if (PlayerScore == 7) {
			GameOver = true;
		}

		// Reset ball's position to center
		mBallNode->setPosition(Ogre::Vector3(0, 0, 0));
	}

	// AI scored
	if (mBallNode->getPosition().x <= -60) {

		Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
		Ogre::TextAreaOverlayElement *te = (Ogre::TextAreaOverlayElement *) om.getOverlayElement("Scoreboard/Panel1/Score1");
		
		// Update score for AI player
		int score = Ogre::StringConverter::parseInt(te->getCaption());
		score++;
		te->setCaption(Ogre::StringConverter::toString(score));

		// Reset status
		HitTopWall = false;
		HitBottomWall = false;
		HitByPlayer = false;
		HitByAI = false;
		Start = true;
		AIScored = true;
		PlayerScored = false;
		BallSpeed = 27;
		AIScore++;

		if (AIScore == 7) {
			GameOver = true;
		}

		// Reset ball's position to center
		mBallNode->setPosition(Ogre::Vector3(0, 0, 0));
	}
}

void
World::startMenu(void) {
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	menuDifficultyOverly = om.getByName("StartMenu");
	menuDifficultyOverly->show();

	menuBeginnerOverly = om.getByName("Beginner");
	menuBeginnerOverly->show();

	menuIntermediateOverly = om.getByName("Intermediate");
	menuIntermediateOverly->show();

	menuExpertOverly = om.getByName("Expert");
	menuExpertOverly->show();

	if (mInputHandler->IsKeyDown(OIS::KC_B)) {
		beginnerDifficulty = true;
		choseDifficulty = true;
	}

	if (mInputHandler->IsKeyDown(OIS::KC_I)) {
		intermediateDifficulty = true;
		choseDifficulty = true;
	}

	if (mInputHandler->IsKeyDown(OIS::KC_E)) {
		expertDifficulty = true;
		choseDifficulty = true;
	}
}

bool
World::getPaused() {
	return Paused;
}

bool
World::getSetGraphics() {
	return setGraphics;
}

bool
World::getBeginnerDifficulty() {
	return beginnerDifficulty;
}

bool
World::getIntermediateDifficulty() {
	return intermediateDifficulty;
}

bool
World::getExpertDifficulty() {
	return expertDifficulty;
}

void
World::endGame() {

	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

	if (PlayerScore == 7) {
		PlayerWonOverly = om.getByName("Player");
		PlayerWonOverly->show();
	}

	if (AIScore == 7) {
		AIWonOverly = om.getByName("AI");
		AIWonOverly->show();
	}
}

void
World::DetachAllObjects() {
	mMiddleWallNode->detachAllObjects();
	mTopWallNode->detachAllObjects();
	mBottomWallNode->detachAllObjects();
	mLeftPaddleNode->detachAllObjects();
	mRightPaddleNode->detachAllObjects();
	mBallNode->detachAllObjects();
}

void
World::serveBall(float time) {

	// Get random number in order to serve ball 
	if (Start) {
		if (AIScored || PlayerScored) {
			random = rand() % 2 + 1;
		} 
		else {
			random = rand() % 4 + 1;
		}
		Start = false;
	}
	
	/* Serve the ball */
	if (!HitTopWall && !HitBottomWall) {
		if (!AIScored && !PlayerScored) {
			if (random == 1) {					// Serve towards top wall and towards AI
				mBallNode->translate(time * BallSpeed, 0, 0);
				mBallNode->translate(0, time * BallSpeed, 0);
				HitByPlayer = true;
			} 
			else if (random == 2) {				// Serve towards bottom wall and towards AI
				mBallNode->translate(time * BallSpeed, 0, 0);
				mBallNode->translate(0, -time * BallSpeed, 0);
				HitByPlayer = true;
			} 
			else if (random == 3) {				// Serve towards top wall and towards Player
				mBallNode->translate(-time * BallSpeed, 0, 0);
				mBallNode->translate(0, time * BallSpeed, 0);
				HitByAI = true;
			} 
			else if (random == 4) {				// Serve towards bottom wall and towards Player
				mBallNode->translate(-time * BallSpeed, 0, 0);
				mBallNode->translate(0, -time * BallSpeed, 0);
				HitByAI = true;
			}
		} 
		else { 
			if (AIScored) {			
				/* Serve ball towards AI */
				if (random == 1) {				
					mBallNode->translate(time * BallSpeed, 0, 0);
					mBallNode->translate(0, time * BallSpeed, 0);
				} else {						
					mBallNode->translate(time * BallSpeed, 0, 0);
					mBallNode->translate(0, -time * BallSpeed, 0);
				}
				HitByPlayer = true;
			} 
			if (PlayerScored) {		
				/* Serve ball towards Player */
				if (random == 1) {				
					mBallNode->translate(-time * BallSpeed, 0, 0);
					mBallNode->translate(0, time * BallSpeed, 0);
				} else {						
					mBallNode->translate(-time *BallSpeed, 0, 0);
					mBallNode->translate(0, -time * BallSpeed, 0);
				}
				HitByAI = true;
			}
		}
	}
}