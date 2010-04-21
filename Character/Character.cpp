/**
 * All system definitions should be put to Platform.h file
 * in platform-specific directory (MoSync/MSVC).
 * There must be no #include <*.h> directives here or in Character.h.
 */
#include "Platform.h"

#include "Character.h"

EventSequence::EventSequence(int code) : code(code) {
}

CharacterEvent::CharacterEvent(const Type type, const String& text) : type(type), text(text) {
}

Character::Character() {
    hitPoints = 3;
    dynamics = 0;
}

CharacterEvent* Character::getNextEvent(int& time) {
    // ToDo: check EventCards, find a first group of close events, return the first one.
	return NULL;
}

String& Character::getStatus() {
    char buffer[256];
    sprintf(buffer, "Хитов: %d\nДинамика: %d", hitPoints, dynamics);
    return *(new String(buffer));
}
