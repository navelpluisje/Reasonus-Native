#ifndef _CSURF_LOG_H
#define _CSURF_LOG_H

#if 0 // 1=Logging enabled, 0=Logging disabled

#include <cstdio>

#define LOG(msg, ...) { \
  std::printf((msg), ##__VA_ARGS__); \
  std::printf("\n"); \
}

#else

#define LOG(msg, ...) {}

#endif

#endif