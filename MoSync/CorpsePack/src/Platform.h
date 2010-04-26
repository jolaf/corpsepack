/**
 * This file is included by Character.cpp
 * and contains platform-specific (MoSync) definitions.
 */

#ifndef __CorpsePack__Platform_h__
#define __CorpsePack__Platform_h__

#include "mavsprintf.h"

#include <MAUtil/String.h>

using namespace MAUtil;

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define mod(x) ((x >= 0) ? (x) : (-x))
#define arraySize(a) (sizeof(a) / sizeof(a[0]))

typedef String string;

#endif // __CorpsePack__Platform_h__
