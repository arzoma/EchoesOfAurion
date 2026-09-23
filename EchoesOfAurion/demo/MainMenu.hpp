#ifndef MAINMENU_HPP
#define MAINMENU_HPP

const int MENU_ORBS = 16;

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

	int orbImg;
	int glowSoftImg;
	int shootingStarImg;

	int fxTick;

	double orbX[MENU_ORBS];
	double orbY[MENU_ORBS];
	double orbSpeed[MENU_ORBS];
	double orbPhase[MENU_ORBS];
	int    orbSize[MENU_ORBS];
	int orbLife[MENU_ORBS];
	int orbMax[MENU_ORBS];

	double starX, starY, starScale;
	int starTimer;
	bool starActive;
	int starLife;

	void initFx();
	void updateFx();
	void drawFx();

public:

	void loadImages();

	void draw();

	void mouseMove(int mx, int my);

	int mouseClick(int button, int state, int mx, int my);

};

#endif