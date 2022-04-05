#include "platform.h"

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
