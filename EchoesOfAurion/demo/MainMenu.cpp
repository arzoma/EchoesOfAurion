#include "MainMenu.hpp"
#include "Constants.hpp"

#include <cstdlib>
#include <cmath>

extern float g_imgAlpha;
extern bool  g_imgAdditive;

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

void MainMenu::loadImages()
{
	menuBackground = iLoadImage("Images//main_menu_bg.png");

	logo = iLoadImage("Images//logo.png");

	newGameButton = iLoadImage("Images//new_game.png");
	continueButton = iLoadImage("Images//continue.png");
	settingsButton = iLoadImage("Images//settings.png");
	exitButton = iLoadImage("Images//exit.png");
	creditsButton = iLoadImage("Images//credits.png");

	hoverNewGameButton = iLoadImage("Images//hover_new_game.png");
	hoverContinueButton = iLoadImage("Images//hover_continue.png");
	hoverSettingsButton = iLoadImage("Images//hover_settings.png");
	hoverExitButton = iLoadImage("Images//hover_exit.png");
	hoverCreditsButton = iLoadImage("Images//hover_credits.png");

	orbImg = iLoadImage("Images//fx_orb.png");
	glowSoftImg = iLoadImage("Images//fx_glow_soft.png");
	shootingStarImg = iLoadImage("Images//fx_shooting_star.png");

	initFx();
}

void MainMenu::initFx()
{
	fxTick = 0;

	for (int i = 0; i < MENU_ORBS; i++)
	{
		orbX[i] = (double)(rand() % SCREEN_WIDTH);
		orbY[i] = (double)(rand() % 300);
		orbSpeed[i] = 0.05 + (rand() % 10) / 100.0;
		orbPhase[i] = (rand() % 628) / 100.0;
		orbSize[i] = 14 + rand() % 24;
		orbMax[i] = 600 + rand() % 700;
		orbLife[i] = rand() % orbMax[i];
	}

	starActive = false;
	starTimer = 200;
	starX = 0.0;
	starY = 0.0;
	starScale = 0.45;
}

void MainMenu::updateFx()
{
	fxTick++;

	for (int i = 0; i < MENU_ORBS; i++)
	{
		orbY[i] += orbSpeed[i];
		orbLife[i]++;

		if (orbLife[i] >= orbMax[i])
		{
			orbLife[i] = 0;
			orbX[i] = (double)(rand() % SCREEN_WIDTH);
			orbY[i] = (double)(rand() % 300);
			orbSpeed[i] = 0.05 + (rand() % 10) / 100.0;
			orbPhase[i] = (rand() % 628) / 100.0;
			orbSize[i] = 14 + rand() % 24;
			orbMax[i] = 600 + rand() % 700;
		}
	}

	if (starActive)
	{
		starX -= 0.888 * 2.4;
		starY -= 0.460 * 2.4;
		starLife++;

		if (starLife >= 300) starActive = false;
	}
	else
	{
		starTimer--;

		if (starTimer <= 0)
		{
			starActive = true;
			starLife = 0;
			starX = (double)(1150 + rand() % 250);
			starY = (double)(700 + rand() % 120);
			starScale = 0.35 + (rand() % 25) / 100.0;
			starTimer = 480 + rand() % 600;
		}
	}
}

void MainMenu::drawFx()
{
	for (int i = 0; i < MENU_ORBS; i++)
	{
		double t = (double)orbLife[i] / (double)orbMax[i];

		float a;
		if (t < 0.25) a = (float)(t / 0.25);
		else if (t > 0.65) a = (float)(1.0 - (t - 0.65) / 0.35);
		else a = 1.0f;

		if (a < 0.0f) a = 0.0f;

		double tw = 0.75 + 0.25 * sin(fxTick * 0.02 + orbPhase[i]);
		double dx = sin(fxTick * 0.006 + orbPhase[i]) * 18.0;

		g_imgAdditive = true;
		g_imgAlpha = a * (float)tw * 0.7f;
		iShowImage((int)(orbX[i] + dx) - orbSize[i] / 2, (int)orbY[i],
			orbSize[i], orbSize[i], orbImg);
	}

	g_imgAdditive = true;
	g_imgAlpha = 0.10f + 0.04f * (float)sin(fxTick * 0.010);
	iShowImage(110, 40, 280, 280, glowSoftImg);

	g_imgAdditive = true;
	g_imgAlpha = 0.08f + 0.03f * (float)sin(fxTick * 0.008 + 2.0);
	iShowImage(860, 20, 240, 240, glowSoftImg);

	if (starActive)
	{
		double t = starLife / 300.0;

		float a;
		if (t < 0.20)      a = (float)(t / 0.20);
		else if (t > 0.60) a = (float)(1.0 - (t - 0.60) / 0.40);
		else               a = 1.0f;

		if (a < 0.0f) a = 0.0f;

		g_imgAdditive = true;
		g_imgAlpha = a * 0.9f;
		iShowImage((int)starX, (int)starY,
			(int)(800 * starScale), (int)(440 * starScale), shootingStarImg);
	}
}

void MainMenu::draw()
{
	// background
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, menuBackground);

	// logo
	iShowImage(390, 420, 500, 281, logo);


	// new game
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 375 && mouseY <= 425)
	{
		iShowImage(490, 315, 300, 169, hoverNewGameButton);
	}
	else
	{
		iShowImage(490, 315, 300, 169, newGameButton);
	}


	// continue
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 295 && mouseY <= 345)
	{
		iShowImage(490, 235, 300, 169, hoverContinueButton);
	}
	else
	{
		iShowImage(490, 235, 300, 169, continueButton);
	}


	// settings
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 215 && mouseY <= 265)
	{
		iShowImage(490, 155, 300, 169, hoverSettingsButton);
	}
	else
	{
		iShowImage(490, 155, 300, 169, settingsButton);
	}


	// credits
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 135 && mouseY <= 185)
	{
		iShowImage(490, 75, 300, 169, hoverCreditsButton);
	}
	else
	{
		iShowImage(490, 75, 300, 169, creditsButton);
	}


	// exit
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 55 && mouseY <= 105)
	{
		iShowImage(490, -5, 300, 169, hoverExitButton);
	}
	else
	{
		iShowImage(490, -5, 300, 169, exitButton);
	}

	updateFx();
	drawFx();
}

void MainMenu::mouseMove(int mx, int my)
{
	mouseX = mx;
	mouseY = my;
}

int MainMenu::mouseClick(int button, int state, int mx, int my)
{

	// new game
	if (mx >= 490 && mx <= 790 && my >= 375 && my <= 425)
	{
		return 1;
	}


	// continue
	if (mx >= 490 && mx <= 790 && my >= 295 && my <= 345)
	{
		return 2;
	}


	// settings
	if (mx >= 490 && mx <= 790 && my >= 215 && my <= 265)
	{
		return 3;
	}


	// credits
	if (mx >= 490 && mx <= 790 && my >= 135 && my <= 185)
	{
		return 4;
	}


	// exit
	if (mx >= 490 && mx <= 790 && my >= 55 && my <= 105)
	{
		return 5;
	}

	return 0;
}