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
    Layout* footer;
    Label* leftSoftButton;
    Label* rightSoftButton;
	enum States { MAIN, POPUP, EVENT } state;

public:
    DrawingContext* drawingContext;

    CorpsePackScreen(DrawingContext* drawingContext) {
        this->drawingContext = drawingContext;
        MAExtent screenSize = maGetScrSize();
        int screenWidth = EXTENT_X(screenSize);
        int screenHeight = EXTENT_Y(screenSize);
        Layout* layout = new Layout(0, 0, screenWidth, screenHeight, NULL, 1, 3);

        const char* title = "CorpsePack v0.1  ËÄÇ: ÂÀÐìèÿ";
        int labelHeight = EXTENT_Y(drawingContext->boldFont->getStringDimensions(title)) + 1;
        header = new Label(0, 0, screenWidth, labelHeight, NULL, title, 0xff0000 /*drawingContext->backgroundColor*/, drawingContext->boldFont);

        footer = new Layout(0, 0, screenWidth, labelHeight, NULL, 2, 1);
        leftSoftButton = new Label(0,0, screenWidth / 2, labelHeight, footer, "Help", 0xff0000, drawingContext->boldFont);
        leftSoftButton->setHorizontalAlignment(Label::HA_LEFT);
        leftSoftButton->setPaddingLeft(EXTENT_X(drawingContext->boldFont->getStringDimensions("M")));
        rightSoftButton = new Label(0,0, screenWidth / 2, labelHeight, footer, "Exit", 0xff0000, drawingContext->boldFont);
        rightSoftButton->setHorizontalAlignment(Label::HA_RIGHT);
        rightSoftButton->setPaddingRight(EXTENT_X(drawingContext->boldFont->getStringDimensions("M")));

        content = new ListBox(0, 0, screenWidth, screenHeight - header->getHeight() - footer->getHeight(), NULL, ListBox::LBO_VERTICAL, ListBox::LBA_LINEAR, false);
        content->setBackgroundColor(0xffffff);
        layout->add(header);
        layout->add(content);
        layout->add(footer);
        main = layout;
        setMain(main);
        state = MAIN;
    }

    ~CorpsePackScreen() {
        delete main;
    }

    void keyPressEvent(int keyCode) {
        switch(keyCode) {
        case MAK_SOFTLEFT:
        case MAK_SOFTRIGHT:
        	if (state == MAIN) {
        		state = POPUP;
        	}
        case MAK_DOWN:
        	if (state == MAIN) {
        		content->selectNextItem(true);
        	}
            break;
        case MAK_UP:
        	if (state == MAIN) {
        		content->selectPreviousItem(true);
        	}
            break;
        }
        maUpdateScreen();
    }

    void keyReleaseEvent(int keyCode) {
    	if (state == POPUP) {
    		state = MAIN;
    	}
        maUpdateScreen();
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

    void keyReleaseEvent(int keyCode) {
    	screen->keyReleaseEvent(keyCode);
		if (keyCode == MAK_SOFTRIGHT) {
			closeEvent(); // calls close();
		}
    }
};

extern "C" int MAMain() {
    Moblet::run(new CorpsePackMoblet());
    return 0;
};
