#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>
#include <cstdlib>

#define GM82CON_API __declspec(dllexport)

typedef double gmreal;
typedef const char* gmstring;

#define realfunc extern "C" GM82CON_API gmreal __cdecl
#define strfunc extern "C" GM82CON_API gmstring __cdecl

/* Shows the console window and opens handles
 */
realfunc gm82con_alloc(void);
/* Hides the console window and kills everything
 */
realfunc gm82con_free(void);

/* Just writes a raw UTF-8 string to console out.
 * Must be rn terminated to print a full line.
 */
realfunc gm82con_write_string(gmstring lineString);

/* Input management */
realfunc gm82con_input_start(void);
strfunc gm82con_input_peek(void);
realfunc gm82con_input_clear(void);
realfunc gm82con_input_stop(void);

realfunc gm82con_set_console_modes(gmreal outputReal, gmreal inputReal);

enum gm82con_event {
	gm82con_event_none = 0, /* initializer value */
	gm82con_event_ctrl_c = (1 << 0 /* CTRL_C_EVENT */), /* 1 */
	gm82con_event_ctrl_break = (1 << 1 /* CTRL_BREAK_EVENT */), /* 2 */
	gm82con_event_ctrl_close = (1 << 2 /* CTRL_CLOSE_EVENT */), /* 4 */
	/* 3 and 4 are reserved by Windows's headers */
	gm82con_event_ctrl_logoff = (1 << 5 /* CTRL_LOGOFF_EVENT */), /* 32 */
	gm82con_event_ctrl_shutdown = (1 << 6 /* CTRL_SHUTDOWN_EVENT */) /* 64 */
};

/* Marks which console events are to be handled by the game,
 * when marked, one can use gm82con_is_event_signaled
 * to check whether an event has been signaled
 */
realfunc gm82con_handle_events(gmreal signalmaskReal);

/* Checks if the events were signaled and clears the signaled flag.
 * Only works for events that were marked through gm82con_handle_events
 */
realfunc gm82con_is_event_signaled(gmreal signalmaskReal);
