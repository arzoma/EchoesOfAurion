#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "Foresttrail.hpp"
#include "Constants.hpp"

#include "glut.h"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

extern float g_imgAlpha;
extern bool  g_imgAdditive;

static const int CLUE_STONE_X = 250, CLUE_STONE_Y = 430;
static const int CLUE_STONE_W = 200, CLUE_STONE_H = 180;

static const int CLUE_TRIGGER_X = 200, CLUE_TRIGGER_Y = 360;
static const int CLUE_TRIGGER_W = 360, CLUE_TRIGGER_H = 340;

static const int SEARCH_ICON_SIZE = 56;
static const int CLEAR_ICON_SIZE = 56;
static const int ICON_REACH = 220;

static const int COMPASS_X = 540, COMPASS_Y = 340, COMPASS_SIZE = 120;

static const int FADE_TICKS = 20;
static const int BOTTOM_TEXT_X = 250, BOTTOM_TEXT_Y = 60;

static const int MISTAKE_TEXT_X = 1090, MISTAKE_TEXT_Y = 660;

static const int SEL_SLOT_X[4] = { 340, 490, 640, 790 };
static const int SEL_SLOT_Y = 300;
static const int SEL_SLOT_SIZE = 110;

static const int SEQ_ICON_X[3] = { 490, 640, 790 };
static const int SEQ_ICON_Y = 250;
static const int SEQ_ICON_SIZE = 110;
static const int SEQ_EMPTY_X[3] = { 505, 625, 745 };
static const int SEQ_EMPTY_Y = 400;
static const int SEQ_EMPTY_SIZE = 90;
static const int SEQ_CLEAR_X = 950, SEQ_CLEAR_Y = 250;
static const int SEQ_CLEAR_W = 120, SEQ_CLEAR_H = 44;

static const int ODD_COL_X[3] = { 500, 630, 760 };
static const int ODD_ROW_Y[2] = { 380, 250 };
static const int ODD_TILE_SIZE = 100;

static const int RIDDLE_TEXT_X = 300;
static const int RIDDLE_TEXT_Y = 560;
static const int RIDDLE_LINE_H = 30;

enum { SYM_SUN = 0, SYM_MOON, SYM_STAR, SYM_LEAF };
enum { RUNE_A = 0, RUNE_B, RUNE_C, RUNE_D, RUNE_E, RUNE_F, RUNE_G, RUNE_H };

static const int ODD_TILES_TRUE[6] = { RUNE_A, RUNE_B, RUNE_C, RUNE_D, RUNE_E, RUNE_F };
static const int ODD_TILES_DECOY[6] = { RUNE_B, RUNE_C, RUNE_D, RUNE_E, RUNE_G, RUNE_H };

static char* WRONG_LINES[3] =
{
	"The stone does not move.",
	"Nothing happens. The carving stays dark.",
	"The branches hold. Whatever you just told the grove, it wasn't true."
};


static PointDef POINTS[FOREST_POINTS] =
{
	{
		"Something glimmers faintly among the roots.",

		"Three mouths open before you, and only one still breathes.\n"
		"Do not trust the road that welcomes you - the forest sets\n"
		"no table for strangers.\n"
		"Turn instead to where the first light climbs.",

		PZ_SELECT, PATH_EAST,
		{
			// WEST - blocked, decoy
			{ PATH_BLOCKED_DECOY, 40, 880, 240, 200,
			"Every living thing in this grove turns its face to me\n"
			"each morning. Wake me, and the grove wakes with me.",
			{ SYM_SUN, 0, 0 }, 1,
			"The trail gives out in thicket. An old fire ring, long cold." },

			// NORTH - open trap
			{ PATH_OPEN_TRAP, 520, 1000, 240, 200,
			0, { 0, 0, 0 }, 0,
			"The path runs twenty paces and ends against a wall of roots." },

			// EAST - blocked, true
			{ PATH_BLOCKED_TRUE, 1000, 880, 240, 200,
			"I am not the fire that wakes the world, nor the small\n"
			"lights that trail behind it.\n"
			"I am the lantern the dark carries. Set your hand on me.",
			{ SYM_MOON, 0, 0 }, 1, 0 }
		}
	},


	{
		"The same markings. The same light. You are not certain that is a good sign.",

		"The light that carried you this far is failing now.\n"
		"Walk the way it falls - follow the sun down into its grave.\n"
		"What stands open was opened for you. Let that trouble you.",

		PZ_SEQUENCE, PATH_WEST,
		{
			// WEST - blocked, true
			{ PATH_BLOCKED_TRUE, 40, 880, 240, 200,
			"First the fire wakes and burns the dark away.\n"
			"Then the lantern rises to keep the watch in its place.\n"
			"Last the small ones gather, and the night is full.",
			{ SYM_SUN, SYM_MOON, SYM_STAR }, 3, 0 },

			// NORTH - blocked, decoy
			{ PATH_BLOCKED_DECOY, 520, 1000, 240, 200,
			"The night grows old before the grove stirs.\n"
			"The small ones fade first. Then the lantern sinks.\n"
			"Only then does the fire come to claim the sky.",
			{ SYM_STAR, SYM_MOON, SYM_SUN }, 3,
			"Branches, then more branches. Someone cut this trail and gave up halfway." },

			// EAST - open trap
			{ PATH_OPEN_TRAP, 1000, 880, 240, 200,
			0, { 0, 0, 0 }, 0,
			"The ground turns to standing water. Whatever this was, the forest has taken it back." }
		}
	},


	{
		"The stone is warm. Whatever is written here was written recently.",

		"The sun is no use to you here; it never reached this deep.\n"
		"Keep the cold against your face and climb.\n"
		"And look closely at what waits - one mark ahead was cut\n"
		"by a hand that did not wander.",

		PZ_ODDONE, PATH_NORTH,
		{
			// WEST - open trap
			{ PATH_OPEN_TRAP, 40, 880, 240, 200,
			0, { 0, 0, 0 }, 0,
			"The trees close ahead of you - not slowly. You step back before they finish." },

			// NORTH - blocked, true
			{ PATH_BLOCKED_TRUE, 520, 1000, 240, 200,
			"The grove signs its name in circles; every mark that grew\n"
			"here closes upon itself.\n"
			"One does not close. One was cut quickly, by someone who\n"
			"did not mean to stay.\n"
			"Find the hand that did not wander.",
			{ RUNE_F, 0, 0 }, 1,
			0 },

			// EAST - blocked, decoy
			{ PATH_BLOCKED_DECOY, 1000, 880, 240, 200,
			"A hundred winters have fed on these marks and the moss\n"
			"has taken them all.\n"
			"All but one. Something here is younger than your journey.\n"
			"Find what does not belong.",
			{ RUNE_H, 0, 0 }, 1,
			"A fresh carving on a dead trunk, and nothing beyond it.\n"
			"Someone marked this place. It wasn't your master's hand." }
		}
	}
};

void ForestTrail::loadImages()
{
	segmentImg = iLoadImage("Images//map_forest_segment.png");
	topClearingImg = iLoadImage("Images//map_forest_top_clearing.png");
	vignetteImg = iLoadImage("Images//fx_vignette.png");

	clueStoneImg = iLoadImage("Images//obj_clue_stone.png");
	clueStoneLitImg = iLoadImage("Images//obj_clue_stone_lit.png");
	glowSoftImg = iLoadImage("Images//fx_glow_soft.png");
	orbImg = iLoadImage("Images//fx_orb.png");

	branchImg[PATH_WEST] = iLoadImage("Images//overlay_branches_west.png");
	branchImg[PATH_NORTH] = iLoadImage("Images//overlay_branches_north.png");
	branchImg[PATH_EAST] = iLoadImage("Images//overlay_branches_east.png");

	searchIcon = iLoadImage("Images//icon_search.png");
	searchIconHover = iLoadImage("Images//icon_search_hover.png");
	clearIcon = iLoadImage("Images//icon_clear_path.png");
	clearIconHover = iLoadImage("Images//icon_clear_path_hover.png");

	compassImg[PATH_WEST] = iLoadImage("Images//icon_compass_w.png");
	compassImg[PATH_NORTH] = iLoadImage("Images//icon_compass_n.png");
	compassImg[PATH_EAST] = iLoadImage("Images//icon_compass_e.png");

	riddleWindowImg = iLoadImage("Images//ui_riddle_window.png");
	puzzleWindowImg = iLoadImage("Images//ui_puzzle_window.png");
	symbolFrameImg = iLoadImage("Images//ui_symbol_frame.png");
	slotEmptyImg = iLoadImage("Images//ui_slot_empty.png");

	symbolImg[SYM_SUN] = iLoadImage("Images//sym_sun.png");
	symbolImg[SYM_MOON] = iLoadImage("Images//sym_moon.png");
	symbolImg[SYM_STAR] = iLoadImage("Images//sym_star.png");
	symbolImg[SYM_LEAF] = iLoadImage("Images//sym_leaf.png");

	runeImg[RUNE_A] = iLoadImage("Images//rune_a.png");
	runeImg[RUNE_B] = iLoadImage("Images//rune_b.png");
	runeImg[RUNE_C] = iLoadImage("Images//rune_c.png");
	runeImg[RUNE_D] = iLoadImage("Images//rune_d.png");
	runeImg[RUNE_E] = iLoadImage("Images//rune_e.png");
	runeImg[RUNE_F] = iLoadImage("Images//rune_f.png");
	runeImg[RUNE_G] = iLoadImage("Images//rune_g.png");
	runeImg[RUNE_H] = iLoadImage("Images//rune_h.png");

	revealImg = iLoadImage("Images//reveal_cloak_scrap.png");
	successImg = iLoadImage("Images//success.png");
	failedImg = iLoadImage("Images//failed.png");

	trailMap.init("Images//map_forest_segment.png", FOREST_WORLD_W, FOREST_WORLD_H, true);
}

Map& ForestTrail::getMap()
{
	return trailMap;
}

void ForestTrail::start(bool playerHasCompass)
{
	for (int p = 0; p < FOREST_POINTS; p++)
	{
		for (int d = 0; d < 3; d++) cleared[p][d] = false;
	}

	mistakes = 0;
	hasCompass = playerHasCompass;
	compassUsed = false;

	overlay = OV_NONE;
	activeDir = -1;
	wrongLineIndex = 0;

	seqCount = 0;
	for (int i = 0; i < 3; i++) seqPicks[i] = -1;

	bottomText[0] = '\0';
	bottomTextTicks = 0;

	atEnding = false;
	endStep = 0;

	fadePhase = 0;
	fadeTick = 0;
	fadePendingPoint = 0;
	fadeIsDeadEnd = false;

	glowTimer = 0.0;
	glowAlpha = 0.0;
	initOrbs();

	finished = false;
	retryRequested = false;

	currentPoint = 0;
}

void ForestTrail::initOrbs()
{
	for (int i = 0; i < FOREST_ORB_COUNT; i++)
	{
		orbY[i] = (double)(rand() % 160);
		orbPhase[i] = (double)(rand() % 628) / 100.0;
		orbSpeed[i] = 0.14 + (double)(rand() % 14) / 100.0;
	}
}

void ForestTrail::buildSegmentObstacles(int p)
{
	int base = p * FOREST_SEG_H;

	trailMap.clearObstacles();

	// the static rects
	trailMap.addObstacle(0, base + 0, 180, 880);
	trailMap.addObstacle(1100, base + 0, 180, 880);
	trailMap.addObstacle(280, base + 880, 240, 320);
	trailMap.addObstacle(760, base + 880, 240, 320);
	trailMap.addObstacle(0, base + 1080, 280, 120);
	trailMap.addObstacle(1000, base + 1080, 280, 120);
	trailMap.addObstacle(0, base - 40, 1280, 40);

	// one branch obstacle per blocked path, switched off once cleared
	for (int d = 0; d < 3; d++)
	{
		branchObstacleIdx[d] = -1;
		branchFade[d] = 0.0;

		if (POINTS[p].path[d].kind == PATH_OPEN_TRAP) continue;

		PathDef &pd = POINTS[p].path[d];
		int idx = trailMap.addObstacle(pd.mouthX, base + pd.mouthY, pd.mouthW, pd.mouthH);
		trailMap.setObstacleActive(idx, !cleared[p][d]);
		branchObstacleIdx[d] = idx;
	}
}

void ForestTrail::enterPoint(int p)
{
	currentPoint = p;
	atEnding = false;

	buildSegmentObstacles(p);

	overlay = OV_NONE;
	activeDir = -1;
	seqCount = 0;
	for (int i = 0; i < 3; i++) seqPicks[i] = -1;
	compassUsed = false;
}

void ForestTrail::enterClearing()
{
	atEnding = true;
	endStep = 0;

	trailMap.clearObstacles();
	trailMap.addObstacle(0, FOREST_SEG_H * FOREST_POINTS - 40, 1280, 40);
	trailMap.addObstacle(0, FOREST_SEG_H * FOREST_POINTS, 180, FOREST_TOP_H);
	trailMap.addObstacle(1100, FOREST_SEG_H * FOREST_POINTS, 180, FOREST_TOP_H);
	trailMap.addObstacle(0, FOREST_WORLD_H - 40, 1280, 40);
}

bool ForestTrail::isInsideBox(int mx, int my, int bx, int by, int bw, int bh)
{
	return mx >= bx && mx <= bx + bw && my >= by && my <= by + bh;
}

bool ForestTrail::playerNearStone(Player &player)
{
	if (atEnding) return false;

	int base = currentPoint * FOREST_SEG_H;
	Rect f = player.getFeetRect();

	return f.x < CLUE_TRIGGER_X + CLUE_TRIGGER_W && f.x + f.w > CLUE_TRIGGER_X &&
		f.y < base + CLUE_TRIGGER_Y + CLUE_TRIGGER_H && f.y + f.h > base + CLUE_TRIGGER_Y;
}

bool ForestTrail::iconVisible(Player &player, int dir)
{
	if (atEnding) return false;
	if (cleared[currentPoint][dir]) return false;
	if (POINTS[currentPoint].path[dir].kind == PATH_OPEN_TRAP) return false;

	PathDef &pd = POINTS[currentPoint].path[dir];

	int cx = pd.mouthX + pd.mouthW / 2;
	int cy = currentPoint * FOREST_SEG_H + pd.mouthY + pd.mouthH / 2;

	int dx = player.getX() + PLAYER_WIDTH / 2 - cx;
	int dy = player.getY() + PLAYER_HEIGHT / 2 - cy;

	return (dx * dx + dy * dy) < ICON_REACH * ICON_REACH;
}

int ForestTrail::mouthUnderPlayer(Player &player)
{
	if (atEnding) return -1;

	int base = currentPoint * FOREST_SEG_H;
	Rect f = player.getFeetRect();

	for (int d = 0; d < 3; d++)
	{
		PathDef &pd = POINTS[currentPoint].path[d];

		// a blocked path is only enterable once its puzzle is solved
		if (pd.kind != PATH_OPEN_TRAP && !cleared[currentPoint][d]) continue;

		int mx = pd.mouthX;
		int my = base + pd.mouthY;

		if (f.x < mx + pd.mouthW && f.x + f.w > mx &&
			f.y < my + pd.mouthH && f.y + f.h > my)
		{
			return d;
		}
	}

	return -1;
}

void ForestTrail::showBottomText(char* text, int ticks)
{
	strcpy_s(bottomText, text);
	bottomTextTicks = ticks;
}

void ForestTrail::registerMistake()
{
	mistakes++;

	if (mistakes >= FOREST_MAX_MISTAKES)
	{
		overlay = OV_RESULT;
	}
}

void ForestTrail::onWrongAnswer()
{
	showBottomText(WRONG_LINES[wrongLineIndex], 250);
	wrongLineIndex = (wrongLineIndex + 1) % 3;

	seqCount = 0;
	for (int i = 0; i < 3; i++) seqPicks[i] = -1;

	registerMistake();
}

void ForestTrail::onPuzzleSolved(int dir)
{
	cleared[currentPoint][dir] = true;

	if (branchObstacleIdx[dir] != -1)
	{
		trailMap.setObstacleActive(branchObstacleIdx[dir], false);
	}

	branchFade[dir] = 1.0;
	overlay = OV_NONE;
	activeDir = -1;

	showBottomText("The branches pull back on their own.", 250);
}

void ForestTrail::startFade(int pendingPoint, bool deadEnd)
{
	fadePhase = 1;
	fadeTick = 0;
	fadePendingPoint = pendingPoint;
	fadeIsDeadEnd = deadEnd;
}

void ForestTrail::updateFade()
{
	if (fadePhase == 0) return;

	fadeTick++;

	if (fadePhase == 1 && fadeTick >= FADE_TICKS)
	{
		fadePhase = 2;
		fadeTick = 0;
	}
	else if (fadePhase == 2 && fadeTick >= FADE_TICKS)
	{
		fadePhase = 0;
	}
}

void ForestTrail::drawFadeOverlay()
{
	if (fadePhase == 0) return;

	float alpha = (fadePhase == 1)
		? (float)fadeTick / FADE_TICKS
		: 1.0f - (float)fadeTick / FADE_TICKS;

	if (alpha < 0.0f) alpha = 0.0f;
	if (alpha > 1.0f) alpha = 1.0f;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, alpha);

	glBegin(GL_QUADS);
	glVertex2f(0.0f, 0.0f);
	glVertex2f((float)SCREEN_WIDTH, 0.0f);
	glVertex2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	glVertex2f(0.0f, (float)SCREEN_HEIGHT);
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ForestTrail::updateGlow(bool near)
{
	glowTimer += 0.01;   // one tick = 10ms

	double pulse = 0.5 + 0.5 * sin(glowTimer * 2.0);
	double target = near ? (0.55 + 0.30 * pulse) : (0.25 + 0.35 * pulse);

	glowAlpha += (target - glowAlpha) * 0.06;
}

void ForestTrail::updateOrbs()
{
	for (int i = 0; i < FOREST_ORB_COUNT; i++)
	{
		orbY[i] += orbSpeed[i];
		if (orbY[i] > 160.0) orbY[i] -= 160.0;
	}
}

void ForestTrail::drawOrbs(int cx, int cy)
{
	for (int i = 0; i < FOREST_ORB_COUNT; i++)
	{
		double t = orbY[i] / 160.0;
		double dx = sin(glowTimer * 1.4 + orbPhase[i]) * (12.0 + 22.0 * t);
		double a = (t < 0.15) ? (t / 0.15) : (1.0 - (t - 0.15) / 0.85);

		g_imgAdditive = true;
		g_imgAlpha = (float)(a * 0.75 * glowAlpha);
		iShowImage((int)(cx + dx) - 12, (int)(cy + orbY[i]), 24, 24, orbImg);
	}
}

void ForestTrail::drawClueStone(int sx, int sy)
{
	g_imgAdditive = true;
	g_imgAlpha = (float)(glowAlpha * 0.55);
	iShowImage(sx - 60, sy - 70, 320, 320, glowSoftImg);

	iShowImage(sx, sy, CLUE_STONE_W, CLUE_STONE_H, clueStoneImg);

	g_imgAdditive = true;
	g_imgAlpha = (float)glowAlpha;
	iShowImage(sx, sy, CLUE_STONE_W, CLUE_STONE_H, clueStoneLitImg);

	drawOrbs(sx + 100, sy + 90);
}

void ForestTrail::drawBranches()
{
	if (atEnding) return;

	int base = currentPoint * FOREST_SEG_H;

	for (int d = 0; d < 3; d++)
	{
		PathDef &pd = POINTS[currentPoint].path[d];
		if (pd.kind == PATH_OPEN_TRAP) continue;

		double a = cleared[currentPoint][d] ? branchFade[d] : 1.0;
		if (a <= 0.0) continue;

		int sx = pd.mouthX;
		int sy = base + pd.mouthY - trailMap.getCameraY();

		g_imgAlpha = (float)a;
		iShowImage(sx, sy, pd.mouthW, pd.mouthH, branchImg[d]);
	}
}

void ForestTrail::update(Player &player)
{
	updateFade();

	if (fadePhase == 2 && fadeTick == 1)
	{
		if (fadeIsDeadEnd)
		{
			player.init(FOREST_SPAWN_X, currentPoint * FOREST_SEG_H + 800);
			player.setFacing(DIR_FRONT);
		}
		else if (fadePendingPoint == -1)
		{
			enterClearing();
			player.init(FOREST_SPAWN_X, FOREST_SEG_H * FOREST_POINTS + 40);
			player.setFacing(DIR_BACK);
		}
		else
		{
			enterPoint(fadePendingPoint);
			player.init(FOREST_SPAWN_X, fadePendingPoint * FOREST_SEG_H + FOREST_SPAWN_Y);
			player.setFacing(DIR_BACK);
		}

		trailMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
	}

	if (bottomTextTicks > 0) bottomTextTicks--;

	updateGlow(playerNearStone(player));
	updateOrbs();

	for (int d = 0; d < 3; d++)
	{
		if (cleared[currentPoint][d] && branchFade[d] > 0.0)
		{
			branchFade[d] -= 0.025;
			if (branchFade[d] < 0.0) branchFade[d] = 0.0;
		}
	}

	if (overlay != OV_NONE || fadePhase != 0) return;

	int d = mouthUnderPlayer(player);

	if (d != -1)
	{
		PathDef &pd = POINTS[currentPoint].path[d];

		if (pd.kind == PATH_BLOCKED_TRUE)
		{
			if (currentPoint + 1 < FOREST_POINTS) startFade(currentPoint + 1, false);
			else startFade(-1, false);          // -1 = the top clearing
		}
		else
		{
			showBottomText(pd.deadEndLine, 320);
			startFade(currentPoint, true);
		}
	}
}

void ForestTrail::draw(Player &player, int mouseX, int mouseY)
{
	int camY = trailMap.getCameraY();

	// one segment drawn three times
	for (int p = 0; p < FOREST_POINTS; p++)
	{
		int sy = p * FOREST_SEG_H - camY;
		if (sy > SCREEN_HEIGHT || sy + FOREST_SEG_H < 0) continue;
		iShowImage(0, sy, FOREST_SEG_W, FOREST_SEG_H, segmentImg);
	}

	int clearingY = FOREST_SEG_H * FOREST_POINTS - camY;
	if (clearingY < SCREEN_HEIGHT && clearingY + FOREST_TOP_H > 0)
	{
		iShowImage(0, clearingY, FOREST_SEG_W, FOREST_TOP_H, topClearingImg);
	}

	if (!atEnding)
	{
		int base = currentPoint * FOREST_SEG_H;

		// compass arrow on the ground
		if (hasCompass && compassUsed)
		{
			iShowImage(COMPASS_X, base + COMPASS_Y - camY, COMPASS_SIZE, COMPASS_SIZE,
				compassImg[POINTS[currentPoint].trueDir]);
		}

		drawClueStone(CLUE_STONE_X, base + CLUE_STONE_Y - camY);
		drawBranches();
	}

	player.draw(0, camY);

	if (!atEnding && overlay == OV_NONE)
	{
		int base = currentPoint * FOREST_SEG_H;

		if (playerNearStone(player))
		{
			int ix = CLUE_STONE_X + CLUE_STONE_W / 2 - SEARCH_ICON_SIZE / 2;
			int iy = base + CLUE_STONE_Y + CLUE_STONE_H + 20 - camY;
			bool hov = isInsideBox(mouseX, mouseY, ix, iy, SEARCH_ICON_SIZE, SEARCH_ICON_SIZE);
			iShowImage(ix, iy, SEARCH_ICON_SIZE, SEARCH_ICON_SIZE, hov ? searchIconHover : searchIcon);
		}

		for (int d = 0; d < 3; d++)
		{
			if (!iconVisible(player, d)) continue;

			PathDef &pd = POINTS[currentPoint].path[d];
			int ix = pd.mouthX + pd.mouthW / 2 - CLEAR_ICON_SIZE / 2;
			int iy = base + pd.mouthY + pd.mouthH / 2 - CLEAR_ICON_SIZE / 2 - camY;
			bool hov = isInsideBox(mouseX, mouseY, ix, iy, CLEAR_ICON_SIZE, CLEAR_ICON_SIZE);
			iShowImage(ix, iy, CLEAR_ICON_SIZE, CLEAR_ICON_SIZE, hov ? clearIconHover : clearIcon);
		}
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, vignetteImg);

	if (overlay == OV_CLUE)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, riddleWindowImg);
		iSetColor(255, 255, 255);

		char buf[600];
		strcpy_s(buf, POINTS[currentPoint].clueRiddle);

		int y = RIDDLE_TEXT_Y;
		char* ctx = 0;
		char* tok = strtok_s(buf, "\n", &ctx);
		while (tok != 0)
		{
			iText(RIDDLE_TEXT_X, y, tok, GLUT_BITMAP_HELVETICA_18);
			y -= RIDDLE_LINE_H;
			tok = strtok_s(0, "\n", &ctx);
		}
	}
	else if (overlay == OV_PUZZLE)
	{
		drawPuzzle(mouseX, mouseY);
	}
	else if (overlay == OV_REVEAL)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, revealImg);
	}
	else if (overlay == OV_RESULT)
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
			(mistakes >= FOREST_MAX_MISTAKES) ? failedImg : successImg);
	}

	char mistakeText[40];
	sprintf_s(mistakeText, "Mistakes: %d/%d", mistakes, FOREST_MAX_MISTAKES);
	iSetColor(255, 255, 255);
	iText(MISTAKE_TEXT_X, MISTAKE_TEXT_Y, mistakeText, GLUT_BITMAP_HELVETICA_18);

	if (bottomTextTicks > 0)
	{
		iSetColor(255, 255, 255);
		iText(BOTTOM_TEXT_X, BOTTOM_TEXT_Y, bottomText, GLUT_BITMAP_HELVETICA_18);
	}
	else if (overlay == OV_NONE && !atEnding && playerNearStone(player))
	{
		iSetColor(255, 255, 255);
		iText(BOTTOM_TEXT_X, BOTTOM_TEXT_Y, POINTS[currentPoint].proximityLine, GLUT_BITMAP_HELVETICA_18);
	}
	else if (overlay == OV_NONE && !atEnding && hasCompass && !compassUsed)
	{
		iSetColor(255, 255, 255);
		iText(BOTTOM_TEXT_X, BOTTOM_TEXT_Y, "Press C - use the compass", GLUT_BITMAP_HELVETICA_18);
	}

	drawFadeOverlay();
}

void ForestTrail::drawPuzzle(int mouseX, int mouseY)
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, puzzleWindowImg);

	PathDef &pd = POINTS[currentPoint].path[activeDir];

	iSetColor(255, 255, 255);

	char buf[600];
	strcpy_s(buf, pd.riddle);

	int y = RIDDLE_TEXT_Y;
	char* ctx = 0;
	char* tok = strtok_s(buf, "\n", &ctx);
	while (tok != 0)
	{
		iText(RIDDLE_TEXT_X, y, tok, GLUT_BITMAP_HELVETICA_18);
		y -= RIDDLE_LINE_H;
		tok = strtok_s(0, "\n", &ctx);
	}

	PuzzleType pz = POINTS[currentPoint].puzzle;

	if (pz == PZ_SELECT)
	{
		for (int i = 0; i < 4; i++)
		{
			iShowImage(SEL_SLOT_X[i], SEL_SLOT_Y, SEL_SLOT_SIZE, SEL_SLOT_SIZE, symbolImg[i]);

			if (isInsideBox(mouseX, mouseY, SEL_SLOT_X[i], SEL_SLOT_Y, SEL_SLOT_SIZE, SEL_SLOT_SIZE))
			{
				iShowImage(SEL_SLOT_X[i] - 8, SEL_SLOT_Y - 8, 126, 126, symbolFrameImg);
			}
		}
	}
	else if (pz == PZ_SEQUENCE)
	{
		for (int i = 0; i < 3; i++)
		{
			iShowImage(SEQ_EMPTY_X[i], SEQ_EMPTY_Y, SEQ_EMPTY_SIZE, SEQ_EMPTY_SIZE, slotEmptyImg);

			if (i < seqCount)
			{
				iShowImage(SEQ_EMPTY_X[i], SEQ_EMPTY_Y, SEQ_EMPTY_SIZE, SEQ_EMPTY_SIZE, symbolImg[seqPicks[i]]);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			bool used = false;
			for (int j = 0; j < seqCount; j++) if (seqPicks[j] == i) used = true;

			g_imgAlpha = used ? 0.35f : 1.0f;
			iShowImage(SEQ_ICON_X[i], SEQ_ICON_Y, SEQ_ICON_SIZE, SEQ_ICON_SIZE, symbolImg[i]);

			if (!used && isInsideBox(mouseX, mouseY, SEQ_ICON_X[i], SEQ_ICON_Y, SEQ_ICON_SIZE, SEQ_ICON_SIZE))
			{
				iShowImage(SEQ_ICON_X[i] - 8, SEQ_ICON_Y - 8, 126, 126, symbolFrameImg);
			}
		}

		iSetColor(255, 255, 255);
		iText(SEQ_CLEAR_X + 24, SEQ_CLEAR_Y + 14, "Clear", GLUT_BITMAP_HELVETICA_18);
	}
	else
	{
		const int* tiles = (pd.kind == PATH_BLOCKED_TRUE) ? ODD_TILES_TRUE : ODD_TILES_DECOY;

		for (int i = 0; i < 6; i++)
		{
			int col = i % 3;
			int row = i / 3;
			int tx = ODD_COL_X[col];
			int ty = ODD_ROW_Y[row];

			iShowImage(tx, ty, ODD_TILE_SIZE, ODD_TILE_SIZE, runeImg[tiles[i]]);

			if (isInsideBox(mouseX, mouseY, tx, ty, ODD_TILE_SIZE, ODD_TILE_SIZE))
			{
				iShowImage(tx - 13, ty - 13, 126, 126, symbolFrameImg);
			}
		}
	}
}

void ForestTrail::handlePuzzleClick(int mx, int my)
{
	PathDef &pd = POINTS[currentPoint].path[activeDir];
	PuzzleType pz = POINTS[currentPoint].puzzle;

	if (pz == PZ_SELECT)
	{
		for (int i = 0; i < 4; i++)
		{
			if (isInsideBox(mx, my, SEL_SLOT_X[i], SEL_SLOT_Y, SEL_SLOT_SIZE, SEL_SLOT_SIZE))
			{
				if (i == pd.answer[0]) onPuzzleSolved(activeDir);
				else onWrongAnswer();
				return;
			}
		}
		return;
	}

	if (pz == PZ_SEQUENCE)
	{
		if (isInsideBox(mx, my, SEQ_CLEAR_X, SEQ_CLEAR_Y, SEQ_CLEAR_W, SEQ_CLEAR_H))
		{
			seqCount = 0;
			for (int i = 0; i < 3; i++) seqPicks[i] = -1;
			return;
		}

		for (int i = 0; i < 3; i++)
		{
			if (!isInsideBox(mx, my, SEQ_ICON_X[i], SEQ_ICON_Y, SEQ_ICON_SIZE, SEQ_ICON_SIZE)) continue;

			for (int j = 0; j < seqCount; j++) if (seqPicks[j] == i) return;

			if (seqCount < 3)
			{
				seqPicks[seqCount] = i;
				seqCount++;
			}

			if (seqCount == 3)
			{
				bool ok = true;
				for (int k = 0; k < 3; k++) if (seqPicks[k] != pd.answer[k]) ok = false;

				if (ok) onPuzzleSolved(activeDir);
				else onWrongAnswer();
			}
			return;
		}
		return;
	}

	const int* tiles = (pd.kind == PATH_BLOCKED_TRUE) ? ODD_TILES_TRUE : ODD_TILES_DECOY;

	for (int i = 0; i < 6; i++)
	{
		int col = i % 3;
		int row = i / 3;

		if (isInsideBox(mx, my, ODD_COL_X[col], ODD_ROW_Y[row], ODD_TILE_SIZE, ODD_TILE_SIZE))
		{
			if (tiles[i] == pd.answer[0]) onPuzzleSolved(activeDir);
			else onWrongAnswer();
			return;
		}
	}
}

void ForestTrail::handleClick(Player &player, int mx, int my)
{
	if (fadePhase != 0) return;

	if (overlay == OV_RESULT)
	{
		if (mistakes >= FOREST_MAX_MISTAKES) retryRequested = true;
		else finished = true;
		return;
	}

	if (overlay == OV_REVEAL)
	{
		overlay = OV_NONE;
		endStep = 1;
		showBottomText("The cloth is his. He meant this to be found.", 400);
		return;
	}

	if (overlay == OV_CLUE)
	{
		overlay = OV_NONE;
		return;
	}

	if (overlay == OV_PUZZLE)
	{
		handlePuzzleClick(mx, my);
		return;
	}

	if (atEnding)
	{
		if (endStep == 0)
		{
			overlay = OV_REVEAL;
		}
		else if (endStep == 1)
		{
			endStep = 2;
			showBottomText("He went deeper than the warden ever knew.", 400);
		}
		else if (endStep == 2)
		{
			overlay = OV_RESULT;
		}
		return;
	}

	int base = currentPoint * FOREST_SEG_H;
	int camY = trailMap.getCameraY();

	if (playerNearStone(player))
	{
		int ix = CLUE_STONE_X + CLUE_STONE_W / 2 - SEARCH_ICON_SIZE / 2;
		int iy = base + CLUE_STONE_Y + CLUE_STONE_H + 20 - camY;

		if (isInsideBox(mx, my, ix, iy, SEARCH_ICON_SIZE, SEARCH_ICON_SIZE))
		{
			overlay = OV_CLUE;
			return;
		}
	}

	for (int d = 0; d < 3; d++)
	{
		if (!iconVisible(player, d)) continue;

		PathDef &pd = POINTS[currentPoint].path[d];
		int ix = pd.mouthX + pd.mouthW / 2 - CLEAR_ICON_SIZE / 2;
		int iy = base + pd.mouthY + pd.mouthH / 2 - CLEAR_ICON_SIZE / 2 - camY;

		if (isInsideBox(mx, my, ix, iy, CLEAR_ICON_SIZE, CLEAR_ICON_SIZE))
		{
			overlay = OV_PUZZLE;
			activeDir = d;
			seqCount = 0;
			for (int i = 0; i < 3; i++) seqPicks[i] = -1;
			return;
		}
	}
}

void ForestTrail::handleKey(unsigned char key)
{
	if ((key == 'c' || key == 'C') && hasCompass && !atEnding && overlay == OV_NONE)
	{
		compassUsed = true;
	}
}

bool ForestTrail::isFinished()
{
	if (!finished) return false;
	finished = false;
	return true;
}

bool ForestTrail::isRetryRequested()
{
	if (!retryRequested) return false;
	retryRequested = false;
	return true;
}