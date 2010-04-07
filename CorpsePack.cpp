#include <conprint.h>
#include <MAUtil/util.h>
#include <MAUtil/Moblet.h>
#include <MAUI/Engine.h>
#include <MAUI/Font.h>
#include <MAUI/Label.h>
#include <MAUI/Layout.h>
#include <MAUI/ListBox.h>
#include <MAUI/Screen.h>
#include "MAHeaders.h"

using namespace MAUtil;
using namespace MAUI;

/*
 * Key reference:
 *
 * L Help (displays this info until key is released)
 * R exit
 *
 * 1
 * 2
 * 3 Status (displays character status until key is released)
 *
 * 4 Wound (displays location menu, produces events)
 * 5 Event (provides form for event code entering)
 * 6 (other special event?)
 *
 * 7
 * 8
 * 9
 *
 * *
 * 0
 * #
 */

class DrawingContext {
public:
	Font* mainFont;
	Font* boldFont;
	int backgroundColor;

	DrawingContext(MAHandle resourceID) {
		int i;
		int position = 0;
		maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
		this->mainFont = new Font(i);
		maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
		this->boldFont = new Font(i);
		maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
		backgroundColor = i;
	}

	DrawingContext(MAHandle mainFontResource, MAHandle boldFontResource, int backgroundColor) {
		this->mainFont = new Font(mainFontResource);
		this->boldFont = new Font(boldFontResource);
		this->backgroundColor = backgroundColor;
	}
};

class CorpsePackScreen : public Screen {
private:
	Widget* main;
	Label* header;
	ListBox* content;
	Label* footer;
	int y;

public:
	DrawingContext* drawingContext;

	CorpsePackScreen(DrawingContext* drawingContext) {
		y = 10;
		this->drawingContext = drawingContext;
    	MAExtent screenSize = maGetScrSize();
    	int screenWidth = EXTENT_X(screenSize);
    	int screenHeight = EXTENT_Y(screenSize);
    	Layout* layout = new Layout(0, 0, screenWidth, screenHeight, NULL, 1, 3);
    	header = new Label(0, 0, screenWidth, 0, NULL, "CorpsePack v0.1  ËÄÇ: ÂÀÐìèÿ", 0xff0000 /*drawingContext->backgroundColor*/, drawingContext->boldFont);
    	header->setDrawBackground(true);
    	header->setAutoSizeX(false);
    	header->setAutoSizeY(true);
		footer = new Label(0, 0, screenWidth, 0, NULL, "Help      Exit", 0xff0000 /*drawingContext->backgroundColor*/, drawingContext->boldFont);
		footer->setDrawBackground(true);
		footer->setAutoSizeX(false);
		footer->setAutoSizeY(true);
		content = new ListBox(0, 0, screenWidth, screenHeight - header->getHeight() - footer->getHeight() - 30, NULL, ListBox::LBO_VERTICAL, ListBox::LBA_LINEAR, false);
		content->setDrawBackground(true);
		layout->add(header);
		layout->add(content);
		layout->add(footer);
		main = layout;
		setMain(main);
	}

	~CorpsePackScreen() {
		delete main;
	}

	void keyPressEvent(int keyCode)	{
		drawingContext->mainFont->drawString(MAUtil::integerToString(keyCode).c_str(), 0, y += 10);
        maUpdateScreen();
    	switch(keyCode) {
		case MAK_DOWN:
			content->selectNextItem(true);
			break;
		case MAK_UP:
			content->selectPreviousItem(true);
			break;
    	}
	}

    void keyReleaseEvent(int keyCode) {
    	drawingContext->mainFont->drawString(MAUtil::integerToString(keyCode).c_str(), 0, y += 10);
        maUpdateScreen();
    	if (keyCode == MAK_SOFTRIGHT) {
			maExit(0);
		}
    }
};

class CorpsePackMoblet : public Moblet {

private:
	CorpsePackScreen* screen;

public:
    CorpsePackMoblet() {
    	screen = new CorpsePackScreen(new DrawingContext(DRAWING_CONTEXT));
    	screen->show();
    }

    ~CorpsePackMoblet() {
    	delete screen;
    }
};

extern "C" int MAMain() {
    Moblet::run(new CorpsePackMoblet());
    return 0;
};
