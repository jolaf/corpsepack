#ifndef __CorpsePack__UIElements_h__
#define __CorpsePack__UIElements_h__

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

    Context(MAHandle resourceID);

    ~Context();

    void setSkinTo(Widget* widget);
};

class AutoUI {
public:
    enum Width { NARROW, WIDE };
    enum Height { LOW, HIGH };
    enum SkinMode { PADDING_ONLY, BACKGROUND, SKIN };

    Width width;
    Height height;
    Context* context;
    SkinMode skinMode;

    AutoUI(Width width, Height height, Context* context, SkinMode skinMode);
};

class ReconfigurableWidget {
public:
    enum Width { NARROW, WIDE };
    enum Height { LOW, HIGH };
    enum SkinMode { PADDING_ONLY, BACKGROUND, SKIN };

    virtual void configure() = 0;
};

class ShortLabel : public Label, public ReconfigurableWidget {
public:
    enum WidthMode { NARROW, WIDE };

    WidthMode widthMode;
    Context* context;
    SkinMode skinMode;

    ShortLabel(const String &caption, WidthMode widthMode = NARROW, Context* context = NULL, SkinMode skinMode = SKIN);

    void configure();
};

// ToDo: AutoListBox, AutoLayout

class PopUp : public ListBox {
protected:
    int keyCode;

public:
    PopUp(Widget* parent, const String& titleCaption, const String& caption, int keyCode = 0, int numButtons = 0, String* buttonCaptions = NULL);
    int processKeyPress(int keyCode);
    int processKeyRelease(int keyCode);
};

class ProgressBar : public Widget {
public:
    enum Direction { LEFT_TO_RIGHT, RIGHT_TO_LEFT };

protected:
    int percentage;
    Direction direction;
    Label* empty;
    Label* full;

    void updateContents();

public:
    ProgressBar(int x, int y, int width, int height, Widget* parent = NULL, int emptyColor = 0xffffff, int fullColor = 0x000000, Direction direction = LEFT_TO_RIGHT, int initialPercentage = 0);

    int getPercentage();
    void setPercentage(int percentage);
    void drawWidget();
};

#endif // __CorpsePack__UIElements_h__
