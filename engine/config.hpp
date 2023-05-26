#pragma once

#define NowInSeconds() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMilliseconds() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInMicroseconds() std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()
#define NowInNanoseconds() std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - Engine::engineStartTP.chornoTimePoint).count()

// Keys integers
#define kReturn 10
#define kBackspace 127
#define kEscape 27
// Keys strings
#define ksUp "\033[A"
#define ksDown "\033[B"
#define ksRight "\033[C"
#define ksLeft "\033[D"
#define ksEnd "\033[F"
#define ksHome "\033[H"
#define ksInsert "\033[2~"
#define ksDelete "\033[3~"
#define ksPageUp "\033[5~"
#define ksPageDown "\033[6~"
#define ksF1 "\033OP"
#define ksF2 "\033OQ"
#define ksF3 "\033OR"
#define ksF4 "\033OS"
#define ksF5 "\033[15~"
#define ksF6 "\033[17~"
#define ksF7 "\033[18~"
#define ksF8 "\033[19~"
#define ksF9 "\033[20~"
#define ksF10 "\033[21~"
#define ksF11 "\033[23~"
#define ksF12 "\033[24~"

// Audio
#define config_framesPerBuffer 128