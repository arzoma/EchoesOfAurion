#ifndef FORESTTRAIL_HPP
#define FORESTTRAIL_HPP

#include "Map.hpp"
#include "Player.hpp"

enum PathDir { PATH_WEST = 0, PATH_NORTH = 1, PATH_EAST = 2 };

enum PathKind
{
	PATH_OPEN_TRAP,      // no branches, walk straight in, dead end
	PATH_BLOCKED_DECOY,  // branches + solvable puzzle, still a dead end
	PATH_BLOCKED_TRUE    // branches + solvable puzzle, continues the trail
};

enum PuzzleType { PZ_SELECT, PZ_SEQUENCE, PZ_ODDONE };

enum ForestOverlay
{
	OV_NONE,
	OV_CLUE,
	OV_PUZZLE,
	OV_DEADEND,
	OV_REVEAL,
	OV_RESULT
};

struct PathDef
{
	PathKind kind;
	int mouthX, mouthY, mouthW, mouthH;
	char* riddle;
	int answer[3];
	int answerLen;
	char* deadEndLine;
};

struct PointDef
{
	char* proximityLine;
	char* clueRiddle;
	PuzzleType puzzle;
	PathDir trueDir;
	PathDef path[3];
};

const int FOREST_ORB_COUNT = 6;

class ForestTrail
{

private:

	Map trailMap;

	int segmentImg;
	int topClearingImg;
	int vignetteImg;

	int clueStoneImg;
	int clueStoneLitImg;
	int glowSoftImg;
	int orbImg;

	int branchImg[3]; // west, north, east
	int searchIcon, searchIconHover;
	int clearIcon, clearIconHover;
	int compassImg[3]; // west, north, east

	int riddleWindowImg;
	int puzzleWindowImg;
	int symbolFrameImg;
	int slotEmptyImg;
	int symbolImg[4]; // sun, moon, star, leaf
	int runeImg[8]; // a -> h

	int revealImg;
	int successImg;
	int failedImg;

	int currentPoint;
	bool cleared[3][3];
	int mistakes;
	bool hasCompass;
	bool compassUsed;
	bool clueRead;

	char cardText[40];
	int cardTick;
	bool cardActive;
	bool clearingArrived;

	ForestOverlay overlay;
	int activeDir; // which path's puzzle is open
	int wrongLineIndex;

	// sequence puzzle
	int seqPicks[3];
	int seqCount;

	// branch obstacle
	int branchObstacleIdx[3];
	double branchFade[3];

	// bottom-of-screen text
	char bottomText[160];
	int bottomTextTicks;

	// ending
	bool atEnding;
	int endStep;

	int fadePhase;
	int fadeTick;
	int fadePendingPoint;
	bool fadeIsDeadEnd;

	// glow orbs
	double glowTimer;
	double glowAlpha;
	double orbY[FOREST_ORB_COUNT];
	double orbPhase[FOREST_ORB_COUNT];
	double orbSpeed[FOREST_ORB_COUNT];

	bool finished;
	bool retryRequested;

	void enterPoint(int p);
	void enterClearing();
	void buildSegmentObstacles(int p);

	bool isInsideBox(int mx, int my, int bx, int by, int bw, int bh);
	bool playerNearStone(Player &player);
	bool iconVisible(Player &player, int dir);
	int  mouthUnderPlayer(Player &player);

	int  searchIconX();
	int  searchIconY(int camY);
	int  clearIconX(int dir);
	int  clearIconY(int dir, int camY);

	int  deadEndRespawnY();

	void showBottomText(char* text, int ticks);
	void showTitleCard(char* text);
	void drawTitleCard();
	void registerMistake();
	void onPuzzleSolved(int dir);
	void onWrongAnswer();

	void startFade(int pendingPoint, bool deadEnd);
	void updateFade();
	void drawFadeOverlay();

	void updateGlow(bool near);
	void initOrbs();
	void updateOrbs();
	void drawClueStone(int sx, int sy);
	void drawOrbs(int cx, int cy);

	void drawBranches();
	void drawPuzzle(int mouseX, int mouseY);
	void handlePuzzleClick(int mx, int my);

public:

	void loadImages();
	void start(bool playerHasCompass);

	void update(Player &player);
	void draw(Player &player, int mouseX, int mouseY);

	void updateCamera(Player &player);

	void handleClick(Player &player, int mx, int my);
	void handleKey(unsigned char key);

	Map& getMap();

	bool isFinished();
	bool isRetryRequested();
	bool isTitleCardShowing();
	bool takeClearingArrived();
	bool isAtClearing();

};

#endif