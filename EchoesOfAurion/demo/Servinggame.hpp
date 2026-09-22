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
	CUST_EATING,
	CUST_WALK_OUT
};

enum McState
{
	MC_IDLE,
	MC_TO_SEAT,
	MC_SERVING,
	MC_RETURNING
};

struct Customer
{
	CustomerState state;
	int type;
	double x;
	int orderDish;
	int servedDish;
	int patienceTicks;
	int patienceMax;
	int spawnCooldown;
	int walkFrame;
	int walkFrameTimer;
	int eatTicks;
	bool angry;
};

const int SERVING_SEATS = 4;
const int SERVING_NPC_TYPES = 3;

const int SERVING_TIME_LIMIT_TICKS = 12000; // 2 minutes at a 10ms tick
const int SERVING_MAX_MISTAKES = 3;

const int SERVING_TARGET_CUSTOMERS = 15;
const int SERVING_TARGET_CUSTOMERS_WITH_PERK = 10;

class ServingGame
{

private:

	int bg;
	int dishListImg;
	int timeAndScoreImg;
	int orderBubble;
	int angryIcon;

	int mcIdle;
	int mcWalkLeft[3];
	int mcWalkRight[3];

	int rnpcSitLeft[SERVING_NPC_TYPES];
	int rnpcSitRight[SERVING_NPC_TYPES];
	int rnpcWalkLeft[SERVING_NPC_TYPES][3];
	int rnpcWalkRight[SERVING_NPC_TYPES][3];

	int dishImages[5]; // soup, grilled_meat, bread, pie, cake

	int successImg;
	int failedImg;
	int hoverSuccessImg;
	int hoverFailedImg;

	ServingPhase phase;

	int timeLeftTicks;
	int score;
	int mistakes;
	int targetCustomers;
	int resultTicks;

	Customer customers[SERVING_SEATS];

	int trayDish;
	double mcX;
	McState mcState;
	int mcTargetSeat;
	int mcWalkFrame;
	int mcWalkFrameTimer;
	int mcServeTicks;

	bool finished;
	bool retryRequested;

	void spawnCustomer(int i);
	void updateCustomer(int i);
	void updateMc();
	void registerMistake();
	void leaveSeat(int i, bool unhappy);

	bool isInsideBox(int mx, int my, int bx, int by, int bw, int bh);
	bool mcFacingRight();

	int customerImage(int i);
	void drawCustomers();
	void drawTableDishes();
	void drawCustomerUi();
	void drawMc();

public:

	void loadImages();
	void start(bool hasPerk = false);

	void update();
	void draw(int mouseX, int mouseY);

	void handleClick(int mx, int my);

	bool isFinished();
	bool isRetryRequested();

};

#endif