#include "Settings.hpp"
#include "Constants.hpp"
#include "Audio.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

const int LABEL_WIDTH = 280;
const int LABEL_HEIGHT = 158;   

const int LABEL_MUSIC_X = 270;
const int LABEL_MUSIC_Y = 315;

const int LABEL_SFX_X = 270;
const int LABEL_SFX_Y = 215;


const int TOGGLE_WIDTH = 110;
const int TOGGLE_HEIGHT = 61;    

const int TOGGLE_MUSIC_X = 860;
const int TOGGLE_MUSIC_Y = 365;

const int TOGGLE_SFX_X = 860;
const int TOGGLE_SFX_Y = 265;

const int BACK_BUTTON_SIZE = 70;
const int BACK_BUTTON_X = 30;
const int BACK_BUTTON_Y = SCREEN_HEIGHT - BACK_BUTTON_SIZE - 30;

void Settings::loadImages()
{
	settingsBackground = iLoadImage("Images//settings_bg.png");

	labelMusic = iLoadImage("Images//music.png");
	labelSfx = iLoadImage("Images//sound_effects.png");

	toggleMusicOn = iLoadImage("Images//toggle_button_on.png");
	toggleMusicOff = iLoadImage("Images//toggle_button_off.png");
	toggleSfxOn = iLoadImage("Images//toggle_button_on.png");
	toggleSfxOff = iLoadImage("Images//toggle_button_off.png");

	backButton = iLoadImage("Images//back_button.png");
	hoverBackButton = iLoadImage("Images//hover_back_button.png");

}

void Settings::draw()
{
	// background
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, settingsBackground);

	// labels
	iShowImage(LABEL_MUSIC_X, LABEL_MUSIC_Y, LABEL_WIDTH, LABEL_HEIGHT, labelMusic);
	iShowImage(LABEL_SFX_X, LABEL_SFX_Y, LABEL_WIDTH, LABEL_HEIGHT, labelSfx);

	// music toggle
	if (musicOn)
	{
		iShowImage(TOGGLE_MUSIC_X, TOGGLE_MUSIC_Y, TOGGLE_WIDTH, TOGGLE_HEIGHT, toggleMusicOn);
	}
	else
	{
		iShowImage(TOGGLE_MUSIC_X, TOGGLE_MUSIC_Y, TOGGLE_WIDTH, TOGGLE_HEIGHT, toggleMusicOff);
	}

	// sfx toggle
	if (sfxOn)
	{
		iShowImage(TOGGLE_SFX_X, TOGGLE_SFX_Y, TOGGLE_WIDTH, TOGGLE_HEIGHT, toggleSfxOn);
	}
	else
	{
		iShowImage(TOGGLE_SFX_X, TOGGLE_SFX_Y, TOGGLE_WIDTH, TOGGLE_HEIGHT, toggleSfxOff);
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

void Settings::mouseMove(int mx, int my)
{
	mouseX = mx;
	mouseY = my;
}

int Settings::mouseClick(int mx, int my)
{
	// toggle music
	if (mx >= TOGGLE_MUSIC_X && mx <= TOGGLE_MUSIC_X + TOGGLE_WIDTH &&
		my >= TOGGLE_MUSIC_Y && my <= TOGGLE_MUSIC_Y + TOGGLE_HEIGHT)
	{
		musicOn = !musicOn;
		setMusicEnabled(musicOn);
		playClick();
	}

	// toggle sfx
	if (mx >= TOGGLE_SFX_X && mx <= TOGGLE_SFX_X + TOGGLE_WIDTH &&
		my >= TOGGLE_SFX_Y && my <= TOGGLE_SFX_Y + TOGGLE_HEIGHT)
	{
		sfxOn = !sfxOn;
		setSfxEnabled(sfxOn);
		if (sfxOn) playClick();
	}

	// back button
	if (mx >= BACK_BUTTON_X && mx <= BACK_BUTTON_X + BACK_BUTTON_SIZE &&
		my >= BACK_BUTTON_Y && my <= BACK_BUTTON_Y + BACK_BUTTON_SIZE)
	{
		playClick();
		return 1;
	}

	return 0;
}