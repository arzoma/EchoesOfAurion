#ifndef MAP_HPP
#define MAP_HPP

struct Rect
{
	int x, y, w, h;
};

const int MAX_OBSTACLES = 300;

class Map
{

private:

	int background;
	int mapWidth;
	int mapHeight;
	bool scrollable;

	Rect obstacles[MAX_OBSTACLES];
	bool obstacleActive[MAX_OBSTACLES];
	int obstacleCount;

	int cameraX;
	int cameraY;

public:

	void init(char imagePath[], int worldWidth, int worldHeight, bool isScrollable);

	int  addObstacle(int obsX, int obsY, int obsW, int obsH);
	void setObstacleActive(int index, bool on);
	void clearObstacles();

	void updateCamera(int playerX, int playerY, int playerWidth, int playerHeight);

	bool isBlocked(Rect targetRect);

	void draw();

	void setCameraY(int y);

	int getCameraX();
	int getCameraY();

};

#endif