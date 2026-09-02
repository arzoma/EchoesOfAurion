#include <cstdio>
#include <cstdlib>
#include "CookingGame.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char string[], void *font);
void iRectangle(double x, double y, double width, double height);

#define GLUT_BITMAP_HELVETICA_18 ((void*)8)

// dish order: 0=soup, 1=grilled_meat, 2=bread, 3=pie, 4=cake
// ingredient order: 0=meat, 1=carrot, 2=mushroom, 3=spice, 4=sugar, 5=flour, 6=egg
static const int DISH_INGREDIENTS[5][3] =
{
	{ 1, 2, 3 }, // soup: carrot, mushroom, spice
	{ 0, 3, 1 }, // grilled meat: meat, spice, carrot
	{ 5, 6, 4 }, // bread: flour, egg, sugar
	{ 5, 6, 2 }, // pie: flour, egg, mushroom
	{ 5, 4, 1 }  // cake: flour, sugar, carrot
};

static const double TIME_TEXT_X = 0.10, TIME_TEXT_Y = 0.90;
static const double SCORE_TEXT_X = 0.75, SCORE_TEXT_Y = 0.90;

static const double DISH_ICON_X = 0.29, DISH_ICON_Y = 0.62, DISH_ICON_SIZE = 0.11;

static const double NEEDED_SLOT_X[3] = { 0.215, 0.275, 0.335 };
static const double NEEDED_SLOT_Y = 0.48;
static const double NEEDED_SLOT_SIZE = 0.045;

static const double INGREDIENT_SLOT_X[7] = { 0.078, 0.175, 0.272, 0.369, 0.466, 0.563, 0.660 };
static const double INGREDIENT_SLOT_Y = 0.26;
static const double INGREDIENT_SLOT_SIZE = 0.065;

static const double COOK_BUTTON_X = 0.585, COOK_BUTTON_Y = 0.076, COOK_BUTTON_W = 0.12, COOK_BUTTON_H = 0.06;

static const double MC_X = 0.72, MC_Y = 0.15, MC_W = 0.22, MC_H = 0.60;
static const double POT_X = 0.735, POT_Y = 0.18, POT_SIZE = 0.13;

static const double RESULT_BUTTON_X = 0.40, RESULT_BUTTON_Y = 0.30, RESULT_BUTTON_W = 0.20, RESULT_BUTTON_H = 0.08;

void CookingGame::loadImages()
{
	bg = iLoadImage("Images//bg_cooking.png");
	timeAndScoreImg = iLoadImage("Images//time_and_score.png");
	ingredientsListImg = iLoadImage("Images//cooking_ingredients_list.png");
	neededDishImg = iLoadImage("Images//needed_dish.png");
	cookButtonImg = iLoadImage("Images//cook_button.png");

	mcIdle = iLoadImage("Images//mc_cooking_idle.png");
	mcHand = iLoadImage("Images//mc_cooking_hand.png");
	pot = iLoadImage("Images//cooking_pot.png");

	ingredientImages[0] = iLoadImage("Images//meat.png");
	ingredientImages[1] = iLoadImage("Images//carrot.png");
	ingredientImages[2] = iLoadImage("Images//mushroom.png");
	ingredientImages[3] = iLoadImage("Images//spice.png");
	ingredientImages[4] = iLoadImage("Images//sugar.png");
	ingredientImages[5] = iLoadImage("Images//flour.png");
	ingredientImages[6] = iLoadImage("Images//egg.png");

	dishImages[0] = iLoadImage("Images//soup.png");
	dishImages[1] = iLoadImage("Images//grilled_meat.png");
	dishImages[2] = iLoadImage("Images//bread.png");
	dishImages[3] = iLoadImage("Images//pie.png");
	dishImages[4] = iLoadImage("Images//cake.png");

	successImg = iLoadImage("Images//success.png");
	failedImg = iLoadImage("Images//failed.png");
}

void CookingGame::start()
{
	phase = COOKING_PLAYING;
	timeLeftTicks = COOKING_TIME_LIMIT_TICKS;
	score = 0;
	mistakes = 0;
	finished = false;
	retryRequested = false;
	pickNewDish();
}

void CookingGame::pickNewDish()
{
	currentDish = rand() % 5;
	selectedCount = 0;
	for (int i = 0; i < 7; i++) selected[i] = false;
}

bool CookingGame::checkSelection()
{
	bool matched[3] = { false, false, false };
	int matchCount = 0;

	for (int i = 0; i < 7; i++)
	{
		if (!selected[i]) continue;

		for (int j = 0; j < 3; j++)
		{
			if (!matched[j] && DISH_INGREDIENTS[currentDish][j] == i)
			{
				matched[j] = true;
				matchCount++;
				break;
			}
		}
	}

	return matchCount == 3;
}

void CookingGame::getIngredientSlotPos(int i, int &x, int &y)
{
	x = (int)(SCREEN_WIDTH * INGREDIENT_SLOT_X[i]);
	y = (int)(SCREEN_HEIGHT * INGREDIENT_SLOT_Y);
}

void CookingGame::getNeededSlotPos(int j, int &x, int &y)
{
	x = (int)(SCREEN_WIDTH * NEEDED_SLOT_X[j]);
	y = (int)(SCREEN_HEIGHT * NEEDED_SLOT_Y);
}

bool CookingGame::isInsideBox(int mx, int my, int bx, int by, int bw, int bh)
{
	return mx >= bx && mx <= bx + bw && my >= by && my <= by + bh;
}

void CookingGame::update()
{
	if (phase == COOKING_PLAYING)
	{
		timeLeftTicks--;
		if (timeLeftTicks <= 0)
		{
			timeLeftTicks = 0;
			phase = COOKING_SUCCESS;
		}
		return;
	}

	if (phase == COOKING_ANIM)
	{
		animTimer--;
		if (animTimer % 10 == 0)
		{
			animFrame = 1 - animFrame;
		}

		if (animTimer <= 0)
		{
			if (lastAttemptCorrect)
			{
				score++;
			}
			else
			{
				mistakes++;
				if (mistakes >= COOKING_MAX_MISTAKES)
				{
					phase = COOKING_FAILED;
					return;
				}
			}

			pickNewDish();
			phase = COOKING_PLAYING;
		}
	}
}

void CookingGame::handleClick(int mx, int my)
{
	int rx = (int)(SCREEN_WIDTH * RESULT_BUTTON_X);
	int ry = (int)(SCREEN_HEIGHT * RESULT_BUTTON_Y);
	int rw = (int)(SCREEN_WIDTH * RESULT_BUTTON_W);
	int rh = (int)(SCREEN_HEIGHT * RESULT_BUTTON_H);

	if (phase == COOKING_SUCCESS)
	{
		if (isInsideBox(mx, my, rx, ry, rw, rh)) finished = true;
		return;
	}

	if (phase == COOKING_FAILED)
	{
		if (isInsideBox(mx, my, rx, ry, rw, rh)) retryRequested = true;
		return;
	}

	if (phase != COOKING_PLAYING)
	{
		return;
	}

	int slotSize = (int)(SCREEN_WIDTH * INGREDIENT_SLOT_SIZE);
	for (int i = 0; i < 7; i++)
	{
		int sx, sy;
		getIngredientSlotPos(i, sx, sy);

		if (isInsideBox(mx, my, sx, sy, slotSize, slotSize))
		{
			if (selected[i])
			{
				selected[i] = false;
				selectedCount--;
			}
			else if (selectedCount < 3)
			{
				selected[i] = true;
				selectedCount++;
			}
			return;
		}
	}

	int cbX = (int)(SCREEN_WIDTH * COOK_BUTTON_X);
	int cbY = (int)(SCREEN_HEIGHT * COOK_BUTTON_Y);
	int cbW = (int)(SCREEN_WIDTH * COOK_BUTTON_W);
	int cbH = (int)(SCREEN_HEIGHT * COOK_BUTTON_H);

	if (isInsideBox(mx, my, cbX, cbY, cbW, cbH) && selectedCount == 3)
	{
		lastAttemptCorrect = checkSelection();
		phase = COOKING_ANIM;
		animTimer = 60;
		animFrame = 0;
	}
}

void CookingGame::draw()
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg);

	int mcImg = (phase == COOKING_ANIM && animFrame == 1) ? mcHand : mcIdle;
	iShowImage((int)(SCREEN_WIDTH * MC_X), (int)(SCREEN_HEIGHT * MC_Y),
		(int)(SCREEN_WIDTH * MC_W), (int)(SCREEN_HEIGHT * MC_H), mcImg);
	iShowImage((int)(SCREEN_WIDTH * POT_X), (int)(SCREEN_HEIGHT * POT_Y),
		(int)(SCREEN_WIDTH * POT_SIZE), (int)(SCREEN_WIDTH * POT_SIZE), pot);

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, timeAndScoreImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ingredientsListImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, neededDishImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, cookButtonImg);

	int slotSize = (int)(SCREEN_WIDTH * INGREDIENT_SLOT_SIZE);
	for (int i = 0; i < 7; i++)
	{
		int sx, sy;
		getIngredientSlotPos(i, sx, sy);
		iShowImage(sx, sy, slotSize, slotSize, ingredientImages[i]);

		if (selected[i])
		{
			iSetColor(255, 255, 255);
			iRectangle(sx, sy, slotSize, slotSize);
		}
	}

	iShowImage((int)(SCREEN_WIDTH * DISH_ICON_X), (int)(SCREEN_HEIGHT * DISH_ICON_Y),
		(int)(SCREEN_WIDTH * DISH_ICON_SIZE), (int)(SCREEN_WIDTH * DISH_ICON_SIZE), dishImages[currentDish]);

	int neededSize = (int)(SCREEN_WIDTH * NEEDED_SLOT_SIZE);
	for (int j = 0; j < 3; j++)
	{
		int nx, ny;
		getNeededSlotPos(j, nx, ny);
		iShowImage(nx, ny, neededSize, neededSize, ingredientImages[DISH_INGREDIENTS[currentDish][j]]);
	}

	char timeText[20], scoreText[20];
	int secondsLeft = timeLeftTicks / 100;
	sprintf_s(timeText, "%02d:%02d", secondsLeft / 60, secondsLeft % 60);
	sprintf_s(scoreText, "Score: %d", score);

	iSetColor(255, 255, 255);
	iText(SCREEN_WIDTH * TIME_TEXT_X, SCREEN_HEIGHT * TIME_TEXT_Y, timeText, GLUT_BITMAP_HELVETICA_18);
	iText(SCREEN_WIDTH * SCORE_TEXT_X, SCREEN_HEIGHT * SCORE_TEXT_Y, scoreText, GLUT_BITMAP_HELVETICA_18);

	if (phase == COOKING_SUCCESS) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, successImg);
	else if (phase == COOKING_FAILED) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, failedImg);
}

bool CookingGame::isFinished() { return finished; }
bool CookingGame::isRetryRequested() { return retryRequested; }