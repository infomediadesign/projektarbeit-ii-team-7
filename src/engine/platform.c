#include "platform.h"

Time platform_time() {
#ifdef _WIN32
  LARGE_INTEGER freq, curtime;

  QueryPerformanceFrequency(&freq); 
  QueryPerformanceCounter(&curtime);

  long int usec = curtime.QuadPart * 1000000 / freq.QuadPart;
  Time t = {.sec = usec / 1000000, .usec = usec};
#else
  struct timeval tv;

  gettimeofday(&tv, NULL);

  Time t = {.sec = tv.tv_sec, .usec = tv.tv_usec};
#endif

  return t;
}

long int platform_time_sec() {
#ifdef _WIN32
  LARGE_INTEGER freq, curtime;

  QueryPerformanceFrequency(&freq); 
  QueryPerformanceCounter(&curtime);

  return curtime.QuadPart / freq.QuadPart;
#else
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return tv->tv_sec;
#endif
}

long int platform_time_usec() {
#ifdef _WIN32
  LARGE_INTEGER freq, curtime;

  QueryPerformanceFrequency(&freq); 
  QueryPerformanceCounter(&curtime);

  return curtime.QuadPart * 1000000 / freq.QuadPart;
#else
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return tv->tv_usec;
#endif
}

thread_t platform_spawn(int (*func)(void *), void *data) {
#ifdef _WIN32
  return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, data, 0, NULL);
#else
  pthread_t handle;

  pthread_create(&handle, NULL, func, data);

  return handle;
#endif
}

void platform_join(thread_t thread) {
#ifdef _WIN32
  WaitForSingleObject(thread, INFINITE);
  CloseHandle(thread);
#else
  pthread_join(thread, NULL);
#endif
}

void platform_sleep(uint64_t milliseconds) {
#ifdef _WIN32
  Sleep((DWORD)milliseconds);
#else
  usleep(milliseconds * 1000);
#endif
}
