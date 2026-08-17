#ifndef MAINMENU_HPP
#define MAINMENU_HPP

class MainMenu{

private:

	int menuBackground;
	int logo;

	int newGameButton;
	int continueButton;
	int settingsButton;
	int creditsButton;
	int exitButton;

	int hoverNewGameButton;
	int hoverContinueButton;
	int hoverSettingsButton;
	int hoverCreditsButton;
	int hoverExitButton;

	int mouseX;
	int mouseY;

public:

	void loadImages();

	void draw();

	void mouseMove(int mx, int my);

	int mouseClick(int button, int state, int mx, int my);

};

#endif