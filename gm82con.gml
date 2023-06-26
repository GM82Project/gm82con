#define console_set_title
    ///console_set_title(string)
    console_write(chr(27)+"]0;"+string(argument0)+chr(27)+"\")


#define console_cursor_hide
    console_write(chr(27)+"[?25l")


#define console_cursor_show
    console_write(chr(27)+"[?25h")


//
//