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
	infoButton = iLoadImage("Images//info.png");

	hoverNewGameButton = iLoadImage("Images//hover_new_game.png");
	hoverContinueButton = iLoadImage("Images//hover_continue.png");
	hoverSettingsButton = iLoadImage("Images//hover_settings.png");
	hoverExitButton = iLoadImage("Images//hover_exit.png");
	hoverInfoButton = iLoadImage("Images//hover_info.png");
}

void MainMenu::draw()
{
	// background
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, background);

	// logo
	iShowImage(340, 500, 600, 150, logo);


	// new game
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 330 && mouseY <= 410)
	{
		iShowImage(490, 330, 300, 80, hoverNewGameButton);
	}
	else
	{
		iShowImage(490, 330, 300, 80, newGameButton);
	}


	// continue
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 240 && mouseY <= 320)
	{
		iShowImage(490, 240, 300, 80, hoverContinueButton);
	}
	else
	{
		iShowImage(490, 240, 300, 80, continueButton);
	}


	// settings
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 150 && mouseY <= 230)
	{
		iShowImage(490, 150, 300, 80, hoverSettingsButton);
	}
	else
	{
		iShowImage(490, 150, 300, 80, settingsButton);
	}


	// exit
	if (mouseX >= 490 && mouseX <= 790 && mouseY >= 60 && mouseY <= 140)
	{
		iShowImage(490, 60, 300, 80, hoverExitButton);
	}
	else
	{
		iShowImage(490, 60, 300, 80, exitButton);
	}


	// information
	if (mouseX >= 1200 && mouseX <= 1250 && mouseY >= 20 && mouseY <= 70)
	{
		iShowImage(1200, 20, 50, 50, hoverInfoButton);
	}
	else
	{
		iShowImage(1200, 20, 50, 50, infoButton);
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
	if (mx >= 490 && mx <= 790 && my >= 330 && my <= 410)
	{
		return 1;
	}


	// continue
	if (mx >= 490 && mx <= 790 && my >= 240 && my <= 320)
	{
		return 2;
	}


	// settings
	if (mx >= 490 && mx <= 790 && my >= 150 && my <= 230)
	{
		return 3;
	}


	// exit
	if (mx >= 490 && mx <= 790 && my >= 60 && my <= 140)
	{
		return 4;
	}


	// information
	if (mx >= 1200 && mx <= 1250 && my >= 20 && my <= 70)
	{
		return 5;
	}

	return 0;
}