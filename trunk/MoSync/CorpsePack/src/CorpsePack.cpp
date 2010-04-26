#include <limits.h>
#include <matime.h>
#include <mavsprintf.h>
#include <maxtoa.h>

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

#include "Util.h"
#include "HeapInfo.h"
#include "UIElements.h"
#include "CorpsePack.h"

Context* context = new Context(CONTEXT);
String& helpText = getString(HELP_TEXT);


class EventTimerListener : public TimerListener {
public:
    Character* character;
    int time;

    EventTimerListener(Character* character) : character(character), time(0) {
        update();
    }

    void runTimerEvent() {
        update();
    }

    /**
     * Must be called each time when it's supposed that Character's event context
     * may have been changed and new events may have been queued.
     * Called automatically from EventTimerListener() and runTimerEvent().
     */
    void update() {
        CharacterEvent* event = character->getNextEvent(time);
        if (event) {
            // ToDo: process event - it must be fast, event confirmation must be done elsewhere.
        }
        if (time >= 0) {
            // If time == 0, do not call getNextEvent() and process it immediately,
            // but set timer and wait for the next tick instead - to make sure
            // runTimerEvent() runs fast.
            Environment::getEnvironment().addTimer(this, time * 1000, 1);
        } else { // time < 0
            // Do not add timer, as no events are expected.
        }
    }
};

class CorpsePackScreen : public Screen, TimerListener {
protected:
    Moblet* moblet;
    Widget* main;
    Label* header;
    ListBox* content;
    Layout* footer;
    Label* leftSoftButton;
    Label* rightSoftButton;
    PopUp* popup;
    PopUp* helpPopUp;
    Engine* engine;
    enum States { MAIN, POPUP_GROWING, POPUP, POPUP_SHRINKING } state;

public:
    CorpsePackScreen(Moblet* moblet) {
        lprintfln("# Started");
        this->moblet = moblet;
        MAExtent screenSize = maGetScrSize();
        Layout* layout = new Layout(0, 0, EXTENT_X(screenSize), EXTENT_Y(screenSize), NULL, 1, 3);

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
        String buttons[] = {"Yes", "No", "Help"};
        helpPopUp = new PopUp(main, "Help", helpText, MAK_SOFTLEFT, 3, buttons);
        engine = &Engine::getSingleton();
        updateStats();
        moblet->addTimer(this, 500, 0);
    }

    ~CorpsePackScreen() {
        delete main;
    }

    void keyPressEvent(int keyCode) {
        lprintfln("Pressed %d", keyCode);
        if (popup && popup->processKeyPress(keyCode)) {
            hidePopUp();
        } else {
            switch(keyCode) {
            case MAK_SOFTLEFT:
                showPopUp(helpPopUp);
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
            hidePopUp();
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

    void runTimerEvent() {
        updateStats();
    }

    void updateStats() {
    	char b1[16], b2[16], title[64];
        sprintf(title, "%s %s %s %d%%", memInfo(b1, maFreeObjectMemory(), maTotalObjectMemory()), memInfo(b2, freeHeapMemory(), totalHeapMemory()), justTime(), maGetBatteryCharge());
        header->setCaption(title);
    }

    void showPopUp(PopUp* popup) {
        this->popup = popup;
        engine->showOverlay(0, 0, popup);
    }

    void hidePopUp() {
        engine->hideOverlay();
        this->popup = NULL;
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
    Character* character = new Character();
    Moblet::run(new CorpsePackMoblet());
    return 0;
};
