#include <cstring>
#include "Inventory.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

#define GLUT_BITMAP_HELVETICA_18 ((void*)8)

const int ICON_X = 1180, ICON_Y = 630, ICON_SIZE = 70;

const int SLOT_X0 = 260, SLOT_X1 = 355, SLOT_X2 = 445, SLOT_X3 = 535, SLOT_X4 = 625;
const int SLOT_Y = 445;
const int SLOT_SIZE = 64;

const int PREVIEW_X = 645, PREVIEW_Y = 158, PREVIEW_SIZE = 398;
const int NAME_TEXT_X = 768, NAME_TEXT_Y = 163;

const int CLOSE_X = 1064, CLOSE_Y = 567, CLOSE_SIZE = 58;

void Inventory::loadImages()
{
	iconImage = iLoadImage("Images//inventory.png");
	iconHoverImage = iLoadImage("Images//hover_inventory.png");
	windowImage = iLoadImage("Images//inventory_window.png");

	pendantIcon = iLoadImage("Images//pendant.png");
	hoodedGiftIcon = iLoadImage("Images//hearth_ember.png");
	compassIcon = iLoadImage("Images//forest_compass.png");
	starwheelIcon = iLoadImage("Images//starwheel.png");
	charmIcon = iLoadImage("Images//veilstep_charm.png");

	hasPendant = false;
	hasHoodedGift = false;
	hasCompass = false;
	hasStarwheel = false;
	hasCharm = false;

	selectedItem = 0;
	isOpen = false;
}

void Inventory::givePendant()
{
	hasPendant = true;
}

bool Inventory::getHasPendant()
{
	return hasPendant;
}

void Inventory::giveHoodedGift()
{
	hasHoodedGift = true;
}

void Inventory::giveCompass()  
{
	hasCompass = true;
}
bool Inventory::getHasCompass()
{
	return hasCompass;
}
int  Inventory::getCompassIcon()
{
	return compassIcon;
}

void Inventory::giveStarwheel()
{
	hasStarwheel = true;
}

bool Inventory::getHasStarwheel()
{
	return hasStarwheel;
}

int Inventory::getStarwheelIcon()
{
	return starwheelIcon;
}

int Inventory::getHoodedGiftIcon()
{
	return hoodedGiftIcon;
}

bool Inventory::getHasHoodedGift()
{
	return hasHoodedGift;
}

void Inventory::giveCharm()
{
	hasCharm = true;
}
bool Inventory::getHasCharm()
{
	return hasCharm;
}
int  Inventory::getCharmIcon()
{
	return charmIcon;
}

bool Inventory::getIsOpen()
{
	return isOpen;
}

bool Inventory::isInsideBox(int mx, int my, int bx, int by, int bw, int bh)
{
	return mx >= bx && mx <= bx + bw && my >= by && my <= by + bh;
}

bool Inventory::handleClick(int mx, int my)
{
	if (isOpen)
	{
		if (isInsideBox(mx, my, CLOSE_X, CLOSE_Y, CLOSE_SIZE, CLOSE_SIZE))
		{
			isOpen = false;
			return true;
		}

		if (hasPendant && isInsideBox(mx, my, SLOT_X0, SLOT_Y, SLOT_SIZE, SLOT_SIZE))
		{
			selectedItem = 1;
		}

		if (hasHoodedGift && isInsideBox(mx, my, SLOT_X1, SLOT_Y, SLOT_SIZE, SLOT_SIZE))
		{
			selectedItem = 2;
		}

		if (hasCompass && isInsideBox(mx, my, SLOT_X2, SLOT_Y, SLOT_SIZE, SLOT_SIZE))
		{
			selectedItem = 3;
		}
		if (hasStarwheel && isInsideBox(mx, my, SLOT_X3, SLOT_Y, SLOT_SIZE, SLOT_SIZE))
		{
			selectedItem = 4;
		}
		if (hasCharm && isInsideBox(mx, my, SLOT_X4, SLOT_Y, SLOT_SIZE, SLOT_SIZE))
		{
			selectedItem = 5;
		}

		return true;
	}

	if (isInsideBox(mx, my, ICON_X, ICON_Y, ICON_SIZE, ICON_SIZE))
	{
		isOpen = true;
		selectedItem = 0;
		return true;
	}

	return false;
}

void Inventory::draw(int mouseX, int mouseY)
{
	if (!isOpen)
	{
		bool hovering = isInsideBox(mouseX, mouseY, ICON_X, ICON_Y, ICON_SIZE, ICON_SIZE);
		iShowImage(ICON_X, ICON_Y, ICON_SIZE, ICON_SIZE, hovering ? iconHoverImage : iconImage);
		return;
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, windowImage);

	if (hasPendant)
	{
		iShowImage(SLOT_X0, SLOT_Y, SLOT_SIZE, SLOT_SIZE, pendantIcon);
	}

	if (hasHoodedGift)
	{
		iShowImage(SLOT_X1, SLOT_Y, SLOT_SIZE, SLOT_SIZE, hoodedGiftIcon);
	}

	if (hasCompass)
	{
		iShowImage(SLOT_X2, SLOT_Y, SLOT_SIZE, SLOT_SIZE, compassIcon);
	}
	if (hasStarwheel)
	{
		iShowImage(SLOT_X3, SLOT_Y, SLOT_SIZE, SLOT_SIZE, starwheelIcon);
	}
	if (hasCharm)
	{
		iShowImage(SLOT_X4, SLOT_Y, SLOT_SIZE, SLOT_SIZE, charmIcon);
	}

	if (selectedItem != 0)
	{
		int previewIcon = pendantIcon;
		char* name = "Guardian's Pendant";

		if (selectedItem == 2) { previewIcon = hoodedGiftIcon; name = "Hearth Ember"; }
		else if (selectedItem == 3) { previewIcon = compassIcon; name = "Forest Compass"; }
		else if (selectedItem == 4) { previewIcon = starwheelIcon; name = "Guardian's Starwheel"; }
		else if (selectedItem == 5) { previewIcon = charmIcon; name = "Veilstep Charm"; }

		iShowImage(PREVIEW_X, PREVIEW_Y, PREVIEW_SIZE, PREVIEW_SIZE, previewIcon);

		iSetColor(255, 255, 255);
		iText(NAME_TEXT_X, NAME_TEXT_Y, name, GLUT_BITMAP_HELVETICA_18);
	}
}