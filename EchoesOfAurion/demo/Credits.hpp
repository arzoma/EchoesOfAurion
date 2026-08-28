#ifndef CREDITS_HPP
#define CREDITS_HPP

class Credits{

private:

	int creditsBackground;

	int frame20;
	int frame22;
	int frame28;

	int hoverFrame20;
	int hoverFrame22;
	int hoverFrame28;

	int backButton;
	int hoverBackButton;

	int infoBox20;
	int infoBox22;
	int infoBox28;

	int hoverSearch20;
	int hoverSearch22;
	int hoverSearch28;

	int openInfoBox;

	int mouseX;
	int mouseY;

public:

	void loadImages();

	void draw();

	void mouseMove(int mx, int my);

	int mouseClick(int button, int state, int mx, int my);

};

#endif