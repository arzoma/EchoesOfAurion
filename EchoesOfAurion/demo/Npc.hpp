#ifndef NPC_HPP
#define NPC_HPP

#include "Map.hpp"

class Npc
{

private:

	int x;
	int y;
	int width;
	int height;

	int sprite;
	char name[30]; // dialogue box name plate

public:

	void init(int startX, int startY, int spriteWidth, int spriteHeight, char npcName[]);

	void loadImage(char imagePath[]);

	void draw(int cameraX, int cameraY);

	bool isPlayerNearby(Rect playerRect, int interactionRange);

	char* getName();
	Rect getRect();

};

#endif