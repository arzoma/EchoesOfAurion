#include "Settings.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);


const int LABEL_WIDTH = 280;
const int LABEL_HEIGHT = 158;   

const int LABEL_MUSIC_X = 129;
const int LABEL_MUSIC_Y = 267;

const int LABEL_SFX_X = 129;
const int LABEL_SFX_Y = 396;


const int TOGGLE_WIDTH = 160;
const int TOGGLE_HEIGHT = 90;    

const int TOGGLE_MUSIC_X = 803;
const int TOGGLE_MUSIC_Y = 279;

const int TOGGLE_SFX_X = 803;
const int TOGGLE_SFX_Y = 445;

void Settings::loadImages()
{
	settingsBackground = iLoadImage("Images//settings_bg.png");

	labelMusic = iLoadImage("Images//music.png");
	labelSfx = iLoadImage("Images//sound_effects.png");

	toggleMusicOn = iLoadImage("Images//toggle_button_on.png");
	toggleMusicOff = iLoadImage("Images//toggle_button_off.png");
	toggleSfxOn = iLoadImage("Images//toggle_button_on.png");
	toggleSfxOff = iLoadImage("Images//toggle_button_off.png");
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
}

void Settings::mouseClick(int button, int state, int x, int y)
{
	// toggle music
	if (x >= TOGGLE_MUSIC_X && x <= TOGGLE_MUSIC_X + TOGGLE_WIDTH &&
		y >= TOGGLE_MUSIC_Y && y <= TOGGLE_MUSIC_Y + TOGGLE_HEIGHT)
	{
		musicOn = !musicOn;
	}

	// toggle sfx
	if (x >= TOGGLE_SFX_X && x <= TOGGLE_SFX_X + TOGGLE_WIDTH &&
		y >= TOGGLE_SFX_Y && y <= TOGGLE_SFX_Y + TOGGLE_HEIGHT)
	{
		sfxOn = !sfxOn;
	}
}