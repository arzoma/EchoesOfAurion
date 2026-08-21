#include "Credits.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

const int FRAME_WIDTH = 346;
const int FRAME_HEIGHT = 614;

const int FRAME_20_X = 92;
const int FRAME_20_Y = 15;

const int FRAME_22_X = 467;
const int FRAME_22_Y = 15;

const int FRAME_28_X = 842;
const int FRAME_28_Y = 15;

void Credits::loadImages()
{
	creditsBackground = iLoadImage("Images//credits_bg.png");

	frame20 = iLoadImage("Images//credits_20.png");
	frame22 = iLoadImage("Images//credits_22.png");
	frame28 = iLoadImage("Images//credits_28.png");

	hoverFrame20 = iLoadImage("Images//hover_credits_20.png");
	hoverFrame22 = iLoadImage("Images//hover_credits_22.png");
	hoverFrame28 = iLoadImage("Images//hover_credits_28.png");
}

void Credits::draw()
{
	// background
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, creditsBackground);

	// id 20
	if (mouseX >= FRAME_20_X && mouseX <= FRAME_20_X + FRAME_WIDTH &&
		mouseY >= FRAME_20_Y && mouseY <= FRAME_20_Y + FRAME_HEIGHT)
	{
		// hover frame
		iShowImage(FRAME_20_X, FRAME_20_Y, FRAME_WIDTH, FRAME_HEIGHT, hoverFrame20);
	}
	else
	{
		// default frame
		iShowImage(FRAME_20_X, FRAME_20_Y, FRAME_WIDTH, FRAME_HEIGHT, frame20);
	}

	// id 22
	if (mouseX >= FRAME_22_X && mouseX <= FRAME_22_X + FRAME_WIDTH &&
		mouseY >= FRAME_22_Y && mouseY <= FRAME_22_Y + FRAME_HEIGHT)
	{
		// hover frame
		iShowImage(FRAME_22_X, FRAME_22_Y, FRAME_WIDTH, FRAME_HEIGHT, hoverFrame22);
	}
	else
	{
		// default frame
		iShowImage(FRAME_22_X, FRAME_22_Y, FRAME_WIDTH, FRAME_HEIGHT, frame22);
	}

	// id 28
	if (mouseX >= FRAME_28_X && mouseX <= FRAME_28_X + FRAME_WIDTH &&
		mouseY >= FRAME_28_Y && mouseY <= FRAME_28_Y + FRAME_HEIGHT)
	{
		// hover frame
		iShowImage(FRAME_28_X, FRAME_28_Y, FRAME_WIDTH, FRAME_HEIGHT, hoverFrame28);
	}
	else
	{
		// default frame
		iShowImage(FRAME_28_X, FRAME_28_Y, FRAME_WIDTH, FRAME_HEIGHT, frame28);
	}
}

void Credits::mouseMove(int mx, int my)
{
	mouseX = mx;
	mouseY = my;
}

void Credits::mouseClick(int button, int state, int mx, int my)
{

}
