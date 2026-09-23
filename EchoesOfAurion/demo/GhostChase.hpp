#ifndef GHOSTCHASE_HPP
#define GHOSTCHASE_HPP

#include "Map.hpp"
#include "Player.hpp"

enum ChasePhase { CHASE_PLAYING, CHASE_SUCCESS, CHASE_FAILED };

enum GhostState { G_PATROL, G_ALERT, G_CHASE, G_RETURN };

struct Wardstone
{
	int x, y;
	bool lit;
	int channelTicks;
};

struct Ghost
{
	double x, y;
	GhostState state;
	int wp;
	int alertTicks;
	int lostTicks;
	int frame, frameTimer;
	bool phasing;
	int stuckTicks;
	double lastX, lastY;
};

const int GC_STONES = 5;
const int GC_DRIFTERS = 3;
const int GC_GHOSTS = GC_DRIFTERS + 1;
const int GC_ALCOVES = 6;

const int GC_TIME_TICKS = 24000;
const int GC_MAX_MISTAKES = 5;

class GhostChase
{

private:

	Map chaseMap;

	int bgImg;
	int stoneImg, stoneLitImg;
	int ringImg;
	int wardGlowImg;
	int driftImg[3];
	int wardenImg[3];
	int ghostGlowImg;
	int darknessImg;
	int alcoveImg;
	int doorImg, doorOpenImg;
	int charmIconImg;
	int successImg, failedImg, hoverSuccessImg, hoverFailedImg;

	ChasePhase phase;
	int timeLeftTicks;
	int mistakes;
	int litCount;
	int resultTicks;

	Wardstone stones[GC_STONES];
	Ghost ghosts[GC_GHOSTS];

	bool hasCharm;
	int invisTicks;
	int charmCooldown;

	int graceTicks;
	int flashTicks;
	bool finalRush;
	int doorObstacleIdx;

	double wardenSpeed;

	bool playerMoved;
	int lastPX, lastPY;

	char bottomText[160];
	int bottomTextTicks;

	bool finished;
	bool retryRequested;

	void buildObstacles();
	void resetGhosts();

	bool rectsOverlap(Rect a, Rect b);
	bool lineOfSight(double gx, double gy, double px, double py);
	bool playerHidden(Player &player);

	void updateChannel(Player &player);
	void updateGhosts(Player &player);
	void checkCaught(Player &player);
	void checkExit(Player &player);

	void lightStone(int i);
	void alarmNearestDrifter(int sx, int sy);
	void catchPlayer(Player &player);
	void showBottomText(char* text, int ticks);

	void drawWorld(Player &player);
	void drawHud(int mouseX, int mouseY);

public:

	void loadImages();
	void start(bool playerHasCharm, Player &player);

	void update(Player &player);
	void updateCamera(Player &player);
	void draw(Player &player, int mouseX, int mouseY);

	void handleKey(unsigned char key);
	void handleClick(int mx, int my);

	Map& getMap();

	bool isFinished();
	bool isRetryRequested();

};

#endif