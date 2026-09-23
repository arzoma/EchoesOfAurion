#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstring>
#include "SaveGame.hpp"

static const int SAVE_VERSION = 1;

bool writeSave(SaveData data)
{
	FILE *f = fopen("savegame.txt", "w");

	if (f == NULL)
	{
		return false;
	}

	fprintf(f, "%d\n", SAVE_VERSION);
	fprintf(f, "%s\n", data.name);
	fprintf(f, "%d\n", data.checkpoint);
	fprintf(f, "%d %d %d %d %d\n",
		data.hasPendant,
		data.hasHoodedGift,
		data.hasCompass,
		data.hasStarwheel,
		data.hasCharm);

	fclose(f);
	return true;
}

bool readSave(SaveData &out)
{
	FILE *f = fopen("savegame.txt", "r");

	if (f == NULL)
	{
		return false;
	}

	SaveData d;

	d.version = 0;
	d.checkpoint = 0;
	d.hasPendant = 0;
	d.hasHoodedGift = 0;
	d.hasCompass = 0;
	d.hasStarwheel = 0;
	d.hasCharm = 0;
	strcpy(d.name, "Traveller");

	int read = 0;

	read = fscanf(f, "%d", &d.version);
	if (read != 1) { fclose(f); return false; }

	read = fscanf(f, "%s", d.name);
	if (read != 1) { fclose(f); return false; }

	read = fscanf(f, "%d", &d.checkpoint);
	if (read != 1) { fclose(f); return false; }

	read = fscanf(f, "%d %d %d %d %d",
		&d.hasPendant,
		&d.hasHoodedGift,
		&d.hasCompass,
		&d.hasStarwheel,
		&d.hasCharm);
	if (read != 5) { fclose(f); return false; }

	fclose(f);

	if (d.version != SAVE_VERSION) return false;
	if (d.checkpoint <= SAVE_NONE) return false;
	if (d.checkpoint > SAVE_ASTRAL) return false;

	out = d;
	return true;
}

bool saveExists()
{
	SaveData tmp;
	return readSave(tmp);
}

void clearSave()
{
	remove("savegame.txt");
}