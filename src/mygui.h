#ifndef MYGUI_H
#define MYGUI_H

int ppgui_shadow_box_proc(int msg, struct DIALOG *d, int c);
int ppgui_button_proc(int msg, struct DIALOG *d, int c);
int ppgui_edit_proc(int msg, struct DIALOG *d, int c);
int ppgui_list_proc(int msg, struct DIALOG *d, int c);
int ppgui_text_list_proc(int msg, struct DIALOG *d, int c);
int ppgui_check_proc(int msg, struct DIALOG *d, int c);
void mygui_init(void);

#endif
