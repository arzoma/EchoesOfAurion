#include <cstdio>
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

enum GameState{

	MAIN_MENU,
	SETTINGS,
	CREDITS,
	NAME_INPUT,
	THRONE_ROOM,
	HALLWAY,
	VILLAGE

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

Npc king;
Npc npc1;
Npc npc2;

DialogueBox dialogueBox;

int mouseX = 0;
int mouseY = 0;

int throneStep = 0;  // 0 = king talking, 1 = mc's reply, 2 = moved on
int hallwayStep = 0; // 0 = mc's lines playing, 1 = "Travel to Emberfall" prompt showing

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

const int THRONE_PLAYER_X = 595, THRONE_PLAYER_Y = 150;
const int HALLWAY_PLAYER_X = 595, HALLWAY_PLAYER_Y = 300;
const int VILLAGE_SPAWN_X = 625, VILLAGE_SPAWN_Y = 1200;

const double TRAVEL_TEXT_X_MIN = 0.40, TRAVEL_TEXT_X_MAX = 0.60;
const double TRAVEL_TEXT_Y_MIN = 0.10, TRAVEL_TEXT_Y_MAX = 0.16;

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

	villageMap.addObstacle(0, 0, 600, 700);      // lake
	villageMap.addObstacle(500, 900, 300, 300);  // house 1
	villageMap.addObstacle(1800, 900, 300, 300); // house 2

	king.init(595, 425, PLAYER_WIDTH, PLAYER_HEIGHT, "King");
	king.loadImage("Images//king.png");

	npc1.init(680, 850, PLAYER_WIDTH, PLAYER_HEIGHT, "NPC1"); // near a house
	npc1.loadImage("Images//idle_front_1.png");

	npc2.init(1850, 650, PLAYER_WIDTH, PLAYER_HEIGHT, "NPC2"); // near the bridge
	npc2.loadImage("Images//idle_front_1.png");
	
	iStart();
	return 0;
}