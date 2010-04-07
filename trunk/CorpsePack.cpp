#include <conprint.h>
#include <MAUtil/util.h>
#include <MAUtil/Moblet.h>
#include <MAUI/Engine.h>
#include <MAUI/Font.h>
#include <MAUI/Label.h>
#include <MAUI/Screen.h>
#include "MAHeaders.h"

using namespace MAUtil;
using namespace MAUI;

/*
 * Key reference:
 *
 * 1 Help (displays this info until key is released)
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
 *
 * L
 * R exit
 */

// ToDo: Derive font size from screen size.

class DrawingContext {
	// Should contain background color and all actual fonts
};

class CorpsePackMoblet : public Moblet {

private:
    MAUI::Font font;
    int y;

public:
    CorpsePackMoblet() : font(CORPSE_PACK_FONT), y(10) {
    	MAUI::Engine& engine = MAUI::Engine::getSingleton();
    	engine.setDefaultFont(&font);
    	font.drawString("CorpsePack v0.1  ËÄÇ: ÂÀÐìèÿ", 0, 0);
        maUpdateScreen();
    }

    void keyPressEvent(int keyCode) {
        font.drawString(MAUtil::integerToString(keyCode).c_str(), 0, y += 10);
        maUpdateScreen();
    }

    void keyReleaseEvent(int keyCode) {
        font.drawString(MAUtil::integerToString(keyCode).c_str(), 0, y += 10);
        maUpdateScreen();
    	if (keyCode == MAK_SOFTRIGHT) {
			maExit(0);
		}
    }
};

extern "C" int MAMain() {
    Moblet::run(new CorpsePackMoblet());
    return 0;
};
