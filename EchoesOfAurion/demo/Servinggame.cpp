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

static const int SEAT_X[3] = { 179, 461, 742 };
static const int SEAT_Y = 36;
static const int NPC_W = 128, NPC_H = 324;

static const int BUBBLE_OFFSET_X = 26, BUBBLE_OFFSET_Y = 302, BUBBLE_SIZE = 115;
static const int BUBBLE_ICON_INSET = 26, BUBBLE_ICON_SIZE = 64;

static const int PATIENCE_OFFSET_Y = 259;
static const int PATIENCE_W = 115, PATIENCE_H = 11;

static const int MC_HOME_X = 1024, MC_Y = 36, MC_W = 154, MC_H = 360;
static const int TRAY_OFFSET_X = 26, TRAY_OFFSET_Y = 216, TRAY_ICON_SIZE = 51;

static const int DISH_SLOT_X[5] = { 128, 282, 435, 589, 742 };
static const int DISH_SLOT_Y = 72;
static const int DISH_SLOT_SIZE = 90;

static const int TIME_TEXT_X = 128, TIME_TEXT_Y = 648;
static const int SCORE_TEXT_X = 960, SCORE_TEXT_Y = 648;
static const int MISTAKES_TEXT_X = 960, MISTAKES_TEXT_Y = 578;

static const int RESULT_BUTTON_X = 512, RESULT_BUTTON_Y = 216;
static const int RESULT_BUTTON_W = 256, RESULT_BUTTON_H = 58;

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
	mcX = MC_HOME_X;
	mcMoving = false;
	mcMovingRight = false;
	mcTargetSeat = -1;
	mcWalkFrame = 0;
	mcWalkFrameTimer = 0;

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
	double seatX = SEAT_X[i];

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

	double targetX = (mcTargetSeat == -1) ? MC_HOME_X : SEAT_X[mcTargetSeat];

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
			mcMovingRight = (MC_HOME_X > mcX);
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
	x = DISH_SLOT_X[i];
	y = DISH_SLOT_Y;
}

void ServingGame::handleClick(int mx, int my)
{

	if (phase == SERVING_SUCCESS)
	{
		if (isInsideBox(mx, my, RESULT_BUTTON_X, RESULT_BUTTON_Y, RESULT_BUTTON_W, RESULT_BUTTON_H))
		{
			finished = true;
		}
		return;
	}

	if (phase == SERVING_FAILED)
	{
		if (isInsideBox(mx, my, RESULT_BUTTON_X, RESULT_BUTTON_Y, RESULT_BUTTON_W, RESULT_BUTTON_H))
		{
			retryRequested = true;
		}
		return;
	}

	for (int i = 0; i < 5; i++)
	{
		int sx, sy;
		getDishSlotPos(i, sx, sy);
		if (isInsideBox(mx, my, sx, sy, DISH_SLOT_SIZE, DISH_SLOT_SIZE))
		{
			trayDish = i;
			return;
		}
	}

	if (trayDish != -1 && !mcMoving)
	{
		for (int i = 0; i < 3; i++)
		{
			if (customers[i].state != CUST_SEATED) continue;

			if (isInsideBox(mx, my, (int)customers[i].x, SEAT_Y, NPC_W, NPC_H))
			{
				mcMoving = true;
				mcTargetSeat = i;
				mcMovingRight = (SEAT_X[i] > mcX);
				return;
			}
		}
	}
}

void ServingGame::draw()
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg);

	for (int i = 0; i < 3; i++)
	{
		Customer &c = customers[i];
		if (c.state == CUST_EMPTY) continue;

		int img;
		if (c.state == CUST_WALK_IN) img = rnpcWalkRight;
		else if (c.state == CUST_WALK_OUT) img = rnpcWalkLeft;
		else img = rnpcSit;

		iShowImage((int)c.x, SEAT_Y, NPC_W, NPC_H, img);

		if (c.state == CUST_SEATED)
		{
			int bx = (int)c.x + BUBBLE_OFFSET_X;
			int by = SEAT_Y + BUBBLE_OFFSET_Y;
			iShowImage(bx, by, BUBBLE_SIZE, BUBBLE_SIZE, orderBubble);

			iShowImage(bx + BUBBLE_ICON_INSET, by + BUBBLE_ICON_INSET, BUBBLE_ICON_SIZE, BUBBLE_ICON_SIZE, dishImages[c.orderDish]);

			double pct = (double)c.patienceTicks / c.patienceMax;
			int barX = (int)c.x;
			int barY = SEAT_Y + (int)(SCREEN_HEIGHT * PATIENCE_OFFSET_Y);

			iSetColor(80, 80, 80);
			iFilledRectangle(barX, barY, PATIENCE_W, PATIENCE_H);

			iSetColor(255, 255, 255);
			iFilledRectangle(barX, barY, (int)(PATIENCE_W * pct), PATIENCE_H);
		}
	}

	int mcImg = mcIdle;
	if (mcMoving)
	{
		if (mcMovingRight) 
		{
			if (mcWalkFrame == 0) mcImg = mcWalkRight1;
			else if (mcWalkFrame == 2) mcImg = mcWalkRight3;
			else mcImg = mcWalkRight2;
		}
		else
		{
			if (mcWalkFrame == 0) mcImg = mcWalkLeft1;
			else if (mcWalkFrame == 2) mcImg = mcWalkLeft3;
			else mcImg = mcWalkLeft2;
		}
	}
	iShowImage((int)mcX, MC_Y, MC_W, MC_H, mcImg);

	if (trayDish != -1)
	{
		int trayX = (int)mcX + TRAY_OFFSET_X;
		int trayY = MC_Y + (int)(SCREEN_HEIGHT * TRAY_OFFSET_Y);
		iShowImage(trayX, trayY, TRAY_ICON_SIZE, TRAY_ICON_SIZE, dishImages[trayDish]);
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, dishListImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, timeAndScoreImg);

	for (int i = 0; i < 5; i++)
	{
		int sx, sy;
		getDishSlotPos(i, sx, sy);
		iShowImage(sx, sy, DISH_SLOT_SIZE, DISH_SLOT_SIZE, dishImages[i]);
	}

	char timeText[20], scoreText[50], mistakeText[50];
	int secondsLeft = timeLeftTicks / 100;
	sprintf_s(timeText, "%02d:%02d", secondsLeft / 60, secondsLeft % 60);
	sprintf_s(scoreText, "Score: %d Target: %d", score, targetCustomers);
	sprintf_s(mistakeText, "Mistakes: %d/%d", mistakes, SERVING_MAX_MISTAKES);

	iSetColor(255, 255, 255);
	iText(TIME_TEXT_X, TIME_TEXT_Y, timeText, GLUT_BITMAP_HELVETICA_18);
	iText(SCORE_TEXT_X, SCORE_TEXT_Y, scoreText, GLUT_BITMAP_HELVETICA_18);

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