#define console_open
    __gm82con_alloc()
    console_set_title(argument0)


#define console_set_title
    ///console_set_title(string)
    console_write(vk_escchar+"]0;"+string(argument0)+vk_escchar+"\")


#define console_cursor_hide
    console_write(vk_escchar+"[?25l")


#define console_cursor_show
    console_write(vk_escchar+"[?25h")


#define console_clear
    console_write(vk_escchar+"[2J"+vk_escchar+"[H")


#define console_clear_line
    console_write(vk_escchar+"[2K"+vk_cr)


//
//