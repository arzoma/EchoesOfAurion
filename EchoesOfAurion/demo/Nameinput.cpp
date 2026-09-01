#include <cstring>
#include "NameInput.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

#define GLUT_BITMAP_HELVETICA_18 ((void*)8)

const double BAR_X_MIN = 0.38, BAR_X_MAX = 0.61;
const double BAR_Y_MIN = 0.44, BAR_Y_MAX = 0.55;

const double START_X_MIN = 0.42, START_X_MAX = 0.58;
const double START_Y_MIN = 0.33, START_Y_MAX = 0.39;

static bool isInside(int mx, int my, double xMinPct, double xMaxPct, double yMinPct, double yMaxPct)
{
	int xMin = (int)(SCREEN_WIDTH * xMinPct);
	int xMax = (int)(SCREEN_WIDTH * xMaxPct);
	int yMin = (int)(SCREEN_HEIGHT * yMinPct);
	int yMax = (int)(SCREEN_HEIGHT * yMaxPct);

	return mx >= xMin && mx <= xMax && my >= yMin && my <= yMax;
}

void NameInput::loadImages()
{
	bgTypeHere = iLoadImage("Images//name_input_typehere.png");
	bgTyping = iLoadImage("Images//name_input.png");
	bgTypeHereHover = iLoadImage("Images//name_input_typehere_hover_startgame.png");
	bgTypingHover = iLoadImage("Images//name_input_hover_startgame.png");

	nameLength = 0;
	playerName[0] = '\0';
	isTyping = false;
	mouseX = 0;
	mouseY = 0;
}

void NameInput::mouseMove(int mx, int my)
{
	mouseX = mx;
	mouseY = my;
}

bool NameInput::mouseClick(int mx, int my)
{
	if (isInside(mx, my, BAR_X_MIN, BAR_X_MAX, BAR_Y_MIN, BAR_Y_MAX))
	{
		isTyping = true;
		return false;
	}

	if (isInside(mx, my, START_X_MIN, START_X_MAX, START_Y_MIN, START_Y_MAX))
	{
		if (nameLength > 0)
		{
			return true;
		}
	}

	return false;
}

bool NameInput::handleKeyPress(unsigned char key)
{
	if (!isTyping)
	{
		return false;
	}

	if (key == 8) // backspace
	{
		if (nameLength > 0)
		{
			nameLength--;
			playerName[nameLength] = '\0';
		}
		return false;
	}

	if (key == 13) // enter
	{
		return nameLength > 0;
	}

	if (nameLength < MAX_NAME_LENGTH && key >= 32 && key <= 126)
	{
		playerName[nameLength] = key;
		nameLength++;
		playerName[nameLength] = '\0';
	}

	return false;
}

void NameInput::draw()
{
	bool hoveringStart = isInside(mouseX, mouseY, START_X_MIN, START_X_MAX, START_Y_MIN, START_Y_MAX);

	int bg;
	if (isTyping)
	{
		bg = hoveringStart ? bgTypingHover : bgTyping;
	}
	else
	{
		bg = hoveringStart ? bgTypeHereHover : bgTypeHere;
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg);

	if (isTyping && nameLength > 0)
	{
		iSetColor(255, 255, 255);
		int textX = (int)(SCREEN_WIDTH * 0.44);
		int textY = (int)(SCREEN_HEIGHT * 0.48);
		iText(textX, textY, playerName, GLUT_BITMAP_HELVETICA_18);
	}
}

char* NameInput::getName()
{
	return playerName;
}