#include <cstdio>
#include <cstdlib>
#include "CookingGame.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);
void iRectangle(double x, double y, double width, double height);

extern float g_imgAlpha;

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

static const int TIME_TEXT_X = 187, TIME_TEXT_Y = 663;
static const int SCORE_TEXT_X = 952, SCORE_TEXT_Y = 662;
static const int MISTAKES_TEXT_X = 952, MISTAKES_TEXT_Y = 590;

static const int DISH_ICON_X = 335, DISH_ICON_Y = 416, DISH_ICON_SIZE = 130;

static const int NEEDED_SLOT_X[3] = { 276, 370, 463 };
static const int NEEDED_SLOT_Y = 318;
static const int NEEDED_SLOT_SIZE = 62;

static const int INGREDIENT_SLOT_X[7] = { 63, 184, 305, 426, 547, 668, 789 };
static const int INGREDIENT_SLOT_Y = 158;
static const int INGREDIENT_SLOT_SIZE = 82;

static const int CHOP_SLOT_X[3] = { 296, 428, 560 };
static const int CHOP_SLOT_Y = 62;
static const int CHOP_SLOT_SIZE = 72;

static const int COOK_BUTTON_X = 728, COOK_BUTTON_Y = 51;
static const int COOK_BUTTON_W = 175, COOK_BUTTON_H = 52;

static const int MC_X = 922, MC_Y = 57, MC_W = 324, MC_H = 432;
static const int POT_X = 946, POT_Y = 100, POT_SIZE = 146;

static const int RESULT_BUTTON_X = 488, RESULT_BUTTON_Y = 238;
static const int RESULT_BUTTON_W = 300, RESULT_BUTTON_H = 60;

static const int RESULT_FADE_TICKS = 40;

void CookingGame::loadImages()
{
	bg = iLoadImage("Images//bg_cooking.png");
	timeAndScoreImg = iLoadImage("Images//time_and_score.png");
	ingredientsListImg = iLoadImage("Images//cooking_ingredients_list.png");
	neededDishImg = iLoadImage("Images//needed_dish.png");
	cookButtonImg = iLoadImage("Images//cook_button.png");
	cookButtonHoverImg = iLoadImage("Images//hover_cook_button.png");

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
	hoverSuccessImg = iLoadImage("Images//hover_success.png");
	hoverFailedImg = iLoadImage("Images//hover_failed.png");
}

void CookingGame::start(bool hasPerk)
{
	phase = COOKING_PLAYING;
	timeLeftTicks = COOKING_TIME_LIMIT_TICKS;
	resultTicks = 0;
	score = 0;
	mistakes = 0;
	finished = false;
	retryRequested = false;
	targetDishes = hasPerk ? COOKING_TARGET_DISHES_WITH_PERK : COOKING_TARGET_DISHES;

	animTimer = 0;
	animFrame = 0;
	lastAttemptCorrect = false;

	pickNewDish();
}

void CookingGame::pickNewDish()
{
	currentDish = rand() % 5;
	selectedCount = 0;
	for (int i = 0; i < 3; i++) picks[i] = -1;
}

bool CookingGame::isSelected(int ingredient)
{
	for (int i = 0; i < selectedCount; i++)
	{
		if (picks[i] == ingredient) return true;
	}
	return false;
}

void CookingGame::togglePick(int ingredient)
{
	for (int i = 0; i < selectedCount; i++)
	{
		if (picks[i] == ingredient)
		{
			// shuffle the rest down so the board stays left-aligned
			for (int j = i; j < selectedCount - 1; j++) picks[j] = picks[j + 1];
			selectedCount--;
			picks[selectedCount] = -1;
			return;
		}
	}

	if (selectedCount < 3)
	{
		picks[selectedCount] = ingredient;
		selectedCount++;
	}
}

bool CookingGame::checkSelection()
{
	bool matched[3] = { false, false, false };
	int matchCount = 0;

	for (int i = 0; i < selectedCount; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (!matched[j] && DISH_INGREDIENTS[currentDish][j] == picks[i])
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
	x = INGREDIENT_SLOT_X[i];
	y = INGREDIENT_SLOT_Y;
}

void CookingGame::getNeededSlotPos(int j, int &x, int &y)
{
	x = NEEDED_SLOT_X[j];
	y = NEEDED_SLOT_Y;
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
			phase = COOKING_FAILED;
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
				if (score >= targetDishes)
				{
					phase = COOKING_SUCCESS;
					return;
				}
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

	if (phase == COOKING_SUCCESS)
	{
		if (isInsideBox(mx, my, RESULT_BUTTON_X, RESULT_BUTTON_Y, RESULT_BUTTON_W, RESULT_BUTTON_H))
		{
			finished = true;
		}
		return;
	}

	if (phase == COOKING_FAILED)
	{
		if (isInsideBox(mx, my, RESULT_BUTTON_X, RESULT_BUTTON_Y, RESULT_BUTTON_W, RESULT_BUTTON_H))
		{	
			retryRequested = true;
		}
		return;
	}

	if (phase != COOKING_PLAYING)
	{
		return;
	}

	for (int i = 0; i < 7; i++)
	{
		int sx, sy;
		getIngredientSlotPos(i, sx, sy);

		if (isInsideBox(mx, my, sx, sy, INGREDIENT_SLOT_SIZE, INGREDIENT_SLOT_SIZE))
		{
			{
				togglePick(i);
				return;
			}
		}

		for (int i = 0; i < selectedCount; i++)
		{
			if (isInsideBox(mx, my, CHOP_SLOT_X[i], CHOP_SLOT_Y, CHOP_SLOT_SIZE, CHOP_SLOT_SIZE))
			{
				togglePick(picks[i]);
				return;
			}
		}

		if (isInsideBox(mx, my, COOK_BUTTON_X, COOK_BUTTON_Y, COOK_BUTTON_W, COOK_BUTTON_H) && selectedCount == 3)
		{
			lastAttemptCorrect = checkSelection();
			phase = COOKING_ANIM;
			animTimer = 60;
			animFrame = 0;
		}

	}
}

void CookingGame::draw(int mouseX, int mouseY)
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg);

	int mcImg = (phase == COOKING_ANIM && animFrame == 1) ? mcHand : mcIdle;
	iShowImage(MC_X, MC_Y, MC_W, MC_H, mcImg);
	iShowImage(POT_X, POT_Y, POT_SIZE, POT_SIZE, pot);

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, timeAndScoreImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ingredientsListImg);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, neededDishImg);

	bool hoveringCook = phase == COOKING_PLAYING && isInsideBox(mouseX, mouseY, COOK_BUTTON_X, COOK_BUTTON_Y, COOK_BUTTON_W, COOK_BUTTON_H);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hoveringCook ? cookButtonHoverImg : cookButtonImg);

	for (int i = 0; i < 7; i++)
	{
		int sx, sy;
		getIngredientSlotPos(i, sx, sy);
		iShowImage(sx, sy, INGREDIENT_SLOT_SIZE, INGREDIENT_SLOT_SIZE, ingredientImages[i]);

		if (isSelected(i))
		{
			iSetColor(255, 255, 255);
			iRectangle(sx, sy, INGREDIENT_SLOT_SIZE, INGREDIENT_SLOT_SIZE);
		}
	}

	for (int i = 0; i < selectedCount; i++)
	{
		iShowImage(CHOP_SLOT_X[i], CHOP_SLOT_Y, CHOP_SLOT_SIZE, CHOP_SLOT_SIZE, ingredientImages[picks[i]]);
	}

	iShowImage(DISH_ICON_X, DISH_ICON_Y, DISH_ICON_SIZE, DISH_ICON_SIZE, dishImages[currentDish]);

	for (int j = 0; j < 3; j++)
	{
		int nx, ny;
		getNeededSlotPos(j, nx, ny);
		iShowImage(nx, ny, NEEDED_SLOT_SIZE, NEEDED_SLOT_SIZE, ingredientImages[DISH_INGREDIENTS[currentDish][j]]);
	}

	char timeText[20], scoreText[50], mistakeText[50];
	int secondsLeft = timeLeftTicks / 100;
	sprintf_s(timeText, "%02d:%02d", secondsLeft / 60, secondsLeft % 60);
	sprintf_s(scoreText, "Score: %d Target: %d", score, targetDishes);
	sprintf_s(mistakeText, "Mistakes: %d/%d", mistakes, COOKING_MAX_MISTAKES);

	iSetColor(255, 255, 255);
	iText(TIME_TEXT_X, TIME_TEXT_Y, timeText, GLUT_BITMAP_HELVETICA_18);
	iText(SCORE_TEXT_X, SCORE_TEXT_Y, scoreText, GLUT_BITMAP_HELVETICA_18);
	iText(MISTAKES_TEXT_X, MISTAKES_TEXT_Y, mistakeText, GLUT_BITMAP_HELVETICA_18);

	bool hoveringSuccess = phase == COOKING_SUCCESS && isInsideBox(mouseX, mouseY, RESULT_BUTTON_X, RESULT_BUTTON_Y, RESULT_BUTTON_W, RESULT_BUTTON_H);
	bool hoveringFailed = phase == COOKING_FAILED && isInsideBox(mouseX, mouseY, RESULT_BUTTON_X, RESULT_BUTTON_Y, RESULT_BUTTON_W, RESULT_BUTTON_H);

	if (phase == COOKING_SUCCESS){
		if (hoveringSuccess){
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hoverSuccessImg);
		}
		else{
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, successImg);
		}
	}
	else if (phase == COOKING_FAILED){
		if (hoveringFailed){
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hoverFailedImg);
		}
		else{
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, failedImg);
		}
	}
}

bool CookingGame::isFinished()
{
	if (!finished) return false;
	finished = false;
	return true;
}

bool CookingGame::isRetryRequested()
{
	if (!retryRequested) return false;
	retryRequested = false;
	return true;
}