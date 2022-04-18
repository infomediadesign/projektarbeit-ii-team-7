#include "platform.h"

Time platform_time() {
#ifdef _WIN32
  LARGE_INTEGER freq, curtime;

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&curtime);

  const i64 usec = (i64)(curtime.QuadPart * 1000000 / freq.QuadPart);
  Time t = {.sec = usec / 1000000, .usec = usec};
#else
  struct timeval tv;

  gettimeofday(&tv, NULL);

  Time t = {.sec = tv.tv_sec, .usec = tv.tv_sec * 1000000 + tv.tv_usec};
#endif

  return t;
}

f64 platform_time_float() {
  Time t = platform_time();

  return t.usec / 1000000.0;
}

i64 platform_time_sec() {
#ifdef _WIN32
  LARGE_INTEGER freq, curtime;

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&curtime);

  return (i64)(curtime.QuadPart / freq.QuadPart);
#else
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return tv.tv_sec;
#endif
}

i64 platform_time_usec() {
#ifdef _WIN32
  LARGE_INTEGER freq, curtime;

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&curtime);

  return (i64)(curtime.QuadPart * 1000000 / freq.QuadPart);
#else
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return tv.tv_sec * 1000000 + tv.tv_usec;
#endif
}

thread_t platform_spawn(ThreadCallback func, void *data) {
#ifdef _WIN32
  return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, data, 0, NULL);
#else
  pthread_t handle;

  pthread_create(&handle, NULL, (void *(*)(void *))func, data);

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

void platform_sleep(const u64 milliseconds) {
#ifdef _WIN32
  Sleep((DWORD)milliseconds);
#else
  usleep(milliseconds * 1000);
#endif
}

void platform_usleep(const u64 microseconds) {
#ifdef _WIN32
  const u64 milliseconds = microseconds / 1000;
  const u64 remainder = microseconds % 1000;
  LARGE_INTEGER freq, curtime;

  if (milliseconds > 0)
    Sleep((DWORD)milliseconds);

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&curtime);

  const LARGE_INTEGER targettime =
      curtime.QuadPart * 1000000 / freq.QuadPart + remainder;

  while (curtime.QuadPart * 1000000 / freq.QuadPart < targettime)
    QueryPerformanceCounter(&curtime);
#else
  usleep(microseconds);
#endif
}

void platform_nsleep(const u64 nanoseconds) {
#ifdef _WIN32
  const u64 milliseconds = nanoseconds / 1000000;
  const u64 remainder = nanoseconds % 1000000;
  LARGE_INTEGER freq, curtime;

  if (milliseconds > 0)
    Sleep((DWORD)milliseconds);

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&curtime);

  const LARGE_INTEGER targettime =
      curtime.QuadPart * 1000000000 / freq.QuadPart + remainder;

  while (curtime.QuadPart * 1000000000 / freq.QuadPart < targettime)
    QueryPerformanceCounter(&curtime);
#else
  const struct timespec tv = {.tv_sec = nanoseconds / 1000000000,
                              .tv_nsec = nanoseconds % 1000000000};

  nanosleep(&tv, NULL);
#endif
}
