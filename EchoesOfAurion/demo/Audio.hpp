#ifndef AUDIO_HPP
#define AUDIO_HPP

enum MusicTrack
{
	MUSIC_NONE = 0,
	MUSIC_MENU,      // main menu, settings, credits, name input
	MUSIC_KITCHEN,   // cooking + serving
	MUSIC_FOREST,    // the lost trail
	MUSIC_TEMPLE,    // ghost chase + mirror hall
	MUSIC_FINAL      // the final fight
};

void playMusic(MusicTrack t);
void stopMusic();
void playClick();

void setMusicEnabled(bool on);
void setSfxEnabled(bool on);

#endif