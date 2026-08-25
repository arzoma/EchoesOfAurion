#include <cstdio>
#include "iGraphics.h"
#include "Balloon.hpp"
#include "Utils.hpp"
#include "MainMenu.hpp"
#include "Settings.hpp"
#include "Credits.hpp"
#include "Constants.hpp"

const int TOTAL_BALLOON = 5;
Balloon balloons[TOTAL_BALLOON];

int hit = 0;
int miss = 0;
char scoreText[50];

enum GameState{

	MAIN_MENU,
	GAMEPLAY,
	SETTINGS,
	CREDITS,

};

GameState currentState = MAIN_MENU;

MainMenu mainMenu;

Settings settings;

Credits credits;

void iDraw()
{
	iClear();

	switch (currentState)
	{
	case MAIN_MENU:

		mainMenu.draw();

		break;


	case GAMEPLAY:

		iSetColor(255, 255, 255);
		iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		iSetColor(0, 0, 0);

		sprintf_s(scoreText, "HIT : %d", hit);
		iText(50, 500, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

		sprintf_s(scoreText, "MISS : %d", miss);
		iText(50, 550, scoreText, GLUT_BITMAP_TIMES_ROMAN_24 );

		for (int i = 0; i < TOTAL_BALLOON; i++)
		{
			iShowImage(
				balloons[i].x,
				balloons[i].y,
				BALLOON_WIDTH,
				BALLOON_HEIGHT,
				balloons[i].image
				);
		}

		break;


	case SETTINGS:

		settings.draw();

		break;


	case CREDITS:

		credits.draw();

		break;
	}
}


void iMouseMove(int mx, int my)
{
	
	if (currentState == MAIN_MENU){

		mainMenu.mouseMove(mx, my);

	}

	if (currentState == SETTINGS){

		settings.mouseMove(mx, my);

	}

	if (currentState == CREDITS){

		credits.mouseMove(mx, my);

	}

}

void iPassiveMouseMove(int mx, int my)
{
	
	if (currentState == MAIN_MENU){

		mainMenu.mouseMove(mx, my);

	}

	if (currentState == SETTINGS){

		settings.mouseMove(mx, my);

	}

	if (currentState == CREDITS){

		credits.mouseMove(mx, my);

	}

}

void iMouse(int button, int state, int mx, int my)
{
	if (currentState == MAIN_MENU)
	{
		int result = mainMenu.mouseClick(button, state, mx, my);

		switch (result)
		{
		case 1:
			// new game
			currentState = GAMEPLAY;
			break;

		case 2:
			// continue
			// to be added
			break;

		case 3:
			// settings
			currentState = SETTINGS;
			break;

		case 4:
			// credits
			currentState = CREDITS;
			break;
		
		case 5:
			// exit
			exit(0);
			break;

		}

		return;
	}

	//settings
	if (currentState == SETTINGS){

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			int result = settings.mouseClick(mx, my);

			if (result == 1)
			{
				// back button clicked
				currentState = MAIN_MENU;
			}
		}

	}

	// credits
	if (currentState == CREDITS){

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			int result = credits.mouseClick(button, state, mx, my);

			if (result == 1)
			{
				// back button clicked
				currentState = MAIN_MENU;
			}
		}

	}

	// gameplay
	if (currentState == GAMEPLAY)
	{
		if (button == GLUT_LEFT_BUTTON &&
			state == GLUT_DOWN)
		{
			for (int i = 0; i < TOTAL_BALLOON; ++i)
			{
				if (balloons[i].isHit(mx, my))
				{
					balloons[i].resetBallon();
					++hit;
				}
			}
		}
	}
}


// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, 
// GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT

void fixedUpdate()
{
	if (isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP))
	{
		
	}
	if (isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT))
	{
		
	}
	if (isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		
	}
	if (isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT))
	{
		
	}

	if (isKeyPressed(' ')) {
		// Playing the audio once
		//mciSendString("play ggsong from 0", NULL, 0, NULL);
	}
}

void update(){
	for (int i = 0; i < TOTAL_BALLOON; i++){
		
		bool isOutOfBound = balloons[i].moveUp();

		if (isOutOfBound){
			miss++;
		}
	}
}

void initBalloons() {
	for (int i = 0; i < TOTAL_BALLOON; ++i) {
		char imageSource[100];
		sprintf_s(imageSource, "Images//balloon%d.png", i + 1);
		int image = iLoadImage(imageSource);
		balloons[i] = Balloon(image);
	}
}


int main()
{

	iSetTimer(10, update);
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Echoes of Aurion");
	
	mainMenu.loadImages();
	settings.loadImages();
	credits.loadImages();

	initBalloons();
	
	iStart();
	return 0;
}