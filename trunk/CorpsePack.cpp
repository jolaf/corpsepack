#include <mavsprintf.h>
#include <MAUtil/util.h>
#include <MAUtil/Environment.h>
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

#define POPUP_STATES 10

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

#define POPUP_PADDING 10

Font* defaultFont = new Font(CORPSE_PACK_FONT);

class PopUp : public ListBox {
protected:
	int keyCode;

public:
	PopUp(const String& caption, int keyCode = 0, int numButtons = 0, String* buttonCaptions = NULL)
			: ListBox(POPUP_PADDING, POPUP_PADDING, EXTENT_X(maGetScrSize()) - 2 * POPUP_PADDING, EXTENT_Y(maGetScrSize()) - 2 * POPUP_PADDING, NULL, ListBox::LBO_VERTICAL) {
		this->keyCode = keyCode;
		Label* content = new Label(0, 0, getWidth(), 0, this);
		content->setMultiLine();
		content->setFont(defaultFont);
		content->setCaption(caption);
		content->setAutoSizeY(true);
		if (numButtons > 0) {
			Layout* buttons = new Layout(0, 0, getWidth(), 10, this, numButtons, 1);
			for (int i = 0; i < numButtons; i++) {
				Label* button = new Label(0, 0, 0, 0, buttons, buttonCaptions[i], 0x000080, defaultFont);
				button->setAutoSizeX(true);
				button->setAutoSizeY(true);
			}
		}
		Engine::getSingleton().showOverlay(0, 0, this);
	}

	virtual ~PopUp() {
		Engine::getSingleton().hideOverlay();
	}

	int processKeyPress(int keyCode) {
		return 0; // ToDo
	}

	int processKeyRelease(int keyCode) {
		if (this->keyCode && keyCode == this->keyCode) {
			return 1;
		}
		return 0;
	}
};

class CorpsePackScreen : public Screen {
protected:
	Moblet* moblet;
    DrawingContext* drawingContext;
    int screenWidth;
    int screenHeight;
    Widget* main;
    Label* header;
    ListBox* content;
    Layout* footer;
    Label* leftSoftButton;
    Label* rightSoftButton;
    PopUp* popup;
	enum States { MAIN, POPUP_GROWING, POPUP, POPUP_SHRINKING } state;

public:
    CorpsePackScreen(Moblet* moblet, DrawingContext* drawingContext) {
    	lprintfln("# Started");
    	this->moblet = moblet;
        this->drawingContext = drawingContext;
        MAExtent screenSize = maGetScrSize();
        screenWidth = EXTENT_X(screenSize);
        screenHeight = EXTENT_Y(screenSize);
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
        popup = NULL;
        main = layout;
        setMain(main);
        state = MAIN;
    }

    ~CorpsePackScreen() {
        delete main;
    }

    void keyPressEvent(int keyCode) {
    	lprintfln("Pressed %d", keyCode);
    	if (popup && popup->processKeyPress(keyCode)) {
    		delete popup;
    	} else {
			String buttons[] = {"Yes", "No", "Help"};
			switch(keyCode) {
			case MAK_SOFTLEFT:
				popup = new PopUp("Help!", MAK_SOFTLEFT, 3, buttons);
				break;
			case MAK_SOFTRIGHT:
				if (state == MAIN) {
					state = POPUP;
				}
				break;
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
    	}
        maUpdateScreen();
    }

    void keyReleaseEvent(int keyCode) {
    	lprintfln("Released %d", keyCode);
    	if (popup && popup->processKeyRelease(keyCode)) {
    		delete popup;
    	} else {
			switch(keyCode) {
			case MAK_SOFTLEFT:
				// if (we're in help) {
				break;
			case MAK_SOFTRIGHT:
				// Shutdown, context should have already been saved in RMS
				moblet->closeEvent(); // calls close();
				break;
			}
			maUpdateScreen();
    	}
    }
};

class CorpsePackMoblet : public Moblet {
private:
    CorpsePackScreen* screen;
public:
    CorpsePackMoblet() {
        screen = new CorpsePackScreen(this, new DrawingContext(DRAWING_CONTEXT));
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
