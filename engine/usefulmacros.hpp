#pragma once

#define NowInSeconds() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()
#define NowInMilliseconds() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()
#define NowInMicroseconds() std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()
#define NowInNanoseconds() std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()

#define kReturn "\12"
#define kBackspace "\10"
#define kEscape "\33"
#define kUp "\33[A"
#define kDown "\33[B"
#define kRight "\33[C"
#define kLeft "\33[D"
#define kEnd "\33[F"
#define kHome "\33[H"
#define kInsert "\33[2~"
#define kDelete "\33[3~"
#define kPageUp "\33[5~"
#define kPageDown "\33[6~"
#define kF1 "\33OP"
#define kF2 "\33OQ"
#define kF3 "\33OR"
#define kF4 "\33OS"
#define kF5 "\33[15~"
#define kF6 "\33[17~"
#define kF7 "\33[18~"
#define kF8 "\33[19~"
#define kF9 "\33[20~"
#define kF10 "\33[21~"
#define kF11 "\33[23~"
#define kF12 "\33[24~"