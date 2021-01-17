#pragma once

#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>

inline void sleep_msec(int msec)
{
  usleep(msec * 1000);
}

inline uint32_t get_time_msec()
{
  struct timeval te;
  gettimeofday(&te, NULL);
  long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
  return uint32_t(milliseconds);
}

inline uint32_t get_time_usec()
{
  struct timeval te;
  gettimeofday(&te, NULL);
  long long useconds = te.tv_sec * 1000000LL + te.tv_usec;
  return uint32_t(useconds);
}

