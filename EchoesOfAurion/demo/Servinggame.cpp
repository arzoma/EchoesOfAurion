#include <cstdio>
#include <cstdlib>
#include "ServingGame.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);
void iFilledRectangle(double x, double y, double width, double height);

#define GLUT_BITMAP_HELVETICA_18 ((void*)8)

// dish order: 0=soup, 1=grilled_meat, 2=bread, 3=pie, 4=cake

static const double SEAT_X[3] = { 0.14, 0.36, 0.58 };
static const double SEAT_Y = 0.05;
static const double NPC_W = 0.10, NPC_H = 0.45;

static const double BUBBLE_OFFSET_X = 0.02, BUBBLE_OFFSET_Y = 0.42, BUBBLE_SIZE = 0.09;
static const double BUBBLE_ICON_INSET = 0.02, BUBBLE_ICON_SIZE = 0.05;

static const double PATIENCE_OFFSET_Y = 0.36;
static const double PATIENCE_W = 0.09, PATIENCE_H = 0.015;

static const double MC_HOME_X = 0.80, MC_Y = 0.05, MC_W = 0.12, MC_H = 0.50;
static const double TRAY_OFFSET_X = 0.02, TRAY_OFFSET_Y = 0.30, TRAY_ICON_SIZE = 0.04;

static const double DISH_SLOT_X[5] = { 0.10, 0.22, 0.34, 0.46, 0.58 };
static const double DISH_SLOT_Y = 0.10;
static const double DISH_SLOT_SIZE = 0.07;

static const double TIME_TEXT_X = 0.10, TIME_TEXT_Y = 0.90;
static const double SCORE_TEXT_X = 0.75, SCORE_TEXT_Y = 0.90;

static const double RESULT_BUTTON_X = 0.40, RESULT_BUTTON_Y = 0.30, RESULT_BUTTON_W = 0.20, RESULT_BUTTON_H = 0.08;

static const double CUSTOMER_WALK_SPEED = 3.0;
static const double MC_WALK_SPEED = 4.0;
static const double OFFSCREEN_X = -300.0;

void ServingGame::loadImages()
{
	bg = iLoadImage("Images//bg_serving.png");
	dishListImg = iLoadImage("Images//serving_dish_list.png");
	timeAndScoreImg = iLoadImage("Images//time_and_score.png");
	orderBubble = iLoadImage("Images//order_bubble.png");

	mcIdle = iLoadImage("Images//mc_serving_idle.png");
	mcWalkLeft1 = iLoadImage("Images//mc_serving_walk_left_1.png");
	mcWalkLeft2 = iLoadImage("Images//mc_serving_walk_left_2.png");
	mcWalkLeft3 = iLoadImage("Images//mc_serving_walk_left_3.png");
	mcWalkRight1 = iLoadImage("Images//mc_serving_walk_right_1.png");
	mcWalkRight2 = iLoadImage("Images//mc_serving_walk_right_2.png");
	mcWalkRight3 = iLoadImage("Images//mc_serving_walk_right_3.png");

	rnpcWalkRight = iLoadImage("Images//rnpc_walk_right.png");
	rnpcWalkLeft = iLoadImage("Images//rnpc_walk_left.png");
	rnpcSit = iLoadImage("Images//rnpc_sit.png");

	dishImages[0] = iLoadImage("Images//soup.png");
	dishImages[1] = iLoadImage("Images//grilled_meat.png");
	dishImages[2] = iLoadImage("Images//bread.png");
	dishImages[3] = iLoadImage("Images//pie.png");
	dishImages[4] = iLoadImage("Images//cake.png");

	successImg = iLoadImage("Images//success.png");
	failedImg = iLoadImage("Images//failed.png");
}

void ServingGame::start(bool hasPerk)
{
	phase = SERVING_PLAYING;
	timeLeftTicks = SERVING_TIME_LIMIT_TICKS;
	score = 0;
	mistakes = 0;
	finished = false;
	retryRequested = false;
	targetCustomers = hasPerk ? SERVING_TARGET_CUSTOMERS_WITH_PERK : SERVING_TARGET_CUSTOMERS;

	trayDish = -1;
	mcX = SCREEN_WIDTH * MC_HOME_X;
	mcMoving = false;
	mcTargetSeat = -1;

	for (int i = 0; i < 3; i++)
	{
		customers[i].state = CUST_EMPTY;
		customers[i].spawnCooldown = 100 + rand() % 300;
	}
}

void ServingGame::registerMistake()
{
	mistakes++;
	if (mistakes >= SERVING_MAX_MISTAKES)
	{
		phase = SERVING_FAILED;
	}
}

void ServingGame::updateCustomer(int i)
{
	Customer &c = customers[i];
	double seatX = SCREEN_WIDTH * SEAT_X[i];

	switch (c.state)
	{
	case CUST_EMPTY:
		c.spawnCooldown--;
		if (c.spawnCooldown <= 0)
		{
			c.state = CUST_WALK_IN;
			c.x = OFFSCREEN_X;
			c.orderDish = rand() % 5;
			c.walkFrame = 0;
			c.walkFrameTimer = 0;
		}
		break;

	case CUST_WALK_IN:
		c.x += CUSTOMER_WALK_SPEED;
		c.walkFrameTimer++;
		if (c.walkFrameTimer >= 15) { c.walkFrameTimer = 0; c.walkFrame = 1 - c.walkFrame; }

		if (c.x >= seatX)
		{
			c.x = seatX;
			c.state = CUST_SEATED;
			c.patienceMax = 2500;
			c.patienceTicks = c.patienceMax;
		}
		break;

	case CUST_SEATED:
		c.patienceTicks--;
		if (c.patienceTicks <= 0)
		{
			c.state = CUST_WALK_OUT;
			registerMistake(); // left unhappy
		}
		break;

	case CUST_WALK_OUT:
		c.x -= CUSTOMER_WALK_SPEED;
		c.walkFrameTimer++;
		if (c.walkFrameTimer >= 15) { c.walkFrameTimer = 0; c.walkFrame = 1 - c.walkFrame; }

		if (c.x <= OFFSCREEN_X)
		{
			c.state = CUST_EMPTY;
			c.spawnCooldown = 300 + rand() % 700;
		}
		break;
	}
}

void ServingGame::updateMcMovement()
{
	if (!mcMoving) return;

	double targetX = (mcTargetSeat == -1) ? SCREEN_WIDTH * MC_HOME_X : SCREEN_WIDTH * SEAT_X[mcTargetSeat];

	if (mcX < targetX)
	{
		mcX += MC_WALK_SPEED;
		if (mcX > targetX) mcX = targetX;
	}
	else if (mcX > targetX)
	{
		mcX -= MC_WALK_SPEED;
		if (mcX < targetX) mcX = targetX;
	}

	mcWalkFrameTimer++;
	if (mcWalkFrameTimer >= 15) { mcWalkFrameTimer = 0; mcWalkFrame = (mcWalkFrame + 1) % 4; }

	if (mcX == targetX)
	{
		mcMoving = false;

		if (mcTargetSeat != -1)
		{
			int seat = mcTargetSeat;

			if (trayDish == customers[seat].orderDish)
			{
				customers[seat].state = CUST_WALK_OUT;
				score++;
				if (score >= targetCustomers)
				{
					phase = SERVING_SUCCESS;
				}
			}
			else
			{
				registerMistake();
			}

			trayDish = -1;
			mcTargetSeat = -1;

			mcMoving = true;
			mcMovingRight = (SCREEN_WIDTH * MC_HOME_X > mcX);
		}
	}
}

void ServingGame::update()
{
	if (phase != SERVING_PLAYING) return;

	timeLeftTicks--;
	if (timeLeftTicks <= 0)
	{
		timeLeftTicks = 0;
		phase = SERVING_FAILED;
		return;
	}

	for (int i = 0; i < 3; i++) updateCustomer(i);
	updateMcMovement();
}

bool ServingGame::isInsideBox(int mx, int my, int bx, int by, int bw, int bh)
{
	return mx >= bx && mx <= bx + bw && my >= by && my <= by + bh;
}

void ServingGame::getDishSlotPos(int i, int &x, int &y)
{
	x = (int)(SCREEN_WIDTH * DISH_SLOT_X[i]);
	y = (int)(SCREEN_HEIGHT * DISH_SLOT_Y);
}

void ServingGame::handleClick(int mx, int my)
{
	int rx = (int)(SCREEN_WIDTH * RESULT_BUTTON_X);
	int ry = (int)(SCREEN_HEIGHT * RESULT_BUTTON_Y);
	int rw = (int)(SCREEN_WIDTH * RESULT_BUTTON_W);
	int rh = (int)(SCREEN_HEIGHT * RESULT_BUTTON_H);

	if (phase == SERVING_SUCCESS)
	{
		if (isInsideBox(mx, my, rx, ry, rw, rh)) finished = true;
		return;
	}

	if (phase == SERVING_FAILED)
	{
		if (isInsideBox(mx, my, rx, ry, rw, rh)) retryRequested = true;
		return;
	}

	int dishSize = (int)(SCREEN_WIDTH * DISH_SLOT_SIZE);
	for (int i = 0; i < 5; i++)
	{
		int sx, sy;
		getDishSlotPos(i, sx, sy);
		if (isInsideBox(mx, my, sx, sy, dishSize, dishSize))
		{
			trayDish = i;
			return;
		}
	}

	if (trayDish != -1 && !mcMoving)
	{
		int npcW = (int)(SCREEN_WIDTH * NPC_W);
		int npcH = (int)(SCREEN_HEIGHT * NPC_H);
		int npcY = (int)(SCREEN_HEIGHT * SEAT_Y);

		for (int i = 0; i < 3; i++)
		{
			if (customers[i].state != CUST_SEATED) continue;

			if (isInsideBox(mx, my, (int)customers[i].x, npcY, npcW, npcH))
			{
				mcMoving = true;
				mcTargetSeat = i;
				mcMovingRight = (SCREEN_WIDTH * SEAT_X[i] > mcX);
				return;
			}
		}
	}
}

void ServingGame::draw()
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg);

	int npcW = (int)(SCREEN_WIDTH * NPC_W);
	int npcH = (int)(SCREEN_HEIGHT * NPC_H);
	int npcY = (int)(SCREEN_HEIGHT * SEAT_Y);

	for (int i = 0; i < 3; i++)
	{
		Customer &c = customers[i];
		if (c.state == CUST_EMPTY) continue;

		int img;
		if (c.state == CUST_WALK_IN) img = rnpcWalkRight;
		else if (c.state == CUST_WALK_OUT) img = rnpcWalkLeft;
		else img = rnpcSit;

		iShowImage((int)c.x, npcY, npcW, npcH, img);

		if (c.state == CUST_SEATED)
		{
			int bx = (int)c.x + (int)(SCREEN_WIDTH * BUBBLE_OFFSET_X);
			int by = npcY + (int)(SCREEN_HEIGHT * BUBBLE_OFFSET_Y);
			int bSize = (int)(SCREEN_WIDTH * BUBBLE_SIZE);
			iShowImage(bx, by, bSize, bSize, orderBubble);

			int iconInset = (int)(SCREEN_WIDTH * BUBBLE_ICON_INSET);
			int iconSize = (int)(SCREEN_WIDTH * BUBBLE_ICON_SIZE);
			iShowImage(bx + iconInset, by + iconInset, iconSize, iconSize, dishImages[c.orderDish]);

			double pct = (double)c.patienceTicks / c.patienceMax;
			int barX = (int)c.x;
			int barY = npcY + (int)(SCREEN_HEIGHT * PATIENCE_OFFSET_Y);
			int barW = (int)(SCREEN_WIDTH * PATIENCE_W);
			int barH = (int)(SCREEN_HEIGHT * PATIENCE_H);

			iSetColor(80, 80, 80);
			iFilledRectangle(barX, barY, barW, barH);

			iSetColor(255, 255, 255);
			iFilledRectangle(barX, barY, (int)(barW * pct), barH);
		}
	}

	int mcImg = mcIdle;
	if (mcMoving)
	{
		if (mcMovingRight) 
		{
			if (mcWalkFrame == 0) mcImg = mcWalkRight1;
			else if (mcWalkFrame == 2) mcImg = mcWalkRight3;
			else mcImg = mcWalkRight2; // frame 1 or 3 - the middle pose
		}
		else
		{
			if (mcWalkFrame == 0) mcImg = mcWalkLeft1;
			else if (mcWalkFrame == 2) mcImg = mcWalkLeft3;
			else mcImg = mcWalkLeft2;
		}
	}
	int mcW = (int)(SCREEN_WIDTH * MC_W);
	int mcH = (int)(SCREEN_HEIGHT * MC_H);
	int mcYpx = (int)(SCREEN_HEIGHT * MC_Y);
	iShowImage((int)mcX, mcYpx, mcW, mcH, mcImg);

	if (trayDish != -1)
	{
		int trayX = (int)mcX + (int)(SCREEN_WIDTH * TRAY_OFFSET_X);
		int trayY = mcYpx + (int)(SCREEN_HEIGHT * TRAY_OFFSET_Y);
		int traySize = (int)(SCREEN_WIDTH * TRAY_ICON_SIZE);
		iShowImage(trayX, trayY, traySize, traySize, dishImages[trayDish]);
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, dishListImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, timeAndScoreImg);

	int dishSize = (int)(SCREEN_WIDTH * DISH_SLOT_SIZE);
	for (int i = 0; i < 5; i++)
	{
		int sx, sy;
		getDishSlotPos(i, sx, sy);
		iShowImage(sx, sy, dishSize, dishSize, dishImages[i]);
	}

	char timeText[20], scoreText[20];
	int secondsLeft = timeLeftTicks / 100;
	sprintf_s(timeText, "%02d:%02d", secondsLeft / 60, secondsLeft % 60);
	sprintf_s(scoreText, "Score: %d Target: %d", score, targetCustomers);

	iSetColor(255, 255, 255);
	iText(SCREEN_WIDTH * TIME_TEXT_X, SCREEN_HEIGHT * TIME_TEXT_Y, timeText, GLUT_BITMAP_HELVETICA_18);
	iText(SCREEN_WIDTH * SCORE_TEXT_X, SCREEN_HEIGHT * SCORE_TEXT_Y, scoreText, GLUT_BITMAP_HELVETICA_18);

	if (phase == SERVING_SUCCESS) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, successImg);
	else if (phase == SERVING_FAILED) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, failedImg);
}

bool ServingGame::isFinished()
{
	if (!finished) return false;
	finished = false;
	return true;
}

bool ServingGame::isRetryRequested()
{
	if (!retryRequested) return false;
	retryRequested = false;
	return true;
}