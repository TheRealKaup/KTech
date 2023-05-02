#pragma once

#define IsKeyDown GetAsyncKeyState // Just a nicer name
#define NowInSeconds() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMilliseconds() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMicroseconds() std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInNanoseconds() std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()

// Pragmas
#pragma execution_character_set( "utf-8" )
#pragma warning(disable:4996)

// Do not remove these. These are winAPI macros that break the engine.
#undef RGB
#undef min
#undef max
#undef PlaySound