#ifndef __CorpsePack__Util_h__
#define __CorpsePack__Util_h__

#include <MAUtil/String.h>

using namespace MAUtil;

#define DAYLIGHT_SAVING_TIME true

String& getString(MAHandle stringResource);

char* greekSize(char* buffer, int value);

char* memInfo(char* buffer, int free, int total);

char* justTime();

#endif // __CorpsePack__Util_h__
