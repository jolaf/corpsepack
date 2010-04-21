
/**
 * This file is included by Character.cpp
 * and contains platform-specific (MSVC) definitions.
 *
 * Do NOT include it in CorpsePack.cpp, it's for Character.cpp only.
 */

#ifndef __Platform_h__
#define __Platform_h__

#include <string>

using namespace std;

#define String string
#define sprintf sprintf_s

#endif // __Platform_h__
