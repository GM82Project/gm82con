#define console_open
    ///console_open(title)
    //title: Window title for the console.
    //Opens a console window and sets the title for it. If a console is already
    //open, this function does nothing.
    __gm82con_alloc()
    console_set_title(argument0)


#define console_set_title
    ///console_set_title(string)
    //string: Window title for the console.
    //Sets the title for the open console window.
    console_write(vk_escchar+"]0;"+string(argument0)+vk_escchar+"\")


#define console_cursor_hide
    ///console_cursor_hide()
    //Hides the console cursor.
    console_write(vk_escchar+"[?25l")


#define console_cursor_show
    ///console_cursor_show()
    //Shows the console cursor.
    console_write(vk_escchar+"[?25h")


#define console_clear
    ///console_clear()
    //Clears the entire console.
    console_write(vk_escchar+"[2J"+vk_escchar+"[H")


#define console_clear_line
    ///console_clear_line()
    //Clears the current line.
    console_write(vk_escchar+"[2K"+vk_cr)


//
//