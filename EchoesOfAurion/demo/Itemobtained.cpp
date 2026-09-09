#include "ItemObtained.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);

static const int ICON_X = 490;
static const int ICON_Y = 265;
static const int ICON_SIZE = 300;

static const int GLOW_TICKS = 12;

void ItemObtained::loadImages()
{
	frames[0] = iLoadImage("Images//item_obtained.png");
	frames[1] = iLoadImage("Images//item_obtained.png");
	frames[2] = iLoadImage("Images//item_obtained.png");

	itemIcon = -1;
	active = false;
	frame = 0;
	frameTimer = 0;
}

void ItemObtained::show(int iconImage)
{
	itemIcon = iconImage;
	active = true;
	frame = 0;
	frameTimer = 0;
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

	if (frame >= 2) return; // 1 -> 2 -> 3

	frameTimer++;
	if (frameTimer >= GLOW_TICKS)
	{
		frameTimer = 0;
		frame++;
	}
}

void ItemObtained::draw()
{
	if (!active) return;

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, frames[frame]);

	if (itemIcon != -1)
	{
		iShowImage(ICON_X, ICON_Y, ICON_SIZE, ICON_SIZE, itemIcon);
	}
}