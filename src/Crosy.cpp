#include "Crosy.h"
#include <stdarg.h>

std::string Crosy::getExePath()
{

#ifdef _WIN32

  std::string exePath = "./";
  char exeFullPathName[MAX_PATH + 1];

  if (GetModuleFileNameA(NULL, exeFullPathName, MAX_PATH + 1))
  {
    char * exeNamePtr = strrchr(exeFullPathName, '\\');

    if (exeNamePtr)
    {
      *++exeNamePtr = '\0';
      exePath = std::string(exeFullPathName);
    }
  }

  return exePath;

#elif __linux__

  std::string exePath = "./";
  char exeFullPathName[PATH_MAX + 1];
  int pathLen = readlink("/proc/self/exe", exeFullPathName, PATH_MAX);

  if (pathLen > 0)
  {
    exeFullPathName[pathLen] = '\0';
    char * exeNamePtr = strrchr(exeFullPathName, '/');

    if (exeNamePtr)
    {
      *++exeNamePtr = '\0';
      exePath = std::string(exeFullPathName);
    }
  }

  return exePath;

#else
  #error unknown platform
#endif
}


uint64_t Crosy::getPerformanceCounter()
{
#ifdef _WIN32

  LARGE_INTEGER counter = { 0, 0 };
  QueryPerformanceCounter(&counter);

  return counter.QuadPart;

#elif __linux__

  timespec ts = { 0, 0 };
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return uint64_t(ts.tv_sec) * 1000000000 + uint64_t(ts.tv_nsec);

#else
#error unknown platform
#endif
}


uint64_t Crosy::getPerformanceFrequency()
{
#ifdef _WIN32

  LARGE_INTEGER perfFreq = { 0, 0 };
  QueryPerformanceFrequency(&perfFreq);

  return perfFreq.QuadPart;

#elif __linux__

  return 1000000000;

#else
#error unknown platform
#endif
}


uint64_t Crosy::getSystemTime()
{
#ifdef _WIN32

  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);

  return ((uint64_t)(ft.dwHighDateTime)) << 32 | ((uint64_t)(ft.dwLowDateTime));

#elif __linux__

  timespec ts = { 0, 0 };
  clock_gettime(CLOCK_REALTIME, &ts);

  return ((uint64_t)(ts.tv_sec) + 11644473600LL) * 100000000 + (uint64_t)(ts.tv_nsec) / 10;
//  return (uint64_t)(ts.tv_sec) * 1000000000 + (uint64_t)(ts.tv_nsec);

#else
#error unknown platform
#endif
}


void Crosy::sleep(unsigned int ms)
{
#ifdef _WIN32

  Sleep(ms);

#elif __linux__

  usleep(ms * 1000);

#else
#error unknown platform
#endif
}


void Crosy::snprintf(char * buf, size_t size, const char * format, ...)
{
  va_list args;
  va_start(args, format);
  vsnprintf(buf, size, format, args);
  va_end(args);
}

#ifdef _WIN32
std::string Crosy::WCHARtoString(wchat_t* str)
{
  wstring ws(str);
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  return conv.to_bytes(ws);
}
#endif

std::string Crosy::getConfigPath()
{
  const std::vector<std::string> priority
  {
#ifdef _WIN32
    WCHARtoString(_wgetenv("AppData")) + "/",
#endif
#ifdef __linux__
    std::string(std::getenv("HOME")) + "/.config/",
    std::string(std::getenv("HOME")) + "/",
#endif
    ""
  };
  for (int i = 0; i < priority.size(); i++)
  {
    std::string path = priority[i];
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
      path += "TetrisGL/";
#ifdef _WIN32
      CreateDirectory(path.c_str(), NULL);
#else
      mkdir(path.c_str(), 0777);
#endif
      return path;
    }
  }
  return getExePath();
}
