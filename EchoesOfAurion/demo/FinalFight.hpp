#ifndef FINALFIGHT_HPP
#define FINALFIGHT_HPP

#include "Map.hpp"
#include "Player.hpp"

enum FightPhase { FIGHT_PLAYING, FIGHT_WIN, FIGHT_LOST };
enum AtkType    { ATK_NONE, ATK_SWEEP, ATK_RAIN, ATK_LANES };

const int FF_SEALS = 5;
const int FF_MONSTERS = 3;
const int FF_RAIN = 8;
const int FF_LANES = 6;
const int FF_RESOLVE = 5;

struct FightSeal
{
	int x, y;
	bool lit;
	int channelTicks;
};

struct FightMonster
{
	bool alive;
	double x, y;
	int frame, frameTimer;
};

class FinalFight
{

private:

	int bgImg;
	int sealImg, sealLitImg;
	int hollowImg[3];
	int monsterImg[3];
	int guardianImg;
	int resolveImg, resolveSpentImg;
	int charmIconImg;
	int successImg, failedImg, hoverSuccessImg, hoverFailedImg;

	FightPhase phase;
	int resultTicks;

	FightSeal seals[FF_SEALS];
	int litCount;

	int resolve;
	int invulnTicks;
	int flashTicks;

	// the hollow's atk cycle
	AtkType atk;
	int atkStage;
	int atkTicks;
	int atkGap;

	double sweepStart;
	int rainX[FF_RAIN], rainY[FF_RAIN];
	bool laneOn[FF_LANES];

	FightMonster monsters[FF_MONSTERS];
	int monsterTimer;

	// the starwheel gives a perk here
	bool hasWheel;
	int holdTicks;
	int holdCooldown;

	int hollowFrame, hollowTimer;

	bool playerMoved;
	int lastPX, lastPY;

	char bottomText[160];
	int bottomTextTicks;

	bool finished;
	bool retryRequested;

	bool rectsOverlap(Rect a, Rect b);
	int  phaseIndex();
	void startAttack();
	void updateAttack(Player &player);
	void updateMonsters(Player &player);
	void updateChannel(Player &player);
	void hitPlayer(Player &player, double fromX, double fromY);
	void showBottomText(char* text, int ticks);

	bool monHintShown;

	void drawTelegraphs();
	void drawHud(int mouseX, int mouseY);

public:

	void loadImages();
	void start(bool playerHasWheel, Player &player);

	void update(Player &player);
	void draw(Player &player, int mouseX, int mouseY);

	void handleKey(unsigned char key);
	void handleClick(int mx, int my);

	Map& getMap();

	bool isFinished();
	bool isRetryRequested();

};

#endif