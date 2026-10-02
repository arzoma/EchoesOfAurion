#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "FinalFight.hpp"
#include "Constants.hpp"

#include "glut.h"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

extern float g_imgAlpha;
extern bool  g_imgAdditive;

int getTextWidth(const char* text);

static const int CX = 640, CY = 360; // center of the arena where the evil deity aka hollow is

static const int SEAL_W = 88, SEAL_H = 88;
static const int SEAL_POS[FF_SEALS][2] =
{
	{ 596, 544 },   // N
	{ 322, 389 },   // NW
	{ 434, 137 },   // SW
	{ 756, 137 },   // SE
	{ 871, 389 }    // NE
};

static const int CHANNEL_TICKS = 250;
static const int RING_SIZE = 150;

static const int SPAWN_X = 596, SPAWN_Y = 100;

static const int HOLLOW_SIZE = 420;
static const int HOLLOW_FRAME_TICKS = 40;

static const int INVULN_TICKS = 80;
static const int KNOCKBACK = 60;
static const int FLASH_TICKS = 26;

static const int SWEEP_TELE = 80, SWEEP_ACT = 60;
static const int RAIN_TELE = 70, RAIN_ACT = 25;
static const int LANES_TELE = 70, LANES_ACT = 40;

static const int GAP_PHASE[3] = { 320, 260, 200 };

static const double SWEEP_ARC = 120.0;
static const int SWEEP_INNER = 175;
static const int RAIN_R = 70;

static const int LANE[FF_LANES][4] =
{
	{ 0, 0, 1280, 120 },
	{ 0, 300, 1280, 120 },
	{ 0, 600, 1280, 120 },
	{ 0, 0, 160, 720 },
	{ 560, 0, 160, 720 },
	{ 1120, 0, 160, 720 }
};

// the random monsters
static const int MON_W = 80, MON_H = 96;
static const int MON_HIT = 54;
static const double MON_SPEED = 1.8;
static const int MON_SPAWN_GAP = 420;
static const int MON_FRAME_TICKS = 12;
static const int MAX_MON_PHASE[3] = { 0, 2, 3 };

// guardian
static const int GUARD_X = 130, GUARD_Y = 320;
static const int GUARD_RADIUS = 140;

// starwheel
static const int HOLD_TICKS = 250;
static const int HOLD_COOLDOWN = 2000;

static const int RESOLVE_X = 540, RESOLVE_Y = 668, RESOLVE_SIZE = 40, RESOLVE_GAP = 46;
static const int SEALS_TEXT_X = 30, SEALS_TEXT_Y = 678;
static const int WHEEL_X = 30, WHEEL_Y = 30, WHEEL_SIZE = 64;

static const int RESULT_BUTTON_X = 488, RESULT_BUTTON_Y = 238;
static const int RESULT_BUTTON_W = 300, RESULT_BUTTON_H = 60;
static const int RESULT_FADE_TICKS = 40;

static void fillQuad(double x, double y, double w, double h, float r, float g, float b, float a)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r, g, b, a);

	glBegin(GL_QUADS);
	glVertex2d(x, y);
	glVertex2d(x + w, y);
	glVertex2d(x + w, y + h);
	glVertex2d(x, y + h);
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

static void fillWedge(double cx, double cy, double r0, double r1, double a0, double a1, float r, float g, float b, float a)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r, g, b, a);

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 40; i++)
	{
		double t = a0 + (a1 - a0) * i / 40.0;
		double rad = t * 3.14159265 / 180.0;
		glVertex2d(cx + cos(rad) * r0, cy + sin(rad) * r0);
		glVertex2d(cx + cos(rad) * r1, cy + sin(rad) * r1);
	}
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

static void ringOutline(double cx, double cy, double r, float rr, float gg, float bb, float a, float width)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(rr, gg, bb, a);
	glLineWidth(width);

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 48; i++)
	{
		double t = i * 2.0 * 3.14159265 / 48.0;
		glVertex2d(cx + cos(t) * r, cy + sin(t) * r);
	}
	glEnd();

	glLineWidth(1.0f);
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

static void drawPlate(char* s)
{
	int w = getTextWidth(s);
	int x = SCREEN_WIDTH / 2 - w / 2;

	fillQuad(x - 20, 44, w + 40, 38, 0.0f, 0.0f, 0.0f, 0.62f);

	iSetColor(255, 255, 255);
	iText(x, 56, s, GLUT_BITMAP_HELVETICA_18);
}

static double norm360(double a)
{
	while (a < 0.0) a += 360.0;
	while (a >= 360.0) a -= 360.0;
	return a;
}

void FinalFight::loadImages()
{
	bgImg = iLoadImage("Images//map_astral_arena.png");

	sealImg = iLoadImage("Images//obj_seal.png");
	sealLitImg = iLoadImage("Images//obj_seal_lit.png");

	hollowImg[0] = iLoadImage("Images//hollow_1.png");
	hollowImg[1] = iLoadImage("Images//hollow_2.png");
	hollowImg[2] = iLoadImage("Images//hollow_3.png");

	monsterImg[0] = iLoadImage("Images//monster_1.png");
	monsterImg[1] = iLoadImage("Images//monster_2.png");
	monsterImg[2] = iLoadImage("Images//monster_3.png");

	guardianImg = iLoadImage("Images//idle_guardian.png");

	resolveImg = iLoadImage("Images//ui_resolve.png");
	resolveSpentImg = iLoadImage("Images//ui_resolve_spent.png");
	charmIconImg = iLoadImage("Images//starwheel.png");

	successImg = iLoadImage("Images//success.png");
	failedImg = iLoadImage("Images//failed.png");
	hoverSuccessImg = iLoadImage("Images//hover_success.png");
	hoverFailedImg = iLoadImage("Images//hover_failed.png");
}

Map& FinalFight::getMap()
{
	static Map arenaMap;
	static bool built = false;

	if (!built)
	{
		built = true;
		arenaMap.init("Images//map_astral_arena.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

		arenaMap.addObstacle(0, 640, 1280, 80);
		arenaMap.addObstacle(0, 0, 360, 160);
		arenaMap.addObstacle(920, 0, 360, 160);
		arenaMap.addObstacle(480, 280, 280, 160);
		arenaMap.addObstacle(0, 160, 80, 560);
		arenaMap.addObstacle(1200, 160, 80, 560);
		arenaMap.addObstacle(0, 0, 1280, 40);
		arenaMap.addObstacle(0, 520, 200, 200);
		arenaMap.addObstacle(1080, 520, 120, 200);
		arenaMap.addObstacle(1080, 160, 200, 120);
		arenaMap.addObstacle(560, 240, 160, 240);
		arenaMap.addObstacle(520, 40, 240, 40);
		arenaMap.addObstacle(0, 600, 400, 120);
		arenaMap.addObstacle(880, 600, 200, 120);
		arenaMap.addObstacle(0, 160, 120, 200);
		arenaMap.addObstacle(0, 400, 120, 320);
		arenaMap.addObstacle(1160, 400, 40, 320);
		arenaMap.addObstacle(800, 40, 480, 40);
		arenaMap.addObstacle(480, 320, 320, 80);
		arenaMap.addObstacle(1160, 280, 120, 80);
		arenaMap.addObstacle(0, 160, 160, 80);
		arenaMap.addObstacle(0, 40, 400, 80);
		arenaMap.addObstacle(0, 560, 240, 160);
		arenaMap.addObstacle(1040, 560, 40, 160);
		arenaMap.addObstacle(0, 480, 160, 240);
		arenaMap.addObstacle(1120, 480, 40, 240);
		arenaMap.addObstacle(0, 160, 200, 40);
		arenaMap.addObstacle(880, 80, 400, 40);
		arenaMap.addObstacle(0, 40, 440, 40);
	}

	return arenaMap;
}

void FinalFight::start(bool playerHasWheel, Player &player)
{
	phase = FIGHT_PLAYING;
	resultTicks = 0;

	for (int i = 0; i < FF_SEALS; i++)
	{
		seals[i].x = SEAL_POS[i][0];
		seals[i].y = SEAL_POS[i][1];
		seals[i].lit = false;
		seals[i].channelTicks = 0;
	}
	litCount = 0;

	resolve = FF_RESOLVE;
	invulnTicks = 120;
	flashTicks = 0;

	atk = ATK_NONE;
	atkStage = 0;
	atkTicks = 0;
	atkGap = 260;

	for (int i = 0; i < FF_MONSTERS; i++) monsters[i].alive = false;
	monsterTimer = MON_SPAWN_GAP;

	monHintShown = false;

	hasWheel = playerHasWheel;
	holdTicks = 0;
	holdCooldown = 0;

	hollowFrame = 0;
	hollowTimer = 0;

	bottomText[0] = '\0';
	bottomTextTicks = 0;

	finished = false;
	retryRequested = false;

	player.init(SPAWN_X, SPAWN_Y);
	player.setFacing(DIR_BACK);
	lastPX = player.getX();
	lastPY = player.getY();
	playerMoved = false;

	showBottomText("Five seals. Stand on one and hold still.", 400);
}

bool FinalFight::rectsOverlap(Rect a, Rect b)
{
	return a.x < b.x + b.w && a.x + a.w > b.x &&
		a.y < b.y + b.h && a.y + a.h > b.y;
}

int FinalFight::phaseIndex()
{
	if (litCount >= 4) return 2;
	if (litCount >= 2) return 1;
	return 0;
}

void FinalFight::showBottomText(char* text, int ticks)
{
	strcpy_s(bottomText, text);
	bottomTextTicks = ticks;
}

void FinalFight::updateChannel(Player &player)
{
	Rect f = player.getFeetRect();

	for (int i = 0; i < FF_SEALS; i++)
	{
		if (seals[i].lit) continue;

		Rect s = { seals[i].x, seals[i].y, SEAL_W, SEAL_H };

		if (!rectsOverlap(f, s) || playerMoved)
		{
			seals[i].channelTicks = 0;
			continue;
		}

		seals[i].channelTicks++;

		if (seals[i].channelTicks >= CHANNEL_TICKS)
		{
			seals[i].lit = true;
			seals[i].channelTicks = 0;
			litCount++;

			if (litCount >= FF_SEALS)
			{
				phase = FIGHT_WIN;
				return;
			}

			atkGap = 40;

			if (litCount == 2) showBottomText("It is paying attention now.", 260);
			else if (litCount == 4) showBottomText("One left. Do not stop moving.", 300);
			else
			{
				char msg[60];
				sprintf_s(msg, "%d of %d.", litCount, FF_SEALS);
				showBottomText(msg, 220);
			}
		}
	}
}

void FinalFight::startAttack()
{
	int p = phaseIndex();

	// phase 0 = sweep only,  phase 1 = sweep or rain,  phase 2 = all.
	int roll = rand() % (p == 0 ? 1 : (p == 1 ? 2 : 3));

	if (roll == 0)
	{
		atk = ATK_SWEEP;
		atkTicks = SWEEP_TELE;
		sweepStart = (double)(rand() % 360);
	}
	else if (roll == 1)
	{
		atk = ATK_RAIN;
		atkTicks = RAIN_TELE;

		for (int i = 0; i < FF_RAIN; i++)
		{
			rainX[i] = 140 + rand() % 1000;
			rainY[i] = 90 + rand() % 540;
		}
	}
	else
	{
		atk = ATK_LANES;
		atkTicks = LANES_TELE;

		for (int i = 0; i < FF_LANES; i++) laneOn[i] = false;

		int picked = 0;
		while (picked < 3)
		{
			int k = rand() % FF_LANES;
			if (!laneOn[k]) { laneOn[k] = true; picked++; }
		}
	}

	atkStage = 0;
}

void FinalFight::updateAttack(Player &player)
{
	if (atk == ATK_NONE)
	{
		atkGap--;
		if (atkGap <= 0) startAttack();
		return;
	}

	atkTicks--;

	if (atkTicks > 0)
	{
		if (atkStage != 1 || invulnTicks > 0) return;

		double px = player.getX() + PLAYER_WIDTH / 2.0;
		double py = player.getY() + PLAYER_HEIGHT / 2.0;
		Rect f = player.getFeetRect();

		if (atk == ATK_SWEEP)
		{
			double dx = px - CX, dy = py - CY;
			double dist = sqrt(dx * dx + dy * dy);

			if (dist > SWEEP_INNER)
			{
				double ang = norm360(atan2(dy, dx) * 180.0 / 3.14159265);
				double rel = norm360(ang - sweepStart);
				if (rel <= SWEEP_ARC) hitPlayer(player, CX, CY);
			}
		}
		else if (atk == ATK_RAIN)
		{
			for (int i = 0; i < FF_RAIN; i++)
			{
				double dx = px - rainX[i], dy = py - rainY[i];
				if (dx * dx + dy * dy < (double)RAIN_R * RAIN_R)
				{
					hitPlayer(player, rainX[i], rainY[i]);
					break;
				}
			}
		}
		else if (atk == ATK_LANES)
		{
			for (int i = 0; i < FF_LANES; i++)
			{
				if (!laneOn[i]) continue;

				Rect l = { LANE[i][0], LANE[i][1], LANE[i][2], LANE[i][3] };
				if (rectsOverlap(f, l))
				{
					hitPlayer(player, px, py - 200);
					break;
				}
			}
		}

		return;
	}

	if (atkStage == 0)
	{
		atkStage = 1;
		atkTicks = (atk == ATK_SWEEP) ? SWEEP_ACT
			: (atk == ATK_RAIN) ? RAIN_ACT : LANES_ACT;
	}
	else
	{
		atk = ATK_NONE;
		atkGap = GAP_PHASE[phaseIndex()];
	}
}

void FinalFight::hitPlayer(Player &player, double fromX, double fromY)
{
	resolve--;
	invulnTicks = INVULN_TICKS;
	flashTicks = FLASH_TICKS;

	for (int i = 0; i < FF_SEALS; i++) seals[i].channelTicks = 0;

	double dx = player.getX() + PLAYER_WIDTH / 2.0 - fromX;
	double dy = player.getY() + PLAYER_HEIGHT / 2.0 - fromY;
	double d = sqrt(dx * dx + dy * dy);
	if (d < 1.0) { dx = 0; dy = 1; d = 1; }

	dx /= d;
	dy /= d;

	Rect f = player.getFeetRect();
	int offX = f.x - player.getX();
	int offY = f.y - player.getY();

	int nx = player.getX();
	int ny = player.getY();

	for (int step = 0; step < KNOCKBACK; step++)
	{
		int tx = player.getX() + (int)(dx * (step + 1));
		int ty = player.getY() + (int)(dy * (step + 1));

		if (tx < 10) break;
		if (tx > SCREEN_WIDTH - PLAYER_WIDTH - 10) break;
		if (ty < 10) break;
		if (ty > SCREEN_HEIGHT - PLAYER_HEIGHT - 10) break;

		Rect probe = { tx + offX, ty + offY, f.w, f.h };
		if (getMap().isBlocked(probe)) break;

		nx = tx;
		ny = ty;
	}

	player.init(nx, ny);
	lastPX = nx;
	lastPY = ny;

	if (resolve <= 0) phase = FIGHT_LOST;
}

void FinalFight::updateMonsters(Player &player)
{
	int maxMon = MAX_MON_PHASE[phaseIndex()];

	int alive = 0;
	for (int i = 0; i < FF_MONSTERS; i++) if (monsters[i].alive) alive++;

	monsterTimer--;
	if (monsterTimer <= 0 && alive < maxMon)
	{
		for (int i = 0; i < FF_MONSTERS; i++)
		{
			if (monsters[i].alive) continue;

			int edge = rand() % 4;
			if (edge == 0)      { monsters[i].x = 60;   monsters[i].y = 100 + rand() % 520; }
			else if (edge == 1) { monsters[i].x = 1220; monsters[i].y = 100 + rand() % 520; }
			else if (edge == 2) { monsters[i].x = 200 + rand() % 880; monsters[i].y = 50; }
			else                { monsters[i].x = 200 + rand() % 880; monsters[i].y = 670; }

			monsters[i].alive = true;
			if (!monHintShown)
			{
				monHintShown = true;
				showBottomText("Lead them to the Guardian. He can still swat them.", 420);
			}
			monsters[i].frame = 0;
			monsters[i].frameTimer = 0;
			break;
		}

		monsterTimer = MON_SPAWN_GAP;
	}

	double px = player.getX() + PLAYER_WIDTH / 2.0;
	double py = player.getY() + PLAYER_HEIGHT / 2.0;

	for (int i = 0; i < FF_MONSTERS; i++)
	{
		if (!monsters[i].alive) continue;

		monsters[i].frameTimer++;
		if (monsters[i].frameTimer >= MON_FRAME_TICKS)
		{
			monsters[i].frameTimer = 0;
			monsters[i].frame = (monsters[i].frame + 1) % 3;
		}

		double dx = px - monsters[i].x, dy = py - monsters[i].y;
		double d = sqrt(dx * dx + dy * dy);
		if (d > 1.0)
		{
			monsters[i].x += dx / d * MON_SPEED;
			monsters[i].y += dy / d * MON_SPEED;
		}

		// the guardian kills anything small that comes near him
		double gx = monsters[i].x - (GUARD_X + PLAYER_WIDTH / 2.0);
		double gy = monsters[i].y - (GUARD_Y + PLAYER_HEIGHT / 2.0);

		if (gx * gx + gy * gy < (double)GUARD_RADIUS * GUARD_RADIUS)
		{
			monsters[i].alive = false;
			continue;
		}

		if (invulnTicks > 0) continue;

		Rect f = player.getFeetRect();
		Rect m = { (int)monsters[i].x - MON_HIT / 2, (int)monsters[i].y - MON_HIT / 2,
			MON_HIT, MON_HIT };

		if (rectsOverlap(f, m))
		{
			hitPlayer(player, monsters[i].x, monsters[i].y);
			monsters[i].alive = false;
		}
	}
}

void FinalFight::update(Player &player)
{
	if (phase != FIGHT_PLAYING)
	{
		if (resultTicks < RESULT_FADE_TICKS) resultTicks++;
		return;
	}

	playerMoved = (player.getX() != lastPX || player.getY() != lastPY);
	lastPX = player.getX();
	lastPY = player.getY();

	Rect fr = player.getFeetRect();
	if (getMap().isBlocked(fr))
	{
		player.init(SPAWN_X, SPAWN_Y);
		lastPX = SPAWN_X;
		lastPY = SPAWN_Y;
	}

	if (invulnTicks > 0) invulnTicks--;
	if (flashTicks > 0) flashTicks--;
	if (holdCooldown > 0) holdCooldown--;
	if (bottomTextTicks > 0) bottomTextTicks--;

	hollowTimer++;
	if (hollowTimer >= HOLLOW_FRAME_TICKS)
	{
		hollowTimer = 0;
		hollowFrame = (hollowFrame + 1) % 3;
	}

	updateChannel(player);
	if (phase != FIGHT_PLAYING) return;

	if (holdTicks > 0)
	{
		holdTicks--;
		return;
	}

	updateAttack(player);
	if (phase != FIGHT_PLAYING) return;

	updateMonsters(player);
}

void FinalFight::handleKey(unsigned char key)
{
	if (phase != FIGHT_PLAYING) return;

	if (key == ' ' && hasWheel && holdCooldown <= 0 && holdTicks <= 0)
	{
		holdTicks = HOLD_TICKS;
		holdCooldown = HOLD_COOLDOWN;
		showBottomText("The wheel holds the moment.", 200);
	}
}

void FinalFight::handleClick(int mx, int my)
{
	if (phase == FIGHT_PLAYING) return;

	if (mx >= RESULT_BUTTON_X && mx <= RESULT_BUTTON_X + RESULT_BUTTON_W &&
		my >= RESULT_BUTTON_Y && my <= RESULT_BUTTON_Y + RESULT_BUTTON_H)
	{
		if (phase == FIGHT_WIN) finished = true;
		else retryRequested = true;
	}
}

void FinalFight::drawTelegraphs()
{
	if (atk == ATK_NONE) return;

	bool tele = (atkStage == 0);

	if (atk == ATK_SWEEP)
	{
		float a = tele ? 0.20f : 0.55f;
		fillWedge(CX, CY, SWEEP_INNER, 900.0,
			sweepStart, sweepStart + SWEEP_ARC,
			0.95f, 0.30f, 0.26f, a);
	}
	else if (atk == ATK_RAIN)
	{
		for (int i = 0; i < FF_RAIN; i++)
		{
			if (tele)
			{
				ringOutline(rainX[i], rainY[i], RAIN_R, 0.95f, 0.35f, 0.28f, 0.85f, 3.0f);
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(1.0f, 0.42f, 0.30f, 0.65f);

				glBegin(GL_TRIANGLE_FAN);
				glVertex2d(rainX[i], rainY[i]);
				for (int k = 0; k <= 24; k++)
				{
					double t = k * 2.0 * 3.14159265 / 24.0;
					glVertex2d(rainX[i] + cos(t) * RAIN_R, rainY[i] + sin(t) * RAIN_R);
				}
				glEnd();

				glDisable(GL_BLEND);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
	}
	else if (atk == ATK_LANES)
	{
		for (int i = 0; i < FF_LANES; i++)
		{
			if (!laneOn[i]) continue;

			float a = tele ? 0.18f : 0.52f;
			fillQuad(LANE[i][0], LANE[i][1], LANE[i][2], LANE[i][3],
				0.95f, 0.32f, 0.26f, a);
		}
	}
}

void FinalFight::draw(Player &player, int mouseX, int mouseY)
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImg);

	iShowImage(CX - HOLLOW_SIZE / 2, CY - HOLLOW_SIZE / 2, HOLLOW_SIZE, HOLLOW_SIZE, hollowImg[hollowFrame]);

	// seals
	for (int i = 0; i < FF_SEALS; i++)
	{
		iShowImage(seals[i].x, seals[i].y, SEAL_W, SEAL_H, sealImg);

		if (seals[i].lit)
		{
			iShowImage(seals[i].x, seals[i].y, SEAL_W, SEAL_H, sealLitImg);
		}
		else if (seals[i].channelTicks > 0)
		{
			float t = (float)seals[i].channelTicks / (float)CHANNEL_TICKS;

			g_imgAlpha = t;
			iShowImage(seals[i].x, seals[i].y, SEAL_W, SEAL_H, sealLitImg);

			ringOutline(seals[i].x + SEAL_W / 2.0, seals[i].y + SEAL_H / 2.0, RING_SIZE / 2.0 * (0.6 + 0.4 * t),
				0.95f, 0.78f, 0.42f, 0.35f + 0.6f * t, 4.0f);
		}
	}

	drawTelegraphs();

	iShowImage(GUARD_X, GUARD_Y, PLAYER_WIDTH, PLAYER_HEIGHT, guardianImg);
	ringOutline(GUARD_X + PLAYER_WIDTH / 2.0, GUARD_Y + PLAYER_HEIGHT / 2.0, GUARD_RADIUS, 0.37f, 0.78f, 0.59f, 0.30f, 2.0f);

	// monsters
	for (int i = 0; i < FF_MONSTERS; i++)
	{
		if (!monsters[i].alive) continue;

		iShowImage((int)monsters[i].x - MON_W / 2, (int)monsters[i].y - MON_H / 2,
			MON_W, MON_H, monsterImg[monsters[i].frame]);
	}

	if (invulnTicks <= 0 || (invulnTicks / 6) % 2 == 0)
	{
		player.draw(0, 0);
	}

	int best = -1;
	double bestD = 0.0;
	double px = player.getX() + PLAYER_WIDTH / 2.0;
	double py = player.getY() + PLAYER_HEIGHT / 2.0;

	for (int i = 0; i < FF_SEALS; i++)
	{
		if (seals[i].lit) continue;

		double dx = seals[i].x + SEAL_W / 2.0 - px;
		double dy = seals[i].y + SEAL_H / 2.0 - py;
		double d = dx * dx + dy * dy;

		if (best == -1 || d < bestD) { best = i; bestD = d; }
	}

	if (best != -1)
	{
		double dx = seals[best].x + SEAL_W / 2.0 - px;
		double dy = seals[best].y + SEAL_H / 2.0 - py;
		double d = sqrt(dx * dx + dy * dy);

		if (d > 1.0)
		{
			double ax = px + dx / d * 54.0;
			double ay = py + dy / d * 54.0 - 40.0;
			fillQuad(ax - 5, ay - 5, 10, 10, 0.72f, 0.66f, 1.0f, 0.75f);
		}
	}

	if (holdTicks > 0)
	{
		fillQuad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.45f, 0.55f, 1.0f, 0.16f);
	}

	if (flashTicks > 0)
	{
		float a = (float)flashTicks / FLASH_TICKS;
		fillQuad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 0.25f, 0.22f, a * 0.55f);
	}

	drawHud(mouseX, mouseY);
}

void FinalFight::drawHud(int mouseX, int mouseY)
{
	char buf[60];

	iSetColor(255, 255, 255);
	sprintf_s(buf, "Seals: %d / %d", litCount, FF_SEALS);
	iText(SEALS_TEXT_X, SEALS_TEXT_Y, buf, GLUT_BITMAP_HELVETICA_18);

	for (int i = 0; i < FF_RESOLVE; i++)
	{
		iShowImage(RESOLVE_X + i * RESOLVE_GAP, RESOLVE_Y, RESOLVE_SIZE, RESOLVE_SIZE,
			(i < resolve) ? resolveImg : resolveSpentImg);
	}

	if (hasWheel)
	{
		g_imgAlpha = (holdCooldown > 0) ? 0.4f : 1.0f;
		iShowImage(WHEEL_X, WHEEL_Y, WHEEL_SIZE, WHEEL_SIZE, charmIconImg);

		float ready = (holdCooldown > 0) ? 1.0f - (float)holdCooldown / HOLD_COOLDOWN : 1.0f;

		fillQuad(WHEEL_X, WHEEL_Y - 12, WHEEL_SIZE, 6, 0.2f, 0.2f, 0.3f, 0.9f);
		fillQuad(WHEEL_X, WHEEL_Y - 12, WHEEL_SIZE * ready, 6, 0.62f, 0.56f, 0.91f, 1.0f);

		if (holdTicks > 0)
		{
			iSetColor(200, 210, 255);
			iText(WHEEL_X + WHEEL_SIZE + 12, WHEEL_Y + 22, "HELD", GLUT_BITMAP_HELVETICA_18);
		}
		else if (holdCooldown <= 0)
		{
			iSetColor(170, 180, 210);
			iText(WHEEL_X + WHEEL_SIZE + 12, WHEEL_Y + 22, "SPACE", GLUT_BITMAP_HELVETICA_18);
		}
	}

	if (bottomTextTicks > 0) drawPlate(bottomText);

	if (phase != FIGHT_PLAYING)
	{
		bool hovering =
			mouseX >= RESULT_BUTTON_X && mouseX <= RESULT_BUTTON_X + RESULT_BUTTON_W &&
			mouseY >= RESULT_BUTTON_Y && mouseY <= RESULT_BUTTON_Y + RESULT_BUTTON_H;

		float a = (float)resultTicks / (float)RESULT_FADE_TICKS;
		if (a > 1.0f) a = 1.0f;

		int img = (phase == FIGHT_WIN)
			? (hovering ? hoverSuccessImg : successImg)
			: (hovering ? hoverFailedImg : failedImg);

		g_imgAlpha = a;
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, img);
	}
}

bool FinalFight::isFinished()
{
	if (!finished) return false;
	finished = false;
	return true;
}

bool FinalFight::isRetryRequested()
{
	if (!retryRequested) return false;
	retryRequested = false;
	return true;
}