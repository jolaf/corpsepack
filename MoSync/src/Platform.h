
/**
 * This file is included by Character.cpp and contains platform-specific (MoSync) definitions.
 *
 * Do NOT include it in CorpsePack.cpp, it's for Character.cpp only.
 */

#ifndef __Platform_h__
#define __Platform_h__

#include "mavsprintf.h"

#include <MAUtil/String.h>

using namespace MAUtil;

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define mod(x) ((x >= 0) ? (x) : (-x))
#define arraySize(a) (sizeof(a) / sizeof(a[0]))

#endif // __Platform_h__
