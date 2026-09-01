#include <cstring>
#include "DialogueBox.hpp"
#include "Constants.hpp"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

#define GLUT_BITMAP_HELVETICA_18 ((void*)8)

void drawWrappedText(
	const char* text,
	int x,
	int y,
	int maxWidth,
	int lineHeight
	)
{
	char line[250];
	char word[100];

	line[0] = '\0';

	const char* p = text;

	while (*p)
	{
		int i = 0;

		while (*p && *p != ' ' && *p != '\n')
		{
			word[i++] = *p++;
		}

		word[i] = '\0';

		char testLine[250];
		strcpy_s(testLine, line);

		if (strlen(testLine) > 0)
			strcat_s(testLine, " ");

		strcat_s(testLine, word);

		int estimatedWidth = (int)strlen(testLine) * 9;

		if (estimatedWidth > maxWidth && strlen(line) > 0)
		{
			iText(x, y, line, GLUT_BITMAP_HELVETICA_18);

			y -= lineHeight;

			strcpy_s(line, word);
		}
		else
		{
			strcpy_s(line, testLine);
		}

		if (*p == ' ')
			p++;

		if (*p == '\n')
		{
			iText(x, y, line, GLUT_BITMAP_HELVETICA_18);
			y -= lineHeight;

			line[0] = '\0';
			p++;
		}
	}

	if (strlen(line) > 0)
		iText(x, y, line, GLUT_BITMAP_HELVETICA_18);
}

const int OPTION_X_MIN = 260;
const int OPTION_X_MAX = 1400;
const int OPTION1_Y_MIN = 250, OPTION1_Y_MAX = 320; // option 1
const int OPTION2_Y_MIN = 170, OPTION2_Y_MAX = 240; // option 2

void DialogueBox::loadImages()
{
	npcBox = iLoadImage("Images//dialogue_box.png");
	mcBox = iLoadImage("Images//dialogue_box_mc.png");
	optionBox = iLoadImage("Images//dialogue_option.png");
	optionHover1 = iLoadImage("Images//dialogue_option_1.png");
	optionHover2 = iLoadImage("Images//dialogue_option_2.png");

	lineCount = 0;
	currentLine = 0;
	active = false;
	showingOptions = false;
}

void DialogueBox::startDialogue(char speakerName[], char* dialogueLines[], int count, bool mcSpeaking)
{
	strcpy_s(speaker, speakerName);

	lineCount = count;
	if (lineCount > MAX_LINES)
	{
		lineCount = MAX_LINES;
	}

	for (int i = 0; i < lineCount; i++)
	{
		strcpy_s(lines[i], dialogueLines[i]);
	}

	currentLine = 0;
	active = true;
	showingOptions = false;
	isMCSpeaking = mcSpeaking;
}

void DialogueBox::advance()
{
	if (!active || showingOptions)
	{
		return;
	}

	currentLine++;
	if (currentLine >= lineCount)
	{
		active = false;
	}
}

bool DialogueBox::isActive()
{
	return active;
}

void DialogueBox::startOptions(char option1[], char option2[])
{
	strcpy_s(optionText1, option1);
	strcpy_s(optionText2, option2);
	showingOptions = true;
	active = true;
}

bool DialogueBox::isShowingOptions()
{
	return showingOptions;
}

int DialogueBox::checkOptionClick(int mx, int my)
{
	if (!showingOptions)
	{
		return 0;
	}

	if (mx >= OPTION_X_MIN && mx <= OPTION_X_MAX)
	{
		if (my >= OPTION1_Y_MIN && my <= OPTION1_Y_MAX) return 1;
		if (my >= OPTION2_Y_MIN && my <= OPTION2_Y_MAX) return 2;
	}

	return 0;
}

void DialogueBox::draw(int mouseX, int mouseY)
{
	if (!active)
	{
		return;
	}

	if (showingOptions)
	{
		int hovered = checkOptionClick(mouseX, mouseY);

		if (hovered == 1)      iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, optionHover1);
		else if (hovered == 2) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, optionHover2);
		else                   iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, optionBox);

		iSetColor(255, 255, 255);
		iText(280, OPTION1_Y_MIN + 25, optionText1, GLUT_BITMAP_HELVETICA_18);
		iText(280, OPTION2_Y_MIN + 25, optionText2, GLUT_BITMAP_HELVETICA_18);

		return;
	}

	int box = isMCSpeaking ? mcBox : npcBox;
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, box);

	iSetColor(255, 255, 255);
	iText(340, 258, speaker, GLUT_BITMAP_HELVETICA_18);            // name plate
	drawWrappedText(
		lines[currentLine],
		280,
		155,
		600,
		30
		); // dialogue text

}