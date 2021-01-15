#pragma once

#include <pthread.h>

class MutexLock
{
  pthread_mutex_t mutex;
  pthread_mutexattr_t attr;

public:
  MutexLock(bool recursive = true)
  {
    if (recursive)
    {
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
      if (pthread_mutex_init(&mutex, &attr) != 0)
        printf("ERROR: pthread_mutex_init error\n");
    }
    else
    {
      if (pthread_mutex_init(&mutex, NULL) != 0)
        printf("ERROR: pthread_mutex_init error\n");
    }
  }

  ~MutexLock()
  {
    pthread_mutex_destroy(&mutex);
  }

  void enter()
  {
    pthread_mutex_lock(&mutex);
  }

  void leave()
  {
    pthread_mutex_unlock(&mutex);
  }

  bool tryEnter()
  {
    return pthread_mutex_trylock(&mutex) == 0;
  }
};

class ScopedLocker
{
  MutexLock * lock;

public:
  ScopedLocker(MutexLock & lock_)
  {
    lock = &lock_;
    lock->enter();
  }

  ~ScopedLocker()
  {
    lock->leave();
  }
};
