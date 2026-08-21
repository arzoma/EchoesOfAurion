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

	int mouseX;
	int mouseY;

public:

	void loadImages();

	void draw();

	void mouseMove(int mx, int my);

	void mouseClick(int button, int state, int mx, int my);

};

#endif