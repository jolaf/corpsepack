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

Reconfigurable::Reconfigurable(Widget* target, int width, int height, Context* context, SkinMode skinMode)
		: target(target), context(context), skinMode(skinMode) {
	target->setDrawBackground(false);
	if (context) {
		if (skinMode != PADDING_ONLY) {
			target->setDrawBackground(true);
			if (skinMode == SKIN && context->skin) {
				target->setSkin(context->skin);
			} else {
				target->setBackgroundColor(context->unselectedBackgroundColor);
			}
		}
		target->setPaddingTop(context->paddingTop);
		target->setPaddingRight(context->paddingRight);
		target->setPaddingBottom(context->paddingBottom);
		target->setPaddingLeft(context->paddingLeft);
	}
	if (width > 0) {
		widthMode = SET;
		target->setWidth(width);
	} else {
		widthMode = (width == 0) ? CHILDREN : /* < 0 */ PARENT;
	}
	if (height > 0) {
		heightMode = SET;
		target->setHeight(height);
	} else {
		heightMode = (height == 0) ? CHILDREN : /* < 0 */ PARENT;
	}
}

ShortLabel::ShortLabel(int width, int height, Context* context, SkinMode skinMode, const String& caption)
		: Label(0, 0, 0, 0, NULL), Reconfigurable(this, width, height, context, skinMode) {
	setMultiLine(false);
	if (context) {
		setFont(context->unselectedFont);
	}
	setCaption(caption);
}

void ShortLabel::configure() {
	int stringDimensions = context->selectedFont->getStringDimensions(caption.c_str());
	if (widthMode == CHILDREN) {
		setWidth(EXTENT_X(stringDimensions) + paddingLeft + paddingRight);
	}
	if (heightMode == CHILDREN) {
		setHeight(EXTENT_Y(stringDimensions) + paddingTop + paddingBottom);
	}
}

enum DimensionMode { SET, CHILDREN, PARENT };

DimensionMode widthMode;
DimensionMode heightMode;

CompactListBox::CompactListBox(int width, int height, Context* context, SkinMode skinMode, ListBoxOrientation orientation)
		: ListBox(0, 0, 0, 0, NULL, orientation), Reconfigurable(this, width, height, context, skinMode) {
}

void CompactListBox::configure() {
	int childrenWidth = 0;
	int childrenHeight = 0;
	int numChildren = children.size();
	for (int i = 0; i < numChildren; i++) {
		Widget* child = children[i];
		Reconfigurable* reconfigurable = (Reconfigurable*) child;
		reconfigurable->configure();
		if (reconfigurable->widthMode != PARENT) {
			int childWidth = child->getWidth();
			if (orientation == LBO_VERTICAL) { // Find maximum width
				if (childWidth > childrenWidth) {
					childrenWidth = childWidth;
				}
			} else { // LBO_HORIZONTAL, find total width
				childrenWidth += childWidth;
			}
		}
		if (reconfigurable->heightMode != PARENT) {
			int childHeight = child->getHeight();
			if (orientation == LBO_HORIZONTAL) { // Find maximum height
				if (childHeight > childrenHeight) {
					childrenHeight = childHeight;
				}
			} else { // LBO_VERTICAL, find total height
				childrenHeight += childHeight;
			}
		}
	}
	if (widthMode == CHILDREN) {
		setWidth(childrenWidth + paddingLeft + paddingRight);
	}
	if (heightMode == CHILDREN) {
		setHeight(childrenHeight + paddingTop + paddingBottom);
	}
	for (int i = 0; i < numChildren; i++) {
		Widget* child = children[i];
		Reconfigurable* reconfigurable = (Reconfigurable*) child; // ToDo: This would crash!!!
		if (reconfigurable->widthMode == PARENT) {
			if (orientation == LBO_HORIZONTAL) {
				// child->setWidth(???); // ToDo: calculate equal width
			} else { // LBO_VERTICAL
				child->setWidth(childrenWidth);
			}
		}
		if (reconfigurable->heightMode != PARENT) {
			if (orientation == LBO_VERTICAL) {
				// child->setHeight(???); // ToDo: calculate equal height
			} else { // LBO_HORIZONTAL
				child->setHeight(childrenHeight);
			}
		}
	}
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
