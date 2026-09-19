#include "Map.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);

void iRectangle(double left, double bottom, double dx, double dy);
void iSetColor(double r, double g, double b);

void Map::init(char imagePath[], int worldWidth, int worldHeight, bool isScrollable)
{
	background = iLoadImage(imagePath);
	mapWidth = worldWidth;
	mapHeight = worldHeight;
	scrollable = isScrollable;
	obstacleCount = 0;
	cameraX = 0;
	cameraY = 0;
}

int Map::addObstacle(int obsX, int obsY, int obsW, int obsH)
{
	if (obstacleCount >= MAX_OBSTACLES)
	{
		return -1;
	}

	obstacles[obstacleCount].x = obsX;
	obstacles[obstacleCount].y = obsY;
	obstacles[obstacleCount].w = obsW;
	obstacles[obstacleCount].h = obsH;
	obstacleActive[obstacleCount] = true;
	obstacleCount++;

	return obstacleCount - 1;
}

void Map::setObstacleActive(int index, bool on)
{
	if (index < 0 || index >= obstacleCount) return;
	obstacleActive[index] = on;
}

void Map::clearObstacles()
{
	obstacleCount = 0;
}

void Map::updateCamera(int playerX, int playerY, int playerWidth, int playerHeight)
{
	if (!scrollable)
	{
		cameraX = 0;
		cameraY = 0;
		return;
	}

	cameraX = playerX + playerWidth / 2 - SCREEN_WIDTH / 2;
	cameraY = playerY + playerHeight / 2 - SCREEN_HEIGHT / 2;

	if (cameraX < 0) cameraX = 0;
	if (cameraY < 0) cameraY = 0;
	if (cameraX > mapWidth - SCREEN_WIDTH)   cameraX = mapWidth - SCREEN_WIDTH;
	if (cameraY > mapHeight - SCREEN_HEIGHT) cameraY = mapHeight - SCREEN_HEIGHT;
}

bool Map::isBlocked(Rect targetRect)
{
	if (targetRect.x < 0 || targetRect.y < 0 ||
		targetRect.x + targetRect.w > mapWidth ||
		targetRect.y + targetRect.h > mapHeight)
	{
		return true;
	}

	for (int i = 0; i < obstacleCount; i++)
	{
		if (!obstacleActive[i]) continue;
		
		Rect o = obstacles[i];

		bool overlap = targetRect.x < o.x + o.w && targetRect.x + targetRect.w > o.x &&
			targetRect.y < o.y + o.h && targetRect.y + targetRect.h > o.y;

		if (overlap)
		{
			return true;
		}
	}

	return false;
}

void Map::draw()
{
	iShowImage(-cameraX, -cameraY, mapWidth, mapHeight, background);

	/*iSetColor(255, 0, 0);

	for (int i = 0; i < obstacleCount; i++)
	{
		iRectangle(
			obstacles[i].x - cameraX,
			obstacles[i].y - cameraY,
			obstacles[i].w,
			obstacles[i].h
			);
	}*/

}

int Map::getCameraX()
{
	return cameraX;
}
int Map::getCameraY()
{
	return cameraY;
}
void Map::setCameraY(int y)
{
	cameraY = y;
}