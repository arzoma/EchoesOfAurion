#ifndef DIALOGUEBOX_HPP
#define DIALOGUEBOX_HPP

const int MAX_LINES = 20;
const int MAX_LINE_LENGTH = 200;

class DialogueBox
{

private:

	int npcBox;
	int currentNpcBox;
	int mcBox;
	int optionBox;
	int optionHover1;
	int optionHover2;

	char speaker[30];
	char lines[MAX_LINES][MAX_LINE_LENGTH];
	int lineCount;
	int currentLine;
	bool active;
	bool isMCSpeaking;

	bool showingOptions;
	char optionText1[100];
	char optionText2[100];

	int revealChars;
	int lineTotalChars;
	int typeTimer;

public:

	void loadImages();

	void startDialogue(char speakerName[], char* dialogueLines[], int count, bool mcSpeaking, int npcBoxImage = -1);
	void advance();
	bool isActive();

	void startOptions(char option1[], char option2[]);
	bool isShowingOptions();
	int checkOptionClick(int mx, int my);

	void update();
	bool isTyping();
	void finishTyping();
	void close();

	void draw(int mouseX, int mouseY);

};

#endif