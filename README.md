# Game Maker 8.2 Console

This is the documentation for the `gm82con` extension.

The extension allows you to manage your own debug console window for whatever purposes.

Written by nkrapivindev for the GameMaker 8.2 project. Adapted by renex.

## Table of contents

This is the table of contents for the documentation.

- [Constants](#constants)
    - [con_event](#con_event)
    - [con_mode](#con_mode)
    - [Miscellaneous](#Miscellaneous)
- [Functions](#functions)
    - [console_open](#console_open)
    - [console_close](#console_close)
    - [console_write](#console_write)
    - [console_input_begin](#console_input_begin)
    - [console_input_get](#console_input_get)
    - [console_input_clear](#console_input_clear)
    - [console_input_end](#console_input_end)
    - [console_set_options](#console_set_options)
    - [console_handle_events](#console_handle_events)
    - [console_is_event_signaled](#console_is_event_signaled)
- [Usage](#usage)

## Constants

This paragraph describes the constants provided by the `con` extension.

Specific values of the constants are not provided on purpose to encourage the developers to actually use the constants instead of hardcoding the numbers.

### con_event

These constants are values that can be used as a bit mask for the `con_handle_events` and `con_is_event_signaled` functions.

- `con_event_none`: just zero, can be used to initialize a variable
- `con_event_ctrl_c`: corresponds to 1 << `CTRL_C_EVENT` in the Windows headers
- `con_event_ctrl_break`: corresponds to 1 << `CTRL_BREAK_EVENT` in the Windows headers
- `con_event_ctrl_close`: corresponds to 1 << `CTRL_CLOSE_EVENT` in the Windows headers
- `con_event_ctrl_logoff`: corresponds to 1 << `CTRL_LOGOFF_EVENT` in the Windows headers
- `con_event_ctrl_shutdown`: corresponds to 1 << `CTRL_SHUTDOWN_EVENT` in the Windows headers

Please see [this page](https://learn.microsoft.com/en-us/windows/console/handlerroutine) for more information about the events.

See the mentioned functions for more information about these constants

### con_mode

These constants are values that correspond to the console mode values from the Windows headers.

Special values:

- `con_mode_dontupdate`, special constant used to *not* update a mode and has no equivalent in the Windows headers

Input modes:

- `con_mode_in_enable_processed_input`, corresponds to `ENABLE_PROCESSED_INPUT` in the Windows headers
- `con_mode_in_enable_line_input`, corresponds to `ENABLE_LINE_INPUT` in the Windows headers
- `con_mode_in_enable_echo_input`, corresponds to `ENABLE_ECHO_INPUT` in the Windows headers
- `con_mode_in_enable_window_input`, corresponds to `ENABLE_WINDOW_INPUT` in the Windows headers
- `con_mode_in_enable_mouse_input`, corresponds to `ENABLE_MOUSE_INPUT` in the Windows headers
- `con_mode_in_enable_insert_mode`, corresponds to `ENABLE_INSERT_MODE` in the Windows headers
- `con_mode_in_enable_quick_edit_mode`, corresponds to `ENABLE_QUICK_EDIT_MODE` in the Windows headers
- `con_mode_in_enable_extended_flags`, corresponds to `ENABLE_EXTENDED_FLAGS` in the Windows headers
- `con_mode_in_enable_auto_position`, corresponds to `ENABLE_AUTO_POSITION` in the Windows headers
- `con_mode_in_enable_virtual_terminal_input`, corresponds to `ENABLE_VIRTUAL_TERMINAL_INPUT` in the Windows headers

Output modes:

- `con_mode_out_enable_processed_output`, corresponds to `ENABLE_PROCESSED_OUTPUT` in the Windows headers
- `con_mode_out_enable_wrap_at_eol_output`, corresponds to `ENABLE_WRAP_AT_EOL_OUTPUT` in the Windows headers
- `con_mode_out_enable_virtual_terminal_processing`, corresponds to `ENABLE_VIRTUAL_TERMINAL_PROCESSING` in the Windows headers
- `con_mode_out_disable_newline_auto_return`, corresponds to `DISABLE_NEWLINE_AUTO_RETURN` in the Windows headers
- `con_mode_out_enable_lvb_grid_worldwide`, corresponds to `ENABLE_LVB_GRID_WORLDWIDE` in the Windows headers

Please see [this page](https://learn.microsoft.com/en-us/windows/console/getconsolemode) on MSDN for more information.

### Miscellaneous

- vk_cr
--Just a carriage return character as a string.
- vk_lf
--Just a newline character as a string.
- vk_crlf
--A carriage return + newline (CRLF, RN) sequence string that is used to mark the end of a line and the start of a new one.

## Functions

This paragraph describes the functions provided by the `con` extension.

### console_open

Help: `console_open()->bool`

Arguments: None

Returns: `bool`, Whether the console window creation was successful.

Purpose:

This function calls `AllocConsole`, sets up console I/O handles and applies the console mode.

### console_close

Help: `console_close()->bool`

Arguments: None

Returns: `bool`, Whether the console window has been destroyed successfully.

Purpose:

This function closes all console I/O handles, stops the input thread, then calls `FreeConsole` to kill the console window.

### console_write

Help: `console_write(line:string)->bool`

Arguments:

|Name|Type|Purpose|
|----|----|-------|
|line|string|The string to write into the console output|

Returns: `bool`, Whether the write operation was successful or not.

Purpose:

This function writes a string into the console output.

To print a full line, the string must be terminated with a carriage return + newline (CRLF, RN) sequence.

You can append [con_rn](#con_rn) to the end of the string to terminate a line properly.

The function has a limit of maximum 4096 UTF-16 characters, to print more characters you need to call the function multiple times (with smaller chunks).

### console_input_begin

Help: `console_input_begin()->bool`

Arguments: None

Returns: `bool`, Whether the input thread is running.

Purpose:

This function starts the input read thread and starts reading input from the console input into a global buffer.

### console_input_get

Help: `console_input_get()->string`

Arguments: None

Returns: `string`, The current contents of the global input buffer.

Purpose:

This function returns whatever is in the global input buffer that is filled in by the input thread.

The global buffer has a maximum size of 8192 characters and shall be cleared periodically if you're expecting large amounts of input.

### console_input_clear

Help: `console_input_clear()->bool`

Arguments: None

Returns: `bool`, Whether the buffer was cleared successfully or not.

Purpose:

This function clears the input buffer.

When expecting large amounts of input from the console it is sometimes necessary to copy the contents of the input buffer into a GML string when the buffer is getting large,
then clear the input buffer and expect more data.

Usually, when working with line-based input (an input is a single line) this is not necessary.

### console_input_end

Help: `console_input_end()->bool`

Arguments: None

Returns: `bool`, Whether the input thread has been stopped successfully or not.

Purpose:

This function cancels the reading operations and stops the input thread, **clearing the input buffer in the process**.

Make sure you preserve the contents of the input buffer somewhere before stopping the input thread.

### console_set_options

Help: `console_set_options(output:con_mode,input:con_mode)->bool`

Arguments:

|Name|Type|Purpose|
|----|----|-------|
|output|con_mode|A [con_mode](#con_mode) constant|
|input|con_mode|A [con_mode](#con_mode) constant|

Returns: `bool`, Whether the console mode(s) were set successfully or not.

Purpose:

Sometimes it is needed to change the mode of the console window to a different one.

This function can be called before or after the creation of the console window.

A constant `con_mode_dontupdate` can be used for the `output` or the `input` argument if you do not wish to update the respective mode.

(for example if you only want to update the `output` mode, pass `con_mode_dontupdate` as the `input` argument)

The default modes enable [VT100 sequences](https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences) for the output (but not input!), so you can use them to interact with the console window.

### console_handle_events

Help: `console_handle_events(signalmask:con_event)->con_event`

Arguments:

|Name|Type|Purpose|
|----|----|-------|
|signalmask|con_event|A bit mask of [con_event](#con_event) constants|

Returns: `con_event`, The previous event handler bit mask.

Purpose:

This function allows you to specify for which console events you wish to override the default system handling.

By default the extension handles `con_event_ctrl_c` and `con_event_ctrl_close` for you.

(meaning the default previous bit mask is `con_event_ctrl_c|con_event_ctrl_close`)

This function can be called before or after console window creation.

### console_is_event_signaled

Help: `console_is_event_signaled(signalmask:con_event)->con_event`

Arguments:

|Name|Type|Purpose|
|----|----|-------|
|signalmask|con_event|A bit mask of [con_event](#con_event) constants|

Returns: `con_event`, a bit mask of events that have been signaled.

Purpose:

This function queries which events have been signaled by the system, returns and then clears the corresponding bits (to prevent querying the same value over and over again).

Meaning, you must preserve the result of this function somewhere if you wish to persist the signaled mask between events.

Example:

```gml
///Example
var _signalmask, _is_ctrl_c, _is_ctrl_close;
_signalmask = console_is_event_signaled(con_event_ctrl_c|con_event_ctrl_close);
_is_ctrl_c = (_signalmask & con_event_ctrl_c) != 0;
_is_ctrl_close = (_signalmask & con_event_ctrl_close) != 0;
// now use the _is_ctrl_c and _is_ctrl_close variables for your handling
```

## Usage

This paragraph provides a usage example in GML:

```gml
///Create
r = vk_cr;
n = vk_lf;
rn = vk_crlf;
console_open();
console_write("Hello, World!" + rn);
// start accepting input:
console_string = "";
// default events mask
events_mask = con_event_ctrl_c|con_event_ctrl_close;
console_input_begin();
console_write("Type something: "); // do not terminate

///Step
var __con_input, __con_len, __con_i, __con_c, __con_r, __con_ctrl;
__con_input = console_input_get();
__con_len = string_length(__con_input);
__con_i = 1;
__con_r = false;
repeat (__con_len + 1) {
    __con_c = string_char_at(__con_input, __con_i);
    if (__con_c == r || __con_c == n) {
        __con_r = true; // end of line
        break;
    } else {
        console_string += __con_c;
    }
    __con_i += 1;
}

__con_ctrl = console_is_event_signaled(events_mask) != 0;
if (__con_ctrl) {
    window_set_caption("Ctrl handler called!");
}

// clear the input buffer when we have copied stuff from it.
console_input_clear();

if (__con_r) {
    if (console_string == "silly") {
        window_set_caption("owo i am such a silly gm user");
        console_write("can't you upgrade to Studio 2.3+ or something?" + rn);
    } else if (console_string == "brazil") {
        window_set_caption("https://www.youtube.com/watch?v=fOvJeinoX3s");
        console_write("YUOR'E GOING TO BRAZIL MWAHAHA" + rn);
    } else if (console_string == "test_utf8") {
        console_write("русский утф-8 текст..." + rn);
    } else {
        console_write("Command not recognized: " + console_string + rn);
    }

    console_write("Type something: ");
    console_string = "";
}

```
