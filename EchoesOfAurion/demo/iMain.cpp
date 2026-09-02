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
#include "CookingGame.hpp"
#include "ServingGame.hpp"

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
	SERVING_GAME

};

GameState currentState = MAIN_MENU;

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

CookingGame cookingGame;
ServingGame servingGame;

int mouseX = 0;
int mouseY = 0;

int throneStep = 0;  // 0 = king talking, 1 = mc's reply, 2 = moved on
int hallwayStep = 0; // 0 = mc's lines playing, 1 = "Travel to Emberfall" prompt showing

int restaurantDialogueStep = 0;

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

char* hallwayLines[] = {
	"Master left over a week ago...",
	"He said he was going to investigate Emberfall Village.",
	"But he never came back.",
	"All he left behind is a note and a strange pendant...",
	"I have to go find him. If master went to Emberfall, that's where I should start looking."
};
const int HALLWAY_LINE_COUNT = 5;

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

const int THRONE_PLAYER_X = 595, THRONE_PLAYER_Y = 150;
const int HALLWAY_PLAYER_X = 595, HALLWAY_PLAYER_Y = 300;
const int VILLAGE_SPAWN_X = 1000, VILLAGE_SPAWN_Y = 400;

const int RESTAURANT_PLAYER_X = 100;
const int RESTAURANT_PLAYER_Y = 100;

const int RESTAURANT_NPC_X = 500;
const int RESTAURANT_NPC_Y = 250;

const int RESTAURANT_DOOR_X = 2026;
const int RESTAURANT_DOOR_Y = 1348;
const int RESTAURANT_DOOR_W = 102;
const int RESTAURANT_DOOR_H = 42;

const int RESTAURANT_TRIGGER_X = 1980;
const int RESTAURANT_TRIGGER_Y = 1250;
const int RESTAURANT_TRIGGER_W = 200;
const int RESTAURANT_TRIGGER_H = 180;

const double TRAVEL_TEXT_X_MIN = 0.40, TRAVEL_TEXT_X_MAX = 0.60;
const double TRAVEL_TEXT_Y_MIN = 0.10, TRAVEL_TEXT_Y_MAX = 0.16;

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
	currentState = THRONE_ROOM;
	throneStep = 0;

	player.init(THRONE_PLAYER_X, THRONE_PLAYER_Y);
	player.setFacing(DIR_BACK);

	dialogueBox.startDialogue("King", kingLines, KING_LINE_COUNT, false);
}

void enterHallway()
{
	// will add a screen fade function later
	currentState = HALLWAY;
	hallwayStep = 0;

	player.init(HALLWAY_PLAYER_X, HALLWAY_PLAYER_Y);
	player.setFacing(DIR_FRONT);

	dialogueBox.startDialogue(nameInput.getName(), hallwayLines, HALLWAY_LINE_COUNT, true);
}

void enterVillage()
{
	currentState = VILLAGE;

	player.init(VILLAGE_SPAWN_X, VILLAGE_SPAWN_Y);
	player.setFacing(DIR_FRONT);

	villageMap.updateCamera(player.getX(), player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT);
}

void enterRestaurant()
{
	currentState = RESTAURANT;

	restaurantDialogueStep = 0;

	player.init(
		RESTAURANT_PLAYER_X,
		RESTAURANT_PLAYER_Y
		);

	player.setFacing(DIR_FRONT);
}

void enterCookingGame()
{
	currentState = COOKING_GAME;

	cookingGame.start();
}

void enterServingGame()
{
	currentState = SERVING_GAME;

	servingGame.start();
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
		dialogueBox.draw(mouseX, mouseY);

		if (hallwayStep == 1)
		{
			iSetColor(255, 255, 255);
			iText((int)(SCREEN_WIDTH * 0.42), (int)(SCREEN_HEIGHT * 0.12),
				"Travel to Emberfall", GLUT_BITMAP_HELVETICA_18);
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

		break;

	case RESTAURANT:

		restaurantMap.draw();

		restaurantNpc.draw(0, 0);

		player.draw(0, 0);

		dialogueBox.draw(mouseX, mouseY);

		break;

	case COOKING_GAME:

		cookingGame.draw();

		break;

	case SERVING_GAME:

		servingGame.draw();

		break;

	}
}


void iMouseMove(int mx, int my)
{
	
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
			currentState = NAME_INPUT;
			break;

		case 2:
			// continue
			// to be added
			break;

		case 3:
			// settings
			currentState = SETTINGS;
			break;

		case 4:
			// credits
			currentState = CREDITS;
			break;
		
		case 5:
			// exit
			exit(0);
			break;

		}

		return;
	}

	//settings
	if (currentState == SETTINGS){

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			int result = settings.mouseClick(mx, my);

			if (result == 1)
			{
				// back button clicked
				currentState = MAIN_MENU;
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
				currentState = MAIN_MENU;
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

		if (dialogueBox.isActive())
		{
			dialogueBox.advance();
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
		// to be added

		return;
	}

	if (currentState == RESTAURANT)
	{
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		{
			return;
		}

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

			if (dialogueBox.isActive())
			{
				return;
			}

			switch (restaurantDialogueStep)
			{

			case 1:

				restaurantDialogueStep = 2;

				dialogueBox.startDialogue(
					nameInput.getName(),
					restaurantMcLines1,
					1,
					true
					);

				break;

			case 2:

				restaurantDialogueStep = 3;

				dialogueBox.startDialogue(
					"Restaurant Owner",
					restaurantOwnerLines2,
					2,
					false
					);

				break;

			case 3:

				restaurantDialogueStep = 4;

				dialogueBox.startDialogue(
					nameInput.getName(),
					restaurantMcLines2,
					1,
					true
					);

				break;

			case 4:

				restaurantDialogueStep = 5;

				dialogueBox.startDialogue(
					"Restaurant Owner",
					restaurantOwnerLines3,
					1,
					false
					);

				break;

			case 5:

				restaurantDialogueStep = 6;

				dialogueBox.startDialogue(
					nameInput.getName(),
					restaurantMcLines3,
					1,
					true
					);

				break;

			case 6:

				restaurantDialogueStep = 7;

				dialogueBox.startOptions(
					restaurantOption1,
					restaurantOption2
					);

				break;
			}

			return;
		}

		if (restaurantDialogueStep == 0)
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

			if (rectanglesOverlap(
				npcRect,
				mouseRect
				))
			{
				restaurantDialogueStep = 1;


				dialogueBox.startDialogue(
					"Restaurant Owner",
					restaurantOwnerLines1,
					1,
					false
					);
			}


			return;
		}


		return;
	}

	if (currentState == COOKING_GAME)
	{
		if (
			button == GLUT_LEFT_BUTTON &&
			state == GLUT_DOWN
			)
		{
			cookingGame.handleClick(
				mx,
				my
				);
		}

		return;
	}

	if (currentState == SERVING_GAME)
	{
		if (
			button == GLUT_LEFT_BUTTON &&
			state == GLUT_DOWN
			)
		{
			servingGame.handleClick(
				mx,
				my
				);
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
}

// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, 
// GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT

void fixedUpdate()
{
	if (currentState != VILLAGE)
	{
		return;
	}

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


	if (
		rectanglesOverlap(
		player.getRect(),
		restaurantTrigger
		)
		)
	{
		enterRestaurant();

		return;
	}

	return;

	if (currentState == RESTAURANT)
	{

		if (dialogueBox.isActive())
		{
			return;
		}


		bool up =
			isKeyPressed('w') ||
			isSpecialKeyPressed(GLUT_KEY_UP);


		bool down =
			isKeyPressed('s') ||
			isSpecialKeyPressed(GLUT_KEY_DOWN);


		bool left =
			isKeyPressed('a') ||
			isSpecialKeyPressed(GLUT_KEY_LEFT);


		bool right =
			isKeyPressed('d') ||
			isSpecialKeyPressed(GLUT_KEY_RIGHT);


		player.handleInput(
			up,
			down,
			left,
			right,
			restaurantMap
			);


		return;
	}

	if (currentState == COOKING_GAME)
	{
		cookingGame.update();


		// Cooking game completed successfully.
		if (cookingGame.isFinished())
		{
			enterRestaurant();

			return;
		}


		// Cooking game failed and player clicked retry.
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


		// Serving game completed successfully.
		if (servingGame.isFinished())
		{
			enterRestaurant();

			return;
		}


		// Serving game failed and player clicked retry.
		if (servingGame.isRetryRequested())
		{
			servingGame.start();

			return;
		}


		return;
	}

}

void update(){

	fixedUpdate();
	player.updateAnimation();

}

int main()
{

	iSetTimer(10, update);
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Echoes of Aurion");
	
	mainMenu.loadImages();
	settings.loadImages();
	credits.loadImages();
	nameInput.loadImages();

	player.loadImages();
	dialogueBox.loadImages();

	throneRoomMap.init("Images//map_throne_room.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
	hallwayMap.init("Images//map_palace_hallway.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	villageMap.init("Images//map_emberfall_village.png", 3200, 1800, true);

	restaurantMap.init("Images//map_restaurant.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	villageMap.addObstacle(0, 0, 765, 30);
	villageMap.addObstacle(0, 0, 740, 73);
	villageMap.addObstacle(0, 73, 728, 56);
	villageMap.addObstacle(0, 129, 709, 177);
	villageMap.addObstacle(0, 306, 670, 70);
	villageMap.addObstacle(0, 376, 460, 26);
	villageMap.addObstacle(0, 402, 664, 111);
	villageMap.addObstacle(0, 513, 585, 63);
	villageMap.addObstacle(0, 576, 518, 27);
	villageMap.addObstacle(0, 603, 476, 69);
	villageMap.addObstacle(0, 672, 559, 138);
	villageMap.addObstacle(0, 810, 541, 57);
	villageMap.addObstacle(0, 867, 516, 93);
	villageMap.addObstacle(0, 960, 416, 132);
	villageMap.addObstacle(0, 1122, 498, 33);
	villageMap.addObstacle(0, 1155, 428, 525);
	villageMap.addObstacle(0, 1488, 1267, 192);
	villageMap.addObstacle(568, 1275, 684, 405);
	villageMap.addObstacle(1426, 1390, 170, 290);
	villageMap.addObstacle(1297, 1569, 87, 111);
	villageMap.addObstacle(1594, 1572, 1606, 108);
	villageMap.addObstacle(1858, 1302, 153, 378);
	villageMap.addObstacle(2011, 1362, 108, 318);
	villageMap.addObstacle(2119, 1293, 177, 387);
	villageMap.addObstacle(2296, 1350, 171, 330);
	villageMap.addObstacle(2587, 1263, 613, 417);
	villageMap.addObstacle(2521, 1233, 679, 30);
	villageMap.addObstacle(2467, 1209, 733, 24);
	villageMap.addObstacle(2425, 1185, 775, 24);
	villageMap.addObstacle(2275, 888, 925, 297);
	villageMap.addObstacle(2600, 834, 600, 54);
	villageMap.addObstacle(1990, 519, 165, 240);
	villageMap.addObstacle(2104, 358, 543, 251);
	villageMap.addObstacle(1498, 390, 261, 297);
	villageMap.addObstacle(835, 741, 201, 276);
	villageMap.addObstacle(1003, 555, 207, 285);

	king.init(595, 425, PLAYER_WIDTH, PLAYER_HEIGHT, "King");
	king.loadImage("Images//king.png");

	npc1.init(172, 1086, PLAYER_WIDTH, PLAYER_HEIGHT, "NPC1"); // near the bridge
	npc1.loadImage("Images//idle_npc_1.png");

	npc2.init(2410, 315, PLAYER_WIDTH, PLAYER_HEIGHT, "NPC2"); // near a house
	npc2.loadImage("Images//idle_npc_2.png");

	restaurantNpc.init(RESTAURANT_NPC_X, RESTAURANT_NPC_Y, PLAYER_WIDTH, PLAYER_HEIGHT, "Restaurant Owner");
	restaurantNpc.loadImage("Images//idle_npc_3.png");

	cookingGame.loadImages();

	servingGame.loadImages();
	
	iStart();
	return 0;
}