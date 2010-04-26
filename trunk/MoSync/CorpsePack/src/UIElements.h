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

class Reconfigurable {
public:
    enum DimensionMode { SET, CHILDREN, PARENT };
    enum SkinMode { PADDING_ONLY, BACKGROUND, SKIN };

    DimensionMode widthMode;
    DimensionMode heightMode;

    Widget* target;
    Context* context;
    SkinMode skinMode;

    Reconfigurable(Widget* target, int width, int height, Context* context, SkinMode skinMode);

    virtual void configure() = 0;
};

class ShortLabel : public Label, public Reconfigurable {
public:
    ShortLabel(int width, int height, Context* context, SkinMode skinMode, const String& caption);

    void configure();
};

class CompactListBox : public ListBox, public Reconfigurable {
public:
    CompactListBox(int width, int height, Context* context, SkinMode skinMode, ListBoxOrientation orientation = LBO_VERTICAL);

    void configure();
};

// ToDo: AutoLayout

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
