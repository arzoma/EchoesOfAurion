#ifndef SERVINGGAME_HPP
#define SERVINGGAME_HPP

enum ServingPhase
{
	SERVING_PLAYING,
	SERVING_SUCCESS,
	SERVING_FAILED
};

enum CustomerState
{
	CUST_EMPTY,
	CUST_WALK_IN,
	CUST_SEATED,
	CUST_WALK_OUT
};

struct Customer
{
	CustomerState state;
	double x;
	int orderDish;
	int patienceTicks;
	int patienceMax;
	int spawnCooldown;
	int walkFrame;
	int walkFrameTimer;
};

const int SERVING_TIME_LIMIT_TICKS = 18000; // 3 minutes at a 10ms tick
const int SERVING_MAX_MISTAKES = 3;

class ServingGame
{

private:

	int bg;
	int dishListImg;
	int timeAndScoreImg;
	int orderBubble;

	int mcIdle;
	int mcWalkLeft1, mcWalkLeft2;
	int mcWalkRight1, mcWalkRight2;

	int rnpcWalkRight;
	int rnpcWalkLeft;
	int rnpcSit;

	int dishImages[5]; // soup, grilled_meat, bread, pie, cake

	int successImg;
	int failedImg;

	ServingPhase phase;

	int timeLeftTicks;
	int score;
	int mistakes;

	Customer customers[3];

	int trayDish;
	double mcX;
	bool mcMoving;
	bool mcMovingRight;
	int mcTargetSeat;
	int mcWalkFrame;
	int mcWalkFrameTimer;

	bool finished;
	bool retryRequested;

	void updateCustomer(int i);
	void updateMcMovement();
	void registerMistake();

	bool isInsideBox(int mx, int my, int bx, int by, int bw, int bh);
	void getDishSlotPos(int i, int &x, int &y);

public:

	void loadImages();
	void start();

	void update();
	void draw();

	void handleClick(int mx, int my);

	bool isFinished();
	bool isRetryRequested();

};

#endif