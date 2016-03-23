#ifndef __World_h_
#define __World_h_

// Forward delcarations of Ogre classes.  Note the Ogre namespace!
namespace Ogre {
    class SceneNode;
    class SceneManager;
	class Overlay;
}

// Forward delcarations of our own classes
class PongCamera;
class InputHandler;


class World
{
public:
	
    World(Ogre::SceneManager *sceneManager, InputHandler *input);
    
    void Think(float time);
	void Models(void);
	void Move(float time);
	void Scoreboard(void);
	void updateScoreboard(void);
	void startMenu(void);
	void getStartMenuInput(void);
	void serveBall(float time);
	void endGame(void);
	void DetachAllObjects(void);

	// Methods for Playing
	Ogre::SceneNode * getBallNode();
	Ogre::SceneNode * getLeftPaddleNode();
	float getBallSpeed();
	float getPaddleSpeed();
	bool getSetGraphics();
	bool getBeginnerDifficulty();
	bool getIntermediateDifficulty();
	bool getExpertDifficulty();
	bool getPaused();

	void addCamera(PongCamera *c) { mCamera = c; }

	Ogre::SceneManager *SceneManager() { return mSceneManager; }

protected:

	Ogre::SceneManager *mSceneManager;

	InputHandler *mInputHandler;
	PongCamera *mCamera;

	Ogre::SceneNode *mCoinNode;
	Ogre::SceneNode *mTopWallNode;
	Ogre::SceneNode *mBottomWallNode;
	Ogre::SceneNode *mMiddleWallNode;
	Ogre::SceneNode *mLeftPaddleNode;
	Ogre::SceneNode *mRightPaddleNode;
	Ogre::SceneNode *mBallNode;
	Ogre::Overlay *overly1;
	Ogre::Overlay *overly2;
	Ogre::Overlay *menuDifficultyOverly;
	Ogre::Overlay *menuBeginnerOverly;
	Ogre::Overlay *menuIntermediateOverly;
	Ogre::Overlay *menuExpertOverly;
	Ogre::Overlay *PlayerWonOverly;
	Ogre::Overlay *AIWonOverly;

	bool Start;
	bool HitTopWall;
	bool HitBottomWall;
	bool HitByAI;
	bool HitByPlayer;
	bool AIScored;
	bool PlayerScored;
	float BallSpeed;
	float PaddleSpeed;
	int numHits;
	int PlayerScore;
	int AIScore;
	int random;

	bool choseDifficulty;
	bool beginnerDifficulty;
	bool intermediateDifficulty;
	bool expertDifficulty;
	bool setGraphics;
	bool AIWon;
	bool PlayerWon;
	bool Paused;
	bool GameOver;

};

#endif