#ifndef SETTINGS_HPP
#define SETTINGS_HPP

class Settings{

private:

	int settingsBackground;

	int labelMusic;
	int labelSfx;

	int toggleMusicOn;
	int toggleMusicOff;
	int toggleSfxOn;
	int toggleSfxOff;

	bool musicOn = true;
	bool sfxOn = true;

	int mouseX;
	int mouseY;

public:

	void loadImages();

	void draw();

	void mouseMove(int mx, int my);

	int mouseClick(int button, int state, int mx, int my);

	bool isMusicOn();
	bool isSfxOn();

};

#endif