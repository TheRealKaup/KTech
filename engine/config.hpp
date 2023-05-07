#pragma once

#define NowInSeconds() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMilliseconds() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMicroseconds() std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInNanoseconds() std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()

// Shift Ctrl Alt
// 00    00   00  00
// (outdated) There is no "False" macro since "False" is zero, which doesn't require a macro
// (update) ACTUALLY - there is. It makes more sense that if the user doesn't care about the modifiers, they wouldn't have to change anything about them,
// but if they care about the modifiers, then they will use a macro
#define Shift_True 0x80 // 10000000
#define Shift_False 0x40 // 01000000
#define Ctrl_True 0x20 // 00100000
#define Ctrl_False 0x10 // 00010000
#define Alt_True 0x8 // 00001000
#define Alt_False 0x4 // 00000100

// Pragmas
// #pragma execution_character_set( "utf-8" )
// #pragma warning(disable:4996)

// Do not remove these. These are winAPI macros that break the engine.
#undef RGB
#undef min
#undef max
#undef PlaySound
