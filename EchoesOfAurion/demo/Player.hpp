#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Map.hpp"

enum Direction
{
	DIR_FRONT,
	DIR_BACK,
	DIR_LEFT,
	DIR_RIGHT
};

const int PLAYER_WIDTH = 90;
const int PLAYER_HEIGHT = 120;
const int PLAYER_SPEED = 3;

class Player
{

private:

	int x;
	int y;

	Direction facing;
	bool moving;

	// idle sprites
	int idleFront1;
	int idleFront2;
	int idleFront3;
	int idleLeft;
	int idleRight;
	int idleBack;

	// walk sprites
	int walkFront1;
	int walkFront2;
	int walkLeft1;
	int walkLeft2;
	int walkRight1;
	int walkRight2;
	int walkBack1;
	int walkBack2;

	// walking animation
	int walkFrame;
	int walkTimer;

	// blink animation (front idle only)
	// 0 = normal, 1 = half-closed, 2 = fully closed, 3 = half-closed (opening)
	int blinkState;
	int blinkTimer;

public:

	void init(int startX, int startY);
	void loadImages();

	void setFacing(Direction d);

	void handleInput(bool up, bool down, bool left, bool right, Map &currentMap);

	void updateAnimation();

	void draw(int cameraX, int cameraY);

	int getX();
	int getY();
	Rect getRect();

};

#endif