#include <matime.h>
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

#define MAX(a, b) (a >= b ? a : b)

String& getString(MAHandle stringResource) {
	int length = maGetDataSize(stringResource);
	String* output;
	if (length) {
		char* buffer = new char[length + 1];
		maReadData(stringResource, buffer, 0, length);
		buffer[length] = '\x00';
		output = new String(buffer);
		delete[] buffer;
	} else {
		output = new String();
	}
	return *output;
}

class Context {
public:
    Font* selectedFont;
    Font* unselectedFont;
    int selectedBackgroundColor;
    int unselectedBackgroundColor;
    byte paddingLeft;
    byte paddingRight;
    byte paddingTop;
    byte paddingBottom;
    byte lineHeight;
    byte paddedLineHeight;
    WidgetSkin* skin;

    Context(MAHandle resourceID) {
        int i;
        int position = 0;
        maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
        selectedFont = new Font(i);
        maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
        unselectedFont = new Font(i);
        maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
        selectedBackgroundColor = i;
        maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
        unselectedBackgroundColor = i;
        maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
        MAHandle selectedSkin = i;
        maReadData(resourceID, &i, position, sizeof(i)); position += sizeof(i);
        MAHandle unselectedSkin = i;
        byte b;
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        byte x1 = b;
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        byte x2 = b;
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        byte y1 = b;
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        byte y2 = b;
        skin = new WidgetSkin(selectedSkin, unselectedSkin, x1, x2, y1, y2, true, true);
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        paddingTop = b;
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        paddingRight = b;
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        paddingBottom = b;
        maReadData(resourceID, &b, position, sizeof(b)); position += sizeof(b);
        paddingLeft = b;
        lineHeight = MAX(EXTENT_Y(selectedFont->getStringDimensions("M")), EXTENT_Y(unselectedFont->getStringDimensions("M")));
        paddedLineHeight = lineHeight + paddingTop + paddingBottom;
    }

    ~Context() {
    	delete selectedFont;
    	delete unselectedFont;
    	delete skin;
    }

    void setSkinTo(Widget* widget) {
    	widget->setBackgroundColor(unselectedBackgroundColor);
    	widget->setSkin(skin);
    	widget->setPaddingLeft(paddingLeft);
    	widget->setPaddingRight(paddingRight);
    	widget->setPaddingTop(paddingTop);
    	widget->setPaddingBottom(paddingBottom);
    }
};

Context* context = new Context(CONTEXT);
String& helpText = getString(HELP_TEXT);

class PopUp : public ListBox {
protected:
	int keyCode;

public:
	PopUp(Widget* parent, const String& titleCaption, const String& caption, int keyCode = 0, int numButtons = 0, String* buttonCaptions = NULL)
			: ListBox(0, 0, parent->getWidth(), parent->getHeight(), NULL, ListBox::LBO_VERTICAL) {
		this->keyCode = keyCode;
		Label* title = new Label(0, 0, getWidth(), context->paddedLineHeight, NULL, titleCaption, context->unselectedBackgroundColor, context->unselectedFont);
		title->setHorizontalAlignment(Label::HA_CENTER);
		context->setSkinTo(title);
		Layout* menu = NULL;
		if (numButtons > 0) {
			menu = new Layout(0, 0, getWidth(), context->paddedLineHeight, NULL, numButtons, 1);
			context->setSkinTo(menu);
			for (int i = 0; i < numButtons; i++) {
				Label* button = new Label(0, 0, menu->getPaddedBounds().width / numButtons, context->lineHeight, menu, buttonCaptions[i], context->unselectedBackgroundColor, context->unselectedFont);
				button->setHorizontalAlignment((i > 0 && i < numButtons - 1) ? Label::HA_CENTER : (i == 0) ? Label::HA_LEFT : Label::HA_RIGHT);
			}
		}
		Label* content = new Label(0, 0, getWidth(), getHeight() - title->getHeight() - (menu ? menu->getHeight() : 0), NULL, caption, context->unselectedBackgroundColor, context->unselectedFont);
		content->setMultiLine();
		context->setSkinTo(content);
		add(title);
		add(content);
		if (menu) {
			add(menu);
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
    Widget* main;
    Label* header;
    ListBox* content;
    Layout* footer;
    Label* leftSoftButton;
    Label* rightSoftButton;
    PopUp* popup;
	enum States { MAIN, POPUP_GROWING, POPUP, POPUP_SHRINKING } state;

public:
    CorpsePackScreen(Moblet* moblet) {
    	lprintfln("# Started");
    	this->moblet = moblet;
        MAExtent screenSize = maGetScrSize();
        Layout* layout = new Layout(0, 0, EXTENT_X(screenSize), EXTENT_Y(screenSize), NULL, 1, 3);

        // sprintf(title, "%d/%d %d %d%%", maFreeObjectMemory(), maTotalObjectMemory(), split_time(maLocalTime(), new tm())->tm_sec, maGetBatteryCharge());
        header = new Label(0, 0, layout->getWidth(), context->paddedLineHeight, NULL, "CorpsePack v0.1  ËÄÇ: ÂÀÐìèÿ", context->unselectedBackgroundColor, context->unselectedFont);
        context->setSkinTo(header);
        footer = new Layout(0, 0, layout->getWidth(), context->paddedLineHeight, NULL, 2, 1);
        context->setSkinTo(footer);
        leftSoftButton = new Label(0, 0, footer->getPaddedBounds().width / 2, context->lineHeight, footer, "Help", context->unselectedBackgroundColor, context->unselectedFont);
        leftSoftButton->setHorizontalAlignment(Label::HA_LEFT);
        rightSoftButton = new Label(0, 0, footer->getPaddedBounds().width / 2, context->lineHeight, footer, "Exit", context->unselectedBackgroundColor, context->unselectedFont);
        rightSoftButton->setHorizontalAlignment(Label::HA_RIGHT);

        content = new ListBox(0, 0, layout->getWidth(), layout->getHeight() - header->getHeight() - footer->getHeight(), NULL, ListBox::LBO_VERTICAL, ListBox::LBA_LINEAR, false);
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
				popup = new PopUp(main, "Help", helpText, MAK_SOFTLEFT, 3, buttons);
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
        screen = new CorpsePackScreen(this);
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
