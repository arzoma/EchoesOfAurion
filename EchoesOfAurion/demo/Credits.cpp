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

const int BACK_BUTTON_SIZE = 70;
const int BACK_BUTTON_X = 30;
const int BACK_BUTTON_Y = SCREEN_HEIGHT - BACK_BUTTON_SIZE - 30;

const int SEARCH_ICON_SIZE = 56;
const int SEARCH_ICON_OFFSET_X = FRAME_WIDTH / 2 - SEARCH_ICON_SIZE / 2;
const int SEARCH_ICON_OFFSET_Y = 50 + SEARCH_ICON_SIZE / 2;

const int SEARCH_20_X = FRAME_20_X + SEARCH_ICON_OFFSET_X;
const int SEARCH_20_Y = FRAME_20_Y + SEARCH_ICON_OFFSET_Y;

const int SEARCH_22_X = FRAME_22_X + SEARCH_ICON_OFFSET_X;
const int SEARCH_22_Y = FRAME_22_Y + SEARCH_ICON_OFFSET_Y;

const int SEARCH_28_X = FRAME_28_X + SEARCH_ICON_OFFSET_X;
const int SEARCH_28_Y = FRAME_28_Y + SEARCH_ICON_OFFSET_Y;

void Credits::loadImages()
{
	creditsBackground = iLoadImage("Images//credits_bg.png");

	frame20 = iLoadImage("Images//credits_20.png");
	frame22 = iLoadImage("Images//credits_22.png");
	frame28 = iLoadImage("Images//credits_28.png");

	hoverFrame20 = iLoadImage("Images//hover_credits_20.png");
	hoverFrame22 = iLoadImage("Images//hover_credits_22.png");
	hoverFrame28 = iLoadImage("Images//hover_credits_28.png");

	backButton = iLoadImage("Images//back_button.png");
	hoverBackButton = iLoadImage("Images//hover_back_button.png");

	infoBox20 = iLoadImage("Images//credits_20_info_box.png");
	infoBox22 = iLoadImage("Images//credits_22_info_box.png");
	infoBox28 = iLoadImage("Images//credits_28_info_box.png");

	hoverSearch20 = iLoadImage("Images//hover_credits_search_20.png");
	hoverSearch22 = iLoadImage("Images//hover_credits_search_22.png");
	hoverSearch28 = iLoadImage("Images//hover_credits_search_28.png");

	openInfoBox = 0;

}

void Credits::draw()
{
	// background
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, creditsBackground);

	// id 20
	if (openInfoBox == 20)
	{
		// info box open
		iShowImage(FRAME_20_X, FRAME_20_Y, FRAME_WIDTH, FRAME_HEIGHT, infoBox20);
	}
	else if (mouseX >= SEARCH_20_X && mouseX <= SEARCH_20_X + SEARCH_ICON_SIZE &&
		mouseY >= SEARCH_20_Y && mouseY <= SEARCH_20_Y + SEARCH_ICON_SIZE)
	{
		// hover search icon
		iShowImage(FRAME_20_X, FRAME_20_Y, FRAME_WIDTH, FRAME_HEIGHT, hoverSearch20);
	}
	else if (mouseX >= FRAME_20_X && mouseX <= FRAME_20_X + FRAME_WIDTH &&
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
	if (openInfoBox == 22)
	{
		// info box open
		iShowImage(FRAME_22_X, FRAME_22_Y, FRAME_WIDTH, FRAME_HEIGHT, infoBox22);
	}
	else if (mouseX >= SEARCH_22_X && mouseX <= SEARCH_22_X + SEARCH_ICON_SIZE &&
		mouseY >= SEARCH_22_Y && mouseY <= SEARCH_22_Y + SEARCH_ICON_SIZE)
	{
		// hover search icon
		iShowImage(FRAME_22_X, FRAME_22_Y, FRAME_WIDTH, FRAME_HEIGHT, hoverSearch22);
	}
	else if (mouseX >= FRAME_22_X && mouseX <= FRAME_22_X + FRAME_WIDTH &&
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
	if (openInfoBox == 28)
	{
		// info box open
		iShowImage(FRAME_28_X, FRAME_28_Y, FRAME_WIDTH, FRAME_HEIGHT, infoBox28);
	}
	else if (mouseX >= SEARCH_28_X && mouseX <= SEARCH_28_X + SEARCH_ICON_SIZE &&
		mouseY >= SEARCH_28_Y && mouseY <= SEARCH_28_Y + SEARCH_ICON_SIZE)
	{
		// hover search icon
		iShowImage(FRAME_28_X, FRAME_28_Y, FRAME_WIDTH, FRAME_HEIGHT, hoverSearch28);
	}
	else if (mouseX >= FRAME_28_X && mouseX <= FRAME_28_X + FRAME_WIDTH &&
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

	// back button
	if (mouseX >= BACK_BUTTON_X && mouseX <= BACK_BUTTON_X + BACK_BUTTON_SIZE &&
		mouseY >= BACK_BUTTON_Y && mouseY <= BACK_BUTTON_Y + BACK_BUTTON_SIZE)
	{
		// hover
		iShowImage(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_SIZE, BACK_BUTTON_SIZE, hoverBackButton);
	}
	else
	{
		// default
		iShowImage(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_SIZE, BACK_BUTTON_SIZE, backButton);
	}

}

void Credits::mouseMove(int mx, int my)
{
	mouseX = mx;
	mouseY = my;
}

int Credits::mouseClick(int button, int state, int mx, int my)
{
	if (mx >= BACK_BUTTON_X && mx <= BACK_BUTTON_X + BACK_BUTTON_SIZE &&
		my >= BACK_BUTTON_Y && my <= BACK_BUTTON_Y + BACK_BUTTON_SIZE)
	{
		openInfoBox = 0;
		return 1;
	}

	// id 20 search icon
	if (mx >= SEARCH_20_X && mx <= SEARCH_20_X + SEARCH_ICON_SIZE &&
		my >= SEARCH_20_Y && my <= SEARCH_20_Y + SEARCH_ICON_SIZE)
	{
		openInfoBox = 20;
		return 0;
	}

	// id 22 search icon
	if (mx >= SEARCH_22_X && mx <= SEARCH_22_X + SEARCH_ICON_SIZE &&
		my >= SEARCH_22_Y && my <= SEARCH_22_Y + SEARCH_ICON_SIZE)
	{
		openInfoBox = 22;
		return 0;
	}

	// id 28 search icon
	if (mx >= SEARCH_28_X && mx <= SEARCH_28_X + SEARCH_ICON_SIZE &&
		my >= SEARCH_28_Y && my <= SEARCH_28_Y + SEARCH_ICON_SIZE)
	{
		openInfoBox = 28;
		return 0;
	}

	// clicking anywhere else to minimize open info box
	openInfoBox = 0;

	return 0;
}
