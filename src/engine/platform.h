#ifndef __ENGINE_PLATFORM_H
#define __ENGINE_PLATFORM_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

/* More like WIN32_JUST_EVIL_AND_MEAN */
#define WIN32_LEAN_AND_MEAN

/*
 * WINDOWS YOU PIECE OF SCRAP
 * REEEEEEEEEEEEEEEEEEEEEEEE
 *
 * Also THANK YOU RAYLIB FOR NOT NAMESPACING
 * I AM SO GLAD YOU DID THAT
 * YAY
 * WOW
 * 10/10 DESIGN CHOICE
 * I AM SO HAPPY I WET MY PANTS
 * ...
 * Why couldn't we just use Vulkan directly for this project?
 */
#define NOGDICAPMASKS     /* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOVIRTUALKEYCODES /* VK_* */
#define NOWINMESSAGES     /* WM_*, EM_*, LB_*, CB_* */
#define NOWINSTYLES       /* WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_* */
#define NOSYSMETRICS      /* SM_* */
#define NOMENUS           /* MF_* */
#define NOICONS           /* IDI_* */
#define NOKEYSTATES       /* MK_* */
#define NOSYSCOMMANDS     /* SC_* */
#define NORASTEROPS       /* Binary and Tertiary raster ops */
#define NOSHOWWINDOW      /* SW_* */
#define OEMRESOURCE       /* OEM Resource values */
#define NOATOM            /* Atom Manager routines */
#define NOCLIPBOARD       /* Clipboard routines */
#define NOCOLOR           /* Screen colors */
#define NOCTLMGR          /* Control and Dialog routines */
#define NODRAWTEXT        /* DrawText() and DT_* */
#define NOGDI             /* All GDI defines and routines */
#define NOKERNEL          /* All KERNEL defines and routines */
#define NOUSER            /* All USER defines and routines */
#define NONLS             /* All NLS defines and routines */
#define NOMB              /* MB_* and MessageBox() */
#define NOMEMMGR          /* GMEM_*, LMEM_*, GHND, LHND, associated routines */
#define NOMETAFILE        /* typedef METAFILEPICT */
#define NOMINMAX          /* Macros min(a,b) and max(a,b) */
#define NOMSG             /* typedef MSG and associated routines */
#define NOOPENFILE        /* OpenFile(), OemToAnsi, AnsiToOem, and OF_* */
#define NOSCROLL          /* SB_* and scrolling routines */
#define NOSERVICE /* All Service Controller routines, SERVICE_ equates, etc.   \
                   */
#define NOSOUND   /* Sound driver routines */
#define NOTEXTMETRIC     /* typedef TEXTMETRIC and associated routines */
#define NOWH             /* SetWindowsHook and WH_* */
#define NOWINOFFSETS     /* GWL_*, GCL_*, associated routines */
#define NOCOMM           /* COMM driver routines */
#define NOKANJI          /* Kanji support stuff. */
#define NOHELP           /* Help engine interface. */
#define NOPROFILER       /* Profiler interface. */
#define NODEFERWINDOWPOS /* DeferWindowPos routines */
#define NOMCX            /* Modem Configuration Extensions */

/* Type required before windows.h inclusion  */
typedef struct tagMSG *LPMSG;

/* WENDOVS DOT HAYCH */
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

#endif
