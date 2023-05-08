#pragma once

#define NowInSeconds() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMilliseconds() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMicroseconds() std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInNanoseconds() std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()

// Keys
/*
#define kUp "\033[A"
#define kDown "\033[B"
#define kRight "\033[C"
#define kLeft "\033[D"
#define kEnd "\033[F"
#define kHome "\033[H"
#define kBackspace 0x7f
#define kPause 0x1a
#define kEscape 0x1b
#define kInsert "\033[2~"
#define kInsert	"\033[2~"
#define kDelete	"\033[3~"
#define kPageUp	"\033[5~"
#define kPageDown "\033[6~"
#define kF1	"\033OP"
#define kF2	"\033OQ"
#define kF3	"\033OR"
#define kF4	"\033OS"
#define kF5	"\033[15~"
#define kF6	"\033[17~"
#define kF7	"\033[18~"
#define kF8	"\033[19~"
#define kF9	"\033[20~"
#define kF10 "\033[21~"
#define kF11 "\033[23~"
#define kF12 "\033[24~"
*/
// Pragmas
// #pragma execution_character_set( "utf-8" )
// #pragma warning(disable:4996)

// Do not remove these. These are winAPI macros that break the engine.
#undef RGB
#undef min
#undef max
#undef PlaySound
