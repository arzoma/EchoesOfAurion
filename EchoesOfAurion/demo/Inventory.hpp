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
	int compassIcon;
	int starwheelIcon;

	bool hasPendant;
	bool hasHoodedGift;
	bool hasCompass;
	bool hasStarwheel;

	int selectedItem; // 0 = none, 1 = pendant, 2 = hearth ember, 3 = compass, 4 = starwheel
	bool isOpen;

	bool isInsideBox(int mx, int my, int bx, int by, int bw, int bh);

public:

	void loadImages();

	void givePendant();
	void giveHoodedGift();

	void giveCompass();
	bool getHasCompass();
	int  getCompassIcon();

	void giveStarwheel();
	bool getHasStarwheel();
	int  getStarwheelIcon();

	int getHoodedGiftIcon();
	bool getHasHoodedGift();

	bool getIsOpen();

	bool handleClick(int mx, int my);

	void draw(int mouseX, int mouseY);

};

#endif