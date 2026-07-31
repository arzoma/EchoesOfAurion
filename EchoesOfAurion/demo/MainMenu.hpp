#ifndef MAINMENU_HPP
#define MAINMENU_HPP

class MainMenu{

private:

	int background;
	int logo;

	int newGameButton;
	int continueButton;
	int settingsButton;
	int exitButton;
	int infoButton;

	int hoverNewGameButton;
	int hoverContinueButton;
	int hoverSettingsButton;
	int hoverExitButton;
	int hoverInfoButton;

	int mouseX;
	int mouseY;

public:

	void loadImages();

	void draw();

	void mouseMove(int mx, int my);

	int mouseClick(int button, int state, int mx, int my);

};

#endif