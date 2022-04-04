#ifndef __ENGINE_PLATFORM_H
#define __ENGINE_PLATFORM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

#include <Windows.h>

typedef pthread_t thread_t;
typedef HANDLE mutex_t;

#else

#include <pthread.h>
#include <unistd.h>

typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;

#endif

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
thread_t platform_spawn(void (*func)(void *), void *data);

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
void platform_sleep(uint64_t milliseconds);

#endif
