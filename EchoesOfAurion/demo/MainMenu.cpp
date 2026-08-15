#include "MainMenu.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

void MainMenu::loadImages()
{
	background = iLoadImage("Images//main_menu_bg.png");

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
}

void MainMenu::draw()
{
	// background
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, background);

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