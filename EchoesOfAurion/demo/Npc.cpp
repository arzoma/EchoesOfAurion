#include <cstring>
#include "Npc.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);

void Npc::init(int startX, int startY, int spriteWidth, int spriteHeight, char npcName[])
{
	x = startX;
	y = startY;
	width = spriteWidth;
	height = spriteHeight;
	strcpy_s(name, npcName);
}

void Npc::loadImage(char imagePath[])
{
	sprite = iLoadImage(imagePath);
}

void Npc::draw(int cameraX, int cameraY)
{
	iShowImage(x - cameraX, y - cameraY, width, height, sprite);
}

bool Npc::isPlayerNearby(Rect playerRect, int interactionRange)
{
	Rect expanded = { x - interactionRange, y - interactionRange,
		width + interactionRange * 2, height + interactionRange * 2 };

	bool overlap = playerRect.x < expanded.x + expanded.w && playerRect.x + playerRect.w > expanded.x &&
		playerRect.y < expanded.y + expanded.h && playerRect.y + playerRect.h > expanded.y;

	return overlap;
}

char* Npc::getName()
{
	return name;
}

Rect Npc::getRect()
{
	Rect r = { x, y, width, height };
	return r;
}