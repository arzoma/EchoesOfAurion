#ifndef MIRRORHALL_HPP
#define MIRRORHALL_HPP

#include "Map.hpp"
#include "Player.hpp"

enum MirrorPhase { MIRROR_PLAYING, MIRROR_SUCCESS, MIRROR_FAILED };

const int MH_ROOMS = 4;
const int MH_MAX_WALLS = 20;
const int MH_MAX_PITS = 8;
const int MH_LAG = 30;

const int MH_TIME_TICKS = 24000;
const int MH_MAX_MISTAKES = 3;

struct MirrorRoom
{
	int mcStartX, mcStartY;
	int refStartX, refStartY;

	int wallCount;
	int walls[MH_MAX_WALLS][4];

	int pitCount;
	int pits[MH_MAX_PITS][4];

	int mcGoalX, mcGoalY;
	int refGoalX, refGoalY;

	int lagTicks;
	char* caption;
};

class MirrorHall
{

private:

	Map roomMap;
	Player reflection;

	int roomBg[MH_ROOMS];
	int seamImg;
	int exitImg, exitLitImg;
	int pitImg;
	int charmIconImg;
	int successImg, failedImg, hoverSuccessImg, hoverFailedImg;

	MirrorPhase phase;
	int room;
	int timeLeftTicks;
	int mistakes;
	int resultTicks;

	bool hasCharm;
	int revealTicks;
	int charmCooldown;

	int roomTicks;
	int clearTicks;
	int flashTicks;

	bool lagUp[MH_LAG], lagDown[MH_LAG], lagLeft[MH_LAG], lagRight[MH_LAG];
	int lagHead;

	bool finished;
	bool retryRequested;

	void buildRoom(int r, Player &player);
	bool rectsOverlap(Rect a, Rect b);
	bool onGoal(Rect f, int gx, int gy);
	void failRoom(Player &player);

public:

	void loadImages();
	void start(bool playerHasCharm, Player &player);

	void handleInput(bool up, bool down, bool left, bool right, Player &player);
	void update(Player &player);
	void draw(Player &player, int mouseX, int mouseY);

	void handleKey(unsigned char key);
	void handleClick(int mx, int my);

	bool isFinished();
	bool isRetryRequested();

};

#endif