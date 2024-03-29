#ifndef __CorpsePack__Character_h__
#define __CorpsePack__Character_h__

#include "Platform.h"

class EventSequence {
public:
    int code;

    EventSequence(int code);
};

class Disease : public EventSequence {
public:
	Disease(int code);
};

class Implant : public EventSequence {
public:
	Implant(int code);
};

class CharacterEvent {
public:
    enum Type { SILENT, CONFIRM, YES_NO };
    enum Confirmation { YES, NO };

    const Type type;
    const string& text;

    CharacterEvent(const Type type, const string& text);
};

class Character {
public:
    int hitPoints;
    int dynamics;

    Character();

    /**
     * Instructs the Character that 'time' seconds has passed
     * and retrieves the first event that should have occurred
     * until that point.
     *
     * The 'time' parameter is updated with the number of seconds
     * after which the next event is going to occur. Thus calling
     * the method repeatedly passing the same variable allows
     * to extract the occurring events one by one.
     *
     * @param   time
     *          Used to specify the number of seconds that have passed.
     *          Also used to return the number of seconds after which
     *          the next event is expected to occur.
     *          0 means the next event is immediately ready.
     *          Negative value means no more events is expected to occur.
     *
     * @return  The first of the events that should have occurred.
     *          NULL means no events should have occurred.
     */
    CharacterEvent* getNextEvent(int& time);

    /**
     * Returns the current character status in text form.
     *
     * @return  String describing current character status.
     */
    string& getStatus();
};

#endif // __CorpsePack__Character_h__
