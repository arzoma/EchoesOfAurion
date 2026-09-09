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
	FOREST_TRAIL

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

int throneStep = 0;  // 0 = king talking, 1 = mc's reply, 2 = moved on
int hallwayStep = 0; // 0 = mc's lines playing, 1 = "Travel to Emberfall" prompt showing
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
	"Master left over a week ago...",
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
	"Yes. He came to Emberfall about a week ago. Do you know where he went?"
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
	"He came here a few days ago. He seemed worried about the sacred seal.",
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
	"I'm looking for the Guardian. He came to Emberfall recently.",
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


const int THRONE_PLAYER_X = 595, THRONE_PLAYER_Y = 150;
const int HALLWAY_PLAYER_X = 595, HALLWAY_PLAYER_Y = 300;
const int VILLAGE_SPAWN_X = 1000, VILLAGE_SPAWN_Y = 340;

const int RESTAURANT_PLAYER_X = 100;
const int RESTAURANT_PLAYER_Y = 100;

const int RESTAURANT_NPC_X = 500;
const int RESTAURANT_NPC_Y = 250;

const int RESTAURANT_DOOR_X = 2143;
const int RESTAURANT_DOOR_Y = 1300;
const int RESTAURANT_DOOR_W = 67;
const int RESTAURANT_DOOR_H = 92;

const int RESTAURANT_TRIGGER_X = 2132;
const int RESTAURANT_TRIGGER_Y = 1167;
const int RESTAURANT_TRIGGER_W = 95;
const int RESTAURANT_TRIGGER_H = 103;

const int VILLAGE_EXIT_X = 0;
const int VILLAGE_EXIT_Y = 0;
const int VILLAGE_EXIT_W = 60;
const int VILLAGE_EXIT_H = 60;

const double TRAVEL_TEXT_X_MIN = 0.40, TRAVEL_TEXT_X_MAX = 0.60;
const double TRAVEL_TEXT_Y_MIN = 0.10, TRAVEL_TEXT_Y_MAX = 0.16;

const int FOREST_SPAWN_WX = 761, FOREST_SPAWN_WY = 130;

const int WARDEN_X = 761, WARDEN_Y = 1040;
const int HERBALIST_X = 3020, HERBALIST_Y = 1310;

const int DEEP_NEAR_X = 1630, DEEP_NEAR_Y = 1500, DEEP_NEAR_W = 116, DEEP_NEAR_H = 470;
const int DEEP_ENTER_X = 1630, DEEP_ENTER_Y = 1790, DEEP_ENTER_W = 116, DEEP_ENTER_H = 180;

const int DEEP_PROMPT_X_MIN = 480, DEEP_PROMPT_X_MAX = 800;
const int DEEP_PROMPT_Y_MIN = 72, DEEP_PROMPT_Y_MAX = 115;

bool rectanglesOverlap(Rect a, Rect b)
{
	return
		a.x < b.x + b.w &&
		a.x + a.w > b.x &&
		a.y < b.y + b.h &&
		a.y + a.h > b.y;
}

void enterThroneRoom()
{
	//currentState = THRONE_ROOM;
	throneStep = 0;

	player.init(THRONE_PLAYER_X, THRONE_PLAYER_Y);
	player.setFacing(DIR_BACK);

	dialogueBox.startDialogue("King", kingLines, KING_LINE_COUNT, false, kingBoxImg);

	requestFade(THRONE_ROOM);
}

void enterHallway()
{
	//currentState = HALLWAY;
	hallwayStep = 0;
	showingNoteImage = false;

	player.init(HALLWAY_PLAYER_X, HALLWAY_PLAYER_Y);
	player.setFacing(DIR_FRONT);

	dialogueBox.startDialogue(nameInput.getName(), hallwayLines1, HALLWAY_LINE_COUNT_1, true);

	requestFade(HALLWAY);
}

void enterVillage()
{
	//currentState = VILLAGE;

	player.init(VILLAGE_SPAWN_X, VILLAGE_SPAWN_Y);
	player.setFacing(DIR_FRONT);

	villageMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	requestFade(VILLAGE);
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

	requestFade(FOREST);
}

void enterForestTrail()
{
	forestTrail.start(inventory.getHasCompass());
	player.init(FOREST_SPAWN_X, FOREST_SPAWN_Y);
	player.setFacing(DIR_BACK);
	forestTrail.getMap().updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

	requestFade(FOREST_TRAIL);
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
}

void enterCookingGame()
{
	//currentState = COOKING_GAME;
	cookingGame.start();
	requestFade(COOKING_GAME);
}

void enterServingGame()
{
	//currentState = SERVING_GAME;
	servingGame.start();
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
				iSetColor(255, 255, 255);
				iText((int)(SCREEN_WIDTH * 0.42), (int)(SCREEN_HEIGHT * 0.12), "Travel to Emberfall", GLUT_BITMAP_HELVETICA_18);
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
		break;

	case COOKING_GAME:

		cookingGame.draw(mouseX, mouseY);
		break;

	case SERVING_GAME:

		servingGame.draw();
		break;

	case FOREST:
	{
				   forestMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
				   forestMap.draw();
				   forestWarden.draw(forestMap.getCameraX(), forestMap.getCameraY());
				   herbalist.draw(forestMap.getCameraX(), forestMap.getCameraY());
				   player.draw(forestMap.getCameraX(), forestMap.getCameraY());

				   Rect nearZone = { DEEP_NEAR_X, DEEP_NEAR_Y, DEEP_NEAR_W, DEEP_NEAR_H };

				   if (!dialogueBox.isActive() && rectanglesOverlap(player.getFeetRect(), nearZone))
				   {
					   iSetColor(255, 255, 255);
					   iText(360, 60, "Something seems to lie in the deeper part of the forest.", GLUT_BITMAP_HELVETICA_18);
				   }

				   if (showDeepPrompt && !dialogueBox.isActive())
				   {
					   iSetColor(255, 255, 255);
					   iText(508, 86, "Enter the Deep Forest", GLUT_BITMAP_HELVETICA_18);
				   }

				   dialogueBox.draw(mouseX, mouseY);
				   inventory.draw(mouseX, mouseY);
				   itemObtained.draw();
	}
		break;

	case FOREST_TRAIL:
		forestTrail.draw(player, mouseX, mouseY);
		break;
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
	if (currentState == MAIN_MENU)
	{
		int result = mainMenu.mouseClick(button, state, mx, my);

		switch (result)
		{
		case 1:
			// new game
			requestFade(NAME_INPUT);
			break;

		case 2:
			// continue
			// to be added
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

		double xPct = (double)mx / SCREEN_WIDTH;
		double yPct = (double)my / SCREEN_HEIGHT;

		if (xPct >= TRAVEL_TEXT_X_MIN && xPct <= TRAVEL_TEXT_X_MAX &&
			yPct >= TRAVEL_TEXT_Y_MIN && yPct <= TRAVEL_TEXT_Y_MAX)
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
					npc1Step = 6;
					dialogueBox.startDialogue("Hooded Man", npc1ReceiveLines, 1, false, hoodedManBoxImg);
				}
				else if (option == 2)
				{
					npc1Step = 6;
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
					case 4: npc1Step = 5; dialogueBox.startOptions(npc1Option1, npc1Option2); break;
					}
				}
				else if (activeVillageNpc == 2)
				{
					switch (npc2Step)
					{
					case 0: npc2Step = 1; dialogueBox.startDialogue(nameInput.getName(), mcNpc2Lines1, 1, true); break;
					case 1: npc2Step = 2; dialogueBox.startDialogue("Villager", npc2Lines2, 1, false, villagerBoxImg); break;
					case 2: npc2Step = 3; dialogueBox.startDialogue(nameInput.getName(), mcNpc2Lines2, 1, true); break;
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
					enterForest();
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

		if (showDeepPrompt &&
			mx >= DEEP_PROMPT_X_MIN && mx <= DEEP_PROMPT_X_MAX &&
			my >= DEEP_PROMPT_Y_MIN && my <= DEEP_PROMPT_Y_MAX)
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
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			forestTrail.handleClick(player, mx, my);
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
}

// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, 
// GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT

void fixedUpdate()
{
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

		player.handleInput(up, down, left, right, forestTrail.getMap());
		forestTrail.getMap().updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);

		return;
	}

}

void update()
{
	updateFade();
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
			cookingGame.start();
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
			servingGame.start();
			return;
		}

		return;
	}

	if (currentState == FOREST_TRAIL)
	{
		forestTrail.update(player);

		if (forestTrail.isFinished())
		{
			// to be added
			return;
		}

		if (forestTrail.isRetryRequested())
		{
			forestTrail.start(inventory.getHasCompass());
			player.init(FOREST_SPAWN_X, FOREST_SPAWN_Y);
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
	cookingGame.loadImages();
	servingGame.loadImages();

	player.loadImages();
	dialogueBox.loadImages();

	throneRoomMap.init("Images//map_throne_room.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
	hallwayMap.init("Images//map_palace_hallway.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	villageMap.init("Images//map_emberfall_village.png", 3200, 1800, true);

	restaurantMap.init("Images//map_restaurant.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	villageMap.addObstacle(0, 1552, 3200, 248);  // top stone wall + everything above it
	villageMap.addObstacle(2683, 890, 517, 662);  // right rock formation
	villageMap.addObstacle(0, 1243, 1348, 308);  // lake upper part, waterfall, upper house
	villageMap.addObstacle(1453, 1263, 1140, 288);  // grass between the two upper paths
	villageMap.addObstacle(1453, 1190, 678, 73);
	villageMap.addObstacle(2227, 1190, 367, 73);
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

	forestTrail.loadImages();

	forestMap.init("Images//map_silverleaf_forest.png", FOREST_WORLD_W_MAP, FOREST_WORLD_H_MAP, true);

	forestMap.addObstacle(0, 1970, 3840, 190);
	forestMap.addObstacle(0, 1384, 1630, 586);
	forestMap.addObstacle(1746, 1384, 2094, 586);
	forestMap.addObstacle(0, 644, 670, 740);
	forestMap.addObstacle(670, 644, 80, 652);
	forestMap.addObstacle(862, 644, 424, 652);
	forestMap.addObstacle(1354, 644, 942, 652);
	forestMap.addObstacle(2390, 1012, 730, 284);
	forestMap.addObstacle(3120, 1012, 720, 372);
	forestMap.addObstacle(2390, 460, 860, 480);
	forestMap.addObstacle(3250, 460, 590, 552);
	forestMap.addObstacle(0, 0, 556, 644);
	forestMap.addObstacle(556, 0, 194, 536);
	forestMap.addObstacle(862, 0, 1434, 536);
	forestMap.addObstacle(2390, 0, 1054, 364);
	forestMap.addObstacle(3444, 0, 396, 460);

	forestWarden.init(WARDEN_X, WARDEN_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Forest Warden");
	forestWarden.loadImage("Images//idle_forest_warden.png");

	herbalist.init(HERBALIST_X, HERBALIST_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Herbalist");
	herbalist.loadImage("Images//idle_herbalist.png");

	wardenBoxImg = iLoadImage("Images//dialogue_box_forest_warden.png");
	herbalistBoxImg = iLoadImage("Images//dialogue_box_herbalist.png");

	king.init(595, 425, PLAYER_WIDTH, PLAYER_HEIGHT, "King");
	king.loadImage("Images//king.png");

	npc1.init(172, 1086, PLAYER_WIDTH, PLAYER_HEIGHT, "Hooded Man"); // near the bridge
	npc1.loadImage("Images//idle_npc_1.png");

	npc2.init(2410, 315, PLAYER_WIDTH, PLAYER_HEIGHT, "Villager"); // near a house
	npc2.loadImage("Images//idle_npc_2.png");

	noteAndPendantImg = iLoadImage("Images//note_and_pendant.png");

	restaurantNpc.init(RESTAURANT_NPC_X, RESTAURANT_NPC_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Restaurant Owner");
	restaurantNpc.loadImage("Images//idle_npc_3.png");

	kingBoxImg = iLoadImage("Images//dialogue_box_king.png");
	villagerBoxImg = iLoadImage("Images//dialogue_box_villager.png");
	hoodedManBoxImg = iLoadImage("Images//dialogue_box_hooded_man.png");
	restaurantOwnerBoxImg = iLoadImage("Images//dialogue_box_restaurant_owner.png");
	
	iStart();
	return 0;
}