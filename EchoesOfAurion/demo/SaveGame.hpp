#ifndef SAVEGAME_HPP
#define SAVEGAME_HPP

// saves the last checkpoint, not exact position + also saves inventory items

enum Checkpoint
{
	SAVE_NONE = 0,
	SAVE_VILLAGE = 1, // arrived in emberfall
	SAVE_RESTAURANT = 2, // finished a restaurant mini-game
	SAVE_FOREST = 3, // arrived in silverleaf
	SAVE_MOONVEIL = 4, // finished the forest trail game, heading to the temple
	SAVE_ASTRAL = 5 // finished a moonveil mini-game, the way to astral sanctum is open
};

struct SaveData
{
	int version;
	char name[30];
	int checkpoint;

	int hasPendant;
	int hasHoodedGift;
	int hasCompass;
	int hasStarwheel;
	int hasCharm;
};

bool saveExists();

bool writeSave(SaveData data);

bool readSave(SaveData &out);

void clearSave();

#endif