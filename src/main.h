#ifndef MAIN_H
#define MAIN_H

#define PP_STATE_TITLE        0
#define PP_STATE_GAME         1
#define PP_STATE_STATS        2
#define PP_STATE_GAME_IN      3
#define PP_STATE_GAME_OUT     4
#define PP_STATE_CHAR         5
#define PP_STATE_LEVEL        6
#define PP_STATE_QUIT         7
#define PP_STATE_DONE         8
#define PP_STATE_LOGO         9
#define PP_STATE_INTRO       10
#define PP_STATE_CREDITS     11
#define PP_STATE_REPLAY      12
#define PP_STATE_DEMO        13
#define PP_STATE_MENU_STATS  14
#define PP_STATE_NEW_PLAYER  15
#define PP_STATE_PAUSE       16

extern int pp_online;

FONT * import_bitmap_font(char * filename, int minchar, int maxchar, int cleanup);
void pp_load_color_map(COLOR_MAP * mp, char * fn);
void pp_save_color_map(COLOR_MAP * mp, char * fn);
void pp_nop(void);
void pp_vsync(void);
void pp_start_fade(PALETTE pal1, PALETTE pal2, int dir);
void pp_handle_fade(void);
void pp_fade(PALETTE pal1, PALETTE pal2, int dir);
void pp_start_song(char * fn, char * dfn);

#endif
