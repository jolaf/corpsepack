#include <MAUI/Font.h>
#include <MAUI/Label.h>
#include <MAUI/Layout.h>
#include <MAUI/ListBox.h>

using namespace MAUtil;
using namespace MAUI;

#include "Platform.h"
#include "UIElements.h"

Context::Context(MAHandle resourceID) {
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
	lineHeight = max(EXTENT_Y(selectedFont->getStringDimensions("M")), EXTENT_Y(unselectedFont->getStringDimensions("M")));
	paddedLineHeight = lineHeight + paddingTop + paddingBottom;
}

Context::~Context() {
	delete selectedFont;
	delete unselectedFont;
	delete skin;
}

void Context::setSkinTo(Widget* widget) {
	widget->setBackgroundColor(unselectedBackgroundColor);
	widget->setSkin(skin);
	widget->setPaddingTop(paddingTop);
	widget->setPaddingRight(paddingRight);
	widget->setPaddingBottom(paddingBottom);
	widget->setPaddingLeft(paddingLeft);
}

AutoUI::AutoUI(Width width, Height height, Context* context, SkinMode skinMode)
		: width(width), height(height), context(context), skinMode(skinMode) {
}

TextLabel::TextLabel(const String &caption, Width width, Height height, Context* context, SkinMode skinMode, bool multiLine)
		: Label(0, 0, 0, 0, NULL), AutoUI(width, height, context, skinMode) {
	setMultiLine(multiLine);
	setFont(context->unselectedFont);
	setCaption(caption);

	if (skinMode == PADDING_ONLY) {
		setDrawBackground(false);
	} else {
		setDrawBackground(true);
		if (skinMode == SKIN && context->skin) {
			setSkin(context->skin);
		} else {
			setBackgroundColor(context->unselectedBackgroundColor);
		}
	}
	if (context) {
		setPaddingTop(context->paddingTop);
		setPaddingRight(context->paddingRight);
		setPaddingBottom(context->paddingBottom);
		setPaddingLeft(context->paddingLeft);
	}
}

void TextLabel::configure() {
	int stringDimensions = context->selectedFont->getStringDimensions(caption.c_str());
	int stringWidth = EXTENT_X(stringDimensions) + paddingLeft + paddingRight;
	int stringHeight = EXTENT_Y(stringDimensions) + paddingTop + paddingBottom;
	int parentWidth = parent->getWidth();

	if (stringWidth > parentWidth) {
		setWidth(parentWidth);
	} else if (width == WIDE ){
		setWidth(parentWidth);
	} else {
		setWidth(stringWidth);
	}
	setHeight(stringHeight); // ToDo: handle multiline somehow
}

PopUp::PopUp(Widget* parent, const String& titleCaption, const String& caption, int keyCode, int numButtons, String* buttonCaptions)
		: ListBox(0, 0, parent->getWidth(), parent->getHeight(), NULL, ListBox::LBO_VERTICAL) {
	this->keyCode = keyCode;
	Label* title = NULL; // new Label(0, 0, getWidth(), context->paddedLineHeight, NULL, titleCaption, context->unselectedBackgroundColor, context->unselectedFont);
	title->setHorizontalAlignment(Label::HA_CENTER);
	//context->setSkinTo(title);
	Layout* menu = NULL;
	if (numButtons > 0) {
		menu = NULL; // new Layout(0, 0, getWidth(), context->paddedLineHeight, NULL, numButtons, 1);
		//context->setSkinTo(menu);
		for (int i = 0; i < numButtons; i++) {
			Label* button = NULL; // new Label(0, 0, menu->getPaddedBounds().width / numButtons, context->lineHeight, menu, buttonCaptions[i], context->unselectedBackgroundColor, context->unselectedFont);
			button->setHorizontalAlignment((i > 0 && i < numButtons - 1) ? Label::HA_CENTER : (i == 0) ? Label::HA_LEFT : Label::HA_RIGHT);
		}
	}
	Label* content = NULL; // new Label(0, 0, getWidth(), getHeight() - title->getHeight() - (menu ? menu->getHeight() : 0), NULL, caption, context->unselectedBackgroundColor, context->unselectedFont);
	//content->setMultiLine();
	//context->setSkinTo(content);
	add(title);
	add(content);
	if (menu) {
		add(menu);
	}
}

int PopUp::processKeyPress(int keyCode) {
	return 0; // ToDo
}

int PopUp::processKeyRelease(int keyCode) {
	if (this->keyCode && keyCode == this->keyCode) {
		return 1;
	}
	return 0;
}

void ProgressBar::updateContents() {
	int fullWidth = (paddedBounds.width * percentage) / 100;
	int emptyWidth = paddedBounds.width - fullWidth;
	empty->setPosition(direction == LEFT_TO_RIGHT ? fullWidth : 0, 0);
	empty->setWidth(emptyWidth);
	empty->setHeight(paddedBounds.height);
	full->setPosition(direction == RIGHT_TO_LEFT ? emptyWidth : 0, 0);
	full->setWidth(fullWidth);
	full->setHeight(paddedBounds.height);
}

ProgressBar::ProgressBar(int x, int y, int width, int height, Widget* parent, int emptyColor, int fullColor, Direction direction, int initialPercentage)
		: Widget(x, y, width, height, parent), direction(direction), percentage(initialPercentage) {
	empty = new Label(0, 0, 0, 0, this);
	empty->setBackgroundColor(emptyColor);
	full = new Label(0, 0, 0, 0, this);
	full->setBackgroundColor(fullColor);
	updateContents();
}

int ProgressBar::getPercentage() {
	return percentage;
}

void ProgressBar::setPercentage(int percentage) {
	this->percentage = percentage;
}

void ProgressBar::drawWidget() {
	updateContents();
	Widget::update();
}
