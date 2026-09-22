#include <cstdio>
#include <cstdlib>
#include "ServingGame.hpp"
#include "Constants.hpp"

#include "glut.h"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);
void iFilledRectangle(double x, double y, double width, double height);

extern float g_imgAlpha;

// dish order: 0=soup, 1=grilled_meat, 2=bread, 3=pie, 4=cake

static const int SEAT_X[SERVING_SEATS] = { 120, 360, 570, 815 };
static const int SEAT_Y[SERVING_SEATS] = { 130, 132, 130, 130 };

static const bool SEAT_FACES_RIGHT[SERVING_SEATS] = { true, false, true, false };

static const int NPC_W = 150, NPC_H = 310;

static const int SERVE_X[SERVING_SEATS] = { 250, 450, 690, 890 };

static const int TABLE_DISH_X[SERVING_SEATS] = { 244, 320, 688, 760 };
static const int TABLE_DISH_Y[SERVING_SEATS] = { 256, 246, 256, 246 };
static const int TABLE_DISH_SIZE = 56;

static const int MC_HOME_X = 1020;
static const int MC_Y = 96;
static const int MC_W = 140, MC_H = 350;

static const int TRAY_DX_FACING_LEFT = 38;
static const int TRAY_DX_FACING_RIGHT = 52;
static const int TRAY_DY = 92;
static const int TRAY_ICON_SIZE = 50;

static const int BUBBLE_W = 130, BUBBLE_H = 130;
static const int BUBBLE_DY = 300;
static const int BUBBLE_DX_FACING_RIGHT = -20;
static const int BUBBLE_DX_FACING_LEFT = 14;
static const int BUBBLE_ICON_DX = 30, BUBBLE_ICON_DY = 32, BUBBLE_ICON_SIZE = 70;

static const int PATIENCE_DX = 0, PATIENCE_DY = 282;
static const int PATIENCE_W = 124, PATIENCE_H = 10;

static const int ANGRY_DX = 62, ANGRY_DY = 228, ANGRY_SIZE = 38;

static const int DISH_SLOT_X[5] = { 75, 248, 428, 605, 782 };
static const int DISH_SLOT_Y = 38;
static const int DISH_SLOT_SIZE = 100;

static const int TIME_TEXT_X = 187, TIME_TEXT_Y = 663;
static const int SCORE_TEXT_X = 952, SCORE_TEXT_Y = 662;
static const int MISTAKES_TEXT_X = 952, MISTAKES_TEXT_Y = 590;

static const int RESULT_BUTTON_X = 488, RESULT_BUTTON_Y = 238;
static const int RESULT_BUTTON_W = 300, RESULT_BUTTON_H = 60;

static const double CUSTOMER_WALK_SPEED = 3.5;
static const double MC_WALK_SPEED = 7.0;
static const double OFFSCREEN_X = -200.0;

static const int EAT_TICKS = 150;
static const int SERVE_PAUSE_TICKS = 25;
static const int WALK_FRAME_TICKS = 10;

static const int PATIENCE_MIN = 1500;
static const int PATIENCE_RANGE = 1200;

static const int RESPAWN_MIN = 150, RESPAWN_RANGE = 350;

static const int RESULT_FADE_TICKS = 40;

static const int WALK_CYCLE[4] = { 0, 1, 2, 1 };

void ServingGame::loadImages()
{
	bg = iLoadImage("Images//bg_serving.png");
	dishListImg = iLoadImage("Images//serving_dish_list.png");
	timeAndScoreImg = iLoadImage("Images//time_and_score.png");
	orderBubble = iLoadImage("Images//order_bubble.png");
	angryIcon = iLoadImage("Images//icon_angry.png");

	mcIdle = iLoadImage("Images//mc_serving_idle.png");
	mcWalkLeft[0] = iLoadImage("Images//mc_serving_walk_left_1.png");
	mcWalkLeft[1] = iLoadImage("Images//mc_serving_walk_left_2.png");
	mcWalkLeft[2] = iLoadImage("Images//mc_serving_walk_left_3.png");
	mcWalkRight[0] = iLoadImage("Images//mc_serving_walk_right_1.png");
	mcWalkRight[1] = iLoadImage("Images//mc_serving_walk_right_2.png");
	mcWalkRight[2] = iLoadImage("Images//mc_serving_walk_right_3.png");

	rnpcSitLeft[0] = iLoadImage("Images//rnpc1_sit_left.png");
	rnpcSitRight[0] = iLoadImage("Images//rnpc1_sit_right.png");
	rnpcWalkLeft[0][0] = iLoadImage("Images//rnpc1_walk_left_1.png");
	rnpcWalkLeft[0][1] = iLoadImage("Images//rnpc1_walk_left_2.png");
	rnpcWalkLeft[0][2] = iLoadImage("Images//rnpc1_walk_left_3.png");
	rnpcWalkRight[0][0] = iLoadImage("Images//rnpc1_walk_right_1.png");
	rnpcWalkRight[0][1] = iLoadImage("Images//rnpc1_walk_right_2.png");
	rnpcWalkRight[0][2] = iLoadImage("Images//rnpc1_walk_right_3.png");

	rnpcSitLeft[1] = iLoadImage("Images//rnpc2_sit_left.png");
	rnpcSitRight[1] = iLoadImage("Images//rnpc2_sit_right.png");
	rnpcWalkLeft[1][0] = iLoadImage("Images//rnpc2_walk_left_1.png");
	rnpcWalkLeft[1][1] = iLoadImage("Images//rnpc2_walk_left_2.png");
	rnpcWalkLeft[1][2] = iLoadImage("Images//rnpc2_walk_left_3.png");
	rnpcWalkRight[1][0] = iLoadImage("Images//rnpc2_walk_right_1.png");
	rnpcWalkRight[1][1] = iLoadImage("Images//rnpc2_walk_right_2.png");
	rnpcWalkRight[1][2] = iLoadImage("Images//rnpc2_walk_right_3.png");

	rnpcSitLeft[2] = iLoadImage("Images//rnpc3_sit_left.png");
	rnpcSitRight[2] = iLoadImage("Images//rnpc3_sit_right.png");
	rnpcWalkLeft[2][0] = iLoadImage("Images//rnpc3_walk_left_1.png");
	rnpcWalkLeft[2][1] = iLoadImage("Images//rnpc3_walk_left_2.png");
	rnpcWalkLeft[2][2] = iLoadImage("Images//rnpc3_walk_left_3.png");
	rnpcWalkRight[2][0] = iLoadImage("Images//rnpc3_walk_right_1.png");
	rnpcWalkRight[2][1] = iLoadImage("Images//rnpc3_walk_right_2.png");
	rnpcWalkRight[2][2] = iLoadImage("Images//rnpc3_walk_right_3.png");

	dishImages[0] = iLoadImage("Images//soup.png");
	dishImages[1] = iLoadImage("Images//grilled_meat.png");
	dishImages[2] = iLoadImage("Images//bread.png");
	dishImages[3] = iLoadImage("Images//pie.png");
	dishImages[4] = iLoadImage("Images//cake.png");

	successImg = iLoadImage("Images//success.png");
	failedImg = iLoadImage("Images//failed.png");
	hoverSuccessImg = iLoadImage("Images//hover_success.png");
	hoverFailedImg = iLoadImage("Images//hover_failed.png");
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
	mcState = MC_IDLE;
	mcTargetSeat = -1;
	mcWalkFrame = 0;
	mcWalkFrameTimer = 0;
	mcServeTicks = 0;
	resultTicks = 0;

	for (int i = 0; i < SERVING_SEATS; i++)
	{
		customers[i].state = CUST_EMPTY;
		customers[i].servedDish = -1;
		customers[i].angry = false;
		customers[i].spawnCooldown = 60 + i * 110 + rand() % 120;
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

void ServingGame::spawnCustomer(int i)
{
	Customer &c = customers[i];

	c.state = CUST_WALK_IN;
	c.type = rand() % SERVING_NPC_TYPES;
	c.x = OFFSCREEN_X;
	c.orderDish = rand() % 5;
	c.servedDish = -1;
	c.angry = false;
	c.walkFrame = 0;
	c.walkFrameTimer = 0;
	c.eatTicks = 0;

	c.patienceMax = PATIENCE_MIN + rand() % PATIENCE_RANGE;
	c.patienceTicks = c.patienceMax;
}

void ServingGame::leaveSeat(int i, bool unhappy)
{
	Customer &c = customers[i];

	c.state = CUST_WALK_OUT;
	c.angry = unhappy;
	c.servedDish = -1;
	c.walkFrame = 0;
	c.walkFrameTimer = 0;
}

void ServingGame::updateCustomer(int i)
{
	Customer &c = customers[i];

	switch (c.state)
	{
	case CUST_EMPTY:
		c.spawnCooldown--;
		if (c.spawnCooldown <= 0)
		{
			spawnCustomer(i);
		}
		break;

	case CUST_WALK_IN:
		c.x += CUSTOMER_WALK_SPEED;
		c.walkFrameTimer++;
		if (c.walkFrameTimer >= WALK_FRAME_TICKS)
		{
			c.walkFrameTimer = 0;
			c.walkFrame = (c.walkFrame + 1) % 4;
		}

		if (c.x >= SEAT_X[i])
		{
			c.x = SEAT_X[i];
			c.state = CUST_SEATED;
		}
		break;

	case CUST_SEATED:
		c.patienceTicks--;
		if (c.patienceTicks <= 0)
		{
			leaveSeat(i, true);
		}
		break;

	case CUST_EATING:
		c.eatTicks--;
		if (c.eatTicks <= 0) leaveSeat(i, false);
		break;

	case CUST_WALK_OUT:
		c.x -= CUSTOMER_WALK_SPEED;

		c.walkFrameTimer++;
		if (c.walkFrameTimer >= WALK_FRAME_TICKS)
		{
			c.walkFrameTimer = 0;
			c.walkFrame = (c.walkFrame + 1) % 4;
		}

		if (c.x <= OFFSCREEN_X)
		{
			c.state = CUST_EMPTY;
			c.spawnCooldown = RESPAWN_MIN + rand() % RESPAWN_RANGE;
		}
		break;
	}
}

bool ServingGame::mcFacingRight()
{
	return mcState == MC_RETURNING;
}

void ServingGame::updateMc()
{
	if (mcState == MC_IDLE) return;

	if (mcState == MC_SERVING)
	{
		mcServeTicks--;
		if (mcServeTicks <= 0) mcState = MC_RETURNING;
		return;
	}

	double targetX = (mcState == MC_TO_SEAT) ? SERVE_X[mcTargetSeat] : MC_HOME_X;

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
	if (mcWalkFrameTimer >= WALK_FRAME_TICKS)
	{
		mcWalkFrameTimer = 0;
		mcWalkFrame = (mcWalkFrame + 1) % 4;
	}

	if (mcX != targetX) return;

	if (mcState == MC_RETURNING)
	{
		mcState = MC_IDLE;
		mcWalkFrame = 0;
		mcTargetSeat = -1;
		return;
	}

	int seat = mcTargetSeat;
	Customer &c = customers[seat];

	if (c.state != CUST_SEATED)
	{
		mcState = MC_RETURNING;
		return;
	}

	if (trayDish == c.orderDish)
	{
		c.servedDish = trayDish;
		c.state = CUST_EATING;
		c.eatTicks = EAT_TICKS;

		score++;
		if (score >= targetCustomers) phase = SERVING_SUCCESS;
	}
	else
	{
		registerMistake();
		leaveSeat(seat, true);
	}

	trayDish = -1;
	mcState = MC_SERVING;
	mcServeTicks = SERVE_PAUSE_TICKS;
}

void ServingGame::update()
{
	if (phase != SERVING_PLAYING)
	{
		if (resultTicks < RESULT_FADE_TICKS) resultTicks++;
		return;
	}

	timeLeftTicks--;
	if (timeLeftTicks <= 0)
	{
		timeLeftTicks = 0;
		phase = SERVING_FAILED;
		return;
	}

	for (int i = 0; i < SERVING_SEATS; i++) updateCustomer(i);
	updateMc();
}

bool ServingGame::isInsideBox(int mx, int my, int bx, int by, int bw, int bh)
{
	return mx >= bx && mx <= bx + bw && my >= by && my <= by + bh;
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

	if (mcState != MC_IDLE) return;

	for (int i = 0; i < 5; i++)
	{
		if (isInsideBox(mx, my, DISH_SLOT_X[i], DISH_SLOT_Y, DISH_SLOT_SIZE, DISH_SLOT_SIZE))
		{
			trayDish = i;
			return;
		}
	}

	if (trayDish == -1) return;

	for (int i = 0; i < SERVING_SEATS; i++)
	{
		if (customers[i].state != CUST_SEATED) continue;

		int boxY = SEAT_Y[i] + 130;

		if (isInsideBox(mx, my, (int)customers[i].x, boxY, NPC_W, NPC_H - 130))
		{
			mcState = MC_TO_SEAT;
			mcTargetSeat = i;
			mcWalkFrame = 0;
			mcWalkFrameTimer = 0;
			return;
		}
	}
}

int ServingGame::customerImage(int i)
{
	Customer &c = customers[i];
	int f = WALK_CYCLE[c.walkFrame];

	if (c.state == CUST_WALK_IN)  return rnpcWalkRight[c.type][f];
	if (c.state == CUST_WALK_OUT) return rnpcWalkLeft[c.type][f];

	return SEAT_FACES_RIGHT[i] ? rnpcSitRight[c.type] : rnpcSitLeft[c.type];
}

void ServingGame::drawTableDishes()
{
	for (int i = 0; i < SERVING_SEATS; i++)
	{
		if (customers[i].servedDish == -1) continue;

		iShowImage(TABLE_DISH_X[i], TABLE_DISH_Y[i],
		TABLE_DISH_SIZE, TABLE_DISH_SIZE, dishImages[customers[i].servedDish]);
	}
}

void ServingGame::drawCustomers()
{
	for (int i = 0; i < SERVING_SEATS; i++)
	{
		if (customers[i].state == CUST_EMPTY) continue;
		iShowImage((int)customers[i].x, SEAT_Y[i], NPC_W, NPC_H, customerImage(i));
	}
}

void ServingGame::drawCustomerUi()
{
	for (int i = 0; i < SERVING_SEATS; i++)
	{
		Customer &c = customers[i];
		if (c.state == CUST_EMPTY) continue;

		int sx = (int)c.x;
		int sy = SEAT_Y[i];

		if (c.state == CUST_WALK_OUT && c.angry)
		{
			iShowImage(sx + ANGRY_DX, sy + ANGRY_DY, ANGRY_SIZE, ANGRY_SIZE, angryIcon);
		}

		if (c.state != CUST_SEATED) continue;

		int bx = sx + (SEAT_FACES_RIGHT[i] ? BUBBLE_DX_FACING_RIGHT : BUBBLE_DX_FACING_LEFT);
		int by = sy + BUBBLE_DY;

		iShowImage(bx, by, BUBBLE_W, BUBBLE_H, orderBubble);
		iShowImage(bx + BUBBLE_ICON_DX, by + BUBBLE_ICON_DY,
		BUBBLE_ICON_SIZE, BUBBLE_ICON_SIZE, dishImages[c.orderDish]);

		double pct = (double)c.patienceTicks / (double)c.patienceMax;
		if (pct < 0.0) pct = 0.0;

		int barX = sx + PATIENCE_DX;
		int barY = sy + PATIENCE_DY;

		iSetColor(45, 45, 55);
		iFilledRectangle(barX, barY, PATIENCE_W, PATIENCE_H);

		if (pct > 0.5) iSetColor(120, 220, 130);
		else if (pct > 0.25) iSetColor(235, 205, 90);
		else iSetColor(225, 80, 70);

		iFilledRectangle(barX, barY, (int)(PATIENCE_W * pct), PATIENCE_H);
	}
}

void ServingGame::drawMc()
{
	int img = mcIdle;

	if (mcState == MC_TO_SEAT || mcState == MC_RETURNING)
	{
		int f = WALK_CYCLE[mcWalkFrame];
		img = mcFacingRight() ? mcWalkRight[f] : mcWalkLeft[f];
	}

	iShowImage((int)mcX, MC_Y, MC_W, MC_H, img);

	if (trayDish != -1)
	{
		int dx = mcFacingRight() ? TRAY_DX_FACING_RIGHT : TRAY_DX_FACING_LEFT;
		iShowImage((int)mcX + dx, MC_Y + TRAY_DY,
		TRAY_ICON_SIZE, TRAY_ICON_SIZE, dishImages[trayDish]);
	}
}

void ServingGame::draw(int mouseX, int mouseY)
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg);

	drawTableDishes();
	drawCustomers();
	drawMc();
	drawCustomerUi();

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, dishListImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, timeAndScoreImg);

	for (int i = 0; i < 5; i++)
	{
		iShowImage(DISH_SLOT_X[i], DISH_SLOT_Y, DISH_SLOT_SIZE, DISH_SLOT_SIZE, dishImages[i]);

		if (trayDish == i)
		{
			iSetColor(255, 255, 255);
			iFilledRectangle(DISH_SLOT_X[i], DISH_SLOT_Y - 6, DISH_SLOT_SIZE, 4);
		}
	}

	char timeText[20], scoreText[50], mistakeText[50];
	int secondsLeft = timeLeftTicks / 100;

	sprintf_s(timeText, "%02d:%02d", secondsLeft / 60, secondsLeft % 60);
	sprintf_s(scoreText, "Served: %d / %d", score, targetCustomers);
	sprintf_s(mistakeText, "Mistakes: %d/%d", mistakes, SERVING_MAX_MISTAKES);

	iSetColor(255, 255, 255);
	iText(TIME_TEXT_X, TIME_TEXT_Y, timeText, GLUT_BITMAP_HELVETICA_18);
	iText(SCORE_TEXT_X, SCORE_TEXT_Y, scoreText, GLUT_BITMAP_HELVETICA_18);
	iText(MISTAKES_TEXT_X, MISTAKES_TEXT_Y, mistakeText, GLUT_BITMAP_HELVETICA_18);

	bool hovering = isInsideBox(mouseX, mouseY,
		RESULT_BUTTON_X, RESULT_BUTTON_Y, RESULT_BUTTON_W, RESULT_BUTTON_H);

	if (phase != SERVING_PLAYING)
	{
		float a = (float)resultTicks / (float)RESULT_FADE_TICKS;
		if (a > 1.0f) a = 1.0f;

		int img = (phase == SERVING_SUCCESS)
			? (hovering ? hoverSuccessImg : successImg)
			: (hovering ? hoverFailedImg : failedImg);

		g_imgAlpha = a;
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, img);
	}
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