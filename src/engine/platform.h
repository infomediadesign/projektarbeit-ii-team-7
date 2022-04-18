#ifndef __ENGINE_PLATFORM_H
#define __ENGINE_PLATFORM_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

/* More like WIN32_JUST_EVIL_AND_MEAN */
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <profileapi.h>

typedef HANDLE thread_t;
typedef HANDLE mutex_t;

#else

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;

#endif

/**
 * @brief Time in seconds and microseconds.
 */
typedef struct Time {
  i64 sec;
  i64 usec;
} Time;

/**
 * @brief Current time.
 *
 * ```
 * Time t = platform_time();
 *
 * if (t.sec > 10) {
 *   mine_bitcoin();
 *   explode_user_cpu();
 * }
 * ```
 *
 * @return Time Current time via the time structure.
 */
Time platform_time();

/**
 * @brief Current time in seconds expressed as a float.
 *
 * ```
 * Time t = platform_time_float();
 *
 * if (t > 7.5f) {
 *   mine_bitcoin();
 *   explode_user_cpu();
 * }
 * ```
 *
 * @return f64 Time Current time as a float.
 */
f64 platform_time_float();

/**
 * @brief Current time in seconds.
 *
 * ```
 * i64 t = platform_time_sec();
 *
 * if (t > 10) {
 *   explode_user_cpu();
 * }
 * ```
 *
 * @return i64 Current time in seconds.
 */
i64 platform_time_sec();

/**
 * @brief Current time in microseconds.
 *
 * **This is 1/1000000 of a second, not 1/1000!**
 *
 * ```
 * i64 start_time = platform_time_usec();
 *
 * code_that_takes_a_while();
 *
 * printf("This took %li microseconds to execute", platform_time_usec() -
 * start_time);
 * ```
 *
 * @return i64 Time in microseconds.
 */
i64 platform_time_usec();

/**
 * @brief Spawns a thread.
 *
 * ```
 * thread_t thr = platform_spawn(func, NULL);
 * platform_join(thr);
 * ```
 *
 * @param func Function to be executed in a thread.
 * @param data Pointer to the data that needs to be passed to the function.
 * @return thread_t Thread ID.
 */
thread_t platform_spawn(ThreadCallback func, void *data);

/**
 * @brief Joins a thread into the main thread.
 *
 * Waits for the thread to terminate, and then joins it
 * into the main thread, freeing it.
 *
 * ```
 * thread_t thr = platform_spawn(func, NULL);
 * platform_join(thr);
 * ```
 *
 * @param thread Thread ID
 */
void platform_join(thread_t thread);

/**
 * @brief Does nothing for a specified amount of milliseconds.
 *
 * ```
 * printf("Hi!");
 *
 * platform_sleep(1000);
 *
 * printf("Hi one second later!");
 * ```
 *
 * @param milliseconds The amount of milliseconds to wait.
 */
void platform_sleep(const u64 milliseconds);

/**
 * @brief Does nothing for a specified amount of microseconds.
 *
 * On Windows, it would sleep for as many milliseconds as it can, and
 * busy-wait the rest of the time, as WinAPI provides no way for
 * sub-millisecond sleep.
 *
 * ```
 * printf("Hi!");
 *
 * platform_usleep(1000000);
 *
 * printf("Hi one second later!");
 * ```
 *
 * @param microseconds The amount of microseconds to wait.
 */
void platform_usleep(const u64 microseconds);

/**
 * @brief Does nothing for a specified amount of nanoseconds.
 *
 * On Windows, it would sleep for as many milliseconds as it can, and
 * busy-wait the rest of the time, as WinAPI provides no way for
 * sub-millisecond sleep.
 *
 * ```
 * printf("Hi!");
 *
 * platform_nsleep(1000000000);
 *
 * printf("Hi one second later!");
 * ```
 *
 * @param nanoseconds The amount of nanoseconds to wait.
 */
void platform_nsleep(const u64 nanoseconds);

#endif
