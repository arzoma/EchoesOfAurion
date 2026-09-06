#ifndef COOKINGGAME_HPP
#define COOKINGGAME_HPP

enum CookingPhase
{
	COOKING_PLAYING,
	COOKING_ANIM,
	COOKING_SUCCESS,
	COOKING_FAILED
};

const int COOKING_TIME_LIMIT_TICKS = 6000; // 1 minute at a 10ms tick
const int COOKING_MAX_MISTAKES = 3;

const int COOKING_TARGET_DISHES = 15;
const int COOKING_TARGET_DISHES_WITH_PERK = 10;

class CookingGame
{

private:

	int bg;
	int timeAndScoreImg;
	int ingredientsListImg;
	int neededDishImg;
	int cookButtonImg;
	int cookButtonHoverImg;

	int mcIdle;
	int mcHand;
	int pot;

	int ingredientImages[7]; // meat, carrot, mushroom, spice, sugar, flour, egg
	int dishImages[5];        // soup, grilled_meat, bread, pie, cake

	int successImg;
	int failedImg;

	CookingPhase phase;

	int timeLeftTicks;
	int score;
	int mistakes;

	int targetDishes;

	int currentDish;
	bool selected[7];
	int selectedCount;

	int animTimer;
	int animFrame;   // 0 or 1 toggle idle/hand
	bool lastAttemptCorrect;

	bool finished;
	bool retryRequested;

	void pickNewDish();
	bool checkSelection();
	void getIngredientSlotPos(int i, int &x, int &y);
	void getNeededSlotPos(int j, int &x, int &y);
	bool isInsideBox(int mx, int my, int bx, int by, int bw, int bh);

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