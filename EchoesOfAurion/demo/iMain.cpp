#include <cstdio>
#include <cstdlib>
#include "iGraphics.h"
#include "Utils.hpp"
#include "MainMenu.hpp"
#include "Settings.hpp"
#include "Credits.hpp"
#include "Constants.hpp"
#include "NameInput.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "Npc.hpp"
#include "DialogueBox.hpp"
#include "Inventory.hpp"
#include "CookingGame.hpp"
#include "ServingGame.hpp"
#include "Foresttrail.hpp"
#include "Itemobtained.hpp"
#include "GhostChase.hpp"
#include "MirrorHall.hpp"
#include "FinalFight.hpp"
#include "SaveGame.hpp"

enum GameState{

	MAIN_MENU,
	SETTINGS,
	CREDITS,
	NAME_INPUT,
	THRONE_ROOM,
	HALLWAY,
	VILLAGE,
	RESTAURANT,
	COOKING_GAME,
	SERVING_GAME,
	FOREST,
	FOREST_TRAIL,
	MOONVEIL_APPROACH,
	MOONVEIL_HALL,
	MOONVEIL_ROOM,
	MOONVEIL_SANCTUM,
	GHOST_CHASE,
	MIRROR_HALL,
	ASTRAL_FLOOR,
	FINAL_FIGHT,
	ASTRAL_END,
	EPILOGUE,
	TITLE_CARD

};

GameState currentState = MAIN_MENU;

const int FADE_TICKS = 20;

enum FadePhase{

	FADE_NONE,
	FADE_OUT,
	FADE_IN

};

FadePhase fadePhase = FADE_NONE;
int fadeTick = 0;
GameState fadePendingState = MAIN_MENU;

void requestFade(GameState next)
{
	fadePendingState = next;
	fadePhase = FADE_OUT;
	fadeTick = 0;
}

void drawFadeOverlay()
{
	if (fadePhase == FADE_NONE) return;

	float alpha;

	if (fadePhase == FADE_OUT)
	{
		alpha = (float)fadeTick / FADE_TICKS;
	}
	else
	{
		alpha = 1.0f - (float)fadeTick / FADE_TICKS;
	}

	if (alpha < 0.0f)
	{
		alpha = 0.0f;
	}
	if (alpha > 1.0f)
	{
		alpha = 1.0f;
	}

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, alpha);
	
	glBegin(GL_QUADS);

	glVertex2f(0.0f, 0.0f);
	glVertex2f((float)SCREEN_WIDTH, 0.0f);
	glVertex2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	glVertex2f(0.0f, (float)SCREEN_HEIGHT);

	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void updateFade()
{
	if (fadePhase == FADE_NONE) return;

	fadeTick++;

	if (fadePhase == FADE_OUT && fadeTick >= FADE_TICKS)
	{
		currentState = fadePendingState;
		fadePhase = FADE_IN;
		fadeTick = 0;
	}
	else if (fadePhase == FADE_IN && fadeTick >= FADE_TICKS)
	{
		fadePhase = FADE_NONE;
	}
}

char titleLine1[64] = "";
char titleLine2[64] = "";
int titleTick = 0;
GameState titlePendingState = MAIN_MENU;

const int TITLE_L1_IN = 0, TITLE_L1_DONE = 45;
const int TITLE_L2_IN = 70, TITLE_L2_DONE = 115;
const int TITLE_HOLD = 190;

const int TITLE_X = 90;
const int TITLE_Y1 = 150;
const int TITLE_Y2 = 115;

void requestTitleCard(char* l1, char* l2, GameState next)
{
	strcpy_s(titleLine1, l1);
	strcpy_s(titleLine2, l2 ? l2 : "");
	titleTick = 0;
	titlePendingState = next;
	requestFade(TITLE_CARD);
}

void drawFadingText(int x, int y, char* s, float alpha, void* font, int track)
{
	if (alpha <= 0.0f) return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	if (track == 0)
	{
		iText(x, y, s, font);
	}
	else
	{
		char one[2];
		one[1] = '\0';
		int cx = x;

		for (int i = 0; s[i] != '\0'; i++)
		{
			one[0] = s[i];
			iText(cx, y, one, font);
			cx += glutBitmapWidth(font, s[i]) + track;
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
}

float titleAlpha(int tick, int inAt, int doneAt)
{
	if (tick <= inAt) return 0.0f;
	if (tick >= doneAt) return 1.0f;
	return (float)(tick - inAt) / (doneAt - inAt);
}

MainMenu mainMenu;
Settings settings;
Credits credits;
NameInput nameInput;

Player player;
Map throneRoomMap;
Map hallwayMap;
Map villageMap;
Map restaurantMap;

Npc king;
Npc npc1;
Npc npc2;
Npc restaurantNpc;

DialogueBox dialogueBox;
Inventory inventory;

CookingGame cookingGame;
ServingGame servingGame;

Map forestMap;
Npc forestWarden;
Npc herbalist;
ForestTrail forestTrail;

ItemObtained itemObtained;

Map approachMap;
Map hallMap;
Map roomMap;
Map sanctumMap;

Npc mage;

GhostChase ghostChase;
MirrorHall mirrorHall;

int mageBoxImg;

int travelAstralImg, travelAstralHoverImg;

bool showAstralPrompt = false;

int mageStep = -1;
int sanctumStep = -1;

Map astralMap;
Npc guardian;
FinalFight finalFight;

int guardianBoxImg;
int hollowBoxImg;
int cageImg, cageBrokenImg;
int sealFloorImg;
int endCardImg[4];

int astralStep = -1;
int endStep = -1;
int epilogueCard = 0;
int epilogueTick = 0;

bool cageBroken = false;

int mouseX = 0;
int mouseY = 0;

int kingBoxImg;
int villagerBoxImg;
int hoodedManBoxImg;
int restaurantOwnerBoxImg;

int noteAndPendantImg;
bool showingNoteImage = false;

int wardenBoxImg;
int herbalistBoxImg;

int wardenStep = -1;
int herbalistStep = -1;
int activeForestNpc = 0; // 0 none, 1 warden, 2 herbalist

bool showDeepPrompt = false;  // "Enter the Deep Forest"

int travelEmberfallImg, travelEmberfallHoverImg;
int travelForestImg, travelForestHoverImg;
int travelSilverleafImg, travelSilverleafHoverImg;

bool showSilverleafPrompt = false;

int revealJournalImg;
int travelMoonveilImg, travelMoonveilHoverImg;

bool showMoonveilPrompt = false;
bool showingJournalImage = false;

int clearingStep = -1;

int throneStep = 0;  // 0 = king talking, 1 = mc's reply, 2 = moved on
int hallwayStep = 0;
int restaurantStep = -1; // -1 = waiting for restaurant npc click, 0 = restaurant npc's lines, 1 = mc's lines, 2 = the two options

int npc1Step = -1;
int npc2Step = -1;
int activeVillageNpc = 0;

char* kingLines[] = {
	"The situation in Aurion is becoming worse by the day.",
	"The Sacred Seals are weakening, and monsters have begun appearing throughout the kingdom.",
	"Our soldiers are doing everything they can, but the attacks continue.",
	"Your master noticed the changes before anyone else.",
	"He began investigating the seals, but he disappeared before we could learn what he had discovered.",
	"You were his apprentice. You know his work better than anyone else.",
	"So, I need your help.",
	"Find your master, and discover what is happening to the sacred seals."
};
const int KING_LINE_COUNT = 8;

char* mcThroneReplyLines[] = {
	"Yes, Your Majesty. I will find him. I promise."
};
const int MC_THRONE_REPLY_COUNT = 1;

char* hallwayLines1[] = {
	"Master left over two months ago...",
	"He said he was going to investigate Emberfall Village.",
	"But he never came back.",
	"All he left behind is a note and a strange pendant...",
};
const int HALLWAY_LINE_COUNT_1 = 4;

char* hallwayLines2[] = {
	"I have to go find him. If master went to Emberfall, that's where I should start looking."
};
const int HALLWAY_LINE_COUNT_2 = 1;

char* restaurantOwnerLines1[] =
{
	"You must be the Guardian's apprentice. I heard you came to the village looking for him."
};

char* restaurantMcLines1[] =
{
	"Yes. He came to Emberfall not too long ago. Do you know where he went?"
};

char* restaurantOwnerLines2[] =
{
	"I might. But I'm afraid I can't talk right now.",
	"My staff fled after the monsters appeared, and I'm running this place alone."
};

char* restaurantMcLines2[] =
{
	"So you want me to help with the restaurant?"
};

char* restaurantOwnerLines3[] =
{
	"Will you? Help me, and I'll tell you everything I know."
};

char* restaurantMcLines3[] =
{
	"Alright, I'll help."
};

char restaurantOption1[] = "I'll cook.";
char restaurantOption2[] = "I'll serve.";

char* restaurantReturnOwnerLines1[] =
{
	"You did well. Thank you.",
	"I suppose I owe you the information I promised."
};

char* restaurantReturnMcLines1[] =
{
	"What do you know about the guardian?"
};

char* restaurantReturnOwnerLines2[] =
{
	"He came here about two months ago. He seemed worried about the sacred seal.",
	"He asked me about the old road leading into Silverleaf Forest."
};

char* restaurantReturnMcLines2[] =
{
	"The forest? Why?"
};

char* restaurantReturnOwnerLines3[] =
{
	"He said he had reason to believe something was wrong there."
};

char* restaurantReturnMcLines3[] =
{
	"So that's where he went.",
	"Thank you. I'll head there next."
};

char* restaurantReturnOwnerLines4[] =
{
	"You're welcome. And be careful."
};


char* npc1Lines1[] =
{
	"You're the Guardian's apprentice, aren't you?"
};
char* mcNpc1Lines1[] =
{
	"You know who I am?"
};
char* npc1Lines2[] =
{
	"I've heard enough. You're looking for your master."
};
char* mcNpc1Lines2[] =
{
	"Then perhaps you know where he went."
};
char* npc1Lines3[] =
{
	"Perhaps. But I don't make a habit of helping strangers."
};
char* mcNpc1Lines3[] =
{
	"Then why are you talking to me?"
};
char* npc1Lines4[] =
{
	"...",
	"I can make an exception. Take this.",
	"It may prove useful where you're headed."
};
char npc1Option1[] = "Receive the item.";
char npc1Option2[] = "Reject it.";
char* npc1ReceiveLines[] =
{
	"Use it wisely."
};
char* npc1RejectLines[] =
{
	"Suit yourself."
};

char* npc2Lines1[] =
{
	"Oh? What brings a noble to our little village?"
};
char* mcNpc2Lines1[] =
{
	"I'm looking for the Guardian. He came to Emberfall not too long ago.",
	"Have you seen him?"
};
char* npc2Lines2[] =
{
	"I'm afraid I don't know much.",
	"Most people left after the monsters started appearing."
};
char* mcNpc2Lines2[] =
{
	"Do you know anyone who might know something?"
};
char* npc2Lines3[] =
{
	"Try the restaurant over there. The owner was here when the Guardian arrived."
};
char* mcNpc2Lines3[] =
{
	"Thank you. I'll ask him."
};

const int RESTAURANT_NPC_LINE_COUNT = 4;
const int MC_RESTAURANT_LINE_COUNT = 3;

char* wardenLines1[] = { "Stop. The forest isn't safe anymore." };
char* mcWardenLines1[] = { "I'm looking for the Guardian. He passed through here recently." };
char* wardenLines2[] = { "The Guardian... yes. I remember him." };
char* mcWardenLines2[] = { "Do you know where he went?" };
char* wardenLines3[] = { "He asked about the old trail leading deeper into the forest. Said he was investigating something." };
char* mcWardenLines3[] = { "Can you tell me where the trail begins?" };
char* wardenLines4[] =
{
	"No. It's too dangerous.",
	"If you still insist on going, I won't stop you. But find your own way."
};
char* mcWardenLines4[] =
{
	"...",
	"Alright. Thank you."
};

char* herbLines1[] = { "You're looking for the Guardian, aren't you?" };
char* mcHerbLines1[] = { "You know about him?" };
char* herbLines2[] = { "I've heard about what happened. If you're heading deeper into the forest, you should be careful." };
char* mcHerbLines2[] = { "I can handle myself." };
char* herbLines3[] =
{
	"Maybe. But the forest has become unpredictable lately.",
	"Take this. It may help you find your way."
};
char* mcHerbLines3[] = { "A compass?" };
char* herbLines4[] = { "Not an ordinary one. It reacts to traces of magic." };
char* mcHerbLines4[] = { "Why give it to me?" };
char* herbLines5[] = { "Because you'll need all the help you can get." };
char* mcHerbLines5[] = { "...Thank you. I'll make good use of it." };

char* clearingLines1[] =
{
	"A piece of cloth, a page, a magic artifact... and a magic circle someone drew and never finished.",
	"Master was here.",
	"Did he leave all of this behind on purpose...?"
};
const int CLEARING_LINE_COUNT_1 = 3;

char* clearingLines2[] =
{
	"Moonveil Temple... I've heard of it.",
	"It's the oldest place in Aurion. The five mages raised it over ground where the veil between worlds runs thin, and everything they learned about the seals was written down inside it.",
	"But there was a monster attack there not long ago. It's been abandoned ever since.",
	"So that's where master went next. Then that's where I'm going.",
	"And he left the artifact for whoever followed him. It will surely be of help."
};
const int CLEARING_LINE_COUNT_2 = 5;

// moonveil main hall convo
char* mageHall1[] = { "Stop there. The floor past the second pillar doesn't hold anymore." };
char* mcHall1[] = { "Who are you?" };
char* mageHall2[] = { "Someone who was here before you. That's all it needs to be." };
char* mcHall2[] = { "I'm looking for the Guardian. He came this way." };
char* mageHall3[] = { "He came through in the spring. He went down. He did not come up." };
char* mcHall3[] = { "And you let him?" };
char* mageHall4[] =
{
	"I did try to stop him.",
	"Moonveil sits over a thin place. The way through isn't a door you open - it's a condition. The temple has to be awake, and it has been asleep since the attack.",
	"Its lights are out. Put them back and the way opens. I have been trying for two months."
};
char* mcHall4[] = { "Then why hasn't it worked?" };
char* mageHall5[] =
{
	"Because it doesn't answer to me.",
	"It answered to you when you walked in. Did you notice? No. Of course you didn't.",
	"Go on, then. If it wakes for you, come and find me."
};

// moonveil inner sanctum convo
char* mcSanct1[] = { "You again?!" };
char* mageSanct1[] = { "I live here. You're the one wandering around someone else's temple at night." };
char* veylSanct1[] =
{
	"I'm Veyl. Since you'll only keep asking.",
	"There are two ways down to the lights. The undercroft flooded years ago and something moved in. The upper halls are dry, and worse in a different way.",
	"Take this either way. It won't make you brave, but it will buy you three seconds, and three seconds is usually the difference."
};
char charmOption1[] = "Take the charm.";
char charmOption2[] = "I'll manage.";

char* veylTook[] = { "Good. Don't spend it early." };
char* veylRefused[] = { "Suit yourself. You'll want it in an hour." };

char* veylSanct2[] =
{
	"One more thing, and then I'll leave you to it.",
	"When the way opens, it won't be because you forced it. It'll be because something on the other side decided to let you in.",
	"Think about what that means before you step through."
};
char routeOption1[] = "Take the undercroft stairs.";
char routeOption2[] = "Take the upper halls.";

// after the mini-games
char* veylAfter[] =
{
	"The lights are back. I felt it from here.",
	"The way is open, and it opened easily. Remember what I said.",
	"Go on. I'll keep the door behind you."
};

char* veylOpen[] =
{
	"There it is. Six months, and it opens for someone who's been here a day."
};
char* mcOpen[] = { "Come with me." };
char* veylOpen2[] =
{
	"No. Someone has to be on this side of it.",
	"If it closes behind you and there's no one holding the door, it doesn't open again. Not for either of you."
};
char* mcOpen2[] = { "...Then I'll be quick." };
char* veylOpen3[] = { "Don't be quick. Be careful. Quick is how your master went in." };

char* mcCage1[] = { "Master -" };
char* guardCage1[] = { "Don't touch it.", "You came." };
char* mcCage2[] = { "You left a letter and a pendant, and a journal on a rock in the middle of a forest. What was I supposed to do?" };
char* guardCage2[] =
{
	"Not this. I was fairly clear about not this.",
	"Listen. The seals aren't broken. They're unattended. They need someone standing at them, and there has only ever been one of us at a time.",
	"Every generation the prophecy picks one person, and every generation that person is a little further from the five who built this. I'm the weakest Guardian Aurion has ever had. You'll be weaker."
};
char* mcCage3[] = { "That's not encouraging." };
char* guardCage3[] = { "It isn't meant to be. It's the reason this is happening." };
char* hollowCage[] =
{
	"HE HAS NOT SAID MY NAME IN SIX MONTHS.",
	"YOU WILL BE EASIER."
};
char* guardCage4[] = { "Get to a seal. Stand there. Don't listen to it and don't answer it - go." };

// after the fight ends
char* mcEnd1[] = { "Is it - is that it?" };
char* guardEnd1[] = { "That's it. For about forty years, if we're lucky." };
char* mcEnd2[] = { "Forty years." };
char* guardEnd2[] =
{
	"It was never a cure. It's a watch. Someone stands here, and the kingdom gets to not think about it.",
	"That's what I came to tell the King, before I got stubborn and came down here alone instead."
};
char* mcEnd3[] = { "You said I'd be weaker than you." };
char* guardEnd3[] =
{
	"You are. You also did in a week what I couldn't do in six months, and you did not do any of it by yourself.",
	"A villager pointed you at a restaurant. A herbalist gave you a compass. A man who wouldn't tell you his name held a door open for a day and a night.",
	"One Guardian was always the mistake. It just took six hundred years for the mistake to catch up."
};

char* EPILOGUE_TEXT[4] =
{
	"The King was told the truth, which had not happened in some time.",
	"Emberfall stopped emptying. The restaurant hired staff.",
	"Veyl stopped being the only one keeping the temple. He complained about the noise.",
	"And the next time the seals need watching, there will be more than one of us."
};

const int THRONE_PLAYER_X = 595, THRONE_PLAYER_Y = 150;
const int HALLWAY_PLAYER_X = 595, HALLWAY_PLAYER_Y = 300;
const int VILLAGE_SPAWN_X = 1000, VILLAGE_SPAWN_Y = 340;
const int MOONVEIL_SPAWN_X = 600, MOONVEIL_SPAWN_Y = 120;

const int RESTAURANT_PLAYER_X = 214;
const int RESTAURANT_PLAYER_Y = 27;

const int RESTAURANT_NPC_X = 500;
const int RESTAURANT_NPC_Y = 380;

const int RESTAURANT_DOOR_X = 2143;
const int RESTAURANT_DOOR_Y = 1300;
const int RESTAURANT_DOOR_W = 67;
const int RESTAURANT_DOOR_H = 92;

const int RESTAURANT_TRIGGER_X = 2130;
const int RESTAURANT_TRIGGER_Y = 1330;
const int RESTAURANT_TRIGGER_W = 100;
const int RESTAURANT_TRIGGER_H = 27;

const int VILLAGE_EXIT_X = 0;
const int VILLAGE_EXIT_Y = 0;
const int VILLAGE_EXIT_W = 60;
const int VILLAGE_EXIT_H = 60;

const int TRAVEL_IMG_X = 465, TRAVEL_IMG_Y = -24;
const int TRAVEL_HIT_X_MIN = 500, TRAVEL_HIT_X_MAX = 780;
const int TRAVEL_HIT_Y_MIN = 55, TRAVEL_HIT_Y_MAX = 95;

const int FOREST_SPAWN_WX = 761, FOREST_SPAWN_WY = 130;

const int WARDEN_X = 761, WARDEN_Y = 1040;
const int HERBALIST_X = 3020, HERBALIST_Y = 1310;

const int DEEP_NEAR_X = 1630, DEEP_NEAR_Y = 1500, DEEP_NEAR_W = 116, DEEP_NEAR_H = 470;
const int DEEP_ENTER_X = 1630, DEEP_ENTER_Y = 1790, DEEP_ENTER_W = 116, DEEP_ENTER_H = 180;

const int DEEP_PROMPT_X_MIN = 480, DEEP_PROMPT_X_MAX = 800;
const int DEEP_PROMPT_Y_MIN = 72, DEEP_PROMPT_Y_MAX = 115;

const int APPROACH_SPAWN_X = 600, APPROACH_SPAWN_Y = 90;
const int GATE_X = 575, GATE_Y = 2235, GATE_W = 130, GATE_H = 70;

const int HALL_SPAWN_X = 600, HALL_SPAWN_Y = 95;
const int MAGE_HALL_X = 880, MAGE_HALL_Y = 300;
const int HALL_DOOR_X = 1175, HALL_DOOR_Y = 240, HALL_DOOR_W = 60, HALL_DOOR_H = 140;

const int ROOM_SPAWN_X = 142, ROOM_SPAWN_Y = 240;
const int ROOM_DOOR_X = 565, ROOM_DOOR_Y = 86, ROOM_DOOR_W = 155, ROOM_DOOR_H = 80;

const int SANCTUM_SPAWN_X = 622, SANCTUM_SPAWN_Y = 110;
const int MAGE_SANCTUM_X = 300, MAGE_SANCTUM_Y = 300;

const int ASTRAL_SPAWN_X = 1240, ASTRAL_SPAWN_Y = 170;
const int CAGE_X = 1180, CAGE_Y = 1400, CAGE_W = 200, CAGE_H = 200;
const int GUARDIAN_FLOOR_X = 1235, GUARDIAN_FLOOR_Y = 1440;

const int FLOOR_SEAL_COUNT = 5;
const int FLOOR_SEAL[FLOOR_SEAL_COUNT][2] =
{
	{ 531, 1081 },
	{ 1221, 1270 },
	{ 1886, 1081 },
	{ 556, 591 },
	{ 1876, 591 }
};
const int FLOOR_SEAL_SIZE = 128;

const int EPILOGUE_IN = 40, EPILOGUE_HOLD = 260;

bool rectanglesOverlap(Rect a, Rect b)
{
	return
		a.x < b.x + b.w &&
		a.x + a.w > b.x &&
		a.y < b.y + b.h &&
		a.y + a.h > b.y;
}

void saveAt(int checkpoint)
{
	SaveData d;

	strcpy(d.name, nameInput.getName());
	d.version = 0;
	d.checkpoint = checkpoint;

	d.hasPendant = inventory.getHasPendant() ? 1 : 0;
	d.hasHoodedGift = inventory.getHasHoodedGift() ? 1 : 0;
	d.hasCompass = inventory.getHasCompass() ? 1 : 0;
	d.hasStarwheel = inventory.getHasStarwheel() ? 1 : 0;
	d.hasCharm = inventory.getHasCharm() ? 1 : 0;

	writeSave(d);
}

void enterThroneRoom()
{
	//currentState = THRONE_ROOM;
	throneStep = 0;

	player.init(THRONE_PLAYER_X, THRONE_PLAYER_Y);
	player.setFacing(DIR_BACK);

	dialogueBox.startDialogue("King", kingLines, KING_LINE_COUNT, false, kingBoxImg);

	requestTitleCard("Throne Room", "Royal Palace of Aurion", THRONE_ROOM);
}

void enterHallway()
{
	//currentState = HALLWAY;
	hallwayStep = 0;
	showingNoteImage = false;

	player.init(HALLWAY_PLAYER_X, HALLWAY_PLAYER_Y);
	player.setFacing(DIR_FRONT);

	dialogueBox.startDialogue(nameInput.getName(), hallwayLines1, HALLWAY_LINE_COUNT_1, true);

	requestTitleCard("Palace Hallway", 0, HALLWAY);
}

void enterVillage()
{
	//currentState = VILLAGE;

	player.init(VILLAGE_SPAWN_X, VILLAGE_SPAWN_Y);
	player.setFacing(DIR_FRONT);

	villageMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	requestTitleCard("Emberfall Village", 0, VILLAGE);

	saveAt(SAVE_VILLAGE);
}

void enterForest()
{
	wardenStep = -1;
	herbalistStep = -1;
	activeForestNpc = 0;
	showDeepPrompt = false;

	player.init(FOREST_SPAWN_WX, FOREST_SPAWN_WY);
	player.setFacing(DIR_BACK);

	forestMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	requestTitleCard("Silverleaf Forest", 0, FOREST);

	saveAt(SAVE_FOREST);
}

void enterForestTrail()
{
	forestTrail.start(inventory.getHasCompass());
	player.init(FOREST_SPAWN_X, FOREST_SPAWN_Y);
	player.setFacing(DIR_BACK);
	forestTrail.updateCamera(player);

	requestFade(FOREST_TRAIL);
}

/*void enterMoonveil()
{
	showMoonveilPrompt = false;

	player.init(MOONVEIL_SPAWN_X, MOONVEIL_SPAWN_Y);
	player.setFacing(DIR_BACK);

	requestTitleCard("Moonveil Temple", "Abandoned since the attack", MOONVEIL);
}*/

void enterMoonveilApproach()
{
	mageStep = -1;
	sanctumStep = -1;
	showMoonveilPrompt = false;
	showAstralPrompt = false;

	player.init(APPROACH_SPAWN_X, APPROACH_SPAWN_Y);
	player.setFacing(DIR_BACK);

	approachMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	requestTitleCard("Moonveil Temple", "Abandoned since the attack", MOONVEIL_APPROACH);

	saveAt(SAVE_MOONVEIL);
}

void enterMoonveilHall()
{
	player.init(HALL_SPAWN_X, HALL_SPAWN_Y);
	player.setFacing(DIR_BACK);

	mageStep = 0;
	dialogueBox.startDialogue("Mage", mageHall1, 1, false, mageBoxImg);

	requestFade(MOONVEIL_HALL);
}

void enterMoonveilRoom()
{
	player.init(ROOM_SPAWN_X, ROOM_SPAWN_Y);
	player.setFacing(DIR_RIGHT);

	requestFade(MOONVEIL_ROOM);
}

void enterMoonveilSanctum()
{
	mage.init(MAGE_SANCTUM_X, MAGE_SANCTUM_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Veyl");
	
	player.init(SANCTUM_SPAWN_X, SANCTUM_SPAWN_Y);
	player.setFacing(DIR_BACK);

	sanctumStep = 0;
	dialogueBox.startDialogue(nameInput.getName(), mcSanct1, 1, true);

	requestFade(MOONVEIL_SANCTUM);
}

void enterGhostChase()
{
	ghostChase.start(inventory.getHasCharm(), player);
	requestFade(GHOST_CHASE);
}

void enterMirrorHall()
{
	mirrorHall.start(inventory.getHasCharm(), player);
	requestFade(MIRROR_HALL);
}

void returnFromMoonveilGame()
{
	player.init(SANCTUM_SPAWN_X, SANCTUM_SPAWN_Y);
	player.setFacing(DIR_FRONT);

	sanctumStep = 200;
	dialogueBox.startDialogue("Veyl", veylAfter, 3, false, mageBoxImg);

	requestFade(MOONVEIL_SANCTUM);

	saveAt(SAVE_ASTRAL);
}

void enterAstralFloor()
{
	astralStep = -1;
	cageBroken = false;

	guardian.init(GUARDIAN_FLOOR_X, GUARDIAN_FLOOR_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Guardian");

	player.init(ASTRAL_SPAWN_X, ASTRAL_SPAWN_Y);
	player.setFacing(DIR_BACK);

	astralMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	requestTitleCard("The Astral Sanctum", "Where the five seals stand", ASTRAL_FLOOR);
}

void enterFinalFight()
{
	finalFight.start(inventory.getHasStarwheel(), player);
	requestFade(FINAL_FIGHT);
}

void enterAstralEnd()
{
	endStep = 0;
	cageBroken = true;

	guardian.init(980, 300, PLAYER_WIDTH, PLAYER_HEIGHT, "Guardian");

	player.init(1150, 300);
	player.setFacing(DIR_LEFT);

	dialogueBox.startDialogue(nameInput.getName(), mcEnd1, 1, true);

	requestFade(ASTRAL_END);
}

void enterEpilogue()
{
	epilogueCard = 0;
	epilogueTick = 0;
	clearSave();
	requestFade(EPILOGUE);
}

void enterRestaurant()
{
	//currentState = RESTAURANT;

	restaurantStep = -1;

	player.init(RESTAURANT_PLAYER_X, RESTAURANT_PLAYER_Y);
	player.setFacing(DIR_FRONT);

	requestFade(RESTAURANT);
}

void returnToRestaurantAfterMinigame()
{
	restaurantStep = 100;

	player.init(RESTAURANT_PLAYER_X, RESTAURANT_PLAYER_Y);
	player.setFacing(DIR_BACK);

	dialogueBox.startDialogue("Restaurant Owner", restaurantReturnOwnerLines1, 2, false, restaurantOwnerBoxImg);

	requestFade(RESTAURANT);

	saveAt(SAVE_RESTAURANT);
}

void enterCookingGame()
{
	//currentState = COOKING_GAME;
	cookingGame.start(inventory.getHasHoodedGift());
	requestFade(COOKING_GAME);
}

void enterServingGame()
{
	//currentState = SERVING_GAME;
	servingGame.start(inventory.getHasHoodedGift());
	requestFade(SERVING_GAME);
}

void exitRestaurantToVillage()
{
	player.init(2160, 1060);
	player.setFacing(DIR_FRONT);

	villageMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	requestFade(VILLAGE);
}

void confirmName()
{
	enterThroneRoom();
}

bool overTravelPrompt(int mx, int my)
{
	return mx >= TRAVEL_HIT_X_MIN && mx <= TRAVEL_HIT_X_MAX &&
		my >= TRAVEL_HIT_Y_MIN && my <= TRAVEL_HIT_Y_MAX;
}

void drawTravelPrompt(int normalImg, int hoverImg)
{
	bool hov = overTravelPrompt(mouseX, mouseY);
	iShowImage(TRAVEL_IMG_X, TRAVEL_IMG_Y, 350, 197, hov ? hoverImg : normalImg);
}

void continueGame()
{
	SaveData d;
	if (!readSave(d)) return;

	nameInput.setName(d.name);

	if (d.hasPendant)    inventory.givePendant();
	if (d.hasHoodedGift) inventory.giveHoodedGift();
	if (d.hasCompass)    inventory.giveCompass();
	if (d.hasStarwheel)  inventory.giveStarwheel();
	if (d.hasCharm)      inventory.giveCharm();

	switch (d.checkpoint)
	{
	case SAVE_VILLAGE:
		enterVillage();
		break;
	case SAVE_RESTAURANT:
		enterVillage();
		break;
	case SAVE_FOREST:
		enterForest();
		break;
	case SAVE_MOONVEIL:
		enterMoonveilApproach();
		break;
	case SAVE_ASTRAL:
		enterMoonveilSanctum();
		sanctumStep = 200;
		break;
	default:
		enterVillage();
		break;
	}
}

void iDraw()
{
	iClear();

	switch (currentState)
	{
	case MAIN_MENU:
		mainMenu.draw();
		break;


	case SETTINGS:
		settings.draw();
		break;


	case CREDITS:
		credits.draw();
		break;

	case NAME_INPUT:
		nameInput.draw();
		break;

	case TITLE_CARD:
		glDisable(GL_TEXTURE_2D);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f((float)SCREEN_WIDTH, 0);
		glVertex2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
		glVertex2f(0, (float)SCREEN_HEIGHT);
		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		drawFadingText(TITLE_X, TITLE_Y1, titleLine1,
		titleAlpha(titleTick, TITLE_L1_IN, TITLE_L1_DONE),
		GLUT_BITMAP_TIMES_ROMAN_24, 6);

		if (titleLine2[0] != '\0')
		{
			drawFadingText(TITLE_X, TITLE_Y2, titleLine2,
			titleAlpha(titleTick, TITLE_L2_IN, TITLE_L2_DONE),
			GLUT_BITMAP_9_BY_15, 2);
		}

		break;

	case THRONE_ROOM:
		throneRoomMap.draw();
		king.draw(0, 0);
		player.draw(0, 0);
		dialogueBox.draw(mouseX, mouseY);
		break;

	case HALLWAY:
		hallwayMap.draw();
		player.draw(0, 0);

		if (showingNoteImage)
		{
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, noteAndPendantImg);
		}
		else
		{
			dialogueBox.draw(mouseX, mouseY);

			if (hallwayStep == 1)
			{
				drawTravelPrompt(travelEmberfallImg, travelEmberfallHoverImg);
			}

		}

		break;

	case VILLAGE:
		villageMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
		villageMap.draw();
		npc1.draw(villageMap.getCameraX(), villageMap.getCameraY());
		npc2.draw(villageMap.getCameraX(), villageMap.getCameraY());
		player.draw(villageMap.getCameraX(), villageMap.getCameraY());

		char coordText[100];

		sprintf_s(
			coordText,
			"Player: X=%d Y=%d",
			player.getX(),
			player.getY()
			);

		iSetColor(255, 255, 255);
		iText(20, 690, coordText, GLUT_BITMAP_HELVETICA_18);

		dialogueBox.draw(mouseX, mouseY);
		inventory.draw(mouseX, mouseY);
		itemObtained.draw();

		break;

	case RESTAURANT:

		restaurantMap.draw();
		restaurantNpc.draw(0, 0);
		player.draw(0, 0);
		dialogueBox.draw(mouseX, mouseY);
		inventory.draw(mouseX, mouseY);

		if (showSilverleafPrompt && !dialogueBox.isActive())
		{
			drawTravelPrompt(travelSilverleafImg, travelSilverleafHoverImg);
		}

		break;

	case COOKING_GAME:

		cookingGame.draw(mouseX, mouseY);
		break;

	case SERVING_GAME:

		servingGame.draw(mouseX, mouseY);
		break;

	case FOREST:
	{
		forestMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
		forestMap.draw();
		forestWarden.draw(forestMap.getCameraX(), forestMap.getCameraY());
		herbalist.draw(forestMap.getCameraX(), forestMap.getCameraY());
		player.draw(forestMap.getCameraX(), forestMap.getCameraY());

		Rect nearZone = { DEEP_NEAR_X, DEEP_NEAR_Y, DEEP_NEAR_W, DEEP_NEAR_H };

		if (!dialogueBox.isActive() && rectanglesOverlap(player.getFeetRect(), nearZone) && !showDeepPrompt)
		{
			iSetColor(255, 255, 255);
			iText(360, 60, "Something seems to lie in the deeper part of the forest.", GLUT_BITMAP_HELVETICA_18);
		}

		if (showDeepPrompt && !dialogueBox.isActive())
		{
			drawTravelPrompt(travelForestImg, travelForestHoverImg);
		}

		char coordText[100];

		sprintf_s(
			coordText,
			"Player: X=%d Y=%d",
			player.getX(),
			player.getY()
			);

		iSetColor(255, 255, 255);
		iText(20, 690, coordText, GLUT_BITMAP_HELVETICA_18);

		dialogueBox.draw(mouseX, mouseY);
		inventory.draw(mouseX, mouseY);
		itemObtained.draw();

	}

		break;

	case FOREST_TRAIL:
		forestTrail.draw(player, mouseX, mouseY);

		if (forestTrail.isAtClearing())
		{
			if (showingJournalImage)
			{
				iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, revealJournalImg);
			}
			else
			{
				dialogueBox.draw(mouseX, mouseY);

				if (showMoonveilPrompt)
				{
					drawTravelPrompt(travelMoonveilImg, travelMoonveilHoverImg);
				}
			}

			itemObtained.draw();
		}

		break;

	case MOONVEIL_APPROACH:
		approachMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
		approachMap.draw();
		player.draw(approachMap.getCameraX(), approachMap.getCameraY());
		inventory.draw(mouseX, mouseY);
		break;

	case MOONVEIL_HALL:
		hallMap.draw();
		mage.draw(0, 0);
		player.draw(0, 0);
		dialogueBox.draw(mouseX, mouseY);
		inventory.draw(mouseX, mouseY);
		break;

	case MOONVEIL_ROOM:
		roomMap.draw();
		player.draw(0, 0);
		inventory.draw(mouseX, mouseY);
		break;

	case MOONVEIL_SANCTUM:
		sanctumMap.draw();
		mage.draw(0, 0);
		player.draw(0, 0);
		dialogueBox.draw(mouseX, mouseY);
		inventory.draw(mouseX, mouseY);
		itemObtained.draw();

		if (showAstralPrompt && !dialogueBox.isActive())
		{
			drawTravelPrompt(travelAstralImg, travelAstralHoverImg);
		}
		break;

	case GHOST_CHASE:
		ghostChase.draw(player, mouseX, mouseY);
		break;

	case MIRROR_HALL:
		mirrorHall.draw(player, mouseX, mouseY);
		break;

	case ASTRAL_FLOOR:
	{
		astralMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
		astralMap.draw();

		int cx = astralMap.getCameraX();
		int cy = astralMap.getCameraY();

		for (int i = 0; i < FLOOR_SEAL_COUNT; i++)
		{
			iShowImage(FLOOR_SEAL[i][0] - cx, FLOOR_SEAL[i][1] - cy,
			FLOOR_SEAL_SIZE, FLOOR_SEAL_SIZE, sealFloorImg);
		}

		guardian.draw(cx, cy);
		iShowImage(CAGE_X - cx, CAGE_Y - cy, CAGE_W, CAGE_H, cageBroken ? cageBrokenImg : cageImg);

		player.draw(cx, cy);

		dialogueBox.draw(mouseX, mouseY);
		inventory.draw(mouseX, mouseY);
		break;
	}

	case FINAL_FIGHT:
		finalFight.draw(player, mouseX, mouseY);
		break;

	case ASTRAL_END:
	{
		astralMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
		astralMap.draw();

		int cx2 = astralMap.getCameraX();
		int cy2 = astralMap.getCameraY();

		for (int i = 0; i < FLOOR_SEAL_COUNT; i++)
		{
			iShowImage(FLOOR_SEAL[i][0] - cx2, FLOOR_SEAL[i][1] - cy2,
			FLOOR_SEAL_SIZE, FLOOR_SEAL_SIZE, sealFloorImg);
		}

		guardian.draw(cx2, cy2);
		player.draw(cx2, cy2);
		dialogueBox.draw(mouseX, mouseY);
		break;
	}

	case EPILOGUE:
	{
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, endCardImg[epilogueCard]);

		float a = titleAlpha(epilogueTick, EPILOGUE_IN, EPILOGUE_IN + 45);
		drawFadingText(120, 120, EPILOGUE_TEXT[epilogueCard], a, GLUT_BITMAP_TIMES_ROMAN_24, 3);
		break;
	}
}
	drawFadeOverlay();
}


void iMouseMove(int mx, int my)
{
	
	mouseX = mx;
	mouseY = my;

	if (currentState == MAIN_MENU){

		mainMenu.mouseMove(mx, my);

	}

	if (currentState == SETTINGS){

		settings.mouseMove(mx, my);

	}

	if (currentState == CREDITS){

		credits.mouseMove(mx, my);

	}
	if (currentState == NAME_INPUT){

		nameInput.mouseMove(mx, my);

	}

}

void iPassiveMouseMove(int mx, int my)
{
	
	iMouseMove(mx, my);

}

void iMouse(int button, int state, int mx, int my)
{
	printf("%d, %d\n", mx, my);
	
	if (currentState == MAIN_MENU)
	{
		int result = mainMenu.mouseClick(button, state, mx, my);

		switch (result)
		{
		case 1:
			// new game
			clearSave();
			requestFade(NAME_INPUT);
			break;

		case 2:
			// continue
			continueGame();
			break;

		case 3:
			// settings
			requestFade(SETTINGS);
			break;

		case 4:
			// credits
			requestFade(CREDITS);
			break;
		
		case 5:
			// exit
			exit(0);
			break;

		}

		return;
	}

	// settings
	if (currentState == SETTINGS){

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			int result = settings.mouseClick(mx, my);

			if (result == 1)
			{
				// back button clicked
				requestFade(MAIN_MENU);
			}
			return;
		}

	}

	// credits
	if (currentState == CREDITS){

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			int result = credits.mouseClick(button, state, mx, my);

			if (result == 1)
			{
				// back button clicked
				requestFade(MAIN_MENU);
			}
			return;
		}

	}

	// gameplay
	if (currentState == NAME_INPUT)
	{
		if (button == GLUT_LEFT_BUTTON &&
			state == GLUT_DOWN)
		{
			bool confirmed = nameInput.mouseClick(mx, my);
			if (confirmed) confirmName();
		}
		return;
	}

	if (currentState == THRONE_ROOM)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		{
			return;
		}

		if (dialogueBox.isActive())
		{
			dialogueBox.advance();
			return;
		}

		if (throneStep == 0)
		{
			throneStep = 1;
			dialogueBox.startDialogue(nameInput.getName(), mcThroneReplyLines, MC_THRONE_REPLY_COUNT, true);
		}
		else if (throneStep == 1)
		{
			throneStep = 2;
			enterHallway();
		}

		return;
	}

	if (currentState == HALLWAY)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		{
			return;
		}

		if (showingNoteImage)
		{
			showingNoteImage = false;
			hallwayStep = 2;
			inventory.givePendant();
			dialogueBox.startDialogue(nameInput.getName(), hallwayLines2, HALLWAY_LINE_COUNT_2, true);
			return;
		}
		if (dialogueBox.isActive())
		{
			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				if (hallwayStep == 0) showingNoteImage = true;
				else if (hallwayStep == 2) hallwayStep = 1;
			}

			return;
		}

		if (hallwayStep == 0)
		{
			hallwayStep = 1;
			return;
		}

		if (overTravelPrompt(mx, my))
		{
			enterVillage();
		}

		return;
	}

	if (currentState == VILLAGE)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		{
			return;
		}

		if (itemObtained.getIsActive())
		{
			itemObtained.hide();
			dialogueBox.startDialogue("Hooded Man", npc1ReceiveLines, 1, false, hoodedManBoxImg);
			return;
		}

		if (inventory.handleClick(mx, my))
		{
			return;
		}

		if (dialogueBox.isActive())
		{
			if (dialogueBox.isShowingOptions())
			{
				int option = dialogueBox.checkOptionClick(mx, my);

				if (option == 1)
				{
					inventory.giveHoodedGift();
					npc1Step = 8;
					dialogueBox.close();
					itemObtained.show(inventory.getHoodedGiftIcon());
				}
				else if (option == 2)
				{
					npc1Step = 8;
					dialogueBox.startDialogue("Hooded Man", npc1RejectLines, 1, false, hoodedManBoxImg);
				}

				return;
			}

			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				if (activeVillageNpc == 1)
				{
					switch (npc1Step)
					{
					case 0: npc1Step = 1; dialogueBox.startDialogue(nameInput.getName(), mcNpc1Lines1, 1, true); break;
					case 1: npc1Step = 2; dialogueBox.startDialogue("Hooded Man", npc1Lines2, 1, false, hoodedManBoxImg); break;
					case 2: npc1Step = 3; dialogueBox.startDialogue(nameInput.getName(), mcNpc1Lines2, 1, true); break;
					case 3: npc1Step = 4; dialogueBox.startDialogue("Hooded Man", npc1Lines3, 1, false, hoodedManBoxImg); break;
					case 4: npc1Step = 5; dialogueBox.startDialogue(nameInput.getName(), mcNpc1Lines3, 1, true); break;
					case 5: npc1Step = 6; dialogueBox.startDialogue("Hooded Man", npc1Lines4, 3, false, hoodedManBoxImg); break;
					case 6: npc1Step = 7; dialogueBox.startOptions(npc1Option1, npc1Option2); break;
					}
				}
				else if (activeVillageNpc == 2)
				{
					switch (npc2Step)
					{
					case 0: npc2Step = 1; dialogueBox.startDialogue(nameInput.getName(), mcNpc2Lines1, 2, true); break;
					case 1: npc2Step = 2; dialogueBox.startDialogue("Villager", npc2Lines2, 2, false, villagerBoxImg); break;
					case 2: npc2Step = 3; dialogueBox.startDialogue(nameInput.getName(), mcNpc2Lines2, 1, true); break;
					case 3: npc2Step = 4; dialogueBox.startDialogue("Villager", npc2Lines3, 1, false, villagerBoxImg); break;
					case 4: npc2Step = 5; dialogueBox.startDialogue(nameInput.getName(), mcNpc2Lines3, 1, true); break;
					}
				}
			}

			return;
		}

		Rect mouseRect = { mx, my, 1, 1 };

		if (npc1Step == -1)
		{
			Rect r = npc1.getRect();
			Rect screenRect = { r.x - villageMap.getCameraX(), r.y - villageMap.getCameraY(), r.w, r.h };

			if (rectanglesOverlap(screenRect, mouseRect))
			{
				activeVillageNpc = 1;
				npc1Step = 0;
				dialogueBox.startDialogue("Hooded Man", npc1Lines1, 1, false, hoodedManBoxImg);
				return;
			}
		}

		if (npc2Step == -1)
		{
			Rect r = npc2.getRect();
			Rect screenRect = { r.x - villageMap.getCameraX(), r.y - villageMap.getCameraY(), r.w, r.h };

			if (rectanglesOverlap(screenRect, mouseRect))
			{
				activeVillageNpc = 2;
				npc2Step = 0;
				dialogueBox.startDialogue("Villager", npc2Lines1, 1, false, villagerBoxImg);
				return;
			}
		}

		return;
	}

	if (currentState == RESTAURANT)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		{
			return;
		}

		if (showSilverleafPrompt && overTravelPrompt(mx, my))
		{
			showSilverleafPrompt = false;
			enterForest();
			return;
		}

		if (inventory.handleClick(mx, my)) return;

		if (dialogueBox.isActive())
		{
			if (dialogueBox.isShowingOptions())
			{
				int option = dialogueBox.checkOptionClick(mx, my);

				// OPTION 1:
				// I'll cook.
				if (option == 1)
				{
					enterCookingGame();
					return;
				}


				// OPTION 2:
				// I'll serve.
				if (option == 2)
				{
					enterServingGame();
					return;
				}

				return;
			}


			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				switch (restaurantStep)
				{
				case 1:
					// owner line 1 finished -> mc replies
					restaurantStep = 2;
					dialogueBox.startDialogue(nameInput.getName(), restaurantMcLines1, 1, true);
					break;

				case 2:
					// mc reply 1 finished -> owner lines 2
					restaurantStep = 3;
					dialogueBox.startDialogue("Restaurant Owner", restaurantOwnerLines2, 2, false, restaurantOwnerBoxImg);
					break;

				case 3:
					// owner lines 2 finished -> mc reply 2
					restaurantStep = 4;
					dialogueBox.startDialogue(nameInput.getName(), restaurantMcLines2, 1, true);
					break;

				case 4:
					// mc reply 2 finished -> owner line 3
					restaurantStep = 5;
					dialogueBox.startDialogue("Restaurant Owner", restaurantOwnerLines3, 1, false, restaurantOwnerBoxImg);
					break;

				case 5:
					// owner line 3 finished -> mc reply 3
					restaurantStep = 6;
					dialogueBox.startDialogue(nameInput.getName(), restaurantMcLines3, 1, true);
					break;

				case 6:
					// mc reply 3 finished -> show the two options
					restaurantStep = 7;
					dialogueBox.startOptions(restaurantOption1, restaurantOption2);
					break;

				case 100:
					restaurantStep = 101;
					dialogueBox.startDialogue(nameInput.getName(), restaurantReturnMcLines1, 1, true);
					break;

				case 101:
					restaurantStep = 102;
					dialogueBox.startDialogue("Restaurant Owner", restaurantReturnOwnerLines2, 2, false, restaurantOwnerBoxImg);
					break;

				case 102:
					restaurantStep = 103;
					dialogueBox.startDialogue(nameInput.getName(), restaurantReturnMcLines2, 1, true);
					break;

				case 103:
					restaurantStep = 104;
					dialogueBox.startDialogue("Restaurant Owner", restaurantReturnOwnerLines3, 1, false, restaurantOwnerBoxImg);
					break;

				case 104:
					restaurantStep = 105;
					dialogueBox.startDialogue(nameInput.getName(), restaurantReturnMcLines3, 2, true);
					break;

				case 105:
					restaurantStep = 106;
					dialogueBox.startDialogue("Restaurant Owner", restaurantReturnOwnerLines4, 1, false, restaurantOwnerBoxImg);
					break;

				case 106:
					restaurantStep = 107;
					showSilverleafPrompt = true;
					break;

				}

				return;
			}
		}

		if (restaurantStep == -1)
		{
			Rect npcRect =
			{
				RESTAURANT_NPC_X,
				RESTAURANT_NPC_Y,
				PLAYER_WIDTH,
				PLAYER_HEIGHT
			};


			Rect mouseRect =
			{
				mx,
				my,
				1,
				1
			};

			if (rectanglesOverlap(npcRect, mouseRect))
			{
				restaurantStep = 1;
				dialogueBox.startDialogue("Restaurant Owner", restaurantOwnerLines1, 1, false, restaurantOwnerBoxImg);
			}

			return;
		}

		return;
	}

	if (currentState == COOKING_GAME)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			cookingGame.handleClick(mx, my);
		}

		return;
	}

	if (currentState == SERVING_GAME)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			servingGame.handleClick(mx, my);
		}

		return;
	}

	if (currentState == FOREST)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

		if (itemObtained.getIsActive())
		{
			itemObtained.hide();
			herbalistStep = 10;
			dialogueBox.startDialogue(nameInput.getName(), mcHerbLines5, 1, true);
			return;
		}

		if (inventory.handleClick(mx, my)) return;

		if (dialogueBox.isActive())
		{
			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				if (activeForestNpc == 1)
				{
					switch (wardenStep)
					{
					case 0: wardenStep = 1; dialogueBox.startDialogue(nameInput.getName(), mcWardenLines1, 1, true); break;
					case 1: wardenStep = 2; dialogueBox.startDialogue("Forest Warden", wardenLines2, 1, false, wardenBoxImg); break;
					case 2: wardenStep = 3; dialogueBox.startDialogue(nameInput.getName(), mcWardenLines2, 1, true); break;
					case 3: wardenStep = 4; dialogueBox.startDialogue("Forest Warden", wardenLines3, 1, false, wardenBoxImg); break;
					case 4: wardenStep = 5; dialogueBox.startDialogue(nameInput.getName(), mcWardenLines3, 1, true); break;
					case 5: wardenStep = 6; dialogueBox.startDialogue("Forest Warden", wardenLines4, 2, false, wardenBoxImg); break;
					case 6: wardenStep = 7; dialogueBox.startDialogue(nameInput.getName(), mcWardenLines4, 2, true); break;
					}
				}
				else if (activeForestNpc == 2)
				{
					switch (herbalistStep)
					{
					case 0: herbalistStep = 1; dialogueBox.startDialogue(nameInput.getName(), mcHerbLines1, 1, true); break;
					case 1: herbalistStep = 2; dialogueBox.startDialogue("Herbalist", herbLines2, 1, false, herbalistBoxImg); break;
					case 2: herbalistStep = 3; dialogueBox.startDialogue(nameInput.getName(), mcHerbLines2, 1, true); break;
					case 3: herbalistStep = 4; dialogueBox.startDialogue("Herbalist", herbLines3, 2, false, herbalistBoxImg); break;
					case 4: herbalistStep = 5; dialogueBox.startDialogue(nameInput.getName(), mcHerbLines3, 1, true); break;
					case 5: herbalistStep = 6; dialogueBox.startDialogue("Herbalist", herbLines4, 1, false, herbalistBoxImg); break;
					case 6: herbalistStep = 7; dialogueBox.startDialogue(nameInput.getName(), mcHerbLines4, 1, true); break;
					case 7: herbalistStep = 8; dialogueBox.startDialogue("Herbalist", herbLines5, 1, false, herbalistBoxImg); break;

					case 8:
		
						herbalistStep = 9;
						inventory.giveCompass();
						itemObtained.show(inventory.getCompassIcon());
						break;
					}
				}
			}

			return;
		}

		if (showDeepPrompt && overTravelPrompt(mx, my))
		{
			enterForestTrail();
			return;
		}

		Rect mouseRect = { mx, my, 1, 1 };

		if (wardenStep == -1)
		{
			Rect r = forestWarden.getRect();
			Rect screenRect = { r.x - forestMap.getCameraX(), r.y - forestMap.getCameraY(), r.w, r.h };

			if (rectanglesOverlap(screenRect, mouseRect))
			{
				activeForestNpc = 1;
				wardenStep = 0;
				dialogueBox.startDialogue("Forest Warden", wardenLines1, 1, false, wardenBoxImg);
				return;
			}
		}

		if (herbalistStep == -1)
		{
			Rect r = herbalist.getRect();
			Rect screenRect = { r.x - forestMap.getCameraX(), r.y - forestMap.getCameraY(), r.w, r.h };

			if (rectanglesOverlap(screenRect, mouseRect))
			{
				activeForestNpc = 2;
				herbalistStep = 0;
				dialogueBox.startDialogue("Herbalist", herbLines1, 1, false, herbalistBoxImg);
				return;
			}
		}

		return;
	}

	if (currentState == FOREST_TRAIL)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

		if (!forestTrail.isAtClearing())
		{
			forestTrail.handleClick(player, mx, my);
			return;
		}

		if (itemObtained.getIsActive())
		{
			itemObtained.hide();
			clearingStep = 6;
			showMoonveilPrompt = true;
			return;
		}

		if (showingJournalImage)
		{
			showingJournalImage = false;
			clearingStep = 3;
			dialogueBox.startDialogue(nameInput.getName(), clearingLines2, CLEARING_LINE_COUNT_2, true);
			return;
		}

		if (dialogueBox.isActive())
		{
			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				if (clearingStep == 0)
				{
					clearingStep = 1;
				}
				else if (clearingStep == 3)
				{
					clearingStep = 5;
					inventory.giveStarwheel();
					itemObtained.show(inventory.getStarwheelIcon());
				}
			}

			return;
		}

		if (clearingStep == 1)
		{
			Rect objRect = { CLEARING_OBJ_X, CLEARING_OBJ_Y, CLEARING_OBJ_W, CLEARING_OBJ_H };
			Rect mouseRect = { mx, my, 1, 1 };

			if (rectanglesOverlap(objRect, mouseRect))
			{
				clearingStep = 2;
				showingJournalImage = true;
			}

			return;
		}

		if (clearingStep == 6 && overTravelPrompt(mx, my))
		{
			enterMoonveilApproach();
			return;
		}

		return;
	}

	if (currentState == MOONVEIL_ROOM)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) inventory.handleClick(mx, my);
		return;
	}

	if (currentState == MOONVEIL_APPROACH)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) inventory.handleClick(mx, my);
		return;
	}

	if (currentState == MOONVEIL_HALL)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

		if (inventory.handleClick(mx, my)) return;

		if (dialogueBox.isActive())
		{
			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				switch (mageStep)
				{
				case 0: mageStep = 1; dialogueBox.startDialogue(nameInput.getName(), mcHall1, 1, true); break;
				case 1: mageStep = 2; dialogueBox.startDialogue("Mage", mageHall2, 1, false, mageBoxImg); break;
				case 2: mageStep = 3; dialogueBox.startDialogue(nameInput.getName(), mcHall2, 1, true); break;
				case 3: mageStep = 4; dialogueBox.startDialogue("Mage", mageHall3, 1, false, mageBoxImg); break;
				case 4: mageStep = 5; dialogueBox.startDialogue(nameInput.getName(), mcHall3, 1, true); break;
				case 5: mageStep = 6; dialogueBox.startDialogue("Mage", mageHall4, 3, false, mageBoxImg); break;
				case 6: mageStep = 7; dialogueBox.startDialogue(nameInput.getName(), mcHall4, 1, true); break;
				case 7: mageStep = 8; dialogueBox.startDialogue("Mage", mageHall5, 3, false, mageBoxImg); break;
				case 8: mageStep = 9; break;
				}
			}
		}

		return;
	}

	if (currentState == MOONVEIL_SANCTUM)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

		if (itemObtained.getIsActive())
		{
			itemObtained.hide();
			sanctumStep = 5;
			dialogueBox.startDialogue("Veyl", veylTook, 1, false, mageBoxImg);
			return;
		}

		if (showAstralPrompt && overTravelPrompt(mx, my))
		{
			showAstralPrompt = false;
			enterAstralFloor();
			return;
		}

		if (inventory.handleClick(mx, my)) return;

		if (dialogueBox.isActive())
		{
			if (dialogueBox.isShowingOptions())
			{
				int option = dialogueBox.checkOptionClick(mx, my);

				if (sanctumStep == 4)
				{
					if (option == 1)
					{
						inventory.giveCharm();
						dialogueBox.close();
						itemObtained.show(inventory.getCharmIcon());
					}
					else if (option == 2)
					{
						sanctumStep = 5;
						dialogueBox.startDialogue("Veyl", veylRefused, 1, false, mageBoxImg);
					}
					return;
				}

				if (sanctumStep == 7)
				{
					if (option == 1) { dialogueBox.close(); enterGhostChase(); }
					else if (option == 2) { dialogueBox.close(); enterMirrorHall(); }
					return;
				}

				return;
			}

			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				switch (sanctumStep)
				{
				case 0: sanctumStep = 1; dialogueBox.startDialogue("Mage", mageSanct1, 1, false, mageBoxImg); break;
				case 1: sanctumStep = 2; dialogueBox.startDialogue("Veyl", veylSanct1, 3, false, mageBoxImg); break;
				case 2: sanctumStep = 4; dialogueBox.startOptions(charmOption1, charmOption2); break;
				case 5: sanctumStep = 6; dialogueBox.startDialogue("Veyl", veylSanct2, 3, false, mageBoxImg); break;
				case 6: sanctumStep = 7; dialogueBox.startOptions(routeOption1, routeOption2); break;
				case 200: sanctumStep = 201; dialogueBox.startDialogue("Veyl", veylOpen, 1, false, mageBoxImg); break;
				case 201: sanctumStep = 202; dialogueBox.startDialogue(nameInput.getName(), mcOpen, 1, true); break;
				case 202: sanctumStep = 203; dialogueBox.startDialogue("Veyl", veylOpen2, 2, false, mageBoxImg); break;
				case 203: sanctumStep = 204; dialogueBox.startDialogue(nameInput.getName(), mcOpen2, 1, true); break;
				case 204: sanctumStep = 205; dialogueBox.startDialogue("Veyl", veylOpen3, 1, false, mageBoxImg); break;
				case 205: sanctumStep = 206; showAstralPrompt = true; break;
				}
			}
		}

		return;
	}

	if (currentState == GHOST_CHASE)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) ghostChase.handleClick(mx, my);
		return;
	}

	if (currentState == MIRROR_HALL)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) mirrorHall.handleClick(mx, my);
		return;
	}

	if (currentState == ASTRAL_FLOOR)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
		if (inventory.handleClick(mx, my)) return;

		if (dialogueBox.isActive())
		{
			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				switch (astralStep)
				{
				case 0: astralStep = 1; dialogueBox.startDialogue("Guardian", guardCage1, 2, false, guardianBoxImg); break;
				case 1: astralStep = 2; dialogueBox.startDialogue(nameInput.getName(), mcCage2, 1, true); break;
				case 2: astralStep = 3; dialogueBox.startDialogue("Guardian", guardCage2, 3, false, guardianBoxImg); break;
				case 3: astralStep = 4; dialogueBox.startDialogue(nameInput.getName(), mcCage3, 1, true); break;
				case 4: astralStep = 5; dialogueBox.startDialogue("Guardian", guardCage3, 1, false, guardianBoxImg); break;
				case 5: astralStep = 6; dialogueBox.startDialogue("The Hollow", hollowCage, 2, false, hollowBoxImg); break;
				case 6: astralStep = 7; cageBroken = true;
					dialogueBox.startDialogue("Guardian", guardCage4, 1, false, guardianBoxImg); break;
				case 7: astralStep = 8; enterFinalFight(); break;
				}
			}
		}

		return;
	}

	if (currentState == FINAL_FIGHT)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) finalFight.handleClick(mx, my);
		return;
	}

	if (currentState == ASTRAL_END)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

		if (dialogueBox.isActive())
		{
			dialogueBox.advance();

			if (!dialogueBox.isActive())
			{
				switch (endStep)
				{
				case 0: endStep = 1; dialogueBox.startDialogue("Guardian", guardEnd1, 1, false, guardianBoxImg); break;
				case 1: endStep = 2; dialogueBox.startDialogue(nameInput.getName(), mcEnd2, 1, true); break;
				case 2: endStep = 3; dialogueBox.startDialogue("Guardian", guardEnd2, 2, false, guardianBoxImg); break;
				case 3: endStep = 4; dialogueBox.startDialogue(nameInput.getName(), mcEnd3, 1, true); break;
				case 4: endStep = 5; dialogueBox.startDialogue("Guardian", guardEnd3, 3, false, guardianBoxImg); break;
				case 5: endStep = 6; enterEpilogue(); break;
				}
			}
		}

		return;
	}

	if (currentState == EPILOGUE)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
		if (epilogueTick < EPILOGUE_IN + 45) return;

		epilogueCard++;
		epilogueTick = 0;

		if (epilogueCard >= 4)
		{
			requestTitleCard("ECHOES OF AURION", 0, CREDITS);
		}

		return;
	}

}

void iKeyboard(unsigned char key)
{
	if (currentState == NAME_INPUT)
	{
		bool confirmed = nameInput.handleKeyPress(key);
		if (confirmed) confirmName();
	}

	if (currentState == FOREST_TRAIL)
	{
		forestTrail.handleKey(key);
	}

	if (currentState == GHOST_CHASE)
	{
		ghostChase.handleKey(key);
	}
	if (currentState == MIRROR_HALL)
	{
		mirrorHall.handleKey(key);
	}
	if (currentState == FINAL_FIGHT)
	{
		finalFight.handleKey(key);
	}
}

// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, 
// GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT

void fixedUpdate()
{
	if (fadePhase != FADE_NONE) return;
	
	if (currentState == VILLAGE)
	{

		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, villageMap);

		Rect restaurantTrigger =
		{
			RESTAURANT_TRIGGER_X,
			RESTAURANT_TRIGGER_Y,
			RESTAURANT_TRIGGER_W,
			RESTAURANT_TRIGGER_H
		};


		if (rectanglesOverlap(player.getFeetRect(), restaurantTrigger))
		{
			enterRestaurant();

		}

		return;
	}

	if (currentState == RESTAURANT)
	{

		if (dialogueBox.isActive())
		{
			return;
		}

		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, restaurantMap);

		Rect villageExitTrigger = { VILLAGE_EXIT_X, VILLAGE_EXIT_Y, VILLAGE_EXIT_W, VILLAGE_EXIT_H };

		if (rectanglesOverlap(player.getFeetRect(), villageExitTrigger))
		{
			exitRestaurantToVillage();
			return;
		}

		return;
	}

	if (currentState == FOREST)
	{
		if (dialogueBox.isActive() || itemObtained.getIsActive()) return;

		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, forestMap);

		Rect enterZone = { DEEP_ENTER_X, DEEP_ENTER_Y, DEEP_ENTER_W, DEEP_ENTER_H };
		showDeepPrompt = rectanglesOverlap(player.getFeetRect(), enterZone);

		return;
	}

	if (currentState == FOREST_TRAIL)
	{
		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		bool frozen =
			forestTrail.isTitleCardShowing() ||
			dialogueBox.isActive() ||
			showingJournalImage ||
			itemObtained.getIsActive() ||
			showMoonveilPrompt;

		if (!frozen)
		{
			player.handleInput(up, down, left, right, forestTrail.getMap());
		}

		forestTrail.updateCamera(player);

		return;
	}

	if (currentState == MOONVEIL_APPROACH)
	{
		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, approachMap);

		Rect gate = { GATE_X, GATE_Y, GATE_W, GATE_H };
		if (rectanglesOverlap(player.getFeetRect(), gate)) enterMoonveilHall();

		return;
	}

	if (currentState == MOONVEIL_HALL)
	{
		if (dialogueBox.isActive()) return;

		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, hallMap);

		if (mageStep >= 9)
		{
			Rect door = { HALL_DOOR_X, HALL_DOOR_Y, HALL_DOOR_W, HALL_DOOR_H };
			if (rectanglesOverlap(player.getFeetRect(), door)) enterMoonveilRoom();
		}

		return;
	}

	if (currentState == MOONVEIL_ROOM)
	{
		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, roomMap);

		Rect door = { ROOM_DOOR_X, ROOM_DOOR_Y, ROOM_DOOR_W, ROOM_DOOR_H };
		if (rectanglesOverlap(player.getFeetRect(), door)) enterMoonveilSanctum();

		return;
	}

	if (currentState == MOONVEIL_SANCTUM)
	{
		if (dialogueBox.isActive() || itemObtained.getIsActive()) return;

		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, sanctumMap);
		return;
	}

	if (currentState == GHOST_CHASE)
	{
		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, ghostChase.getMap());
		ghostChase.updateCamera(player);
		return;
	}

	if (currentState == MIRROR_HALL)
	{
		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		mirrorHall.handleInput(up, down, left, right, player);
		return;
	}

	if (currentState == ASTRAL_FLOOR)
	{
		if (dialogueBox.isActive()) return;

		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, astralMap);

		if (astralStep == -1)
		{
			Rect cageZone = { CAGE_X - 80, CAGE_Y - 140, CAGE_W + 160, 180 };

			if (rectanglesOverlap(player.getFeetRect(), cageZone))
			{
				astralStep = 0;
				dialogueBox.startDialogue(nameInput.getName(), mcCage1, 1, true);
			}
		}

		return;
	}

	if (currentState == FINAL_FIGHT)
	{
		bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
		bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
		bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);

		player.handleInput(up, down, left, right, finalFight.getMap());
		return;
	}

}

void update()
{
	updateFade();

	if (currentState == TITLE_CARD && fadePhase == FADE_NONE)
	{
		titleTick++;
		if (titleTick >= TITLE_HOLD) requestFade(titlePendingState);
	}

	itemObtained.update();
	dialogueBox.update();
	fixedUpdate();
	player.updateAnimation();

	if (currentState == COOKING_GAME)
	{
		cookingGame.update();

		if (cookingGame.isFinished())
		{
			returnToRestaurantAfterMinigame();
			return;
		}

		if (cookingGame.isRetryRequested())
		{
			cookingGame.start(inventory.getHasHoodedGift());
			return;
		}

		return;
	}

	if (currentState == SERVING_GAME)
	{
		servingGame.update();

		if (servingGame.isFinished())
		{
			returnToRestaurantAfterMinigame();
			return;
		}

		if (servingGame.isRetryRequested())
		{
			servingGame.start(inventory.getHasHoodedGift());
			return;
		}

		return;
	}

	if (currentState == FOREST_TRAIL)
	{
		forestTrail.update(player);

		if (forestTrail.takeClearingArrived())
		{
			clearingStep = 0;
			showingJournalImage = false;
			showMoonveilPrompt = false;
			dialogueBox.startDialogue(nameInput.getName(), clearingLines1, CLEARING_LINE_COUNT_1, true);
		}

		if (forestTrail.isFinished())
		{
			// to be added
			return;
		}

		if (forestTrail.isRetryRequested())
		{
			forestTrail.start(inventory.getHasCompass());
			player.init(FOREST_SPAWN_X, FOREST_SPAWN_Y);
			forestTrail.updateCamera(player);
			return;
		}

		return;
	}

	if (currentState == GHOST_CHASE)
	{
		ghostChase.update(player);

		if (ghostChase.isFinished())
		{
			returnFromMoonveilGame();
			return;
		}

		if (ghostChase.isRetryRequested())
		{
			ghostChase.start(inventory.getHasCharm(), player);
			return;
		}

		return;
	}

	if (currentState == MIRROR_HALL)
	{
		mirrorHall.update(player);

		if (mirrorHall.isFinished())
		{
			returnFromMoonveilGame();
			return;
		}

		if (mirrorHall.isRetryRequested())
		{
			mirrorHall.start(inventory.getHasCharm(), player);
			return;
		}

		return;
	}

	if (currentState == EPILOGUE)
	{
		epilogueTick++;
		return;
	}

	if (currentState == FINAL_FIGHT)
	{
		finalFight.update(player);

		if (finalFight.isFinished())
		{
			enterAstralEnd();
			return;
		}

		if (finalFight.isRetryRequested())
		{
			finalFight.start(inventory.getHasStarwheel(), player);
			return;
		}

		return;
	}
}

int main()
{

	iSetTimer(10, update);
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Echoes of Aurion");
	
	mainMenu.loadImages();
	settings.loadImages();
	credits.loadImages();
	nameInput.loadImages();
	inventory.loadImages();
	itemObtained.loadImages();
	cookingGame.loadImages();
	servingGame.loadImages();
	ghostChase.loadImages();
	mirrorHall.loadImages();
	finalFight.loadImages();

	player.loadImages();
	dialogueBox.loadImages();

	throneRoomMap.init("Images//map_throne_room.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
	hallwayMap.init("Images//map_palace_hallway.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	villageMap.init("Images//map_emberfall_village.png", 3200, 1800, true);

	restaurantMap.init("Images//map_restaurant.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	villageMap.addObstacle(0, 1552, 3200, 248);  // top stone wall + everything above it
	villageMap.addObstacle(2683, 890, 517, 662);  // right rock formation
	villageMap.addObstacle(0, 1243, 1348, 308);  // lake upper part, waterfall, upper house
	villageMap.addObstacle(1453, 1333, 1140, 218);  // grass between the two upper paths
	villageMap.addObstacle(1453, 1190, 678, 143);
	villageMap.addObstacle(2227, 1190, 367, 143);
	villageMap.addObstacle(0, 1190, 778, 53);
	villageMap.addObstacle(875, 1190, 473, 53);
	villageMap.addObstacle(0, 1122, 510, 68);  // lake edge above the big bridge
	villageMap.addObstacle(0, 433, 510, 627);  // lower half lake
	villageMap.addObstacle(510, 433, 132, 622);
	villageMap.addObstacle(760, 415, 522, 640);  // central grass block, left
	villageMap.addObstacle(1375, 415, 438, 640);  // central grass block, right
	villageMap.addObstacle(1943, 890, 740, 165);
	villageMap.addObstacle(2822, 302, 378, 588);
	villageMap.addObstacle(1943, 368, 798, 423);  // lower house and its garden
	villageMap.addObstacle(0, 0, 415, 433);  // bottom-left water
	villageMap.addObstacle(1943, 302, 403, 67);
	villageMap.addObstacle(2462, 302, 280, 67);
	villageMap.addObstacle(415, 0, 207, 353);
	villageMap.addObstacle(2827, 0, 373, 302);
	villageMap.addObstacle(760, 0, 1053, 298);
	villageMap.addObstacle(1943, 0, 798, 218);
	villageMap.addObstacle(2742, 0, 80, 217);
	villageMap.addObstacle(187, 1086, 60, 30);
	villageMap.addObstacle(2425, 315, 60, 30);
	villageMap.addObstacle(2610, 1045, 72, 62);
	villageMap.addObstacle(2560, 1045, 50, 35);
	villageMap.addObstacle(0, 1024, 516, 65); // big bridge railing
	villageMap.addObstacle(404, 323, 215, 78); // small bridge railing

	forestTrail.loadImages();

	forestMap.init("Images//map_silverleaf_forest.png", FOREST_WORLD_W_MAP, FOREST_WORLD_H_MAP, true);

	forestMap.addObstacle(0, 1970, 3840, 190);
	forestMap.addObstacle(0, 1384, 1630, 586);
	forestMap.addObstacle(1746, 1384, 2094, 586);
	forestMap.addObstacle(0, 644, 670, 740);
	forestMap.addObstacle(670, 644, 98, 652);
	forestMap.addObstacle(862, 644, 424, 700);
	forestMap.addObstacle(1354, 644, 942, 672);
	forestMap.addObstacle(2390, 1012, 730, 327);
	forestMap.addObstacle(3120, 1012, 720, 372);
	forestMap.addObstacle(2390, 460, 860, 480);
	forestMap.addObstacle(3250, 460, 590, 552);
	forestMap.addObstacle(0, 0, 556, 644);
	forestMap.addObstacle(556, 0, 194, 588);
	forestMap.addObstacle(862, 0, 1434, 536);
	forestMap.addObstacle(2390, 0, 1054, 364);
	forestMap.addObstacle(3444, 0, 396, 460);
	forestMap.addObstacle(736, 1030, 160, 149);
	forestMap.addObstacle(1396, 1313, 64, 31);
	forestMap.addObstacle(1985, 1297, 55, 41);
	forestMap.addObstacle(2281, 754, 112, 82);

	approachMap.init("Images//map_moonveil_approach.png", 1280, 2400, true);
	hallMap.init("Images//map_moonveil_hall.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
	roomMap.init("Images//map_moonveil_room.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
	sanctumMap.init("Images//map_moonveil_sanctum.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	// moonveil approach
	approachMap.addObstacle(0, 0, 540, 160);
	approachMap.addObstacle(745, 0, 535, 160);
	approachMap.addObstacle(0, 160, 520, 320);
	approachMap.addObstacle(760, 160, 520, 320);
	approachMap.addObstacle(0, 480, 520, 480);
	approachMap.addObstacle(775, 480, 505, 480);
	approachMap.addObstacle(0, 960, 460, 320);
	approachMap.addObstacle(825, 960, 455, 320);
	approachMap.addObstacle(0, 1280, 535, 640);
	approachMap.addObstacle(770, 1280, 510, 640);
	approachMap.addObstacle(0, 1920, 530, 160);
	approachMap.addObstacle(750, 1920, 530, 160);
	approachMap.addObstacle(0, 2080, 450, 230);
	approachMap.addObstacle(810, 2080, 470, 230);
	approachMap.addObstacle(0, 2310, 1280, 90);

	// moonveil main hall
	hallMap.addObstacle(0, 0, 58, 720);
	hallMap.addObstacle(0, 648, 1280, 72);
	hallMap.addObstacle(0, 0, 1280, 76);
	hallMap.addObstacle(1230, 0, 50, 230);
	hallMap.addObstacle(1230, 385, 50, 335);
	hallMap.addObstacle(895, 435, 320, 213);
	hallMap.addObstacle(1100, 78, 115, 95);
	hallMap.addObstacle(192, 465, 60, 135);
	hallMap.addObstacle(402, 465, 60, 135);
	hallMap.addObstacle(820, 462, 60, 138);
	hallMap.addObstacle(190, 155, 62, 138);
	hallMap.addObstacle(402, 155, 60, 138);
	hallMap.addObstacle(818, 152, 62, 140);

	// moonveil middle room
	roomMap.addObstacle(0, 0, 70, 720);
	roomMap.addObstacle(1212, 0, 68, 720);
	roomMap.addObstacle(0, 645, 1280, 75);
	roomMap.addObstacle(0, 0, 1280, 86);
	roomMap.addObstacle(412, 432, 60, 118);
	roomMap.addObstacle(808, 432, 60, 118);
	roomMap.addObstacle(406, 162, 62, 118);
	roomMap.addObstacle(806, 162, 62, 118);
	roomMap.addObstacle(1060, 348, 105, 180);
	roomMap.addObstacle(105, 500, 98, 70);
	roomMap.addObstacle(1078, 192, 76, 76);

	// moonveil inner sanctum
	sanctumMap.addObstacle(0, 0, 62, 720);
	sanctumMap.addObstacle(1218, 0, 62, 720);
	sanctumMap.addObstacle(0, 0, 585, 74);
	sanctumMap.addObstacle(700, 0, 580, 74);
	sanctumMap.addObstacle(0, 648, 1280, 72);
	sanctumMap.addObstacle(160, 608, 175, 112);
	sanctumMap.addObstacle(950, 545, 268, 175);
	sanctumMap.addObstacle(486, 478, 60, 125);
	sanctumMap.addObstacle(740, 478, 60, 125);
	sanctumMap.addObstacle(176, 172, 62, 130);
	sanctumMap.addObstacle(1030, 172, 62, 130);

	astralMap.init("Images//map_astral_sanctum.png", 2560, 1800, true);

	astralMap.addObstacle(0, 1640, 2560, 160);
	astralMap.addObstacle(0, 0, 2560, 120);
	astralMap.addObstacle(0, 120, 120, 1680);
	astralMap.addObstacle(2440, 120, 120, 1680);
	astralMap.addObstacle(0, 120, 1160, 80);
	astralMap.addObstacle(1400, 120, 1160, 80);
	astralMap.addObstacle(0, 1600, 760, 200);
	astralMap.addObstacle(1800, 1600, 640, 200);
	astralMap.addObstacle(0, 200, 160, 520);
	astralMap.addObstacle(2400, 200, 160, 520);
	astralMap.addObstacle(0, 1160, 160, 640);
	astralMap.addObstacle(2400, 1160, 40, 640);
	astralMap.addObstacle(1120, 1600, 320, 200);
	astralMap.addObstacle(0, 200, 240, 120);
	astralMap.addObstacle(440, 200, 240, 40);
	astralMap.addObstacle(2320, 200, 240, 120);
	astralMap.addObstacle(1880, 200, 200, 40);
	astralMap.addObstacle(0, 1520, 240, 280);
	astralMap.addObstacle(2320, 1520, 80, 280);
	astralMap.addObstacle(720, 200, 120, 40);
	astralMap.addObstacle(1720, 200, 120, 40);
	astralMap.addObstacle(0, 320, 200, 80);
	astralMap.addObstacle(2360, 320, 200, 80);
	astralMap.addObstacle(0, 200, 280, 80);
	astralMap.addObstacle(2280, 200, 280, 80);
	astralMap.addObstacle(0, 1560, 280, 240);
	astralMap.addObstacle(2280, 1560, 40, 240);
	astralMap.addObstacle(0, 1480, 200, 320);
	astralMap.addObstacle(2360, 1480, 40, 320);
	astralMap.addObstacle(0, 200, 320, 40);
	astralMap.addObstacle(1080, 200, 40, 40);
	astralMap.addObstacle(2240, 200, 320, 40);

	mage.init(MAGE_HALL_X, MAGE_HALL_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Mage");
	mage.loadImage("Images//idle_mage.png");

	forestWarden.init(WARDEN_X, WARDEN_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Forest Warden");
	forestWarden.loadImage("Images//idle_forest_warden.png");

	herbalist.init(HERBALIST_X, HERBALIST_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Herbalist");
	herbalist.loadImage("Images//idle_herbalist.png");

	king.init(595, 425, PLAYER_WIDTH, PLAYER_HEIGHT, "King");
	king.loadImage("Images//king.png");

	npc1.init(172, 1086, PLAYER_WIDTH, PLAYER_HEIGHT, "Hooded Man"); // near the bridge
	npc1.loadImage("Images//idle_npc_1.png");

	npc2.init(2410, 315, PLAYER_WIDTH, PLAYER_HEIGHT, "Villager"); // near a house
	npc2.loadImage("Images//idle_npc_2.png");

	noteAndPendantImg = iLoadImage("Images//note_and_pendant.png");
	revealJournalImg = iLoadImage("Images//reveal_journal_page.png");

	restaurantNpc.init(RESTAURANT_NPC_X, RESTAURANT_NPC_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Restaurant Owner");
	restaurantNpc.loadImage("Images//idle_npc_3.png");

	restaurantMap.addObstacle(113, 55, 96, 16);
	restaurantMap.addObstacle(52, 69, 222, 163);
	restaurantMap.addObstacle(93, 206, 207, 135);
	restaurantMap.addObstacle(0, 218, 79, 161);
	restaurantMap.addObstacle(0, 379, 722, 338);
	restaurantMap.addObstacle(705, 245, 575, 475);
	restaurantMap.addObstacle(444, 167, 91, 26);
	restaurantMap.addObstacle(396, 183, 197, 152);
	restaurantMap.addObstacle(737, 40, 102, 13);
	restaurantMap.addObstacle(677, 61, 234, 155);
	restaurantMap.addObstacle(1250, 163, 29, 88);

	travelEmberfallImg = iLoadImage("Images//travel_emberfall.png");
	travelEmberfallHoverImg = iLoadImage("Images//hover_travel_emberfall.png");
	travelForestImg = iLoadImage("Images//travel_forest.png");
	travelForestHoverImg = iLoadImage("Images//hover_travel_forest.png");
	travelSilverleafImg = iLoadImage("Images//travel_silverleaf.png");
	travelSilverleafHoverImg = iLoadImage("Images//hover_travel_silverleaf.png");
	travelMoonveilImg = iLoadImage("Images//travel_moonveil.png");
	travelMoonveilHoverImg = iLoadImage("Images//hover_travel_moonveil.png");
	travelAstralImg = iLoadImage("Images//travel_astral.png");
	travelAstralHoverImg = iLoadImage("Images//hover_travel_astral.png");

	guardian.init(GUARDIAN_FLOOR_X, GUARDIAN_FLOOR_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Guardian");
	guardian.loadImage("Images//idle_guardian.png");

	guardianBoxImg = iLoadImage("Images//dialogue_box_guardian.png");
	hollowBoxImg = iLoadImage("Images//dialogue_box_hollow.png");
	cageImg = iLoadImage("Images//obj_cage.png");
	cageBrokenImg = iLoadImage("Images//obj_cage_broken.png");
	sealFloorImg = iLoadImage("Images//obj_seal.png");

	kingBoxImg = iLoadImage("Images//dialogue_box_king.png");
	villagerBoxImg = iLoadImage("Images//dialogue_box_villager.png");
	hoodedManBoxImg = iLoadImage("Images//dialogue_box_hooded_man.png");
	restaurantOwnerBoxImg = iLoadImage("Images//dialogue_box_restaurant_owner.png");
	wardenBoxImg = iLoadImage("Images//dialogue_box_forest_warden.png");
	herbalistBoxImg = iLoadImage("Images//dialogue_box_herbalist.png");
	mageBoxImg = iLoadImage("Images//dialogue_box_mage.png");
	
	endCardImg[0] = iLoadImage("Images//end_card_1.png");
	endCardImg[1] = iLoadImage("Images//end_card_2.png");
	endCardImg[2] = iLoadImage("Images//end_card_3.png");
	endCardImg[3] = iLoadImage("Images//end_card_4.png");

	iStart();
	return 0;
}