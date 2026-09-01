#ifndef NAMEINPUT_HPP
#define NAMEINPUT_HPP

const int MAX_NAME_LENGTH = 15;

class NameInput
{

private:

	int bgTypeHere;
	int bgTyping;
	int bgTypeHereHover;
	int bgTypingHover;

	char playerName[MAX_NAME_LENGTH + 1];
	int nameLength;

	bool isTyping;

	int mouseX;
	int mouseY;

public:

	void loadImages();

	void mouseMove(int mx, int my);

	bool mouseClick(int mx, int my);

	bool handleKeyPress(unsigned char key);

	void draw();

	char* getName();

};

#endif