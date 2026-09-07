#ifndef INVENTORY_HPP
#define INVENTORY_HPP

class Inventory
{

private:

	int iconImage;
	int iconHoverImage;
	int windowImage;

	int pendantIcon;
	int hoodedGiftIcon; // placeholder

	bool hasPendant;
	bool hasHoodedGift;

	int selectedItem; // 0 = none, 1 = pendant, 2 = hooded gift
	bool isOpen;

	bool isInsideBox(int mx, int my, int bx, int by, int bw, int bh);

public:

	void loadImages();

	void givePendant();
	void giveHoodedGift();

	bool getIsOpen();

	bool handleClick(int mx, int my);

	void draw(int mouseX, int mouseY);

};

#endif