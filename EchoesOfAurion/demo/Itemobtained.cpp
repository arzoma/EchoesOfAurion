#include "ItemObtained.hpp"
#include "Constants.hpp"

#include <cmath>

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);

extern float g_imgAlpha;

static const int ICON_X = 490;
static const int ICON_Y = 265;
static const int ICON_SIZE = 300;

static const int GLOW_TICKS = 12;

void ItemObtained::loadImages()
{
	frames[0] = iLoadImage("Images//item_obtained_1.png");
	frames[1] = iLoadImage("Images//item_obtained_2.png");
	frames[2] = iLoadImage("Images//item_obtained_3.png");

	itemIcon = -1;
	active = false;
	popupAlpha = 0.0;
	glowTimer = 0.0;
}

void ItemObtained::show(int iconImage)
{
	itemIcon = iconImage;
	active = true;
	popupAlpha = 0.0;
	glowTimer = 0.0;;
}

void ItemObtained::hide()
{
	active = false;
}

bool ItemObtained::getIsActive()
{
	return active;
}

void ItemObtained::update()
{
	if (!active) return;

	if (popupAlpha < 1.0)
	{
		popupAlpha += 0.04;
		if (popupAlpha > 1.0) popupAlpha = 1.0;
	}

	glowTimer += 0.01;
}

void ItemObtained::draw()
{
	if (!active) return;

	double pulse = 0.5 + 0.5 * sin(glowTimer * 1.6);

	g_imgAlpha = (float)popupAlpha;
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, frames[0]);

	g_imgAlpha = (float)(popupAlpha * pulse * 0.85);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, frames[1]);

	g_imgAlpha = (float)(popupAlpha * pulse * pulse * 0.7);
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, frames[2]);

	if (itemIcon != -1)
	{
		g_imgAlpha = (float)popupAlpha;
		iShowImage(ICON_X, ICON_Y, ICON_SIZE, ICON_SIZE, itemIcon);
	}
}