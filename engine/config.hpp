#pragma once

#define NowInSeconds() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()
#define NowInMilliseconds() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()
#define NowInMicroseconds() std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()
#define NowInNanoseconds() std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count()

#define kReturn "\10"
#define kBackspace "\127"
#define kEscape "\033"
#define kUp "\033[A"
#define kDown "\033[B"
#define kRight "\033[C"
#define kLeft "\033[D"
#define kEnd "\033[F"
#define kHome "\033[H"
#define kInsert "\033[2~"
#define kDelete "\033[3~"
#define kPageUp "\033[5~"
#define kPageDown "\033[6~"
#define kF1 "\033OP"
#define kF2 "\033OQ"
#define kF3 "\033OR"
#define kF4 "\033OS"
#define kF5 "\033[15~"
#define kF6 "\033[17~"
#define kF7 "\033[18~"
#define kF8 "\033[19~"
#define kF9 "\033[20~"
#define kF10 "\033[21~"
#define kF11 "\033[23~"
#define kF12 "\033[24~"