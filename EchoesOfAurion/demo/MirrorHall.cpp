#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "MirrorHall.hpp"
#include "Constants.hpp"

#include "glut.h"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

extern float g_imgAlpha;

int getTextWidth(const char* text);

static const int SEAM_X = 640;
static const int SEAM_W = 40;
static const int GOAL_SIZE = 84;
static const int PIT_SIZE = 96;

static const int CLEAR_HOLD = 50;
static const int FLASH_TICKS = 26;
static const int STARE_AT = 400;
static const int STARE_FOR = 30;

static const int REVEAL_TICKS = 200;
static const int CHARM_COOLDOWN = 1000;

static const int RESULT_BUTTON_X = 488, RESULT_BUTTON_Y = 238;
static const int RESULT_BUTTON_W = 300, RESULT_BUTTON_H = 60;
static const int RESULT_FADE_TICKS = 40;

static const int HUD_ROOM_X = 30, HUD_ROOM_Y = 678;
static const int HUD_TIME_X = 1090, HUD_TIME_Y = 678;
static const int HUD_MISS_X = 1090, HUD_MISS_Y = 648;
static const int HUD_CHARM_X = 30, HUD_CHARM_Y = 30, HUD_CHARM_SIZE = 64;

static MirrorRoom ROOMS[MH_ROOMS] =
{
	{
		60, 60, 1130, 60,
		5,
		{
			{ 0, 0, 20, 720 },
			{ 1260, 0, 20, 720 },
			{ 0, 0, 1280, 20 },
			{ 0, 660, 1280, 60 },
			{ 620, 0, 40, 720 }
		},
			0, { { 0, 0, 0, 0 } },
			470, 520, 726, 520,
			0,
			"He moves the other way. Get both of you onto the sigils."
	},

	{
		60, 60, 1130, 60,
		11,
		{
			{ 0, 0, 20, 720 }, { 1260, 0, 20, 720 },
			{ 0, 0, 1280, 20 }, { 0, 660, 1280, 60 }, { 620, 0, 40, 720 },
			{ 200, 200, 70, 260 }, { 380, 420, 70, 200 }, { 120, 480, 70, 160 },
			{ 760, 140, 70, 240 }, { 900, 380, 70, 240 }, { 1090, 220, 70, 200 }
		},
			0, { { 0, 0, 0, 0 } },
			520, 560, 700, 560,
			0,
			"A wall on one side is a brake. Use it."
	},

	{
		60, 60, 1130, 60,
		9,
		{
			{ 0, 0, 20, 720 }, { 1260, 0, 20, 720 },
			{ 0, 0, 1280, 20 }, { 0, 660, 1280, 60 }, { 620, 0, 40, 720 },
			{ 240, 300, 70, 220 }, { 430, 120, 70, 200 },
			{ 820, 460, 70, 180 }, { 1060, 300, 70, 220 }
		},
			3,
			{
				{ 760, 160, PIT_SIZE, PIT_SIZE },
				{ 930, 160, PIT_SIZE, PIT_SIZE },
				{ 930, 330, PIT_SIZE, PIT_SIZE }
		},
			300, 580, 1150, 560,
			0,
			"The holes are on his side. Walk him around them."
	},

	{
		60, 300, 1130, 300,
		11,
		{
			{ 0, 0, 20, 720 }, { 1260, 0, 20, 720 },
			{ 0, 0, 1280, 20 }, { 0, 660, 1280, 60 }, { 620, 0, 40, 720 },
			{ 180, 420, 70, 200 }, { 400, 180, 70, 230 }, { 120, 160, 70, 140 },
			{ 760, 400, 70, 220 }, { 1020, 180, 70, 240 }, { 1150, 430, 70, 160 }
		},
			4,
			{
				{ 300, 150, PIT_SIZE, PIT_SIZE },
				{ 250, 560, PIT_SIZE, PIT_SIZE },
				{ 880, 160, PIT_SIZE, PIT_SIZE },
				{ 900, 540, PIT_SIZE, PIT_SIZE }
		},
			540, 120, 690, 600,
			MH_LAG,
			"He is running late now. Plan further ahead."
	}
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

void MirrorHall::loadImages()
{
	roomBg[0] = iLoadImage("Images//map_mirror_room_1.png");
	roomBg[1] = iLoadImage("Images//map_mirror_room_2.png");
	roomBg[2] = iLoadImage("Images//map_mirror_room_3.png");
	roomBg[3] = iLoadImage("Images//map_mirror_room_4.png");

	seamImg = iLoadImage("Images//fx_mirror_seam.png");
	exitImg = iLoadImage("Images//obj_mirror_exit.png");
	exitLitImg = iLoadImage("Images//obj_mirror_exit_lit.png");
	pitImg = iLoadImage("Images//obj_pit.png");
	charmIconImg = iLoadImage("Images//ui_charm_icon.png");

	successImg = iLoadImage("Images//success.png");
	failedImg = iLoadImage("Images//failed.png");
	hoverSuccessImg = iLoadImage("Images//hover_success.png");
	hoverFailedImg = iLoadImage("Images//hover_failed.png");

	reflection.loadImages();
	roomMap.init("Images//map_mirror_room_1.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
}

void MirrorHall::buildRoom(int r, Player &player)
{
	MirrorRoom &R = ROOMS[r];

	roomMap.clearObstacles();
	for (int i = 0; i < R.wallCount; i++)
	{
		roomMap.addObstacle(R.walls[i][0], R.walls[i][1], R.walls[i][2], R.walls[i][3]);
	}

	player.init(R.mcStartX, R.mcStartY);
	player.setFacing(DIR_BACK);

	reflection.init(R.refStartX, R.refStartY);
	reflection.setFacing(DIR_BACK);

	roomTicks = 0;
	clearTicks = 0;

	lagHead = 0;
	for (int i = 0; i < MH_LAG; i++)
	{
		lagUp[i] = false; lagDown[i] = false;
		lagLeft[i] = false; lagRight[i] = false;
	}
}

void MirrorHall::start(bool playerHasCharm, Player &player)
{
	phase = MIRROR_PLAYING;
	room = 0;
	timeLeftTicks = MH_TIME_TICKS;
	mistakes = 0;
	resultTicks = 0;

	hasCharm = playerHasCharm;
	revealTicks = 0;
	charmCooldown = 0;
	flashTicks = 0;

	finished = false;
	retryRequested = false;

	buildRoom(0, player);
}

bool MirrorHall::rectsOverlap(Rect a, Rect b)
{
	return a.x < b.x + b.w && a.x + a.w > b.x &&
		a.y < b.y + b.h && a.y + a.h > b.y;
}

bool MirrorHall::onGoal(Rect f, int gx, int gy)
{
	Rect g = { gx, gy, GOAL_SIZE, GOAL_SIZE };
	return rectsOverlap(f, g);
}

void MirrorHall::handleInput(bool up, bool down, bool left, bool right, Player &player)
{
	if (phase != MIRROR_PLAYING || clearTicks > 0) return;

	player.handleInput(up, down, left, right, roomMap);

	bool rUp = up, rDown = down, rLeft = left, rRight = right;

	if (ROOMS[room].lagTicks > 0)
	{
		rUp = lagUp[lagHead];
		rDown = lagDown[lagHead];
		rLeft = lagLeft[lagHead];
		rRight = lagRight[lagHead];

		lagUp[lagHead] = up;
		lagDown[lagHead] = down;
		lagLeft[lagHead] = left;
		lagRight[lagHead] = right;

		lagHead = (lagHead + 1) % MH_LAG;
	}

	reflection.handleInput(rUp, rDown, rRight, rLeft, roomMap);
	reflection.updateAnimation();
}

void MirrorHall::failRoom(Player &player)
{
	mistakes++;
	flashTicks = FLASH_TICKS;

	if (mistakes >= MH_MAX_MISTAKES)
	{
		phase = MIRROR_FAILED;
		return;
	}

	buildRoom(room, player);
}

void MirrorHall::update(Player &player)
{
	if (phase != MIRROR_PLAYING)
	{
		if (resultTicks < RESULT_FADE_TICKS) resultTicks++;
		return;
	}

	timeLeftTicks--;
	if (timeLeftTicks <= 0)
	{
		timeLeftTicks = 0;
		phase = MIRROR_FAILED;
		return;
	}

	roomTicks++;
	if (flashTicks > 0) flashTicks--;
	if (revealTicks > 0) revealTicks--;
	if (charmCooldown > 0) charmCooldown--;

	MirrorRoom &R = ROOMS[room];

	Rect mf = player.getFeetRect();
	Rect rf = reflection.getFeetRect();

	for (int i = 0; i < R.pitCount; i++)
	{
		Rect p = { R.pits[i][0], R.pits[i][1], R.pits[i][2], R.pits[i][3] };
		if (rectsOverlap(mf, p) || rectsOverlap(rf, p))
		{
			failRoom(player);
			return;
		}
	}

	bool both = onGoal(mf, R.mcGoalX, R.mcGoalY) && onGoal(rf, R.refGoalX, R.refGoalY);

	if (!both)
	{
		clearTicks = 0;
		return;
	}

	clearTicks++;
	if (clearTicks < CLEAR_HOLD) return;

	if (room + 1 >= MH_ROOMS)
	{
		phase = MIRROR_SUCCESS;
		return;
	}

	room++;
	buildRoom(room, player);
}

void MirrorHall::handleKey(unsigned char key)
{
	if (phase != MIRROR_PLAYING) return;

	if (key == ' ' && hasCharm && charmCooldown <= 0)
	{
		revealTicks = REVEAL_TICKS;
		charmCooldown = CHARM_COOLDOWN;
	}
}

void MirrorHall::handleClick(int mx, int my)
{
	if (phase == MIRROR_PLAYING) return;

	if (mx >= RESULT_BUTTON_X && mx <= RESULT_BUTTON_X + RESULT_BUTTON_W &&
		my >= RESULT_BUTTON_Y && my <= RESULT_BUTTON_Y + RESULT_BUTTON_H)
	{
		if (phase == MIRROR_SUCCESS) finished = true;
		else retryRequested = true;
	}
}

void MirrorHall::draw(Player &player, int mouseX, int mouseY)
{
	MirrorRoom &R = ROOMS[room];

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, roomBg[room]);

	for (int i = 0; i < R.pitCount; i++)
	{
		iShowImage(R.pits[i][0], R.pits[i][1], R.pits[i][2], R.pits[i][3], pitImg);
	}

	Rect mf = player.getFeetRect();
	Rect rf = reflection.getFeetRect();

	iShowImage(R.mcGoalX, R.mcGoalY, GOAL_SIZE, GOAL_SIZE,
		onGoal(mf, R.mcGoalX, R.mcGoalY) ? exitLitImg : exitImg);
	iShowImage(R.refGoalX, R.refGoalY, GOAL_SIZE, GOAL_SIZE,
		onGoal(rf, R.refGoalX, R.refGoalY) ? exitLitImg : exitImg);

	if (revealTicks > 0)
	{
		float a = 0.30f;
		if (revealTicks < 40) a = 0.30f * revealTicks / 40.0f;

		for (int i = 0; i < R.wallCount; i++)
		{
			int ox = R.walls[i][0];
			if (ox < SEAM_X) continue;
			if (ox >= 1260) continue;

			int mx2 = SCREEN_WIDTH - ox - R.walls[i][2];
			fillQuad((float)mx2, (float)R.walls[i][1], (float)R.walls[i][2], (float)R.walls[i][3],
				0.62f, 0.56f, 1.0f, a);
		}

		for (int i = 0; i < R.pitCount; i++)
		{
			int ox = R.pits[i][0];
			if (ox < SEAM_X) continue;

			int mx2 = SCREEN_WIDTH - ox - R.pits[i][2];
			fillQuad((float)mx2, (float)R.pits[i][1], (float)R.pits[i][2], (float)R.pits[i][3],
				1.0f, 0.35f, 0.30f, a);
		}
	}

	player.draw(0, 0);

	// see through sprite
	bool staring = (room == 2 && roomTicks >= STARE_AT && roomTicks < STARE_AT + STARE_FOR);

	if (staring)
	{
		g_imgAlpha = 0.85f;
		reflection.setFacing(DIR_LEFT);
		reflection.draw(0, 0);
	}
	else
	{
		g_imgAlpha = 0.72f;
		reflection.draw(0, 0);
	}

	iShowImage(SEAM_X - SEAM_W / 2, 0, SEAM_W, SCREEN_HEIGHT, seamImg);

	if (flashTicks > 0)
	{
		float a = (float)flashTicks / FLASH_TICKS;
		fillQuad(0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 1.0f, 0.3f, 0.25f, a * 0.55f);
	}

	char buf[60];
	int secondsLeft = timeLeftTicks / 100;

	iSetColor(255, 255, 255);

	sprintf_s(buf, "Room %d / %d", room + 1, MH_ROOMS);
	iText(HUD_ROOM_X, HUD_ROOM_Y, buf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(buf, "%02d:%02d", secondsLeft / 60, secondsLeft % 60);
	iText(HUD_TIME_X, HUD_TIME_Y, buf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(buf, "Mistakes: %d/%d", mistakes, MH_MAX_MISTAKES);
	iText(HUD_MISS_X, HUD_MISS_Y, buf, GLUT_BITMAP_HELVETICA_18);

	if (hasCharm)
	{
		g_imgAlpha = (charmCooldown > 0) ? 0.4f : 1.0f;
		iShowImage(HUD_CHARM_X, HUD_CHARM_Y, HUD_CHARM_SIZE, HUD_CHARM_SIZE, charmIconImg);

		float ready = (charmCooldown > 0) ? 1.0f - (float)charmCooldown / CHARM_COOLDOWN : 1.0f;

		fillQuad((float)HUD_CHARM_X, (float)(HUD_CHARM_Y - 12), (float)HUD_CHARM_SIZE, 6.0f,
			0.2f, 0.2f, 0.3f, 0.9f);
		fillQuad((float)HUD_CHARM_X, (float)(HUD_CHARM_Y - 12), HUD_CHARM_SIZE * ready, 6.0f,
			0.62f, 0.56f, 0.91f, 1.0f);
	}

	if (roomTicks < 420) drawPlate(ROOMS[room].caption);

	if (phase != MIRROR_PLAYING)
	{
		bool hovering =
			mouseX >= RESULT_BUTTON_X && mouseX <= RESULT_BUTTON_X + RESULT_BUTTON_W &&
			mouseY >= RESULT_BUTTON_Y && mouseY <= RESULT_BUTTON_Y + RESULT_BUTTON_H;

		float a = (float)resultTicks / (float)RESULT_FADE_TICKS;
		if (a > 1.0f) a = 1.0f;

		int img = (phase == MIRROR_SUCCESS)
			? (hovering ? hoverSuccessImg : successImg)
			: (hovering ? hoverFailedImg : failedImg);

		g_imgAlpha = a;
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, img);
	}
}

bool MirrorHall::isFinished()
{
	if (!finished) return false;
	finished = false;
	return true;
}

bool MirrorHall::isRetryRequested()
{
	if (!retryRequested) return false;
	retryRequested = false;
	return true;
}