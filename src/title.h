#ifndef TITLE_H
#define TITLE_H

#define MAX_MENUS                     64
#define MAX_MENU_ITEMS                32
#define PP_MENU_BLANK                  0
#define PP_MENU_MAIN                   1
#define PP_MENU_PLAY                   2
#define PP_MENU_PLAY_1P                3
#define PP_MENU_PLAY_2P                4
#define PP_MENU_PLAY_3P                5
#define PP_MENU_PLAY_4P                6
#define PP_MENU_OPTIONS                7
#define PP_MENU_OPTIONS_GRAPHICS       8
#define PP_MENU_OPTIONS_AUDIO          9
#define PP_MENU_WEAPONS               10 /* weapon and powerup menus (must change parent manually) */
#define PP_MENU_POWERUPS              11
#define PP_MENU_CONTROLS              12
#define PP_MENU_CONTROLS_CUSTOM       13
#define PP_MENU_CONTROLS_CUSTOM_P1    14
#define PP_MENU_CONTROLS_CUSTOM_P2    15
#define PP_MENU_CONTROLS_CUSTOM_P3    16
#define PP_MENU_CONTROLS_CUSTOM_P4    17
#define PP_MENU_PLAY_2P_SPLAT         22
#define PP_MENU_PLAY_2P_SPLAT_SETUP   23
#define PP_MENU_PLAY_2P_HUNTER        24
#define PP_MENU_PLAY_2P_HUNTER_SETUP  25
#define PP_MENU_PLAY_2P_TAG           26
#define PP_MENU_PLAY_2P_TAG_SETUP     27
#define PP_MENU_PLAY_2P_FLAG          28
#define PP_MENU_PLAY_2P_FLAG_SETUP    29
#define PP_MENU_PLAY_3P_SPLAT         30
#define PP_MENU_PLAY_3P_SPLAT_SETUP   31
#define PP_MENU_PLAY_3P_HUNTER        32
#define PP_MENU_PLAY_3P_HUNTER_SETUP  33
#define PP_MENU_PLAY_3P_TAG           34
#define PP_MENU_PLAY_3P_TAG_SETUP     35
#define PP_MENU_PLAY_3P_FLAG          36
#define PP_MENU_PLAY_3P_FLAG_SETUP    37
#define PP_MENU_PLAY_4P_SPLAT         38
#define PP_MENU_PLAY_4P_SPLAT_SETUP   39
#define PP_MENU_PLAY_4P_HUNTER        40
#define PP_MENU_PLAY_4P_HUNTER_SETUP  41
#define PP_MENU_PLAY_4P_TAG           42
#define PP_MENU_PLAY_4P_TAG_SETUP     43
#define PP_MENU_PLAY_4P_FLAG          44
#define PP_MENU_PLAY_4P_FLAG_SETUP    45

#define PP_MENU_EXIT                  49
#define PP_MENU_DONE                  50

#define PP_MENU_AMMO                  51
#define PP_MENU_SPECIAL               52
#define PP_MENU_MISC                  53

#define PP_MENU_ITEM_TYPE_NORMAL       0
#define PP_MENU_ITEM_TYPE_FLAG         1
#define PP_MENU_ITEM_TYPE_HEADER       2 /* non-selectable */
#define PP_MENU_ITEM_TYPE_VARIABLE     3 /* text or integer variable */
#define PP_MENU_ITEM_TYPE_STRING       4 /* print the pointed string */
#define PP_MENU_ITEM_TYPE_IMAGE        5 /* image item */

typedef struct
{
	/* image */
	BITMAP * bp;

	/* offset from location of menu */
	int ox, oy;
	FONT * fp;

	/* item name */
//	char name[128];
	char * name;

	/* option pointer (should stay between max and min) */
	int * dp;

	/* option text pointer */
	char ** dp2;
	int type, choice, vis;

	/* procedure for left keypress */
    void(*left_proc)();

    /* procedure for right keypress */
    void(*right_proc)();

    /* procedure for enter keypress */
    void(*enter_proc)();

} PP_MENU_ITEM;

typedef struct
{
	/* menu items */
	PP_MENU_ITEM item[MAX_MENU_ITEMS];
	int items;
	int parent;
	char * title;

	int selected;
	int x, y;

} PP_MENU;

extern void (*pp_next_proc)();
extern int pp_start_time;
//extern int pp_entered[4];
//extern int pp_started[4];
//extern int pp_datad[4];
extern int pp_waiting;
//extern int pp_character[4];
//extern int pp_name[4];
extern int pp_game_type;
extern int pp_slev;
extern int pp_menu_idle_time;
extern int pp_next_reset;
extern int pp_menu_bg_x;
extern int pp_menu_bg_y;

void pp_menu_play_server(void);
void pp_menu_play_client_connect(void);
void pp_select_menu(int num);
void pp_create_menu(PP_MENU * mp, char * title, int x, int y, int parent);
//void pp_add_menu_item(PP_MENU * mp, BITMAP * bp, void(*left_proc)(), void(*right_proc)(), void(*enter_proc)());
//void pp_add_menu_item(PP_MENU * mp, int type, char * name, int x, int y, int * dp, char * dp2[], void(*left_proc)(), void(*right_proc)(), void(*enter_proc)());
void pp_add_menu_item(PP_MENU * mp, FONT * fp, int ox, int oy, char * name, void(*left_proc)(), void(*right_proc)(), void(*enter_proc)());
void pp_add_menu_flag(PP_MENU * mp, FONT * fp, int ox, int oy, char * name, int * dp, void(*left_proc)(), void(*right_proc)(), void(*enter_proc)());
void pp_add_menu_header(PP_MENU * mp, FONT * fp, int ox, int oy, char * name);
void pp_add_menu_variable(PP_MENU * mp, FONT * fp, int ox, int oy, int * dp, char * dp2[]);
void pp_add_menu_string(PP_MENU * mp, FONT * fp, int ox, int oy, char * string);
void pp_add_menu_image(PP_MENU * mp, FONT * fp, int ox, int oy, BITMAP * bp);
void pp_menu_fix_item(PP_MENU * mp);
void pp_menu_next_item(PP_MENU * mp);
void pp_menu_prev_item(PP_MENU * mp);
void pp_draw_menu(BITMAP * bp, PP_MENU * mp);
void pp_title_logic(void);
void pp_title_draw(void);
int char_is_available(int n);
int level_is_available(int n);
void get_first_char(void);
void get_first_level(void);
void delete_extension(char * text);

#endif
