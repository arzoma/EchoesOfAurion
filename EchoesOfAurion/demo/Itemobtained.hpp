#ifndef ITEMOBTAINED_HPP
#define ITEMOBTAINED_HPP

class ItemObtained
{

private:

	int frames[3];
	int itemIcon;

	bool active;
	int frame;
	int frameTimer;

public:

	void loadImages();

	void show(int iconImage);
	void hide();
	bool getIsActive();

	void update();
	void draw();

};

#endif