#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <windows.h>

#elif __linux__

#include <X11/Xlib.h>
#include <unistd.h>

#else

#error unknown platform

#endif

#include <vector>
#include <limits.h>
#include <stdint.h>
#include <string>
#include <string.h>

namespace Crosy
{
  std::string getExePath();
  uint64_t getPerformanceCounter();
  uint64_t getPerformanceFrequency();
  uint64_t getSystemTime();
  void sleep(unsigned int timeMs);
  void snprintf(char * buf, size_t size, const char * format, ...);
}
