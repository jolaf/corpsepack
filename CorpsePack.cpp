#include <conprint.h>
#include <MAUtil/Moblet.h>
#include <MAUI/Screen.h>
#include <MAUI/Label.h>
#include <MAUI/Font.h>
#include "MAHeaders.h"

using namespace MAUtil;
using namespace MAUI;

class CorpsePack : public Moblet {
private:
    MAUI::Font font;
public:
    CorpsePack() : font(CORPSE_PACK_FONT) {
    }

    void keyPressEvent(int keyCode) {
        font.drawString("Hello world! Всем привет!", 2, 2);
        maUpdateScreen();
    }
    void keyReleaseEvent(int keyCode) {
        // todo: handle key releases
    }
};

extern "C" int MAMain() {
    Moblet::run(new CorpsePack());
    return 0;
};
