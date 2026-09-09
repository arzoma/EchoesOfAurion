#include "Player.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);

void Player::init(int startX, int startY)
{
	x = startX;
	y = startY;

	facing = DIR_FRONT;
	moving = false;

	walkFrame = 0;
	walkTimer = 0;

	blinkState = 0;
	blinkTimer = 300;
}

void Player::loadImages()
{
	idleFront1 = iLoadImage("Images//idle_front_1.png");
	idleFront2 = iLoadImage("Images//idle_front_2.png");
	idleFront3 = iLoadImage("Images//idle_front_3.png");
	idleLeft = iLoadImage("Images//idle_left.png");
	idleRight = iLoadImage("Images//idle_right.png");
	idleBack = iLoadImage("Images//idle_back.png");

	walkFront1 = iLoadImage("Images//walk_front_1.png");
	walkFront2 = iLoadImage("Images//walk_front_2.png");
	walkLeft1 = iLoadImage("Images//walk_left_1.png");
	walkLeft2 = iLoadImage("Images//walk_left_2.png");
	walkRight1 = iLoadImage("Images//walk_right_1.png");
	walkRight2 = iLoadImage("Images//walk_right_2.png");
	walkBack1 = iLoadImage("Images//walk_back_1.png");
	walkBack2 = iLoadImage("Images//walk_back_2.png");
}

void Player::setFacing(Direction d)
{
	facing = d;
}

void Player::handleInput(bool up, bool down, bool left, bool right, Map &currentMap)
{
	int dx = 0;
	int dy = 0;

	if (up)
	{
		dy = PLAYER_SPEED;
		facing = DIR_BACK;
	}
	else if (down)
	{
		dy = -PLAYER_SPEED;
		facing = DIR_FRONT;
	}

	if (left)
	{
		dx = -PLAYER_SPEED;
		facing = DIR_LEFT;
	}
	else if (right)
	{
		dx = PLAYER_SPEED;
		facing = DIR_RIGHT;
	}

	moving = (dx != 0 || dy != 0);

	if (!moving)
	{
		return;
	}

	if (!currentMap.isBlocked(feetRectAt(x + dx, y)))
	{
		x += dx;
	}

	if (!currentMap.isBlocked(feetRectAt(x, y + dy)))
	{
		y += dy;
	}
}

void Player::updateAnimation()
{
	if (moving)
	{
		walkTimer++;
		if (walkTimer >= 15)
		{
			walkTimer = 0;
			walkFrame = (walkFrame + 1) % 4;
		}
		return;
	}

	walkFrame = 0;

	if (facing != DIR_FRONT)
	{
		return;
	}

	blinkTimer--;
	if (blinkTimer > 0)
	{
		return;
	}

	switch (blinkState)
	{
	case 0: // normal to half-closed
		blinkState = 1;
		blinkTimer = 8;
		break;

	case 1: // half to fully closed
		blinkState = 2;
		blinkTimer = 10;
		break;

	case 2: // closed to half again (opening)
		blinkState = 3;
		blinkTimer = 8;
		break;

	case 3: // half to back to normal, wait for next blink
	default:
		blinkState = 0;
		blinkTimer = 300;
		break;
	}
}

void Player::draw(int cameraX, int cameraY)
{
	int image = idleFront1;

	if (moving)
	{
		switch (facing)
		{
		case DIR_BACK:
			image = (walkFrame == 0) ? walkBack1 : walkBack2;  break;
		case DIR_LEFT:
			if (walkFrame == 0) image = walkLeft1;
			else if (walkFrame == 2) image = walkLeft2;
			else image = idleLeft;
			break;
		case DIR_RIGHT:
			if (walkFrame == 0) image = walkRight1;
			else if (walkFrame == 2) image = walkRight2;
			else image = idleRight;
			break;
		case DIR_FRONT:
		default:
			image = (walkFrame == 0) ? walkFront1 : walkFront2; break;
		}
	}
	else
	{
		switch (facing)
		{
		case DIR_BACK:
			image = idleBack;  break;
		case DIR_LEFT:
			image = idleLeft;  break;
		case DIR_RIGHT:
			image = idleRight; break;
		case DIR_FRONT:
		default:
			if (blinkState == 1 || blinkState == 3)  image = idleFront2;
			else if (blinkState == 2)                image = idleFront3;
			else                                      image = idleFront1;
			break;
		}
	}

	iShowImage(x - cameraX, y - cameraY, PLAYER_WIDTH, PLAYER_HEIGHT, image);
}

int Player::getX() { return x; }
int Player::getY() { return y; }

Rect Player::getRect()
{
	Rect r = { x, y, PLAYER_WIDTH, PLAYER_HEIGHT };
	return r;
}

Rect Player::feetRectAt(int px, int py)
{
	Rect r;
	r.x = px + (PLAYER_WIDTH - PLAYER_FEET_W) / 2;
	r.y = py;
	r.w = PLAYER_FEET_W;
	r.h = PLAYER_FEET_H;
	return r;
}

Rect Player::getFeetRect()
{
	return feetRectAt(x, y);
}