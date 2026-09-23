#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "GhostChase.hpp"
#include "Constants.hpp"

#include "glut.h"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

extern float g_imgAlpha;
extern bool  g_imgAdditive;

int getTextWidth(const char* text);

static const int UW = 2560, UH = 1440;

static const int SPAWN_X = 400, SPAWN_Y = 760;

static const int STONE_W = 96, STONE_H = 96;
static const int CHANNEL_TICKS = 150;

static const int RING_SIZE = 160;
static const int WARD_GLOW_SIZE = 260;

static const int DRIFT_W = 96, DRIFT_H = 128;
static const int WARDEN_W = 128, WARDEN_H = 160;
static const int GHOST_HIT = 56;
static const int GHOST_GLOW_SIZE = 220;
static const int GHOST_VISIBLE_RANGE = 820;

static const int SIGHT_RANGE = 260;
static const int ALERT_TICKS = 50;
static const int LOST_TICKS = 150;

static const double DRIFT_PATROL = 1.6;
static const double DRIFT_CHASE = 2.2;
static const double WARDEN_BASE = 1.05;
static const double WARDEN_PER_STONE = 0.09;

static const int GRACE_TICKS = 90;
static const int FLASH_TICKS = 30;
static const int WALK_FRAME_TICKS = 14;

static const int GHOST_MOVE = 36;

static const int INVIS_TICKS = 400;
static const int CHARM_COOLDOWN = 900;

static const int DARK_W = 2560, DARK_H = 1440;

static const int DOOR_X = 1940, DOOR_Y = 170, DOOR_W = 110, DOOR_H = 100;

static const int RESULT_BUTTON_X = 488, RESULT_BUTTON_Y = 238;
static const int RESULT_BUTTON_W = 300, RESULT_BUTTON_H = 60;
static const int RESULT_FADE_TICKS = 40;

static const int HUD_LIGHTS_X = 30, HUD_LIGHTS_Y = 678;
static const int HUD_TIME_X = 1090, HUD_TIME_Y = 678;
static const int HUD_MISS_X = 1090, HUD_MISS_Y = 648;
static const int HUD_CHARM_X = 30, HUD_CHARM_Y = 30, HUD_CHARM_SIZE = 64;

static const int STONE_POS[GC_STONES][2] =
{
	{ 420, 960 },
	{ 520, 300 },
	{ 1220, 1140 },
	{ 1200, 520 },
	{ 2060, 1080 }
};

static const int ALCOVES[GC_ALCOVES][4] =
{
	{ 320, 1160, 110, 90 },
	{ 200, 300, 110, 90 },
	{ 1120, 1040, 110, 90 },
	{ 1100, 720, 110, 90 },
	{ 1960, 820, 110, 90 },
	{ 2000, 460, 110, 90 }
};

static const int WP_COUNT[GC_DRIFTERS] = { 4, 4, 2 };
static const int WP[GC_DRIFTERS][8][2] =
{
	{ { 330, 1230 }, { 590, 1010 }, { 630, 1010 }, { 590, 1010 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 1970, 1210 }, { 2030, 390 }, { 2210, 330 }, { 2030, 390 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 1290, 1210 }, { 1150, 430 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
};

static char* FLAVOUR[3] =
{
	"Something in the water moved.",
	"The light holds. Four more.",
	"You are not alone down here."
};

static void drawPlate(char* s)
{
	int w = getTextWidth(s);
	int x = SCREEN_WIDTH / 2 - w / 2;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.6f);

	glBegin(GL_QUADS);
	glVertex2f((float)(x - 20), 44.0f);
	glVertex2f((float)(x + w + 20), 44.0f);
	glVertex2f((float)(x + w + 20), 82.0f);
	glVertex2f((float)(x - 20), 82.0f);
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	iSetColor(255, 255, 255);
	iText(x, 56, s, GLUT_BITMAP_HELVETICA_18);
}

static void fillQuad(float x, float y, float w, float h, float r, float g, float b, float a)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r, g, b, a);

	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void GhostChase::loadImages()
{
	bgImg = iLoadImage("Images//map_undercroft.png");

	stoneImg = iLoadImage("Images//obj_wardstone.png");
	stoneLitImg = iLoadImage("Images//obj_wardstone_lit.png");
	ringImg = iLoadImage("Images//fx_ward_ring.png");
	wardGlowImg = iLoadImage("Images//fx_ward_glow.png");

	driftImg[0] = iLoadImage("Images//ghost_drift_1.png");
	driftImg[1] = iLoadImage("Images//ghost_drift_2.png");
	driftImg[2] = iLoadImage("Images//ghost_drift_3.png");

	wardenImg[0] = iLoadImage("Images//ghost_warden_1.png");
	wardenImg[1] = iLoadImage("Images//ghost_warden_2.png");
	wardenImg[2] = iLoadImage("Images//ghost_warden_3.png");

	ghostGlowImg = iLoadImage("Images//fx_ghost_glow.png");
	darknessImg = iLoadImage("Images//fx_darkness.png");
	alcoveImg = iLoadImage("Images//fx_alcove_safe.png");

	doorImg = iLoadImage("Images//obj_undercroft_door.png");
	doorOpenImg = iLoadImage("Images//obj_undercroft_door_open.png");
	charmIconImg = iLoadImage("Images//ui_charm_icon.png");

	successImg = iLoadImage("Images//success.png");
	failedImg = iLoadImage("Images//failed.png");
	hoverSuccessImg = iLoadImage("Images//hover_success.png");
	hoverFailedImg = iLoadImage("Images//hover_failed.png");

	chaseMap.init("Images//map_undercroft.png", UW, UH, true);
	buildObstacles();
}

Map& GhostChase::getMap()
{
	return chaseMap;
}

void GhostChase::buildObstacles()
{
	chaseMap.clearObstacles();

	chaseMap.addObstacle(0, 0, 2560, 120);
	chaseMap.addObstacle(0, 1340, 2560, 100);
	chaseMap.addObstacle(440, 1320, 1960, 120);
	chaseMap.addObstacle(2400, 120, 160, 1320);
	chaseMap.addObstacle(660, 1300, 1500, 140);
	chaseMap.addObstacle(660, 1280, 1100, 160);
	chaseMap.addObstacle(700, 780, 240, 660);
	chaseMap.addObstacle(0, 320, 140, 1120);
	chaseMap.addObstacle(0, 800, 180, 640);
	chaseMap.addObstacle(0, 220, 80, 1220);
	chaseMap.addObstacle(1380, 720, 360, 260);
	chaseMap.addObstacle(2300, 120, 260, 360);
	chaseMap.addObstacle(2360, 480, 200, 400);
	chaseMap.addObstacle(0, 1060, 200, 380);
	chaseMap.addObstacle(2280, 600, 280, 240);
	chaseMap.addObstacle(1400, 420, 400, 160);
	chaseMap.addObstacle(360, 480, 500, 120);
	chaseMap.addObstacle(660, 1240, 300, 200);
	chaseMap.addObstacle(1400, 440, 420, 140);
	chaseMap.addObstacle(680, 780, 320, 180);
	chaseMap.addObstacle(1380, 400, 360, 160);
	chaseMap.addObstacle(700, 800, 320, 180);
	chaseMap.addObstacle(0, 1180, 220, 260);
	chaseMap.addObstacle(700, 860, 440, 120);
	chaseMap.addObstacle(0, 1240, 240, 200);
	chaseMap.addObstacle(700, 840, 340, 140);
	chaseMap.addObstacle(0, 1260, 260, 180);
	chaseMap.addObstacle(900, 120, 760, 60);
	chaseMap.addObstacle(0, 1320, 380, 120);
	chaseMap.addObstacle(0, 1280, 280, 160);
	chaseMap.addObstacle(0, 120, 1960, 20);
	chaseMap.addObstacle(660, 860, 480, 80);
	chaseMap.addObstacle(1540, 980, 180, 200);
	chaseMap.addObstacle(2260, 680, 300, 100);
	chaseMap.addObstacle(1360, 460, 480, 60);
	chaseMap.addObstacle(2280, 200, 280, 100);
	chaseMap.addObstacle(1380, 740, 460, 60);
	chaseMap.addObstacle(1360, 780, 440, 60);
	chaseMap.addObstacle(2240, 1280, 160, 160);
	chaseMap.addObstacle(2380, 1000, 180, 140);
	chaseMap.addObstacle(0, 120, 240, 100);
	chaseMap.addObstacle(360, 480, 600, 40);
	chaseMap.addObstacle(1540, 1040, 200, 120);
	chaseMap.addObstacle(1320, 340, 160, 140);
	chaseMap.addObstacle(700, 1100, 260, 80);
	chaseMap.addObstacle(1700, 680, 80, 240);
	chaseMap.addObstacle(2240, 140, 320, 60);
	chaseMap.addObstacle(2300, 1260, 100, 180);
	chaseMap.addObstacle(1300, 400, 440, 40);
	chaseMap.addObstacle(1680, 700, 140, 120);
	chaseMap.addObstacle(840, 140, 840, 20);
	chaseMap.addObstacle(0, 860, 200, 80);
	chaseMap.addObstacle(1700, 680, 100, 160);
	chaseMap.addObstacle(700, 980, 260, 60);
	chaseMap.addObstacle(540, 460, 100, 140);
	chaseMap.addObstacle(1500, 980, 220, 60);
	chaseMap.addObstacle(1520, 1120, 40, 320);
	chaseMap.addObstacle(0, 320, 160, 80);
	chaseMap.addObstacle(2020, 120, 540, 20);
	chaseMap.addObstacle(1520, 1260, 60, 180);
	chaseMap.addObstacle(360, 520, 520, 20);
	chaseMap.addObstacle(1340, 480, 500, 20);
	chaseMap.addObstacle(980, 180, 60, 160);
	chaseMap.addObstacle(1480, 980, 240, 40);
	chaseMap.addObstacle(1540, 1080, 220, 40);
	chaseMap.addObstacle(2340, 480, 220, 40);
	chaseMap.addObstacle(2180, 140, 380, 20);
	chaseMap.addObstacle(0, 140, 340, 20);
	chaseMap.addObstacle(2220, 160, 340, 20);
	chaseMap.addObstacle(2260, 200, 300, 20);
	chaseMap.addObstacle(1340, 840, 40, 140);
	chaseMap.addObstacle(1100, 840, 40, 140);
	chaseMap.addObstacle(0, 160, 280, 20);
	chaseMap.addObstacle(1880, 1200, 20, 240);
	chaseMap.addObstacle(680, 1200, 20, 240);
	chaseMap.addObstacle(2320, 840, 240, 20);
	chaseMap.addObstacle(2320, 480, 240, 20);
	chaseMap.addObstacle(1520, 1040, 220, 20);
	chaseMap.addObstacle(360, 600, 200, 20);
	chaseMap.addObstacle(940, 180, 100, 40);
	chaseMap.addObstacle(1300, 180, 180, 20);
	chaseMap.addObstacle(0, 360, 180, 20);
	chaseMap.addObstacle(1660, 580, 140, 20);
	chaseMap.addObstacle(920, 180, 140, 20);
	chaseMap.addObstacle(2220, 1300, 20, 140);
	chaseMap.addObstacle(1400, 580, 100, 20);
	chaseMap.addObstacle(1080, 1120, 20, 80);
	chaseMap.addObstacle(280, 1060, 20, 80);
	chaseMap.addObstacle(2060, 640, 20, 80);
	chaseMap.addObstacle(2280, 400, 20, 80);
	chaseMap.addObstacle(940, 300, 40, 40);
	chaseMap.addObstacle(340, 260, 20, 80);
	chaseMap.addObstacle(1500, 1160, 80, 20);
	chaseMap.addObstacle(960, 220, 80, 20);
	chaseMap.addObstacle(1880, 940, 20, 60);
	chaseMap.addObstacle(780, 260, 20, 60);
	chaseMap.addObstacle(1180, 260, 20, 60);
	chaseMap.addObstacle(1900, 260, 20, 60);
	chaseMap.addObstacle(2100, 260, 20, 60);
	chaseMap.addObstacle(1520, 1180, 60, 20);
	chaseMap.addObstacle(1340, 1120, 40, 20);
	chaseMap.addObstacle(280, 800, 20, 40);
	chaseMap.addObstacle(1060, 480, 20, 40);
	chaseMap.addObstacle(1340, 200, 40, 20);
	chaseMap.addObstacle(1120, 180, 40, 20);
	chaseMap.addObstacle(1860, 1180, 20, 20);
	chaseMap.addObstacle(2220, 1180, 20, 20);
	chaseMap.addObstacle(580, 1120, 20, 20);
	chaseMap.addObstacle(2220, 940, 20, 20);
	chaseMap.addObstacle(1060, 660, 20, 20);
	chaseMap.addObstacle(920, 320, 20, 20);
	chaseMap.addObstacle(640, 140, 20, 20);

	/*doorObstacleIdx = chaseMap.addObstacle(DOOR_X, DOOR_Y, DOOR_W, DOOR_H);
	chaseMap.setObstacleActive(doorObstacleIdx, true);*/
}

void GhostChase::resetGhosts()
{
	for (int i = 0; i < GC_DRIFTERS; i++)
	{
		ghosts[i].x = WP[i][0][0];
		ghosts[i].y = WP[i][0][1];
		ghosts[i].state = G_PATROL;
		ghosts[i].wp = 1;
		ghosts[i].alertTicks = 0;
		ghosts[i].lostTicks = 0;
		ghosts[i].frame = 0;
		ghosts[i].frameTimer = i * 5;
		ghosts[i].phasing = false;
		ghosts[i].stuckTicks = 0;
		ghosts[i].lastX = ghosts[i].x;
		ghosts[i].lastY = ghosts[i].y;
	}

	ghosts[GC_DRIFTERS].x = 2150;
	ghosts[GC_DRIFTERS].y = 1180;
	ghosts[GC_DRIFTERS].state = G_CHASE;
	ghosts[GC_DRIFTERS].wp = 0;
	ghosts[GC_DRIFTERS].alertTicks = 0;
	ghosts[GC_DRIFTERS].lostTicks = 0;
	ghosts[GC_DRIFTERS].frame = 0;
	ghosts[GC_DRIFTERS].frameTimer = 0;
	ghosts[GC_DRIFTERS].phasing = true;
	ghosts[GC_DRIFTERS].stuckTicks = 0;
	ghosts[GC_DRIFTERS].lastX = ghosts[GC_DRIFTERS].x;
	ghosts[GC_DRIFTERS].lastY = ghosts[GC_DRIFTERS].y;
}

void GhostChase::start(bool playerHasCharm, Player &player)
{
	phase = CHASE_PLAYING;
	timeLeftTicks = GC_TIME_TICKS;
	mistakes = 0;
	litCount = 0;
	resultTicks = 0;

	for (int i = 0; i < GC_STONES; i++)
	{
		stones[i].x = STONE_POS[i][0];
		stones[i].y = STONE_POS[i][1];
		stones[i].lit = false;
		stones[i].channelTicks = 0;
	}

	hasCharm = playerHasCharm;
	invisTicks = 0;
	charmCooldown = 0;

	graceTicks = 60;
	flashTicks = 0;
	finalRush = false;
	wardenSpeed = WARDEN_BASE;

	bottomText[0] = '\0';
	bottomTextTicks = 0;

	finished = false;
	retryRequested = false;

	buildObstacles();
	resetGhosts();

	player.init(SPAWN_X, SPAWN_Y);
	player.setFacing(DIR_BACK);
	lastPX = player.getX();
	lastPY = player.getY();
	playerMoved = false;

	chaseMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	showBottomText("Five wardstones. Stand on one and hold still.", 400);
}

void GhostChase::updateCamera(Player &player)
{
	chaseMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
}

bool GhostChase::rectsOverlap(Rect a, Rect b)
{
	return a.x < b.x + b.w && a.x + a.w > b.x &&
		a.y < b.y + b.h && a.y + a.h > b.y;
}

bool GhostChase::lineOfSight(double gx, double gy, double px, double py)
{
	double dx = px - gx;
	double dy = py - gy;
	double dist = sqrt(dx * dx + dy * dy);

	if (dist < 1.0) return true;

	int steps = (int)(dist / 12.0);
	for (int i = 1; i < steps; i++)
	{
		double t = (double)i / steps;
		Rect probe = { (int)(gx + dx * t), (int)(gy + dy * t), 4, 4 };
		if (chaseMap.isBlocked(probe)) return false;
	}

	return true;
}

bool GhostChase::playerHidden(Player &player)
{
	if (invisTicks > 0) return true;
	if (playerMoved) return false;

	Rect f = player.getFeetRect();
	for (int i = 0; i < GC_ALCOVES; i++)
	{
		Rect a = { ALCOVES[i][0], ALCOVES[i][1], ALCOVES[i][2], ALCOVES[i][3] };
		if (rectsOverlap(f, a)) return true;
	}

	return false;
}

void GhostChase::showBottomText(char* text, int ticks)
{
	strcpy_s(bottomText, text);
	bottomTextTicks = ticks;
}

void GhostChase::alarmNearestDrifter(int sx, int sy)
{
	int best = -1;
	double bestD = 0.0;

	for (int i = 0; i < GC_DRIFTERS; i++)
	{
		double dx = ghosts[i].x - sx;
		double dy = ghosts[i].y - sy;
		double d = dx * dx + dy * dy;

		if (best == -1 || d < bestD) { best = i; bestD = d; }
	}

	if (best != -1)
	{
		ghosts[best].state = G_CHASE;
		ghosts[best].lostTicks = 0;
	}
}

void GhostChase::lightStone(int i)
{
	stones[i].lit = true;
	stones[i].channelTicks = 0;
	litCount++;

	alarmNearestDrifter(stones[i].x, stones[i].y);
	wardenSpeed = WARDEN_BASE + WARDEN_PER_STONE * litCount;

	if (litCount >= GC_STONES)
	{
		finalRush = true;
		//chaseMap.setObstacleActive(doorObstacleIdx, false);

		for (int g = 0; g < GC_GHOSTS; g++)
		{
			ghosts[g].state = G_CHASE;
			ghosts[g].lostTicks = 0;
		}

		showBottomText("The way is open. RUN.", 600);
	}
	else
	{
		char msg[80];
		sprintf_s(msg, "%d of %d. %s", litCount, GC_STONES, FLAVOUR[litCount % 3]);
		showBottomText(msg, 300);
	}
}

void GhostChase::updateChannel(Player &player)
{
	Rect f = player.getFeetRect();

	for (int i = 0; i < GC_STONES; i++)
	{
		if (stones[i].lit) continue;

		Rect s = { stones[i].x, stones[i].y, STONE_W, STONE_H };

		if (!rectsOverlap(f, s) || playerMoved)
		{
			stones[i].channelTicks = 0;
			continue;
		}

		stones[i].channelTicks++;
		if (stones[i].channelTicks >= CHANNEL_TICKS) lightStone(i);
	}
}

void GhostChase::updateGhosts(Player &player)
{
	double px = player.getX() + PLAYER_WIDTH / 2.0;
	double py = player.getY() + PLAYER_HEIGHT / 2.0;

	bool hidden = playerHidden(player) || graceTicks > 0;

	for (int i = 0; i < GC_GHOSTS; i++)
	{
		Ghost &g = ghosts[i];

		g.frameTimer++;
		if (g.frameTimer >= WALK_FRAME_TICKS)
		{
			g.frameTimer = 0;
			g.frame = (g.frame + 1) % 3;
		}

		if (g.phasing)
		{
			if (litCount == 0 && !finalRush) continue;

			double dx = px - g.x, dy = py - g.y;
			double d = sqrt(dx * dx + dy * dy);

			if (hidden && !finalRush)
			{
				if (d > 1.0)
				{
					g.x -= dx / d * wardenSpeed * 1.5;
					g.y -= dy / d * wardenSpeed * 1.5;
				}

				if (g.x < 120) g.x = 120;
				if (g.x > UW - 120) g.x = UW - 120;
				if (g.y < 120) g.y = 120;
				if (g.y > UH - 120) g.y = UH - 120;

				continue;
			}

			if (d > 1.0)
			{
				g.x += dx / d * wardenSpeed;
				g.y += dy / d * wardenSpeed;
			}
			continue;
		}

		if (!finalRush)
		{
			double dx = px - g.x, dy = py - g.y;
			double d2 = dx * dx + dy * dy;
			bool sees = !hidden && d2 < (double)SIGHT_RANGE * SIGHT_RANGE &&
				lineOfSight(g.x, g.y, px, py);

			if (sees)
			{
				if (g.state == G_PATROL || g.state == G_RETURN)
				{
					g.state = G_ALERT;
					g.alertTicks = ALERT_TICKS;
				}
				else if (g.state == G_CHASE)
				{
					g.lostTicks = 0;
				}
			}
			else
			{
				if (g.state == G_ALERT) g.state = G_RETURN;
				else if (g.state == G_CHASE)
				{
					g.lostTicks++;
					if (g.lostTicks >= LOST_TICKS) g.state = G_RETURN;
				}
			}
		}

		if (g.state == G_ALERT)
		{
			g.alertTicks--;
			if (g.alertTicks <= 0)
			{
				g.state = G_CHASE;
				g.lostTicks = 0;
			}
			continue;
		}

		double tx, ty, speed;

		if (g.state == G_CHASE)
		{
			tx = px; ty = py; speed = DRIFT_CHASE;
		}
		else
		{
			tx = WP[i][g.wp][0];
			ty = WP[i][g.wp][1];
			speed = DRIFT_PATROL;
		}

		double dx = tx - g.x, dy = ty - g.y;
		double d = sqrt(dx * dx + dy * dy);

		if (d < 8.0)
		{
			if (g.state == G_RETURN) g.state = G_PATROL;
			else if (g.state == G_PATROL) g.wp = (g.wp + 1) % WP_COUNT[i];
			continue;
		}

		double sx = dx / d * speed;
		double sy = dy / d * speed;

		Rect probe = { (int)(g.x + sx) - GHOST_MOVE / 2, (int)g.y - GHOST_MOVE / 2, GHOST_MOVE, GHOST_MOVE };
		if (!chaseMap.isBlocked(probe)) g.x += sx;

		probe.x = (int)g.x - GHOST_MOVE / 2;
		probe.y = (int)(g.y + sy) - GHOST_MOVE / 2;
		if (!chaseMap.isBlocked(probe)) g.y += sy;

		if (fabs(g.x - g.lastX) + fabs(g.y - g.lastY) < 0.4)
		{
			g.stuckTicks++;
			if (g.stuckTicks > 100)
			{
				g.stuckTicks = 0;
				g.wp = (g.wp + 1) % WP_COUNT[i];
				g.state = G_PATROL;
			}
		}
		else
		{
			g.stuckTicks = 0;
		}

		g.lastX = g.x;
		g.lastY = g.y;
	}
}

void GhostChase::catchPlayer(Player &player)
{
	mistakes++;
	flashTicks = FLASH_TICKS;
	graceTicks = GRACE_TICKS;

	for (int i = 0; i < GC_STONES; i++) stones[i].channelTicks = 0;

	for (int i = 0; i < GC_DRIFTERS; i++)
	{
		ghosts[i].state = G_RETURN;
		ghosts[i].lostTicks = 0;
	}

	player.init(SPAWN_X, SPAWN_Y);
	player.setFacing(DIR_BACK);
	lastPX = player.getX();
	lastPY = player.getY();

	chaseMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	if (mistakes >= GC_MAX_MISTAKES) phase = CHASE_FAILED;
	else showBottomText("It found you. Back to the stairs.", 260);
}

void GhostChase::checkCaught(Player &player)
{
	if (graceTicks > 0 || invisTicks > 0) return;

	Rect f = player.getFeetRect();

	for (int i = 0; i < GC_GHOSTS; i++)
	{
		Rect g = { (int)ghosts[i].x - GHOST_HIT / 2, (int)ghosts[i].y - GHOST_HIT / 2,
			GHOST_HIT, GHOST_HIT };

		if (rectsOverlap(f, g))
		{
			catchPlayer(player);
			return;
		}
	}
}

void GhostChase::checkExit(Player &player)
{
	Rect f = player.getFeetRect();
	Rect door = { DOOR_X - 70, DOOR_Y - 70, DOOR_W + 140, DOOR_H + 140 };

	if (rectsOverlap(f, door)) phase = CHASE_SUCCESS;
}

void GhostChase::update(Player &player)
{
	if (phase != CHASE_PLAYING)
	{
		if (resultTicks < RESULT_FADE_TICKS) resultTicks++;
		return;
	}

	timeLeftTicks--;
	if (timeLeftTicks <= 0)
	{
		timeLeftTicks = 0;
		phase = CHASE_FAILED;
		return;
	}

	playerMoved = (player.getX() != lastPX || player.getY() != lastPY);
	lastPX = player.getX();
	lastPY = player.getY();

	if (graceTicks > 0) graceTicks--;
	if (flashTicks > 0) flashTicks--;
	if (invisTicks > 0) invisTicks--;
	if (charmCooldown > 0) charmCooldown--;
	if (bottomTextTicks > 0) bottomTextTicks--;

	updateChannel(player);
	updateGhosts(player);
	checkCaught(player);

	if (finalRush) checkExit(player);
}

void GhostChase::handleKey(unsigned char key)
{
	if (phase != CHASE_PLAYING) return;

	if (key == ' ' && hasCharm && charmCooldown <= 0)
	{
		invisTicks = INVIS_TICKS;
		charmCooldown = CHARM_COOLDOWN;

		// everything chasing loses the thread at once
		if (!finalRush)
		{
			for (int i = 0; i < GC_DRIFTERS; i++)
			{
				if (ghosts[i].state == G_CHASE || ghosts[i].state == G_ALERT)
					ghosts[i].state = G_RETURN;
			}
		}

		showBottomText("The charm answers. Three seconds.", 200);
	}
}

void GhostChase::handleClick(int mx, int my)
{
	if (phase == CHASE_PLAYING) return;

	if (mx >= RESULT_BUTTON_X && mx <= RESULT_BUTTON_X + RESULT_BUTTON_W &&
		my >= RESULT_BUTTON_Y && my <= RESULT_BUTTON_Y + RESULT_BUTTON_H)
	{
		if (phase == CHASE_SUCCESS) finished = true;
		else retryRequested = true;
	}
}

void GhostChase::drawWorld(Player &player)
{
	int camX = chaseMap.getCameraX();
	int camY = chaseMap.getCameraY();

	chaseMap.draw();

	// the exit door
	iShowImage(DOOR_X - camX, DOOR_Y - camY, DOOR_W, DOOR_H,
		finalRush ? doorOpenImg : doorImg);

	// wardstones
	for (int i = 0; i < GC_STONES; i++)
	{
		int sx = stones[i].x - camX;
		int sy = stones[i].y - camY;

		iShowImage(sx, sy, STONE_W, STONE_H, stoneImg);

		if (stones[i].lit)
		{
			iShowImage(sx, sy, STONE_W, STONE_H, stoneLitImg);
		}
		else if (stones[i].channelTicks > 0)
		{
			float t = (float)stones[i].channelTicks / (float)CHANNEL_TICKS;

			g_imgAlpha = t;
			iShowImage(sx, sy, STONE_W, STONE_H, stoneLitImg);

			g_imgAlpha = t;
			iShowImage(sx + STONE_W / 2 - RING_SIZE / 2, sy + STONE_H / 2 - RING_SIZE / 2,
				RING_SIZE, RING_SIZE, ringImg);
		}
	}

	player.draw(camX, camY);

	// ghosts
	for (int i = 0; i < GC_GHOSTS; i++)
	{
		int w = ghosts[i].phasing ? WARDEN_W : DRIFT_W;
		int h = ghosts[i].phasing ? WARDEN_H : DRIFT_H;
		int img = ghosts[i].phasing ? wardenImg[ghosts[i].frame] : driftImg[ghosts[i].frame];

		iShowImage((int)ghosts[i].x - camX - w / 2, (int)ghosts[i].y - camY - h / 2, w, h, img);
	}
}

void GhostChase::draw(Player &player, int mouseX, int mouseY)
{
	drawWorld(player);

	int camX = chaseMap.getCameraX();
	int camY = chaseMap.getCameraY();

	int lx = player.getX() - camX + PLAYER_WIDTH / 2;
	int ly = player.getY() - camY + PLAYER_HEIGHT / 2;
	iShowImage(lx - DARK_W / 2, ly - DARK_H / 2, DARK_W, DARK_H, darknessImg);

	for (int i = 0; i < GC_STONES; i++)
	{
		if (!stones[i].lit) continue;

		g_imgAdditive = true;
		g_imgAlpha = 0.85f;
		iShowImage(stones[i].x - camX + STONE_W / 2 - WARD_GLOW_SIZE / 2,
		stones[i].y - camY + STONE_H / 2 - WARD_GLOW_SIZE / 2,
		WARD_GLOW_SIZE, WARD_GLOW_SIZE, wardGlowImg);
	}

	if (finalRush)
	{
		g_imgAdditive = true;
		g_imgAlpha = 0.9f;
		iShowImage(DOOR_X - camX + DOOR_W / 2 - WARD_GLOW_SIZE / 2,
		DOOR_Y - camY + DOOR_H / 2 - WARD_GLOW_SIZE / 2,
		WARD_GLOW_SIZE, WARD_GLOW_SIZE, wardGlowImg);
	}

	double px = player.getX() + PLAYER_WIDTH / 2.0;
	double py = player.getY() + PLAYER_HEIGHT / 2.0;

	for (int i = 0; i < GC_GHOSTS; i++)
	{
		double dx = ghosts[i].x - px, dy = ghosts[i].y - py;
		double d = sqrt(dx * dx + dy * dy);
		if (d > GHOST_VISIBLE_RANGE) continue;

		float a = (float)(1.0 - d / GHOST_VISIBLE_RANGE);
		a = a * a * 0.85f;

		g_imgAdditive = true;
		g_imgAlpha = a;
		iShowImage((int)ghosts[i].x - camX - GHOST_GLOW_SIZE / 2,
			(int)ghosts[i].y - camY - GHOST_GLOW_SIZE / 2,
			GHOST_GLOW_SIZE, GHOST_GLOW_SIZE, ghostGlowImg);
	}

	for (int i = 0; i < GC_ALCOVES; i++)
	{
		g_imgAdditive = true;
		g_imgAlpha = 0.20f;
		iShowImage(ALCOVES[i][0] + ALCOVES[i][2] / 2 - camX - 70,
			ALCOVES[i][1] + ALCOVES[i][3] / 2 - camY - 70,
			140, 140, alcoveImg);
	}

	if (invisTicks > 0 || (!playerMoved && playerHidden(player)))
	{
		g_imgAlpha = 0.8f;
		iShowImage(lx - 70, ly - 70, 140, 140, alcoveImg);
	}

	if (flashTicks > 0)
	{
		float a = (float)flashTicks / FLASH_TICKS;
		fillQuad(0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, a * 0.8f);
	}

	drawHud(mouseX, mouseY);
}

void GhostChase::drawHud(int mouseX, int mouseY)
{
	char buf[60];
	int secondsLeft = timeLeftTicks / 100;

	iSetColor(255, 255, 255);

	sprintf_s(buf, "Lights: %d / %d", litCount, GC_STONES);
	iText(HUD_LIGHTS_X, HUD_LIGHTS_Y, buf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(buf, "%02d:%02d", secondsLeft / 60, secondsLeft % 60);
	iText(HUD_TIME_X, HUD_TIME_Y, buf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(buf, "Mistakes: %d/%d", mistakes, GC_MAX_MISTAKES);
	iText(HUD_MISS_X, HUD_MISS_Y, buf, GLUT_BITMAP_HELVETICA_18);

	// charm icon + cooldown bar
	if (hasCharm)
	{
		g_imgAlpha = (charmCooldown > 0) ? 0.4f : 1.0f;
		iShowImage(HUD_CHARM_X, HUD_CHARM_Y, HUD_CHARM_SIZE, HUD_CHARM_SIZE, charmIconImg);

		float ready = (charmCooldown > 0)
			? 1.0f - (float)charmCooldown / CHARM_COOLDOWN
			: 1.0f;

		fillQuad((float)HUD_CHARM_X, (float)(HUD_CHARM_Y - 12), (float)HUD_CHARM_SIZE, 6.0f,
			0.2f, 0.2f, 0.3f, 0.9f);
		fillQuad((float)HUD_CHARM_X, (float)(HUD_CHARM_Y - 12), HUD_CHARM_SIZE * ready, 6.0f,
			0.62f, 0.56f, 0.91f, 1.0f);

		if (invisTicks > 0)
		{
			iSetColor(200, 190, 255);
			iText(HUD_CHARM_X + HUD_CHARM_SIZE + 12, HUD_CHARM_Y + 22, "HIDDEN",
				GLUT_BITMAP_HELVETICA_18);
		}
		else if (charmCooldown <= 0)
		{
			iSetColor(170, 180, 210);
			iText(HUD_CHARM_X + HUD_CHARM_SIZE + 12, HUD_CHARM_Y + 22, "SPACE",
				GLUT_BITMAP_HELVETICA_18);
		}
	}

	if (bottomTextTicks > 0) drawPlate(bottomText);

	if (phase != CHASE_PLAYING)
	{
		bool hovering =
			mouseX >= RESULT_BUTTON_X && mouseX <= RESULT_BUTTON_X + RESULT_BUTTON_W &&
			mouseY >= RESULT_BUTTON_Y && mouseY <= RESULT_BUTTON_Y + RESULT_BUTTON_H;

		float a = (float)resultTicks / (float)RESULT_FADE_TICKS;
		if (a > 1.0f) a = 1.0f;

		int img = (phase == CHASE_SUCCESS)
			? (hovering ? hoverSuccessImg : successImg)
			: (hovering ? hoverFailedImg : failedImg);

		g_imgAlpha = a;
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, img);
	}
}

bool GhostChase::isFinished()
{
	if (!finished) return false;
	finished = false;
	return true;
}

bool GhostChase::isRetryRequested()
{
	if (!retryRequested) return false;
	retryRequested = false;
	return true;
}