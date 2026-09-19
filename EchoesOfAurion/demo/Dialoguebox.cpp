#include <cstring>
#include "DialogueBox.hpp"
#include "Constants.hpp"
#include "glut.h"

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int img);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);

//#define GLUT_BITMAP_HELVETICA_18 ((void*)8)

//int glutBitmapWidth(void* font, int character);

const int NAMEPLATE_CENTER_X = 425;
const int NAMEPLATE_TEXT_Y = 207;

const int TEXT_X = 230;
const int TEXT_FIRST_LINE_Y = 126;
const int TEXT_MAX_WIDTH = 670;
const int TEXT_LINE_HEIGHT = 30;

const int OPTION_X_MIN = 615, OPTION_X_MAX = 1147;
const int OPTION1_Y_MIN = 289, OPTION1_Y_MAX = 317;   // upper row
const int OPTION2_Y_MIN = 242, OPTION2_Y_MAX = 270;   // lower row
const int OPTION_TEXT_X = 645;
const int OPTION_TEXT_PAD = 9;

const int TYPE_SPEED = 75;

int getTextWidth(const char* text)
{
	int width = 0;

	for (int i = 0; text[i] != '\0'; i++)
	{
		width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	return width;
}

static int wrapIntoLines(const char* text, int maxWidth, char out[][250], int maxLines)
{
	char word[100];
	char line[250];
	int count = 0;

	line[0] = '\0';
	const char* p = text;

	while (*p)
	{
		int i = 0;
		while (*p && *p != ' ' && *p != '\n') word[i++] = *p++;
		word[i] = '\0';

		char testLine[250];
		strcpy_s(testLine, line);
		if (strlen(testLine) > 0) strcat_s(testLine, " ");
		strcat_s(testLine, word);

		if (getTextWidth(testLine) > maxWidth && strlen(line) > 0)
		{
			if (count < maxLines) strcpy_s(out[count++], line);
			strcpy_s(line, word);
		}
		else
		{
			strcpy_s(line, testLine);
		}

		if (*p == ' ') p++;

		if (*p == '\n')
		{
			if (count < maxLines) strcpy_s(out[count++], line);
			line[0] = '\0';
			p++;
		}
	}

	if (strlen(line) > 0 && count < maxLines) strcpy_s(out[count++], line);

	return count;
}

int wrappedLength(const char* text, int maxWidth)
{
	char wrapped[12][250];
	int n = wrapIntoLines(text, maxWidth, wrapped, 12);

	int total = 0;
	for (int i = 0; i < n; i++) total += (int)strlen(wrapped[i]);

	return total;
}

void drawWrappedText(const char* text, int x, int y, int maxWidth, int lineHeight, int reveal)
{
	char wrapped[12][250];
	int n = wrapIntoLines(text, maxWidth, wrapped, 12);

	int shown = 0;

	for (int i = 0; i < n; i++)
	{
		int len = (int)strlen(wrapped[i]);

		if (reveal >= shown + len)
		{
			iText(x, y, wrapped[i], GLUT_BITMAP_HELVETICA_18);
		}
		else if (reveal > shown)
		{
			int k = reveal - shown;
			char partial[250];
			for (int c = 0; c < k; c++) partial[c] = wrapped[i][c];
			partial[k] = '\0';
			iText(x, y, partial, GLUT_BITMAP_HELVETICA_18);
			break;
		}
		else break;

		shown += len;
		y -= lineHeight;
	}
}

/*const int OPTION_X_MIN = 260;
const int OPTION_X_MAX = 1400;
const int OPTION1_Y_MIN = 250, OPTION1_Y_MAX = 320; // option 1
const int OPTION2_Y_MIN = 170, OPTION2_Y_MAX = 240; // option 2*/

void DialogueBox::loadImages()
{
	npcBox = iLoadImage("Images//dialogue_box.png");
	mcBox = iLoadImage("Images//dialogue_box_mc.png");
	optionBox = iLoadImage("Images//dialogue_options.png");
	optionHover1 = iLoadImage("Images//dialogue_option_1.png");
	optionHover2 = iLoadImage("Images//dialogue_option_2.png");

	lineCount = 0;
	currentLine = 0;
	active = false;
	showingOptions = false;
}

void DialogueBox::startDialogue(char speakerName[], char* dialogueLines[], int count, bool mcSpeaking, int npcBoxImage)
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
	revealChars = 0;
	typeTimer = 0;
	lineTotalChars = wrappedLength(lines[0], TEXT_MAX_WIDTH);
	active = true;
	showingOptions = false;
	isMCSpeaking = mcSpeaking;
	currentNpcBox = (npcBoxImage == -1) ? npcBox : npcBoxImage;
}

void DialogueBox::advance()
{
	if (!active || showingOptions)
	{
		return;
	}

	if (isTyping())
	{
		finishTyping();
		return;
	}

	currentLine++;

	if (currentLine >= lineCount)
	{
		active = false;
		return;
	}

	revealChars = 0;
	typeTimer = 0;
	lineTotalChars = wrappedLength(lines[currentLine], TEXT_MAX_WIDTH);
}

bool DialogueBox::isActive()
{
	return active;
}

void DialogueBox::startOptions(char option1[], char option2[])
{
	strcpy_s(optionText1, option1);
	strcpy_s(optionText2, option2);

	if (currentLine >= lineCount) currentLine = lineCount - 1;
	if (currentLine < 0) currentLine = 0;
	revealChars = lineTotalChars = wrappedLength(lines[currentLine], TEXT_MAX_WIDTH);

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

void DialogueBox::update()
{
	if (!active || showingOptions) return;
	if (revealChars >= lineTotalChars) return;

	typeTimer += TYPE_SPEED;
	if (typeTimer >= 100)
	{
		revealChars += typeTimer / 100;
		typeTimer = typeTimer % 100;
		if (revealChars > lineTotalChars) revealChars = lineTotalChars;
	}
}

bool DialogueBox::isTyping()
{
	return active && !showingOptions && revealChars < lineTotalChars;
}

void DialogueBox::finishTyping()
{
	revealChars = lineTotalChars;
}

void DialogueBox::close()
{
	active = false;
	showingOptions = false;
}

void DialogueBox::draw(int mouseX, int mouseY)
{
	if (!active)
	{
		return;
	}

	int box = isMCSpeaking ? mcBox : currentNpcBox;
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, box);

	iSetColor(255, 255, 255);

	int nameWidth = getTextWidth(speaker);
	iText(NAMEPLATE_CENTER_X - nameWidth / 2, NAMEPLATE_TEXT_Y, speaker, GLUT_BITMAP_HELVETICA_18);

	drawWrappedText(lines[currentLine], TEXT_X, TEXT_FIRST_LINE_Y, TEXT_MAX_WIDTH, TEXT_LINE_HEIGHT, revealChars);

	if (showingOptions)
	{
		int hovered = checkOptionClick(mouseX, mouseY);

		if (hovered == 1)      iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, optionHover1);
		else if (hovered == 2) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, optionHover2);
		else                   iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, optionBox);

		iSetColor(255, 255, 255);
		iText(OPTION_TEXT_X, OPTION1_Y_MIN + 9, optionText1, GLUT_BITMAP_HELVETICA_18);
		iText(OPTION_TEXT_X, OPTION2_Y_MIN + 9, optionText2, GLUT_BITMAP_HELVETICA_18);

	}

}