#include <allegro.h>
#include <stdlib.h>
#include "ncds.h"
#include "animate.h"
#include "chared.h"
#include "itools.h"
#include "wincb.h"
#include "mygui.h"

int admin_mode = 0;
int my_mouse_x, my_mouse_y, my_mouse_b;

#define OLD_PLAYER_STATES       12
#define PLAYER_STATES           24
#define PS_STAND_RIGHT           0
#define PS_STAND_LEFT            1
#define PS_STAND_UP_RIGHT        2
#define PS_STAND_UP_LEFT         3
#define PS_WALK_RIGHT            4
#define PS_WALK_LEFT             5
#define PS_DUCK_RIGHT            6
#define PS_DUCK_LEFT             7
#define PS_JUMP_RIGHT            8
#define PS_JUMP_LEFT             9
#define PS_FALL_RIGHT           10
#define PS_FALL_LEFT            11
#define PS_FSTAND_RIGHT         12
#define PS_FSTAND_LEFT          13
#define PS_FSTAND_UP_RIGHT      14
#define PS_FSTAND_UP_LEFT       15
#define PS_FWALK_RIGHT          16
#define PS_FWALK_LEFT           17
#define PS_FDUCK_RIGHT          18
#define PS_FDUCK_LEFT           19
#define PS_FJUMP_RIGHT          20
#define PS_FJUMP_LEFT           21
#define PS_FFALL_RIGHT          22
#define PS_FFALL_LEFT           23

#define GUN_FLASH_TYPES        4
#define GUN_FLASH_RIGHT        0
#define GUN_FLASH_LEFT         1
#define GUN_FLASH_UP_RIGHT     2
#define GUN_FLASH_UP_LEFT      3

/* this data can be saved to a file */
typedef struct
{

	ANIMATION * ap[PLAYER_STATES]; /* all animations should have same dimensions */
	ANIMATION * pap;
	ANIMATION * ppap;
	ANIMATION * gap[GUN_FLASH_TYPES];
	int cx_table[PLAYER_STATES], cy_table[PLAYER_STATES], cb_table[PLAYER_STATES], cr_table[PLAYER_STATES], cw_table[PLAYER_STATES], ch_table[PLAYER_STATES];
	int px_table[PLAYER_STATES], py_table[PLAYER_STATES];
	int gx_table[PLAYER_STATES], gy_table[PLAYER_STATES];
	int mx, my, mr; /* center of character */
	int ix, iy; /* location of pointer relative to character */
	int pf;
	SAMPLE * sound[PP_MAX_SOUNDS];

} PLAYER_DATA;

typedef struct
{
	int x, y, w, h, ox, oy;
	int * dx, * dy;
	int active;
} PEG;

PEG peg[32];

char * state_name[PLAYER_STATES + 4 + 1 + 1] =
{
	"Stand Right",
	"Stand Left",
	"Stand Right, Look Up",
	"Stand Left, Look Up",
	"Walk Right",
	"Walk Left",
	"Duck Right",
	"Duck Left",
	"Jump Right",
	"Jump Left",
	"Fall Right",
	"Fall Left",
	"Stand Right (F)",
	"Stand Left (F)",
	"Stand Right, Look Up (F)",
	"Stand Left, Look Up (F)",
	"Walk Right (F)",
	"Walk Left (F)",
	"Duck Right (F)",
	"Duck Left (F)",
	"Jump Right (F)",
	"Jump Left (F)",
	"Fall Right (F)",
	"Fall Left (F)",
	"Gun Flash Right",
	"Gun Flash Left",
	"Gun Flash Up (Right)",
	"Gun Flash Up (Left)",
	"Paintball",
	"Paint"
};

int selected_state = 0;
int selected_sound = 0;
BITMAP * char_screen = NULL;
BITMAP * char_scratch = NULL;
BITMAP * char_clipboard = NULL;
PALETTE  char_palette;
ANIMATION * it_ani;
ANIMATION * pointer_ani;
PLAYER_DATA player_data;
SAMPLE * sound[PP_MAX_SOUNDS] = {NULL};
int char_done = 0;
char filename[1024] = {0};
char cfilename[1024] = {0};
char nfilename[1024] = {0};
int char_frame = 0;
int dg_table[PLAYER_STATES] =  {  0,   1,   2,   3,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   2,   3,   0,   1,   0,   1,   0,   1,   0,   1};
int a_table[PLAYER_STATES] =  {  0, 128, 192, 192,   0, 128,   0, 128,   0, 128,   0, 128,   0, 128, 192, 192,   0, 128,   0, 128,   0, 128,   0, 128};
int need_to_save = 0;
int tool_map[3][3] = {0, 1, 6, 2, 3, 7, 4, 5, 8};
char * ani_help_text = NULL;

#define ITEMS       5
#define ITEM_CMAP   0
#define ITEM_GUN    1
#define ITEM_PAINT  2
#define ITEM_IT     3
#define ITEM_CENTER 4
int item = ITEM_CMAP;

char * item_text[ITEMS] = {"CMAP", "Gun", "Paint", "Arrow", "Pointer"};

#define STATES     4
//#define STATE_CHAR 0
#define STATE_ANI     0
#define STATE_POS     1
#define STATE_SCRATCH 2
#define STATE_SOUND   3
int state = STATE_ANI;
char * state_text[STATES] = {"Animation View", "Layout View", "Scratch Pad", "Sounds"};
char * sound_text[PP_MAX_SOUNDS] = {"Fire", "Splat", "Reload A", "Reload B", "Ammo", "Cloak", "Gem", "Jump", "Land", "Hit", "Bump", "Time", "", "", "Teleport", "Teleport Out", "Run", "Deflect", "Flag", "Bank", "Base", "", "Power Jump", "", "", "", "", "", "", "Crush", "Target", "Bounce", "Ricochet", "Game Start", "Game Win", "Game Enter", "Fly", "Fly Power", "", "Turbo Power", NULL};
int    sound_order[PP_MAX_SOUNDS] = {0};
int    sound_orders = 0;
extern SAMPLE * ncds_internal_sample;
ANIMATION * ap = NULL;
BITMAP * bp;
int ani_frame = 0;
int ani_color = 0;
int ani_scale = 3;
int ani_tool = TOOL_PIXEL;

/* points to bitmap with clipboard operations going on */
BITMAP * ani_sel_bp = NULL;
BITMAP * ani_sel_fbp = NULL;
BITMAP * ani_sel_cbp = NULL;
int ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, ani_sel_float = 0, ani_sel_v = 1;
int pegged = -1;
int pox, poy;
BITMAP * mouse_corner;
BITMAP * ani_tool_image[9] = {NULL};
int mouse_on = 1;
int cmap_locked = 1;
char wtext[256] = {0};
char htext[256] = {0};

/* recording code */
#define CONTROL_MODE_USER       0
#define CONTROL_MODE_TUT_RECORD 1
#define CONTROL_MODE_TUT_PLAY   2
PACKFILE * control_log = NULL;
int control_mode;
int logic_frames = 0;

void read_mouse(void);

void logic_timer(void)
{
	logic_frames++;
}

int my_popup_dialog(DIALOG * dp, int n)
{
	int ret;
	
   	ret = popup_dialog(dp, n);
   	logic_frames = 0;
   	
   	return ret;
}

void add_sound(int i)
{
	sound_order[sound_orders] = i;
	sound_orders++;
}

MENU file_menu[] =
{
    {"New\tF4", file_menu_new, NULL, 0, NULL},
    {"Open\tF3", file_menu_load, NULL, 0, NULL},
    {"Save\tF2", file_menu_save, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
	{"Exit\tEsc", file_menu_exit, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU admin_file_menu[] =
{
    {"New\tF4", file_menu_new, NULL, 0, NULL},
    {"Open\tF3", file_menu_load, NULL, 0, NULL},
    {"Save\tF2", file_menu_save, NULL, 0, NULL},
    {"Web Update", file_menu_web_update, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
	{"Exit\tEsc", file_menu_exit, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU view_menu[] =
{
    {"Animation", view_menu_ani, NULL, D_SELECTED, NULL},
    {"Layout", view_menu_pos, NULL, 0, NULL},
    {"Scratch", view_menu_scratch, NULL, 0, NULL},
    {"Sounds", view_menu_sounds, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU edit_menu[] =
{
    {"Cut\tSh+Del", tool_cut, NULL, 0, NULL},
    {"Copy\tCtrl+C", tool_copy, NULL, 0, NULL},
    {"Paste\tCtrl+V", tool_paste, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Mirror\tH", tool_mirror, NULL, 0, NULL},
    {"Flip\tV", tool_flip, NULL, 0, NULL},
    {"Turn\tR", tool_turn, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU frame_shift_menu[] =
{
    {"Left\tA", frame_shift_left, NULL, 0, NULL},
    {"Right\tD", frame_shift_right, NULL, 0, NULL},
    {"Up\tW", frame_shift_up, NULL, 0, NULL},
    {"Down\tS", frame_shift_down, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU frame_menu[] =
{
    {"Load", frame_menu_import, NULL, 0, NULL},
    {"Save", frame_menu_export, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Insert", frame_menu_insert, NULL, 0, NULL},
    {"Add", frame_menu_add, NULL, 0, NULL},
    {"Delete", frame_menu_delete, NULL, 0, NULL},
    {"Clear", frame_menu_clear, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Shift", NULL, frame_shift_menu, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU speed_menu[] =
{
    {"Decrease\t<", speed_menu_decrease, NULL, 0, NULL},
    {"Increase\t>", speed_menu_increase, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU ani_shift_menu[] =
{
    {"Left\tShft+A", ani_shift_left, NULL, 0, NULL},
    {"Right\tShft+D", ani_shift_right, NULL, 0, NULL},
    {"Up\tShft+W", ani_shift_up, NULL, 0, NULL},
    {"Down\tShft+S", ani_shift_down, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU ani_create_menu[] = 
{
    {"Rotation", ani_menu_rotate, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Flipped", ani_menu_qo, NULL, 0, NULL},
    {"Fire", ani_menu_fire, NULL, 0, NULL},
    {"All Fire", ani_menu_fire_all, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU ani_sub_menu[] =
{
    {"New", ani_menu_new, NULL, 0, NULL},
    {"Load", ani_sub_menu_import, NULL, 0, NULL},
    {"Save", ani_sub_menu_export, NULL, 0, NULL},
    {"Import", frame_menu_import_sequence, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Frame", NULL, frame_menu, 0, NULL},
    {"Speed", NULL, speed_menu, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Create", NULL, ani_create_menu, 0, NULL},
    {"Shift", NULL, ani_shift_menu, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU ani_menu[] =
{
	{"File", NULL, file_menu, 0, NULL},
	{"Edit", NULL, edit_menu, 0, NULL},
	{"View", NULL, view_menu, 0, NULL},
	{"ANI", NULL, ani_sub_menu, 0, NULL},
	{"X", menu_nop, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU item_menu[] =
{
	{"CMAP", item_menu_cmap, NULL, D_SELECTED, NULL},
	{"Flash", item_menu_flash, NULL, 0, NULL},
	{"Paintball", item_menu_paintball, NULL, 0, NULL},
	{"It", item_menu_it, NULL, 0, NULL},
	{"Arrow", item_menu_arrow, NULL, 0, NULL},
	{"", NULL, NULL, 0, NULL},
	{"Paint Spot", item_menu_pf, NULL, 0, NULL},
	{"Lock CMAP", item_menu_lock, NULL, D_SELECTED, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU pos_menu[] =
{
	{"File", NULL, file_menu, 0, NULL},
	{"View", NULL, view_menu, 0, NULL},
	{"Item", NULL, item_menu, 0, NULL},
	{"X", menu_nop, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU scratch_menu[] =
{
	{"File", NULL, file_menu, 0, NULL},
	{"Edit", NULL, edit_menu, 0, NULL},
	{"View", NULL, view_menu, 0, NULL},
	{"X", menu_nop, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU sound_sub_menu[] =
{
	{"Patch\tEnter", sound_menu_patch, NULL, 0, NULL},
	{"Unpatch\tBackspace", sound_menu_unpatch, NULL, 0, NULL},
	{"Blank\tDel", sound_menu_blank, NULL, 0, NULL},
	{"Export\tF5", sound_menu_export, NULL, 0, NULL},
	{"Listen\tSpace", sound_menu_listen, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU sound_menu[] =
{
	{"File", NULL, file_menu, 0, NULL},
	{"View", NULL, view_menu, 0, NULL},
	{"Sound", NULL, sound_sub_menu, 0, NULL},
	{"X", menu_nop, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

DIALOG help_dialog[] = 
{
   { d_textbox_proc, 0, 0, 640, 480, 8, 31, 0, 0, 0, 0, NULL, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG ani_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_menu_proc, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, ani_menu, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG pos_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_menu_proc, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, pos_menu, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG scratch_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_menu_proc, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, scratch_menu, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG sound_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_menu_proc, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, sound_menu, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG rotate_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { ppgui_shadow_box_proc, 80, 60, 160, 80, 2, 23, 0, 0, 0, 0, NULL, NULL, NULL },
   { d_ctext_proc, 108, 68, 104, 8, 2, 23, 0, 0, 0, 0, "Rotation", NULL, NULL },
   { d_text_proc, 96, 84, 72, 8, 2, 23, 0, 0, 0, 0, "Speed  :", NULL, NULL },
   { d_text_proc, 96, 100, 72, 8, 2, 23, 0, 0, 0, 0, "Degrees:", NULL, NULL },
   { ppgui_edit_proc, 166, 84, 32, 8, 2, 23, 0, 0, 3, 0, wtext, NULL, NULL },
   { ppgui_edit_proc, 166, 100, 32, 8, 2, 23, 0, 0, 3, 0, htext, NULL, NULL },
   { ppgui_button_proc, 92, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "OK", NULL, NULL },
   { ppgui_button_proc, 164, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "Cancel", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG new_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { ppgui_shadow_box_proc, 80, 60, 160, 80, 2, 23, 0, 0, 0, 0, NULL, NULL, NULL },
   { d_ctext_proc, 108, 68, 104, 8, 2, 23, 0, 0, 0, 0, "New Character", NULL, NULL },
   { d_text_proc, 88, 84, 64, 8, 2, 23, 0, 0, 0, 0, "Width :", NULL, NULL },
   { d_text_proc, 88, 100, 64, 8, 2, 23, 0, 0, 0, 0, "Height:", NULL, NULL },
   { ppgui_edit_proc, 150, 84, 32, 8, 2, 23, 0, 0, 3, 0, wtext, NULL, NULL },
   { ppgui_edit_proc, 150, 100, 32, 8, 2, 23, 0, 0, 3, 0, htext, NULL, NULL },
   { ppgui_button_proc, 92, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "OK", NULL, NULL },
   { ppgui_button_proc, 164, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "Cancel", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG new_ani_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { ppgui_shadow_box_proc, 80, 60, 160, 80, 2, 23, 0, 0, 0, 0, NULL, NULL, NULL },
   { d_ctext_proc, 108, 68, 104, 8, 2, 23, 0, 0, 0, 0, "New ANI", NULL, NULL },
   { d_text_proc, 88, 84, 64, 8, 2, 23, 0, 0, 0, 0, "Width :", NULL, NULL },
   { d_text_proc, 88, 100, 64, 8, 2, 23, 0, 0, 0, 0, "Height:", NULL, NULL },
   { ppgui_edit_proc, 150, 84, 32, 8, 2, 23, 0, 0, 3, 0, wtext, NULL, NULL },
   { ppgui_edit_proc, 150, 100, 32, 8, 2, 23, 0, 0, 3, 0, htext, NULL, NULL },
   { ppgui_button_proc, 92, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "OK", NULL, NULL },
   { ppgui_button_proc, 164, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "Cancel", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG pf_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { ppgui_shadow_box_proc, 80, 60, 160, 80, 2, 23, 0, 0, 0, 0, NULL, NULL, NULL },
   { d_ctext_proc, 108, 68, 104, 8, 2, 23, 0, 0, 0, 0, "Paint Leniency", NULL, NULL },
   { d_text_proc, 88, 88, 64, 8, 2, 23, 0, 0, 0, 0, "Amount :", NULL, NULL },
   { ppgui_edit_proc, 150, 88, 32, 8, 2, 23, 0, 0, 3, 0, wtext, NULL, NULL },
   { ppgui_button_proc, 92, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "OK", NULL, NULL },
   { ppgui_button_proc, 164, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "Cancel", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

int player_save_character_soth(PLAYER_DATA * dp, char * fn)
{
	int i;
	PACKFILE * fp;
	char stuff[1024] = {0};
	
	fp = pack_fopen(fn, "w");
	if(!fp)
	{
		return 0;
	}
	pack_fwrite(stuff, 128, fp);
	pack_fwrite(stuff, 128, fp);
	pack_fwrite(stuff, 1024, fp);
	save_ani_fp(dp->ap[0], fp, NULL);
	save_ani_fp(dp->ap[0], fp, NULL);
	save_ani_fp(dp->ap[0], fp, NULL);
	save_ani_fp(dp->ap[1], fp, NULL);
	save_ani_fp(dp->ap[2], fp, NULL);
	save_ani_fp(dp->ap[3], fp, NULL);
	save_ani_fp(dp->ap[3], fp, NULL);
	save_ani_fp(dp->ap[3], fp, NULL);
	save_ani_fp(dp->ap[4], fp, NULL);
	save_ani_fp(dp->ap[5], fp, NULL);
	save_ani_fp(dp->ap[6], fp, NULL);
	save_ani_fp(dp->ap[7], fp, NULL);
	save_ani_fp(dp->ap[7], fp, NULL);
	save_ani_fp(dp->ap[7], fp, NULL);
	save_ani_fp(dp->ap[4], fp, NULL);
	save_ani_fp(dp->ap[4], fp, NULL);
	pack_fclose(fp);
	
	return 1;
}

void toggle_mouse(void)
{
	if(mouse_on)
	{
		set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
		position_mouse(mouse_x * 2, mouse_y * 2);
		read_mouse();
		mouse_on = 0;
	}
	else
	{
		position_mouse(mouse_x / 2, mouse_y / 2);
		read_mouse();
		set_mouse_range(0, 0, 319, 239);
		mouse_on = 1;
	}
}

char * load_text(char * fn)
{
	char * otext;
	PACKFILE * fp;
	int i;
	int s = file_size(fn);
	
	otext = malloc(s + 1);
	fp = pack_fopen(fn, "r");
	for(i = 0; i < s; i++)
	{
		otext[i] = pack_getc(fp);
	}
	pack_fclose(fp);
	
	return otext;
}

void free_text(char * text)
{
	if(text)
	{
		free(text);
	}
}

void clear_clipboard(void)
{
    ani_sel_bp = NULL;
	if(ani_sel_fbp)
	{
		destroy_bitmap(ani_sel_fbp);
		ani_sel_fbp = NULL;
	}
	if(ani_sel_cbp)
	{
		destroy_bitmap(ani_sel_cbp);
		ani_sel_cbp = NULL;
	}
	if(char_clipboard)
	{
		destroy_bitmap(char_clipboard);
		char_clipboard = NULL;
	}
	ani_sel_float = 0;
}

void set_menu_start(void)
{
	file_menu[2].flags = D_DISABLED;
	ani_menu[1].flags = D_DISABLED;
	ani_menu[2].flags = D_DISABLED;
	ani_menu[3].flags = D_DISABLED;
}

void set_menu_edit(void)
{
	if(!cb_full())
	{
		edit_menu[2].flags = D_DISABLED;
	}
	else
	{
		edit_menu[2].flags = 0;
	}
	if(ani_sel_bp)
	{
		edit_menu[0].flags = 0;
		edit_menu[1].flags = 0;
		edit_menu[4].flags = 0;
		edit_menu[5].flags = 0;
		edit_menu[6].flags = 0;
	}
	else
	{
		edit_menu[0].flags = D_DISABLED;
		edit_menu[1].flags = D_DISABLED;
		edit_menu[4].flags = D_DISABLED;
		edit_menu[5].flags = D_DISABLED;
		edit_menu[6].flags = D_DISABLED;
	}
}

void set_menu_loaded(void)
{
	file_menu[2].flags = 0;
	ani_menu[1].flags = 0;
	ani_menu[2].flags = 0;
	ani_menu[3].flags = 0;
	set_menu_edit();
}

void set_menu_frame(void)
{
	if(ap)
	{
		if(ap->f > 1)
		{
			frame_menu[5].flags = 0;
		}
		else
		{
			frame_menu[5].flags = D_DISABLED;
		}
	}
}

void set_item_menu(void)
{
	if(selected_state < OLD_PLAYER_STATES)
	{
		item_menu[1].flags = D_DISABLED;
		item_menu[2].flags = D_DISABLED;
	}
	else
	{
		item_menu[1].flags = 0;
		item_menu[2].flags = 0;
	}
}

void check_item(int i)
{
	int j;

	for(j = 0; j < ITEMS; j++)
	{
		if(i == j)
		{
			item_menu[j].flags = D_SELECTED;
		}
		else
		{
			item_menu[j].flags = 0;
		}
	}
}

void save_char_fp(PLAYER_DATA * dp, PACKFILE * fp)
{
	int i;

	pack_iputl(12345, fp);
	if(dp->pap)
	{
		pack_iputl(1, fp);
		save_ani_fp(dp->pap, fp, NULL);
	}
	else
	{
		pack_iputl(0, fp);
	}
	if(dp->ppap)
	{
		pack_iputl(1, fp);
		save_ani_fp(dp->ppap, fp, NULL);
	}
	else
	{
		pack_iputl(0, fp);
	}
	for(i = 0; i < GUN_FLASH_TYPES; i++)
	{
		if(dp->gap[i])
		{
			pack_iputl(1, fp);
			save_ani_fp(dp->gap[i], fp, NULL);
		}
		else
		{
			pack_iputl(0, fp);
		}
	}
	for(i = 0; i < PLAYER_STATES; i++)
	{
		if(dp->ap[i])
		{
			pack_iputl(1, fp);
			save_ani_fp(dp->ap[i], fp, NULL);
		}
		else
		{
			pack_iputl(0, fp);
		}
		pack_iputl(dp->cx_table[i], fp);
		pack_iputl(dp->cy_table[i], fp);
		pack_iputl(dp->cw_table[i], fp);
		pack_iputl(dp->ch_table[i], fp);
		pack_iputl(dp->px_table[i], fp);
		pack_iputl(dp->py_table[i], fp);
		pack_iputl(dp->gx_table[i], fp);
		pack_iputl(dp->gy_table[i], fp);
	}
	pack_iputl(dp->mx, fp);
	pack_iputl(dp->my, fp);
	pack_iputl(dp->mr, fp);
	pack_iputl(dp->ix, fp);
	pack_iputl(dp->iy, fp);
	pack_iputl(dp->pf, fp);
}

void load_old_char_fp(PLAYER_DATA * dp, PACKFILE * fp)
{
	int i;
	int temp;

	temp = pack_igetl(fp);
	if(temp)
	{
		dp->pap = load_ani_fp(fp, NULL);
	}
	else
	{
		dp->pap = NULL;
	}
	temp = pack_igetl(fp);
	if(temp)
	{
		dp->ppap = load_ani_fp(fp, NULL);
	}
	else
	{
		dp->ppap = NULL;
	}
	for(i = 0; i < GUN_FLASH_TYPES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->gap[i] = load_ani_fp(fp, NULL);
		}
		else
		{
			dp->gap[i] = NULL;
		}
	}
	for(i = 0; i < OLD_PLAYER_STATES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->ap[i] = load_ani_fp(fp, NULL);
			dp->ap[i + OLD_PLAYER_STATES] = duplicate_ani(dp->ap[i]);
		}
		else
		{
			dp->ap[i] = NULL;
			dp->ap[i + OLD_PLAYER_STATES] = NULL;
		}
		dp->cx_table[i] = pack_igetl(fp);
		dp->cy_table[i] = pack_igetl(fp);
		dp->cw_table[i] = pack_igetl(fp);
		dp->ch_table[i] = pack_igetl(fp);
		dp->px_table[i] = pack_igetl(fp);
		dp->py_table[i] = pack_igetl(fp);
		dp->gx_table[i] = pack_igetl(fp);
		dp->gy_table[i] = pack_igetl(fp);
		dp->cx_table[i + OLD_PLAYER_STATES] = dp->cx_table[i];
		dp->cy_table[i + OLD_PLAYER_STATES] = dp->cy_table[i];
		dp->cw_table[i + OLD_PLAYER_STATES] = dp->cw_table[i];
		dp->ch_table[i + OLD_PLAYER_STATES] = dp->ch_table[i];
		dp->px_table[i + OLD_PLAYER_STATES] = dp->px_table[i];
		dp->py_table[i + OLD_PLAYER_STATES] = dp->py_table[i];
		dp->gx_table[i + OLD_PLAYER_STATES] = dp->gx_table[i];
		dp->gy_table[i + OLD_PLAYER_STATES] = dp->gy_table[i];
	}
	dp->mx = pack_igetl(fp);
	dp->my = pack_igetl(fp);
	dp->mr = pack_igetl(fp);
	dp->ix = pack_igetl(fp);
	dp->iy = pack_igetl(fp);
}

void load_char_fp(PLAYER_DATA * dp, PACKFILE * fp)
{
	int i;
	int temp;

	temp = pack_igetl(fp);
	if(temp)
	{
		dp->pap = load_ani_fp(fp, NULL);
	}
	else
	{
		dp->pap = NULL;
	}
	temp = pack_igetl(fp);
	if(temp)
	{
		dp->ppap = load_ani_fp(fp, NULL);
	}
	else
	{
		dp->ppap = NULL;
	}
	for(i = 0; i < GUN_FLASH_TYPES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->gap[i] = load_ani_fp(fp, NULL);
		}
		else
		{
			dp->gap[i] = NULL;
		}
	}
	for(i = 0; i < PLAYER_STATES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->ap[i] = load_ani_fp(fp, NULL);
		}
		else
		{
			dp->ap[i] = NULL;
		}
		dp->cx_table[i] = pack_igetl(fp);
		dp->cy_table[i] = pack_igetl(fp);
		dp->cw_table[i] = pack_igetl(fp);
		dp->ch_table[i] = pack_igetl(fp);
		dp->px_table[i] = pack_igetl(fp);
		dp->py_table[i] = pack_igetl(fp);
		dp->gx_table[i] = pack_igetl(fp);
		dp->gy_table[i] = pack_igetl(fp);
	}
	dp->mx = pack_igetl(fp);
	dp->my = pack_igetl(fp);
	dp->mr = pack_igetl(fp);
	dp->ix = pack_igetl(fp);
	dp->iy = pack_igetl(fp);
	dp->pf = pack_igetl(fp);
	for(i = 0; i < PP_MAX_SOUNDS; i++)
	{
		dp->sound[i] = sound[i];
	}
}

int detect_char_type(char * fn)
{
	ANIMATION * ap;
	PACKFILE * fp;

	fp = pack_fopen(fn, "r");
	pack_igetl(fp);
	ap = load_ani_fp(fp, NULL);
	pack_fclose(fp);
	if(ap != NULL)
	{
		destroy_ani(ap);
		return 0;
	}
	return 1;
}

int load_sounds(PLAYER_DATA * dp, const char * fn)
{
	PACKFILE * fp;
	int i;
	
	fp = pack_fopen(fn, "r");
	if(!fp)
	{
		pack_fclose(fp);
		return 0;
	}
	for(i = 0; i < 64; i++)
	{
		if(pack_getc(fp))
		{
			dp->sound[i] = ncds_load_wav_fp(fp);
			if(dp->sound[i]->len <= 1)
			{
				dp->sound[i] = ncds_internal_sample;
			}
		}
	}
	pack_fclose(fp);
	return 1;
}

int load_char(PLAYER_DATA * dp, char * fn)
{
	PACKFILE * fp;
	int type;
	char ffn[1024];

	type = detect_char_type(fn);
	fp = pack_fopen(fn, "r");
	if(!fp)
	{
		return 0;
	}
	if(type == 0)
	{
		load_old_char_fp(dp, fp);
	}
	else
	{
		pack_igetl(fp);
		load_char_fp(dp, fp);
	}
	pack_fclose(fp);
	replace_extension(ffn, fn, "pps", 1023);
	if(exists(ffn))
	{
		load_sounds(dp, ffn);
	}
	return 1;
}

int save_sounds(PLAYER_DATA * dp, const char * fn)
{
	PACKFILE * fp;
	int i;
	
	fp = pack_fopen(fn, "w");
	if(!fp)
	{
		pack_fclose(fp);
		return 0;
	}
	for(i = 0; i < PP_MAX_SOUNDS; i++)
	{
		if(dp->sound[i] && dp->sound[i] != sound[i])
		{
			pack_putc(1, fp);
			ncds_save_wav_fp(dp->sound[i], fp);
		}
		else
		{
			pack_putc(0, fp);
		}
	}
	pack_fclose(fp);
	return 1;
}

int save_char(PLAYER_DATA * dp, char * fn)
{
	PACKFILE * fp;

	fp = pack_fopen(fn, "w");
	if(!fp)
	{
		return 0;
	}
	save_char_fp(dp, fp);
	pack_fclose(fp);
	return 1;
}

void char_init(void)
{
	int i;

	allegro_init();
    set_window_title("Paintball Party Character Designer");
//	set_window_close_button(0);
	set_display_switch_mode(SWITCH_BACKGROUND);
	set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
	mygui_init();
//	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
	install_keyboard();
	install_mouse();
	install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
//	my_gui_init(&my_mouse_x, &my_mouse_y, NULL, &my_mouse_b, &user_control);
//	my_gui_init(&my_mouse_x, &my_mouse_y, NULL, &my_mouse_b, NULL);
	ncds_internal_sample = create_sample(8, 0, 11025, 1);
	for(i = 0; i < ncds_internal_sample->len; i++)
	{
		((char *)ncds_internal_sample->data)[i] = 128;
	}
	set_mouse_range(0, 0, 319, 239);
	char_screen = create_bitmap(320, 240);
	char_scratch = create_bitmap(64, 64);
	clear_bitmap(char_scratch);
	it_ani = load_ani("pp.dat#animations/it_pointer.ani", char_palette);
	pointer_ani = load_ani("pp.dat#animations/pointer.ani", NULL);
	mouse_corner = load_pcx("chared.dat#mouse_corner.pcx", NULL);
	ani_tool_image[0] = load_pcx("chared.dat#tool0.pcx", NULL);
	ani_tool_image[1] = load_pcx("chared.dat#tool1.pcx", NULL);
	ani_tool_image[2] = load_pcx("chared.dat#tool2.pcx", NULL);
	ani_tool_image[3] = load_pcx("chared.dat#tool3.pcx", NULL);
	ani_tool_image[4] = load_pcx("chared.dat#tool4.pcx", NULL);
	ani_tool_image[5] = load_pcx("chared.dat#tool5.pcx", NULL);
	ani_tool_image[6] = load_pcx("chared.dat#tool6.pcx", NULL);
	ani_tool_image[7] = load_pcx("chared.dat#tool7.pcx", NULL);
	ani_tool_image[8] = load_pcx("chared.dat#tool8.pcx", NULL);
	ani_help_text = load_text("chared.txt");

	sound[PP_SOUND_FIRE] = ncds_load_wav("pp.dat#sounds/fire.wav");
	add_sound(PP_SOUND_FIRE);
	sound[PP_SOUND_SPLAT] = ncds_load_wav("pp.dat#sounds/splat.wav");
	add_sound(PP_SOUND_SPLAT);
	sound[PP_SOUND_RELOAD_A] = ncds_load_wav("pp.dat#sounds/reload_a.wav");
	add_sound(PP_SOUND_RELOAD_A);
	sound[PP_SOUND_RELOAD_B] = ncds_load_wav("pp.dat#sounds/reload_b.wav");
	add_sound(PP_SOUND_RELOAD_B);
	sound[PP_SOUND_AMMO] = ncds_load_wav("pp.dat#sounds/ammo.wav");
	add_sound(PP_SOUND_AMMO);
	sound[PP_SOUND_CLOAK] = ncds_load_wav("pp.dat#sounds/cloak.wav");
	add_sound(PP_SOUND_CLOAK);
	sound[PP_SOUND_GEM] = ncds_load_wav("pp.dat#sounds/gem.wav");
	add_sound(PP_SOUND_GEM);
	sound[PP_SOUND_JUMP] = ncds_load_wav("pp.dat#sounds/jump.wav");
	add_sound(PP_SOUND_JUMP);
	sound[PP_SOUND_FLY] = ncds_load_wav("pp.dat#sounds/fly.wav");
	add_sound(PP_SOUND_FLY);
	sound[PP_SOUND_LAND] = ncds_load_wav("pp.dat#sounds/land.wav");
	add_sound(PP_SOUND_LAND);
	sound[PP_SOUND_HIT] = ncds_load_wav("pp.dat#sounds/hit.wav");
	add_sound(PP_SOUND_HIT);
	sound[PP_SOUND_BUMP] = ncds_load_wav("pp.dat#sounds/bump.wav");
	add_sound(PP_SOUND_BUMP);
	sound[PP_SOUND_TIME] = ncds_load_wav("pp.dat#sounds/time.wav");
	add_sound(PP_SOUND_TIME);
	sound[PP_SOUND_RUN] = ncds_load_wav("pp.dat#sounds/run.wav");
	add_sound(PP_SOUND_RUN);
	sound[PP_SOUND_DEFLECT] = ncds_load_wav("pp.dat#sounds/deflect.wav");
	add_sound(PP_SOUND_DEFLECT);
	sound[PP_SOUND_FLAG] = ncds_load_wav("pp.dat#sounds/flag.wav");
	add_sound(PP_SOUND_FLAG);
	sound[PP_SOUND_BANK] = ncds_load_wav("pp.dat#sounds/bank.wav");
	add_sound(PP_SOUND_BANK);
	sound[PP_SOUND_BASE] = ncds_load_wav("pp.dat#sounds/base.wav");
	add_sound(PP_SOUND_BASE);
	sound[PP_SOUND_PJUMP] = ncds_load_wav("pp.dat#sounds/pjump.wav");
	add_sound(PP_SOUND_PJUMP);
	sound[PP_SOUND_PFLY] = ncds_load_wav("pp.dat#sounds/pfly.wav");
	add_sound(PP_SOUND_PFLY);
	sound[PP_SOUND_PTURBO] = ncds_load_wav("pp.dat#sounds/pturbo.wav");
	add_sound(PP_SOUND_PTURBO);
	sound[PP_SOUND_TELE_IN] = ncds_load_wav("pp.dat#sounds/tele_in.wav");
	add_sound(PP_SOUND_TELE_IN);
	sound[PP_SOUND_TELE_OUT] = ncds_load_wav("pp.dat#sounds/tele_in.wav");
	add_sound(PP_SOUND_TELE_OUT);
	sound[PP_SOUND_CRUSH] = ncds_load_wav("pp.dat#sounds/crush.wav");
	add_sound(PP_SOUND_CRUSH);
	sound[PP_SOUND_TARGET] = ncds_load_wav("pp.dat#sounds/target.wav");
	add_sound(PP_SOUND_TARGET);
	sound[PP_SOUND_BOUNCE] = ncds_load_wav("pp.dat#sounds/bounce.wav");
	add_sound(PP_SOUND_BOUNCE);
	sound[PP_SOUND_RICOCHET] = ncds_load_wav("pp.dat#sounds/ricochet.wav");
	add_sound(PP_SOUND_RICOCHET);
	add_sound(PP_SOUND_START);
	add_sound(PP_SOUND_WIN);
	sound[PP_SOUND_ENTER] = ncds_load_wav("pp.dat#sounds/menu_enter.wav");
	add_sound(PP_SOUND_ENTER);
	sound[PP_SOUND_RESPAWN] = ncds_load_wav("pp.dat#sounds/respawn.wav");
	sound[PP_SOUND_MENU_PICK] = ncds_load_wav("pp.dat#sounds/menu_pick.wav");
	sound[PP_SOUND_MENU_NEXT] = ncds_load_wav("pp.dat#sounds/menu_next.wav");
	sound[PP_SOUND_MENU_LEFT] = ncds_load_wav("pp.dat#sounds/menu_left.wav");
	sound[PP_SOUND_MENU_RIGHT] = ncds_load_wav("pp.dat#sounds/menu_right.wav");
	sound[PP_SOUND_MENU_FLAG] = ncds_load_wav("pp.dat#sounds/menu_flag.wav");
	sound[PP_SOUND_MENU_ENTER] = ncds_load_wav("pp.dat#sounds/menu_enter.wav");
	sound[PP_SOUND_MENU_ESC] = ncds_load_wav("pp.dat#sounds/menu_esc.wav");
	sound[PP_SOUND_MENU_MOVE] = ncds_load_wav("pp.dat#sounds/menu_move.wav");

	set_palette(char_palette);
	gui_bg_color = 23;
	gui_mg_color = 12;
	gui_fg_color = 2;
//	gui_fg_color = makecol(64, 64, 64);
//	gui_mg_color = makecol(128, 128, 128);
//	gui_bg_color = makecol(255, 255, 255);
	set_mouse_sprite(NULL);
	for(i = 0; i < PLAYER_STATES; i++)
	{
		player_data.ap[i] = NULL;
	}
	player_data.pap = NULL;
	for(i = 0; i < GUN_FLASH_TYPES; i++)
	{
		player_data.gap[i] = NULL;
	}
	for(i = 0; i < 32; i++)
	{
		peg[i].active = 0;
	}
	set_menu_start();
	set_menu_frame();
	install_int_ex(logic_timer, BPS_TO_TIMER(60));
}

void char_exit(void)
{
	int i;

	destroy_sample(ncds_internal_sample);
	destroy_bitmap(char_screen);
	destroy_bitmap(char_scratch);
	destroy_ani(it_ani);
	destroy_ani(pointer_ani);
	destroy_bitmap(mouse_corner);
	for(i = 0; i < PLAYER_STATES; i++)
	{
		destroy_ani(player_data.ap[i]);
	}
	for(i = 0; i < 4; i++)
	{
		destroy_ani(player_data.gap[i]);
	}
	for(i = 0; i < PP_MAX_SOUNDS; i++)
	{
		if(player_data.sound[i] && player_data.sound[i] != sound[i])
		{
			ncds_free_sample(player_data.sound[i]);
		}
	}
	for(i = 0; i < 9; i++)
	{
		destroy_bitmap(ani_tool_image[i]);
	}
	destroy_ani(player_data.pap);
	destroy_ani(player_data.ppap);
	free_text(ani_help_text);
	for(i = 0; i < PP_MAX_SOUNDS; i++)
	{
		ncds_free_sample(sound[i]);
	}
}

int grab_frame(BITMAP * sp, int seq)
{
	BITMAP * dp;
	PALETTE spal;
	int sx = 0, sy = 0;
	int ox = 0, oy = 0;
	int step = 0;
	int w = 0;
	int i;
	int gx[256], gy[256], g = 0;
	
	scare_mouse();
//	dp = create_bitmap(player_data.ap[0]->w, player_data.ap[0]->h);
	dp = create_bitmap(ap->w, ap->h);
	clear_bitmap(dp);
	if(sp->w <= char_screen->w && sp->h <= char_screen->h)
	{
		set_mouse_range(0, 0, sp->w - dp->w, sp->h - dp->h);
		step = 1;
	}
	else
	{
		set_mouse_range(0, 0, sp->w - char_screen->w, sp->h - char_screen->h);
	}
	while(step < 2)
	{
		read_mouse();
		switch(step)
		{
			case 0:
			{
				ox = my_mouse_x;
				oy = my_mouse_y;
//				if(ox > char_screen->w - sp->w)
//				{
//					ox = char_screen->w - sp->w;
//				}
//				if(oy > char_screen->h - sp->h)
//				{
//					oy = char_screen->h - sp->h;
//				}
//				position_mouse(ox, oy);
				if((my_mouse_b & 1) && !w)
				{
					w = 1;
					step = 1;
					set_mouse_range(0, 0, sp->w - dp->w, sp->h - dp->h);
				}
				if((my_mouse_b & 2) && !w)
				{
					w = 1;
					return 1;
				}
				break;
			}
			case 1:
			{
				sx = my_mouse_x;
				sy = my_mouse_y;
				if(sx + ap->w > sp->w)
				{
					sx = sp->w - ap->w;
				}
				if(sy + ap->h > sp->h)
				{
					sy = sp->h - ap->h;
				}
				position_mouse(sx, sy);
				if((my_mouse_b & 1) && !w)
				{
					w = 1;
					if(!seq)
					{
						step = 2;
					}
					draw_sprite(dp, sp, -sx, -sy);
					gx[g] = sx;
					gy[g] = sy;
					g++;
					if(!seq)
					{
						blit(dp, ap->image[ani_frame], 0, 0, 0, 0, ap->image[ani_frame]->w, ap->image[ani_frame]->h);
					}
					else
					{
						animation_add_frame(ap, dp);
					}
				}
				if((my_mouse_b & 2) && !w)
				{
					w = 1;
					step = 0;
					set_mouse_range(0, 0, sp->w - char_screen->w, sp->h - char_screen->h);
				}
				break;
			}
		}
		if(!(my_mouse_b & 1) && !(my_mouse_b & 2))
		{
			w = 0;
		}
		
		clear_to_color(char_screen, makecol(0, 64, 128));
		drawing_mode(DRAW_MODE_XOR, NULL, 0, 0);
//		blit(sp, char_screen, 0, 0, -ox, -oy, char_screen->w, char_screen->h);
		blit(sp, char_screen, 0, 0, -ox, -oy, sp->w, sp->h);
		if(step == 1)
		{
			rect(char_screen, sx - ox, sy - oy, sx - ox + ap->w - 1, sy - oy + ap->h - 1, makecol(255, 255, 255));
		}
		for(i = 0; i < g; i++)
		{
			rect(char_screen, gx[i] - ox, gy[i] - oy, gx[i] + ap->w - ox - 1, gy[i] + ap->h - oy - 1, makecol(255, 255, 255));
		}
		drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
//		vsync();
		stretch_blit(char_screen, screen, 0, 0, 320, 240, 0, 0, 640, 480);
	}
//	draw_sprite(dp, sp, -sx, -sy);
	destroy_bitmap(dp);
	unscare_mouse();
	return 1;
}

void update_tables(void)
{
	int i, diff = 0;

	if(player_data.cy_table[selected_state - selected_state % 2] == player_data.cy_table[selected_state - selected_state % 2 + 1])
	{
		diff = player_data.ch_table[selected_state + ((selected_state % 2) == 0 ? 1 : -1)] - player_data.ch_table[selected_state];
	}
	if(selected_state != PS_DUCK_RIGHT && selected_state != PS_DUCK_LEFT && selected_state != PS_FDUCK_RIGHT && selected_state != PS_FDUCK_LEFT)
	{
		for(i = 0; i < PLAYER_STATES; i++)
		{
			if(i != PS_DUCK_RIGHT && i != PS_DUCK_LEFT && i != PS_FDUCK_RIGHT && i != PS_FDUCK_LEFT)
			{
				player_data.cx_table[i] = player_data.cx_table[selected_state];
				player_data.cy_table[i] = player_data.cy_table[selected_state];
				player_data.cw_table[i] = player_data.cw_table[selected_state];
				player_data.ch_table[i] = player_data.ch_table[selected_state];
			}
			else
			{
				player_data.cx_table[i] = player_data.cx_table[selected_state];
				player_data.cw_table[i] = player_data.cw_table[selected_state];
				player_data.cy_table[i] = player_data.cy_table[selected_state] + (player_data.ch_table[selected_state] - player_data.ch_table[i]) + diff;
				player_data.ch_table[i] = player_data.ch_table[i] - diff;
//				player_data.ch_table[i] = player_data.ch_table[0] - player_data.cy_table[i];
			}
		}
	}
	else
	{
		for(i = 0; i < PLAYER_STATES; i++)
		{
			if(i == PS_FDUCK_RIGHT || i == PS_FDUCK_LEFT || i == PS_DUCK_RIGHT || i == PS_DUCK_LEFT)
			{
				player_data.cx_table[i] = player_data.cx_table[selected_state];
				player_data.cy_table[i] = player_data.cy_table[selected_state];
				player_data.cw_table[i] = player_data.cw_table[selected_state];
				player_data.ch_table[i] = player_data.ch_table[selected_state];
			}
			else
			{
				player_data.cx_table[i] = player_data.cx_table[selected_state];
				player_data.cw_table[i] = player_data.cw_table[selected_state];
				player_data.ch_table[i] = player_data.ch_table[i] - diff;
			}
		}
	}
}

void read_mouse(void)
{
	my_mouse_x = mouse_x;
	my_mouse_y = mouse_y;
	my_mouse_b = mouse_b;
}

void user_control(void)
{
	int i, c, k;
	char f[1024] = {0};
	
	if(control_mode == CONTROL_MODE_USER)
	{
//		poll_keyboard();
//		poll_mouse();
		read_mouse();
//		my2mouse();
		
		/* see if we want to go into recording mode */
//		if(key[KEY_F10])
//		{
//			toggle_mouse();
//			if(file_select_ex("Log File:", f, "pct", 1000, 320, 240))
//			{
//				replace_extension(f, f, "pct", 1000);
//				control_log = pack_fopen(f, "w");
//				if(control_log)
//				{
//					control_mode = CONTROL_MODE_TUT_RECORD;
//				}
//			}
//			toggle_mouse();
//			key[KEY_F10] = 0;
//		}
//		if(key[KEY_F11])
//		{
//			toggle_mouse();
//			if(file_select_ex("Log File:", f, "pct", 1000, 320, 240))
//			{
//				control_log = pack_fopen(f, "r");
//				if(control_log)
//				{
//					control_mode = CONTROL_MODE_TUT_PLAY;
//				}
//			}
//			toggle_mouse();
//			key[KEY_F11] = 0;
//		}
	}
	else if(control_mode == CONTROL_MODE_TUT_RECORD)
	{
//		poll_keyboard();
//		poll_mouse();
		read_mouse();

		/* record mouse */
		pack_iputw(my_mouse_x, control_log);
		pack_iputw(my_mouse_y, control_log);
		pack_iputl(my_mouse_b, control_log);

		/* only record pressed keys */
/*		c = 0;
		for(i = 0; i < KEY_MAX; i++)
		{
			if(key[i])
			{
				c++;
			}
		}
		pack_iputw(c, control_log);
		for(i = 0; i < KEY_MAX; i++)
		{
			if(key[i])
			{
				pack_iputw(i, control_log);
			}
		} */

		if(key[KEY_ESC])
		{
			pack_fclose(control_log);
			control_log = NULL;
			key[KEY_ESC] = 0;
			control_mode = CONTROL_MODE_USER;
		}
	}
	else if(control_mode == CONTROL_MODE_TUT_PLAY)
	{
		
		/* read controls from file */
		my_mouse_x = pack_igetw(control_log);
		my_mouse_y = pack_igetw(control_log);
		my_mouse_b = pack_igetl(control_log);
		if(mouse_x != my_mouse_x || mouse_y != my_mouse_y)
		{
			position_mouse(my_mouse_x, my_mouse_y);
		}
		
//		read_mouse();
		
/*		for(i = 0; i < KEY_MAX; i++)
		{
			key[i] = 0;
		}
		clear_keybuf();
		c = pack_igetw(control_log);
		for(i = 0; i < c; i++)
		{
			k = pack_igetw(control_log);
			key[k] = 1;
			simulate_keypress(k << 8);
		} */
		if(key[KEY_ESC] || pack_feof(control_log))
		{
			pack_fclose(control_log);
			control_log = NULL;
			key[KEY_ESC] = 0;
			control_mode = CONTROL_MODE_USER;
		}
	}
}

void global_control(void)
{
	user_control();
	if(key[KEY_F1])
	{
		toggle_mouse();
		update();
		help_dialog[0].dp = ani_help_text;
		centre_dialog(help_dialog);
    	my_popup_dialog(help_dialog, 0);
		toggle_mouse();
    	key[KEY_F1] = 0;
    	key[KEY_ESC] = 0;
    }
	if(key[KEY_F2])
	{
		toggle_mouse();
		update();
		file_menu_save();
		toggle_mouse();
		key[KEY_ENTER] = 0;
		key[KEY_F2] = 0;
	}
	if(key[KEY_F3])
	{
		toggle_mouse();
		update();
		file_menu_load();
		toggle_mouse();
		key[KEY_ENTER] = 0;
		key[KEY_F3] = 0;
	}
	if(key[KEY_F4])
	{
		toggle_mouse();
		update();
		file_menu_new();
		toggle_mouse();
		key[KEY_ENTER] = 0;
		key[KEY_F4] = 0;
	}
	if(key[KEY_F6])
	{
		if(player_data.ap[0])
		{
			if(file_select_ex("Save SOTH Character:", filename, "sc", 1023, 320, 240))
			{
				replace_extension(filename, filename, "sc", 1023);
				player_save_character_soth(&player_data, filename);
			}
		}
		key[KEY_ENTER] = 0;
		key[KEY_ESC] = 0;
		key[KEY_F6] = 0;
	}
	if(key[KEY_TAB] && player_data.ap[0])
	{
		state++;
		if(state >= STATES)
		{
			state = 0;
		}
		if(state == STATE_ANI)
		{
			view_menu[0].flags = D_SELECTED;
			view_menu[1].flags = 0;
			view_menu[2].flags = 0;
			view_menu[3].flags = 0;
		}
		else if(state == STATE_POS)
		{
			view_menu[1].flags = D_SELECTED;
			view_menu[0].flags = 0;
			view_menu[2].flags = 0;
			view_menu[3].flags = 0;
			if(selected_state >= PLAYER_STATES)
			{
				selected_state = 0;
			}
			peg_item();
		}
		else if(state == STATE_SCRATCH)
		{
			view_menu[2].flags = D_SELECTED;
			view_menu[0].flags = 0;
			view_menu[1].flags = 0;
			view_menu[3].flags = 0;
		}
		else if(state == STATE_SOUND)
		{
			view_menu[3].flags = D_SELECTED;
			view_menu[0].flags = 0;
			view_menu[1].flags = 0;
			view_menu[2].flags = 0;
		}
		key[KEY_TAB] = 0;
	}
	if(key[KEY_ESC])
	{
		toggle_mouse();
		update();
		file_menu_exit();
		toggle_mouse();
		key[KEY_ESC] = 0;
		key[KEY_ENTER] = 0;
		key[KEY_SPACE] = 0;
	}
}

void fix_select(void)
{
	int n;
	
	if(ani_sel_bp)
	{
		if(ani_sel_x2 < ani_sel_x)
		{
			n = ani_sel_x;
			ani_sel_x = ani_sel_x2;
			ani_sel_x2 = n;
		}
		if(ani_sel_y2 < ani_sel_y)
		{
			n = ani_sel_y;
			ani_sel_y = ani_sel_y2;
			ani_sel_y2 = n;
		}
	}
}

void tool_select(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
//    int * ex, * ey;
	ani_sel_bp = bp;
	if(!bp)
	{
		return;
	}
    ani_sel_x = (my_mouse_x - dx) / s;
    ani_sel_y = (my_mouse_y - dy) / s;
//    ex = &ani_sel_x2;
//    ey = &ani_sel_y2;

    if(ani_sel_float)
    {
	    ani_sel_float = 0;
    }
    while(my_mouse_b & 1)
    {
	    read_mouse();
	    ani_sel_x2 = (my_mouse_x - dx) / s;
	    ani_sel_y2 = (my_mouse_y - dy) / s;
	    if(ani_sel_x > bp->w - 1)
	    {
		    ani_sel_x = bp->w - 1;
    	}
    	else if(ani_sel_x < 0)
    	{
	    	ani_sel_x = 0;
    	}
	    if(ani_sel_x2 > bp->w - 1)
	    {
		    ani_sel_x2 = bp->w - 1;
    	}
    	else if(ani_sel_x2 < 0)
    	{
	    	ani_sel_x2 = 0;
    	}
    	if(ani_sel_y > bp->h - 1)
    	{
	    	ani_sel_y = bp->h - 1;
	    }
    	else if(ani_sel_y < 0)
    	{
	    	ani_sel_y = 0;
    	}
    	if(ani_sel_y2 > bp->h - 1)
    	{
	    	ani_sel_y2 = bp->h - 1;
	    }
    	else if(ani_sel_y2 < 0)
    	{
	    	ani_sel_y2 = 0;
    	}
        proc();
    }
   	fix_select();
    if(char_clipboard)
    {
	    destroy_bitmap(char_clipboard);
	    char_clipboard = NULL;
    }
    if(ani_sel_fbp)
    {
	    destroy_bitmap(ani_sel_fbp);
	    ani_sel_fbp = NULL;
    }
    if(ani_sel_bp)
    {
    	char_clipboard = create_bitmap(ani_sel_x2 - ani_sel_x + 1, ani_sel_y2 - ani_sel_y + 1);
    	clear_bitmap(char_clipboard);
    	ani_sel_fbp = create_bitmap(ani_sel_bp->w, ani_sel_bp->h);
    	clear_bitmap(ani_sel_fbp);
    	draw_sprite(char_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
    }
}

int tool_cut(void)
{
	if(ani_sel_cbp)
    {
	    destroy_bitmap(ani_sel_cbp);
	    ani_sel_cbp = NULL;
    }
    ani_sel_cbp = create_bitmap(ani_sel_x2 - ani_sel_x + 1, ani_sel_y2 - ani_sel_y + 1);
    clear_bitmap(ani_sel_cbp);
    if(char_clipboard)
    {
    	draw_sprite(ani_sel_cbp, char_clipboard, 0, 0);
	    cb_put_bitmap(char_clipboard, char_palette);
    }
    if(!ani_sel_float)
    {
    	rectfill(ani_sel_bp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
    }
    else
    {
	    clear_bitmap(ani_sel_bp);
	    draw_sprite(ani_sel_bp, ani_sel_fbp, 0, 0);
    }
    ani_sel_bp = NULL;
    set_menu_edit();
    return 1;
}

int tool_copy(void)
{
    if(ani_sel_cbp)
    {
	    destroy_bitmap(ani_sel_cbp);
	    ani_sel_cbp = NULL;
    }
    ani_sel_cbp = create_bitmap(ani_sel_x2 - ani_sel_x + 1, ani_sel_y2 - ani_sel_y + 1);
    clear_bitmap(ani_sel_cbp);
    draw_sprite(ani_sel_cbp, char_clipboard, 0, 0);
    cb_put_bitmap(char_clipboard, char_palette);
    set_menu_edit();
    return 1;
}

int tool_paste(void)
{
	BITMAP * cb_temp;
	
	if(char_clipboard)
	{
		destroy_bitmap(char_clipboard);
		char_clipboard = NULL;
	}
	if(ani_sel_fbp)
	{
		destroy_bitmap(ani_sel_fbp);
		ani_sel_fbp = NULL;
	}
    if(ani_sel_cbp)
    {
	    destroy_bitmap(ani_sel_cbp);
	    ani_sel_cbp = NULL;
    }
	ani_sel_bp = bp;
//	char_clipboard = create_bitmap(ani_sel_cbp->w, ani_sel_cbp->h);
//	clear_bitmap(char_clipboard);
//	draw_sprite(char_clipboard, ani_sel_cbp, 0, 0);

	/* attempt to get image from Windows clipboard */
	cb_temp = cb_get_bitmap(char_palette);
	
	if(cb_temp)
	{
		char_clipboard = create_bitmap(cb_temp->w, cb_temp->h);
		ani_sel_cbp = create_bitmap(cb_temp->w, cb_temp->h);
		
		/* converts the clipboard image to proper color format */
		blit(cb_temp, char_clipboard, 0, 0, 0, 0, char_clipboard->w, char_clipboard->h);
		blit(cb_temp, ani_sel_cbp, 0, 0, 0, 0, ani_sel_cbp->w, ani_sel_cbp->h);
		destroy_bitmap(cb_temp);
		
		/* create floating bitmap */
		ani_sel_fbp = create_bitmap(ani_sel_bp->w, ani_sel_bp->h);
		clear_bitmap(ani_sel_fbp);
		
		/* copy image data to floating bitmap */
		draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
		draw_sprite(ani_sel_bp, char_clipboard, 0, 0);
		ani_sel_x = 0;
		ani_sel_y = 0;
		ani_sel_x2 = char_clipboard->w - 1;
		ani_sel_y2 = char_clipboard->h - 1;
		ani_sel_float = 1;
	    ani_tool = TOOL_SELECT;
	    set_menu_edit();
	}
	return 1;
}

int tool_mirror(void)
{
   	rectfill(ani_sel_bp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
    clear_bitmap(ani_sel_fbp);
    draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
	clear_bitmap(ani_sel_bp);
	draw_sprite(ani_sel_bp, ani_sel_fbp, 0, 0);
    draw_sprite_h_flip(ani_sel_bp, char_clipboard, ani_sel_x, ani_sel_y);
    clear_bitmap(char_clipboard);
    draw_sprite(char_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
    return 1;
}

int tool_flip(void)
{
   	rectfill(ani_sel_bp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
    clear_bitmap(ani_sel_fbp);
    draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
	clear_bitmap(ani_sel_bp);
	draw_sprite(ani_sel_bp, ani_sel_fbp, 0, 0);
    draw_sprite_v_flip(ani_sel_bp, char_clipboard, ani_sel_x, ani_sel_y);
    clear_bitmap(char_clipboard);
    draw_sprite(char_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
    return 1;
}

int tool_turn(void)
{
	int w, h, cx, cy;
	
	if(!ani_sel_float)
	{
		rectfill(ani_sel_bp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
    	clear_bitmap(ani_sel_fbp);
	    draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
	    ani_sel_float = 1;
	}
	clear_bitmap(ani_sel_bp);
	draw_sprite(ani_sel_bp, ani_sel_fbp, 0, 0);
		char_clipboard = turn_bitmap(char_clipboard);
		draw_sprite(ani_sel_bp, char_clipboard, ani_sel_x, ani_sel_y);
//    rotate_sprite(ani_sel_bp, char_clipboard, ani_sel_x, ani_sel_y, itofix(64));
    w = ani_sel_x2 - ani_sel_x;
    h = ani_sel_y2 - ani_sel_y;
		ani_sel_x2 = ani_sel_x + h;
		ani_sel_y2 = ani_sel_y + w;
//    cx = (ani_sel_x + ani_sel_x2) / 2;
//    cy = (ani_sel_y + ani_sel_y2) / 2;
//    ani_sel_y = cy - w / 2;
//    ani_sel_y2 = cy + w / 2 + w % 2;
//    ani_sel_x = cx - h / 2;
//    ani_sel_x2 = cx + h / 2 + h % 2;
//    destroy_bitmap(char_clipboard);
//    char_clipboard = create_bitmap(ani_sel_x2 - ani_sel_x + 1, ani_sel_y2 - ani_sel_y + 1);
//    clear_bitmap(char_clipboard);
//    draw_sprite(char_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
    return 1;
}

void tool_move(BITMAP * nbp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int px = (my_mouse_x - dx) / s;
    int py = (my_mouse_y - dy) / s;
    int nx, ny;
    int ax = ani_sel_x;
    int ay = ani_sel_y;
    BITMAP * cbp = create_bitmap(bp->w, bp->h);

	if(!ani_sel_float)
	{
    	rectfill(nbp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
	    clear_bitmap(ani_sel_fbp);
	    draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
    	ani_sel_float = 1;
    }
    else
    {
	    clear_bitmap(nbp);
	    draw_sprite(nbp, ani_sel_fbp, 0, 0);
//		blit(nbp, cbp, 0, 0, 0, 0, cbp->w, cbp->h);
    }
//    blit(nbp, cbp, 0, 0, 0, 0, bp->w, bp->h);
    ani_sel_v = 0;
    while(my_mouse_b & 1)
    {
	    read_mouse();
	    nx = ((my_mouse_x - dx) / s - (px - ax));
	    ny = ((my_mouse_y - dy) / s - (py - ay));
	    ani_sel_x2 -= ani_sel_x - nx;
	    ani_sel_y2 -= ani_sel_y - ny;
	    ani_sel_x = nx;
	    ani_sel_y = ny;
        blit(ani_sel_fbp, nbp, 0, 0, 0, 0, nbp->w, nbp->h);
        draw_sprite(nbp, char_clipboard, nx, ny);
        proc();
    }
    destroy_bitmap(cbp);
    ani_sel_v = 1;
}

void ani_control(void)
{
	BITMAP * scratch;

    if(key[KEY_M] || (my_mouse_x <= 0 && my_mouse_y <= 0))
    {
	    toggle_mouse();
	    update();
		my_popup_dialog(ani_dialog, 0);
		toggle_mouse();
	    key[KEY_M] = 0;
	    key[KEY_ESC] = 0;
    }
	if(player_data.ap[0])
	{
		if(selected_state < PLAYER_STATES)
		{
			ap = player_data.ap[selected_state];
		}
		else if(selected_state < PLAYER_STATES + 4)
		{
			ap = player_data.gap[selected_state - PLAYER_STATES];
		}
		else if(selected_state < PLAYER_STATES + 5)
		{
			ap = player_data.pap;
		}
		else
		{
			ap = player_data.ppap;
		}
		bp = ap->image[ani_frame];
		set_menu_frame();
		if(key[KEY_UP])
		{
			ani_frame--;
			if(ani_frame < 0)
			{
				ani_frame = ap->f - 1;
			}
			bp = ap->image[ani_frame];
			key[KEY_UP] = 0;
		}
		if(key[KEY_DOWN])
		{
			ani_frame++;
			if(ani_frame >= ap->f)
			{
				ani_frame = 0;
			}
			bp = ap->image[ani_frame];
			key[KEY_DOWN] = 0;
		}
		if(key[KEY_LEFT])
		{
			selected_state--;
			if(selected_state < 0)
			{
				selected_state = PLAYER_STATES + 6 - 1;
			}
			if(ani_frame >= ap->f)
			{
				ani_frame = 0;
			}
			bp = ap->image[ani_frame];
			if(selected_state >= OLD_PLAYER_STATES)
			{
				ani_create_menu[3].flags = D_DISABLED;
			}
			else
			{
				ani_create_menu[3].flags = 0;
			}
			if(selected_state >= PLAYER_STATES)
			{
				ani_create_menu[2].flags = D_DISABLED;
			}
			else
			{
				ani_create_menu[2].flags = 0;
			}
			key[KEY_LEFT] = 0;
		}
		if(key[KEY_RIGHT])
		{
			selected_state++;
			if(selected_state >= PLAYER_STATES + 6)
			{
				selected_state = 0;
			}
			if(ani_frame >= ap->f)
			{
				ani_frame = 0;
			}
			bp = ap->image[ani_frame];
			if(selected_state >= OLD_PLAYER_STATES)
			{
				ani_create_menu[3].flags = D_DISABLED;
			}
			else
			{
				ani_create_menu[3].flags = 0;
			}
			if(selected_state >= PLAYER_STATES)
			{
				ani_create_menu[2].flags = D_DISABLED;
			}
			else
			{
				ani_create_menu[2].flags = 0;
			}
			key[KEY_RIGHT] = 0;
		}
		if(key[KEY_A])
		{
			if(key[KEY_LSHIFT])
			{
				ani_shift_left();
			}
			else
			{
				frame_shift_left();
			}
			key[KEY_A] = 0;
		}
		if(key[KEY_D])
		{
			if(key[KEY_LSHIFT])
			{
				ani_shift_right();
			}
			else
			{
				frame_shift_right();
			}
			key[KEY_D] = 0;
		}
		if(key[KEY_W])
		{
			if(key[KEY_LSHIFT])
			{
				ani_shift_up();
			}
			else
			{
				frame_shift_up();
			}
			key[KEY_W] = 0;
		}
		if(key[KEY_S])
		{
			if(key[KEY_LSHIFT])
			{
				ani_shift_down();
			}
			else
			{
				frame_shift_down();
			}
			key[KEY_S] = 0;
		}
		if((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && key[KEY_C] && ani_sel_bp)
		{
			tool_copy();
			key[KEY_C] = 0;
		}
		else if((((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && key[KEY_V]) || ((key[KEY_LSHIFT] || key[KEY_RSHIFT]) && key[KEY_INSERT])) && ani_sel_cbp)
		{
			tool_paste();
			key[KEY_V] = 0;
			key[KEY_INSERT] = 0;
		}
		else if((key[KEY_LSHIFT] || key[KEY_RSHIFT]) && key[KEY_DEL] && ani_sel_bp)
		{
			tool_cut();
			key[KEY_DEL] = 0;
		}
		if(key[KEY_H] && ani_sel_bp)
		{
			tool_mirror();
			key[KEY_H] = 0;
		}
		else if(key[KEY_V] && ani_sel_bp)
		{
			tool_flip();
			key[KEY_V] = 0;
		}
		else if(key[KEY_R] && ani_sel_bp)
		{
			tool_turn();
			key[KEY_R] = 0;
		}
		if(key[KEY_PLUS_PAD])
		{
			ani_scale++;
			key[KEY_PLUS_PAD] = 0;
		}
		if(key[KEY_MINUS_PAD])
		{
			ani_scale--;
			if(ani_scale < 1)
			{
				ani_scale = 1;
			}
			key[KEY_MINUS_PAD] = 0;
		}
		if(key[KEY_COMMA])
		{
			ap->d++;
			key[KEY_COMMA] = 0;
		}
		if(key[KEY_STOP])
		{
			ap->d--;
			if(ap->d < 1)
			{
				ap->d = 0;
			}
			key[KEY_STOP] = 0;
		}
		if(my_mouse_b & 1)
		{
			if(my_mouse_x < 64 && my_mouse_y >= 240 - 64)
			{
				ani_color = my_mouse_x / 4 + ((my_mouse_y - (240 - 64)) / 4) * 16;
			}
			else if(my_mouse_x >= 64 + 8 && my_mouse_x < 64 + 8 + 48 && my_mouse_y >= 240 - 64 + 8 && my_mouse_y < 240 - 64 + 8 + 48)
			{
				ani_tool = tool_map[(my_mouse_y - (240 - 64 + 8)) / 16][(my_mouse_x - (64 + 8)) / 16];
				if(ani_tool != TOOL_SELECT)
				{
					ani_sel_bp = NULL;
					if(ani_sel_fbp)
					{
						destroy_bitmap(ani_sel_fbp);
						ani_sel_fbp = NULL;
					}
				}
			}
			else if(my_mouse_x > 320 - ap->w * ani_scale && my_mouse_y < ap->h * ani_scale)
			{
			    need_to_save = 1;
			    switch(ani_tool)
			    {
				    case TOOL_PIXEL:
				    {
						putpixel(bp, (my_mouse_x - (320 - ap->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale, ani_color);
						break;
					}
				    case TOOL_FILL:
				    {
						tool_fill(bp, (my_mouse_x - (320 - ap->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale, ani_color);
						break;
					}
				    case TOOL_REPLACE:
				    {
						tool_replace(bp, (my_mouse_x - (320 - ap->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale, ani_color);
						break;
					}
				    case TOOL_LINE:
				    {
						tool_line(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_RECT:
				    {
						tool_rect(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_FRECT:
				    {
						tool_frect(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_CIRCLE:
				    {
						tool_circle(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_FCIRCLE:
				    {
						tool_fcircle(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_SELECT:
				    {
					    if(ani_sel_bp && ani_sel_bp == bp && my_mouse_x >= 320 - ap->w * ani_scale + ani_sel_x * ani_scale && my_mouse_x < 320 - ap->w * ani_scale + ani_sel_x2 * ani_scale + ani_scale && my_mouse_y >= ani_sel_y * ani_scale && my_mouse_y <= ani_sel_y2 * ani_scale + ani_scale)
					    {
							tool_move(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
					    }
					    else
					    {
							tool_select(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
						}
						logic_frames = 0;
						break;
					}
			    }
			}
		    else if(ani_sel_bp && ani_sel_bp == bp && my_mouse_x >= 320 - ap->w * ani_scale + ani_sel_x * ani_scale && my_mouse_x < 320 - ap->w * ani_scale + ani_sel_x2 * ani_scale + ani_scale && my_mouse_y >= ani_sel_y * ani_scale && my_mouse_y <= ani_sel_y2 * ani_scale + ani_scale)
		    {
				tool_move(bp, 1, 0, 0, (320 - ap->w * ani_scale), 0, ani_scale, ani_color, update);
				logic_frames = 0;
		    }
			set_menu_edit();
		}
		if(my_mouse_b & 2)
		{
			if(my_mouse_x > 320 - ap->w * ani_scale && my_mouse_y < ap->h * ani_scale)
			{
				ani_color = getpixel(bp, (my_mouse_x - (320 - ap->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale);
				ani_sel_bp = NULL;
				if(ani_sel_fbp)
				{
					destroy_bitmap(ani_sel_fbp);
					ani_sel_fbp = NULL;
				}
				if(ani_tool == TOOL_SELECT)
				{
					ani_tool = TOOL_PIXEL;
				}
				set_menu_edit();
			}
		}
	}
}

void draw_pal(BITMAP * bp, PALETTE pal, int x, int y, int s)
{
	int i, j, k;

	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < s; j++)
		{
			for(k = 0; k < s; k++)
			{
				putpixel(bp, x + (i % 16) * s + k, y + (i / 16) * s + j, i);
			}
		}
	}
	putpixel(bp, x + ani_color % 16 * s + 1, y + ani_color / 16 * s + 1, makecol(0, 0, 0));
	putpixel(bp, x + ani_color % 16 * s + 1, y + ani_color / 16 * s + 2, makecol(255, 255, 255));
}

void good_scale(BITMAP * bp, BITMAP * sprite, int x, int y, int s)
{
	int i, j, k, l;

	for(i = 0; i < sprite->h; i++)
	{
		for(j = 0; j < sprite->w; j++)
		{
			for(k = 0; k < s; k++)
			{
				for(l = 0; l < s; l++)
				{
					putpixel(bp, x + j * s + l, y + i * s + k, sprite->line[i][j]);
				}
			}
		}
	}
}

void ani_update(void)
{
	clear_to_color(char_screen, makecol(0, 64, 128));
	if(ap)
	{
		if(ani_frame >= ap->f)
		{
			ani_frame = 0;
		}
		draw_ani(char_screen, ap, 32, 32, char_frame);
		good_scale(char_screen, ap->image[ani_frame], 320 - ap->w * ani_scale, 0, ani_scale);
	}
	textprintf_ex(char_screen, font, 0, 0, makecol(255, 255, 255), -1, "%s (%d)", state_name[selected_state], ani_frame);
	draw_pal(char_screen, char_palette, 0, 240 - 64, 4);
	draw_sprite(char_screen, ani_tool_image[ani_tool], 64 + 8, 240 - 64 + 8);
	if(mouse_on)
	{
		draw_sprite(char_screen, mouse_sprite, my_mouse_x, my_mouse_y);
	}
//	if(char_frame % 2 == 0 && bp == ani_sel_bp && bp != NULL && ani_sel_v)
	if(bp == ani_sel_bp && bp != NULL && ani_sel_v)
	{
		rect(char_screen, ani_sel_x < ani_sel_x2 ? 320 - ap->w * ani_scale + ani_sel_x * ani_scale : 320 - ap->w * ani_scale + ani_sel_x2 * ani_scale, ani_sel_y < ani_sel_y2 ? ani_sel_y * ani_scale : ani_sel_y2 * ani_scale, ani_sel_x2 > ani_sel_x ? 320 - ap->w * ani_scale + ani_sel_x2 * ani_scale + ani_scale - 1 : 320 - ap->w * ani_scale + ani_sel_x * ani_scale + ani_scale - 1, ani_sel_y2 > ani_sel_y ? ani_sel_y2 * ani_scale + ani_scale - 1 : ani_sel_y * ani_scale + ani_scale - 1, makecol(255, 255, 255));
	}
//	vsync();
//	blit(char_screen, screen, 0, 0, 0, 0, 320, 240);
	stretch_blit(char_screen, screen, 0, 0, 320, 240, 0, 0, 640, 480);
//	char_frame++;
}

void cr2cw(int st)
{
	int i;

	player_data.cw_table[st] = player_data.cr_table[st] - player_data.cx_table[st] + 1;
	player_data.ch_table[st] = player_data.cb_table[st] - player_data.cy_table[st] + 1;
}

void cw2cr(int st)
{
	int i;

	player_data.cr_table[st] = player_data.cx_table[st] + player_data.cw_table[st] - 1;
	player_data.cb_table[st] = player_data.cy_table[st] + player_data.ch_table[st] - 1;
}

void clear_pegs(void)
{
	int i;

	for(i = 0; i < 32; i++)
	{
		peg[i].active = 0;
		peg[i].dx = NULL;
		peg[i].dy = NULL;
	}
}

void add_peg(int * dx, int * dy, int ox, int oy)
{
	int i;

	for(i = 0; i < 32; i++)
	{
		if(!peg[i].active)
		{
			peg[i].x = *dx - 2 + 32 + ox;
			peg[i].y = *dy - 2 + 32 + oy;
			peg[i].w = 5;
			peg[i].h = 5;
			peg[i].dx = dx;
			peg[i].dy = dy;
			peg[i].active = 1;
			peg[i].ox = ox;
			peg[i].oy = oy;
			break;
		}
	}
}

void draw_pegs(void)
{
	int i;

	if(pegged < 0)
	{
		for(i = 0; i < 32; i++)
		{
			if(peg[i].active)
			{
				rectfill(char_screen, peg[i].x, peg[i].y, peg[i].x + peg[i].w - 1, peg[i].y + peg[i].h - 1, makecol(255, 255, 255));
				rect(char_screen, peg[i].x, peg[i].y, peg[i].x + peg[i].w - 1, peg[i].y + peg[i].h - 1, makecol(0, 0, 0));
			}
		}
	}
}

void peg_item(void)
{
	clear_pegs();
	switch(item)
	{
		case ITEM_CMAP:
		{
			cw2cr(selected_state);
			add_peg(&player_data.cx_table[selected_state], &player_data.cy_table[selected_state], 0, 0);
			add_peg(&player_data.cr_table[selected_state], &player_data.cb_table[selected_state], 0, 0);
			break;
		}
		case ITEM_GUN:
		{
			if(selected_state >= OLD_PLAYER_STATES)
			{
				add_peg(&player_data.gx_table[selected_state], &player_data.gy_table[selected_state], player_data.gap[dg_table[selected_state]]->w / 2, player_data.gap[dg_table[selected_state]]->h / 2);
			}
			else
			{
				item = ITEM_CMAP;
			}
			break;
		}
		case ITEM_PAINT:
		{
			if(selected_state >= OLD_PLAYER_STATES)
			{
				add_peg(&player_data.px_table[selected_state], &player_data.py_table[selected_state], player_data.pap->w / 2, player_data.pap->h / 2);
			}
			else
			{
				item = ITEM_CMAP;
			}
			break;
		}
		case ITEM_IT:
		{
			add_peg(&player_data.ix, &player_data.iy, it_ani->w / 2, it_ani->h / 2);
			break;
		}
		case ITEM_CENTER:
		{
			add_peg(&player_data.mx, &player_data.my, 0, 0);
			add_peg(&player_data.mr, &player_data.my, player_data.mx, 0);
			break;
		}
	}
	pegged = -1;
}

void pos_control(void)
{
	int i;

    if(key[KEY_M] || (my_mouse_x <= 0 && my_mouse_y <= 0 && !(my_mouse_b & 1)))
    {
	    toggle_mouse();
	    update();
	    my_popup_dialog(pos_dialog, 0);
	    toggle_mouse();
	    key[KEY_M] = 0;
	    key[KEY_ESC] = 0;
    }
	if(!peg[0].active)
	{
		peg_item();
	}
	if(key[KEY_ENTER])
	{
		item++;
		if(selected_state < OLD_PLAYER_STATES)
		{
			while(item < ITEMS && (item == ITEM_GUN || item == ITEM_PAINT))
			{
				item++;
			}
		}
		if(item >= ITEMS)
		{
			item = 0;
		}
		check_item(item);
		key[KEY_ENTER] = 0;
		peg_item();
	}
	if(key[KEY_LEFT])
	{
		selected_state--;
		if(selected_state < 0)
		{
			selected_state = PLAYER_STATES - 1;
		}
		if(selected_state < OLD_PLAYER_STATES)
		{
			if(item < ITEMS && (item == ITEM_GUN || item == ITEM_PAINT))
			{
				item  = 0;
				check_item(item);
			}
		}
		peg_item();
		set_item_menu();
		key[KEY_LEFT] = 0;
	}
	if(key[KEY_RIGHT])
	{
		selected_state++;
		if(selected_state >= PLAYER_STATES)
		{
			selected_state = 0;
		}
		if(selected_state < OLD_PLAYER_STATES)
		{
			if(item < ITEMS && (item == ITEM_GUN || item == ITEM_PAINT))
			{
				item  = 0;
				check_item(item);
			}
		}
		peg_item();
		set_item_menu();
		key[KEY_RIGHT] = 0;
	}
    if(key[KEY_C])
    {
	    need_to_save = 1;
		player_data.mx = player_data.ap[selected_state]->w / 2;
		player_data.my = player_data.ap[selected_state]->h / 2;
		player_data.mr = 32;
		key[KEY_C] = 0;
	}
	if(pegged < 0)
	{
		for(i = 0; i < 32; i++)
		{
			if(peg[i].active)
			{
				if(my_mouse_x > peg[i].x && my_mouse_x < peg[i].x + peg[i].w - 1 && my_mouse_y > peg[i].y && my_mouse_y < peg[i].y + peg[i].h - 1)
				{
					set_mouse_sprite(mouse_corner);
					if(my_mouse_b & 1)
					{
						pegged = i;
						pox = my_mouse_x - peg[i].x;
						poy = my_mouse_y - peg[i].y;
					}
					break;
				}
			}
		}
		if(i >= 32)
		{
			set_mouse_sprite(NULL);
			pegged = -1;
		}
	}
	else if(my_mouse_b & 1)
	{
	    need_to_save = 1;
		peg[pegged].x = my_mouse_x - pox;
		peg[pegged].y = my_mouse_y - poy;
		if(peg[pegged].dx)
		{
			*peg[pegged].dx = peg[pegged].x + 2 - 32 - peg[pegged].ox;
		}
		if(peg[pegged].dy)
		{
			*peg[pegged].dy = peg[pegged].y + 2 - 32 - peg[pegged].oy;
		}
		if(item == ITEM_CMAP)
		{
			cr2cw(selected_state);
			if(cmap_locked)
			{
				update_tables();
			}
		}
	}
	else
	{
		pegged = -1;
	}
}

void pos_update(void)
{
	clear_to_color(char_screen, makecol(0, 64, 128));
	if(selected_state < PLAYER_STATES)
	{
		if(player_data.ap[selected_state])
		{
			/* draw the editor portion */
			draw_ani(char_screen, player_data.ap[selected_state], 32, 32, char_frame);
			rect(char_screen, 32 + player_data.cx_table[selected_state], 32 + player_data.cy_table[selected_state], 32 + player_data.cx_table[selected_state] + player_data.cw_table[selected_state] - 1, 32 + player_data.cy_table[selected_state] + player_data.ch_table[selected_state] - 1, makecol(255, 0, 0));
			if(selected_state >= OLD_PLAYER_STATES)
			{
				if(player_data.pap)
				{
					rect(char_screen, 32 + player_data.px_table[selected_state], 32 + player_data.py_table[selected_state], 32 + player_data.px_table[selected_state] + player_data.pap->w - 1, 32 + player_data.py_table[selected_state] + player_data.pap->h - 1, makecol(255, 0, 0));
				}
				if(player_data.gap[dg_table[selected_state]])
				{
					rect(char_screen, 32 + player_data.gx_table[selected_state], 32 + player_data.gy_table[selected_state], 32 + player_data.gx_table[selected_state] + player_data.gap[dg_table[selected_state]]->w - 1, 32 + player_data.gy_table[selected_state] + player_data.gap[dg_table[selected_state]]->h - 1, makecol(255, 0, 0));
				}
			}
			rect(char_screen, 32 + player_data.ix, 32 + player_data.iy, 32 + player_data.ix + it_ani->w - 1, 32 + player_data.iy + it_ani->h - 1, makecol(255, 0, 0));
			circle(char_screen, 32 + player_data.mx, 32 + player_data.my, player_data.mr, makecol(255, 0, 0));

			switch(item)
			{
				case ITEM_CMAP:
				{
					rect(char_screen, 32 + player_data.cx_table[selected_state], 32 + player_data.cy_table[selected_state], 32 + player_data.cx_table[selected_state] + player_data.cw_table[selected_state] - 1, 32 + player_data.cy_table[selected_state] + player_data.ch_table[selected_state] - 1, makecol(0, 255, 0));
					break;
				}
				case ITEM_GUN:
				{
					rect(char_screen, 32 + player_data.gx_table[selected_state], 32 + player_data.gy_table[selected_state], 32 + player_data.gx_table[selected_state] + player_data.gap[dg_table[selected_state]]->w - 1, 32 + player_data.gy_table[selected_state] + player_data.gap[dg_table[selected_state]]->h - 1, makecol(0, 255, 0));
					break;
				}
				case ITEM_PAINT:
				{
					rect(char_screen, 32 + player_data.px_table[selected_state], 32 + player_data.py_table[selected_state], 32 + player_data.px_table[selected_state] + player_data.pap->w - 1, 32 + player_data.py_table[selected_state] + player_data.pap->h - 1, makecol(0, 255, 0));
					rect(char_screen, 32 + player_data.px_table[selected_state] + player_data.pf, 32 + player_data.py_table[selected_state] + player_data.pf, 32 + player_data.px_table[selected_state] + player_data.pap->w - player_data.pf - 1, 32 + player_data.py_table[selected_state] + player_data.pap->h - player_data.pf - 1, makecol(255, 255, 0));
					break;
				}
				case ITEM_IT:
				{
					rect(char_screen, 32 + player_data.ix, 32 + player_data.iy, 32 + player_data.ix + it_ani->w - 1, 32 + player_data.iy + it_ani->h - 1, makecol(0, 255, 0));
					break;
				}
				case ITEM_CENTER:
				{
					circle(char_screen, 32 + player_data.mx, 32 + player_data.my, player_data.mr, makecol(0, 255, 0));
					break;
				}
			}
			draw_pegs();

			/* draw the preview portion */
			if(selected_state >= OLD_PLAYER_STATES)
			{
				rotate_sprite(char_screen, get_ani(player_data.pap, char_frame), 32 + player_data.px_table[selected_state], 120 + player_data.py_table[selected_state], itofix(a_table[selected_state]));
			}
			draw_ani(char_screen, player_data.ap[selected_state], 32, 120, char_frame);
			if(selected_state >= OLD_PLAYER_STATES)
			{
				draw_ani(char_screen, player_data.gap[dg_table[selected_state]], 32 + player_data.gx_table[selected_state], 120 + player_data.gy_table[selected_state], char_frame);
			}
			draw_ani(char_screen, it_ani, 32 + player_data.ix, 120 + player_data.iy, char_frame);
			pivot_sprite(char_screen, get_ani(pointer_ani, char_frame), 32 + player_data.mx, 120 + player_data.my, player_data.mr, pointer_ani->h / 2, itofix(char_frame % 256));
		}
	}
	textprintf_ex(char_screen, font, 0, 0, makecol(255, 255, 255), -1, "%s - %s", state_name[selected_state], item_text[item]);
	if(mouse_on && pegged < 0)
	{
		if(mouse_sprite == mouse_corner)
		{
			draw_sprite(char_screen, mouse_sprite, my_mouse_x - 8, my_mouse_y - 8);
		}
		else
		{
			draw_sprite(char_screen, mouse_sprite, my_mouse_x, my_mouse_y);
		}
	}
//	vsync();
//	blit(char_screen, screen, 0, 0, 0, 0, 320, 240);
	stretch_blit(char_screen, screen, 0, 0, 320, 240, 0, 0, 640, 480);
//	char_frame++;
}

void scratch_control(void)
{
	bp = char_scratch;
	if(key[KEY_M] || (my_mouse_x <= 0 && my_mouse_y <= 0 && !(my_mouse_b & 1)))
    {
	    toggle_mouse();
	    update();
	    my_popup_dialog(scratch_dialog, 0);
	    toggle_mouse();
	    key[KEY_M] = 0;
	    key[KEY_ESC] = 0;
    }
	if((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && key[KEY_C] && ani_sel_bp)
	{
		tool_copy();
		key[KEY_C] = 0;
	}
	else if((((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && key[KEY_V]) || ((key[KEY_LSHIFT] || key[KEY_RSHIFT]) && key[KEY_INSERT])) && ani_sel_cbp)
	{
		tool_paste();
		key[KEY_V] = 0;
		key[KEY_INSERT] = 0;
	}
	else if((key[KEY_LSHIFT] || key[KEY_RSHIFT]) && key[KEY_DEL] && ani_sel_bp)
	{
		tool_cut();
		key[KEY_DEL] = 0;
	}
	if(key[KEY_H] && ani_sel_bp)
	{
		tool_mirror();
		key[KEY_H] = 0;
	}
	else if(key[KEY_V] && ani_sel_bp)
	{
		tool_flip();
		key[KEY_V] = 0;
	}
	else if(key[KEY_R] && ani_sel_bp)
	{
		tool_turn();
		key[KEY_R] = 0;
	}
	if(key[KEY_PLUS_PAD])
	{
		ani_scale++;
		key[KEY_PLUS_PAD] = 0;
	}
	if(key[KEY_MINUS_PAD])
	{
		ani_scale--;
		if(ani_scale < 1)
		{
			ani_scale = 1;
		}
		key[KEY_MINUS_PAD] = 0;
	}
	if(my_mouse_b & 1)
	{
		if(my_mouse_x < 64 && my_mouse_y >= 240 - 64)
		{
			ani_color = my_mouse_x / 4 + ((my_mouse_y - (240 - 64)) / 4) * 16;
		}
		else if(my_mouse_x >= 64 + 8 && my_mouse_x < 64 + 8 + 48 && my_mouse_y >= 240 - 64 + 8 && my_mouse_y < 240 - 64 + 8 + 48)
		{
			ani_tool = tool_map[(my_mouse_y - (240 - 64 + 8)) / 16][(my_mouse_x - (64 + 8)) / 16];
			if(ani_tool != TOOL_SELECT)
			{
				ani_sel_bp = NULL;
				if(ani_sel_fbp)
				{
					destroy_bitmap(ani_sel_fbp);
					ani_sel_fbp = NULL;
				}
			}
		}
		else if(my_mouse_x > 320 - bp->w * ani_scale && my_mouse_y < bp->h * ani_scale)
		{
		    need_to_save = 1;
		    switch(ani_tool)
		    {
			    case TOOL_PIXEL:
			    {
					putpixel(bp, (my_mouse_x - (320 - bp->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale, ani_color);
					break;
				}
			    case TOOL_FILL:
			    {
					tool_fill(bp, (my_mouse_x - (320 - bp->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale, ani_color);
					break;
				}
			    case TOOL_REPLACE:
			    {
					tool_replace(bp, (my_mouse_x - (320 - bp->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale, ani_color);
					break;
				}
			    case TOOL_LINE:
			    {
					tool_line(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
					break;
				}
			    case TOOL_RECT:
			    {
					tool_rect(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
					break;
				}
			    case TOOL_FRECT:
			    {
					tool_frect(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
					break;
				}
			    case TOOL_CIRCLE:
			    {
					tool_circle(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
					break;
				}
			    case TOOL_FCIRCLE:
			    {
					tool_fcircle(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
					break;
				}
			    case TOOL_SELECT:
			    {
				    if(ani_sel_bp && ani_sel_bp == bp && my_mouse_x >= 320 - bp->w * ani_scale + ani_sel_x * ani_scale && my_mouse_x < 320 - bp->w * ani_scale + ani_sel_x2 * ani_scale + ani_scale && my_mouse_y >= ani_sel_y * ani_scale && my_mouse_y <= ani_sel_y2 * ani_scale + ani_scale)
				    {
						tool_move(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
				    }
				    else
				    {
						tool_select(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
					}
					break;
				}
		    }
		}
	    else if(ani_sel_bp && ani_sel_bp == bp && my_mouse_x >= 320 - bp->w * ani_scale + ani_sel_x * ani_scale && my_mouse_x < 320 - bp->w * ani_scale + ani_sel_x2 * ani_scale + ani_scale && my_mouse_y >= ani_sel_y * ani_scale && my_mouse_y <= ani_sel_y2 * ani_scale + ani_scale)
	    {
			tool_move(bp, 1, 0, 0, (320 - bp->w * ani_scale), 0, ani_scale, ani_color, update);
	    }
		set_menu_edit();
	}
	if(my_mouse_b & 2)
	{
		if(my_mouse_x > 320 - char_scratch->w * ani_scale && my_mouse_y < char_scratch->h * ani_scale)
		{
			ani_color = getpixel(char_scratch, (my_mouse_x - (320 - char_scratch->w * ani_scale)) / ani_scale, my_mouse_y / ani_scale);
			ani_sel_bp = NULL;
			if(ani_sel_fbp)
			{
				destroy_bitmap(ani_sel_fbp);
				ani_sel_fbp = NULL;
			}
			if(ani_tool == TOOL_SELECT)
			{
				ani_tool = TOOL_PIXEL;
			}
			set_menu_edit();
		}
	}
}

void scratch_update(void)
{
	clear_to_color(char_screen, makecol(0, 64, 128));
	good_scale(char_screen, char_scratch, 320 - char_scratch->w * ani_scale, 0, ani_scale);
	draw_pal(char_screen, char_palette, 0, 240 - 64, 4);
	draw_sprite(char_screen, ani_tool_image[ani_tool], 64 + 8, 240 - 64 + 8);
	if(mouse_on)
	{
		draw_sprite(char_screen, mouse_sprite, my_mouse_x, my_mouse_y);
	}
//	if(char_frame % 2 == 0 && bp == ani_sel_bp && bp != NULL && ani_sel_v)
	if(bp == ani_sel_bp && bp != NULL && ani_sel_v)
	{
		rect(char_screen, ani_sel_x < ani_sel_x2 ? 320 - bp->w * ani_scale + ani_sel_x * ani_scale : 320 - bp->w * ani_scale + ani_sel_x2 * ani_scale, ani_sel_y < ani_sel_y2 ? ani_sel_y * ani_scale : ani_sel_y2 * ani_scale, ani_sel_x2 > ani_sel_x ? 320 - bp->w * ani_scale + ani_sel_x2 * ani_scale + ani_scale - 1 : 320 - bp->w * ani_scale + ani_sel_x * ani_scale + ani_scale - 1, ani_sel_y2 > ani_sel_y ? ani_sel_y2 * ani_scale + ani_scale - 1 : ani_sel_y * ani_scale + ani_scale - 1, makecol(255, 255, 255));
	}
//	vsync();
//	blit(char_screen, screen, 0, 0, 0, 0, 320, 240);
	stretch_blit(char_screen, screen, 0, 0, 320, 240, 0, 0, 640, 480);
//	char_frame++;
}

void sound_control(void)
{
    if(key[KEY_M] || (my_mouse_x <= 0 && my_mouse_y <= 0))
    {
	    toggle_mouse();
	    update();
		my_popup_dialog(sound_dialog, 0);
		toggle_mouse();
	    key[KEY_M] = 0;
	    key[KEY_ESC] = 0;
    }
    if(key[KEY_ENTER])
    {
		toggle_mouse();
		update();
	    sound_menu_patch();
		toggle_mouse();
	    key[KEY_ENTER] = 0;
    }
    if(key[KEY_BACKSPACE])
    {
	    sound_menu_unpatch();
	    key[KEY_BACKSPACE] = 0;
    }
    if(key[KEY_DEL])
    {
	    sound_menu_blank();
	    key[KEY_DEL] = 0;
    }
    if(key[KEY_F5])
    {
		toggle_mouse();
		update();
	    sound_menu_export();
	    toggle_mouse();
	    key[KEY_F5] = 0;
    }
    if(key[KEY_SPACE])
    {
	    sound_menu_listen();
	    key[KEY_SPACE] = 0;
    }
    if(key[KEY_UP])
    {
	    selected_sound--;
	    if(selected_sound < 0)
	    {
		    selected_sound = sound_orders - 1;
	    }
	    key[KEY_UP] = 0;
    }
    if(key[KEY_DOWN])
    {
	    selected_sound++;
	    if(selected_sound >= sound_orders)
	    {
		    selected_sound = 0;
	    }
	    key[KEY_DOWN] = 0;
    }
}

void sound_update(void)
{
	int i;
	
	clear_to_color(char_screen, makecol(0, 64, 128));
	for(i = 0; i < sound_orders; i++)
	{
		textprintf_ex(char_screen, font, 0, i * 8, player_data.sound[sound_order[i]] == sound[sound_order[i]] ? makecol(128, 128, 128) : player_data.sound[sound_order[i]] == ncds_internal_sample ? makecol(255, 0, 0) : makecol(255, 255, 255), -1, " %s%s", sound_order[selected_sound] == sound_order[i] ? ">" : " ", sound_text[sound_order[i]]);
	}
	if(mouse_on)
	{
		draw_sprite(char_screen, mouse_sprite, my_mouse_x, my_mouse_y);
	}
//	vsync();
	stretch_blit(char_screen, screen, 0, 0, 320, 240, 0, 0, 640, 480);
}

void control(void)
{
	global_control();
	switch(state)
	{
		case STATE_ANI:
		{
			ani_control();
			break;
		}
		case STATE_POS:
		{
			pos_control();
			break;
		}
		case STATE_SCRATCH:
		{
			scratch_control();
			break;
		}
		case STATE_SOUND:
		{
			sound_control();
			break;
		}
		default:
		{
			pos_control();
			break;
		}
	}
	char_frame++;
}

void update(void)
{
	switch(state)
	{
		case STATE_ANI:
		{
			ani_update();
			break;
		}
		case STATE_POS:
		{
			pos_update();
			break;
		}
		case STATE_SCRATCH:
		{
			scratch_update();
			break;
		}
		case STATE_SOUND:
		{
			sound_update();
			break;
		}
		default:
		{
			break;
		}
	}
}

void char_main(void)
{
	int i;
	
	while(!char_done)
	{
		for(i = 0; i < logic_frames; i++)
		{
			control();
		}
		logic_frames = 0;
		update();
		rest(0);
	}
}

int main(int argc, char * argv[])
{
	int i;
	
	for(i = 1; i < argc; i++)
	{
		if(!stricmp(argv[i], "-admin"))
		{
			admin_mode = 1;
		}
	}
	if(admin_mode)
	{
		sound_menu[0].child = admin_file_menu;
		pos_menu[0].child = admin_file_menu;
		ani_menu[0].child = admin_file_menu;
		scratch_menu[0].child = admin_file_menu;
	}
	char_init();
	char_main();
	char_exit();

	return 0;
}
END_OF_MAIN()

int file_menu_new(void)
{
	int is;
	int i;
	BITMAP * bp;
	int new_ok = 1;
	int sel;

	if(need_to_save)
	{
		sel = alert(NULL, "Forget changes?", NULL, "Yes", "No", 0, 0);
		if(sel == 2)
		{
			new_ok = 0;
		}
	}
	if(new_ok)
	{
		sprintf(wtext, "45");
		sprintf(htext, "48");
		centre_dialog(new_dialog);
    	is = my_popup_dialog(new_dialog, 0);
    	if(is == 6)
    	{
		    need_to_save = 0;
	    	/* create the new character data */
	    	bp = create_bitmap(atoi(wtext), atoi(htext));
	    	clear_bitmap(bp);
			for(i = 0; i < PLAYER_STATES; i++)
			{
				player_data.ap[i] = create_ani(bp->w, bp->h);
				animation_add_frame(player_data.ap[i], bp);
				player_data.cx_table[i] = 0;
				player_data.cy_table[i] = 0;
				player_data.cw_table[i] = player_data.ap[i]->w;
				player_data.ch_table[i] = player_data.ap[i]->h;
			}
			destroy_bitmap(bp);

			/* set some default values */
			player_data.mx = player_data.ap[0]->w / 2;
			player_data.my = player_data.ap[0]->h / 2;
			player_data.mr = 32;
			player_data.pf = 0;

			/* load some default data */
			player_data.gap[0] = load_ani("chared.dat#gun_right.ani", NULL);
			player_data.gap[1] = load_ani("chared.dat#gun_left.ani", NULL);
			player_data.gap[2] = load_ani("chared.dat#gun_up_right.ani", NULL);
			player_data.gap[3] = load_ani("chared.dat#gun_up_left.ani", NULL);
			player_data.pap = load_ani("chared.dat#paint.ani", NULL);
			player_data.ppap = load_ani("chared.dat#particle.ani", NULL);
			set_menu_loaded();
			for(i = 0; i < PP_MAX_SOUNDS; i++)
			{
				player_data.sound[i] = sound[i];
			}
			clear_clipboard();
    	}
    }
	logic_frames = 0;
	return 1;
}

int file_menu_load(void)
{
	if(file_select_ex("Load Character:", filename, "ppc", 1023, 320, 240))
	{
		need_to_save = 0;
		load_char(&player_data, filename);
		strcpy(cfilename, filename);
		set_menu_loaded();
		peg_item();
	}
	key[KEY_ENTER] = 0;
	logic_frames = 0;
	return 1;
}

void web_update(const char * fn, int attrib, int param)
{
	int i, pos;
	char * cfn;
//	char fn[1024];
	char folderfn[1024];
	char sfilename[1024];
	char ifilename[1024];
	char webfn[1024];
	char command[1024];
	BITMAP * webbp;
	
	webbp = create_bitmap(bp->w * 2, bp->h * 2);
	clear_bitmap(webbp);
	stretch_sprite(webbp, bp, 0, 0, webbp->w, webbp->h);
	cfn = get_filename(fn);
	for(i = 0, pos = 0; i < 1024; i++)
	{
		if(cfn[i] >= 'a' && cfn[i] <= 'z')
		{
			webfn[pos] = cfn[i];
			pos++;
		}
		else if(cfn[i] >= 'A' && cfn[i] <= 'Z')
		{
			webfn[pos] = cfn[i] + 32;
			pos++;
		}
		else if(cfn[i] >= '0' && cfn[i] <= '9')
		{
			webfn[pos] = cfn[i];
			pos++;
		}
		else if(cfn[i] == '.')
		{
			webfn[pos] = 0;
			pos++;
			break;
		}
		if(cfn[i] == 0)
		{
			webfn[pos] = 0;
			pos++;
			break;
		}
	}
	strcpy(folderfn, "C:\\newchars\\");
//	strcpy(fn, folderfn);
//	strcat(fn, webfn);
//	replace_extension(fn, fn, "", 1023);
	strcpy(sfilename, cfn);
	replace_extension(sfilename, sfilename, "pps", 1023);
	strcpy(ifilename, cfilename);
	replace_extension(ifilename, ifilename, "png", 1023);
		
	/* create the image */
	sprintf(command, "c:\\pa\\pacomp.exe -c2 -a c:\\newchars\\ppc_%s.zip %s %s", webfn, cfilename, sfilename);
//	textprintf(screen, font, 0, 32, 31, "%s", command);
//	readkey();
	system(command);
	sprintf(command, "c:\\newchars\\ppc_%s.png", webfn);
	save_png(command, webbp, char_palette);
	destroy_bitmap(webbp);
	return;
}

int file_menu_web_update(void)
{
	int i, pos;
	char * cfn;
	char fn[1024];
	char folderfn[1024];
	char sfilename[1024];
	char ifilename[1024];
	char webfn[1024];
	char command[1024];
	BITMAP * webbp;
	
	webbp = create_bitmap(bp->w * 2, bp->h * 2);
	clear_bitmap(webbp);
	stretch_sprite(webbp, bp, 0, 0, webbp->w, webbp->h);
	cfn = get_filename(cfilename);
	for(i = 0, pos = 0; i < 1024; i++)
	{
		if(cfn[i] >= 'a' && cfn[i] <= 'z')
		{
			webfn[pos] = cfn[i];
			pos++;
		}
		else if(cfn[i] >= 'A' && cfn[i] <= 'Z')
		{
			webfn[pos] = cfn[i] + 32;
			pos++;
		}
		else if(cfn[i] >= '0' && cfn[i] <= '9')
		{
			webfn[pos] = cfn[i];
			pos++;
		}
		else if(cfn[i] == '.')
		{
			webfn[pos] = 0;
			pos++;
			break;
		}
		if(cfn[i] == 0)
		{
			webfn[pos] = 0;
			pos++;
			break;
		}
	}
	strcpy(folderfn, "C:\\newchars\\");
	strcpy(fn, folderfn);
	strcat(fn, webfn);
	replace_extension(fn, fn, "", 1023);
	strcpy(sfilename, cfilename);
	replace_extension(sfilename, sfilename, "pps", 1023);
	strcpy(ifilename, cfilename);
	replace_extension(ifilename, ifilename, "png", 1023);
		
	/* create the image */
	sprintf(command, "c:\\pa\\pacomp.exe -c2 -a c:\\newchars\\ppc_%s.zip %s %s", webfn, cfilename, sfilename);
//	textprintf(screen, font, 0, 32, 31, "%s", command);
//	readkey();
	system(command);
	sprintf(command, "c:\\newchars\\ppc_%s.png", webfn);
	save_png(command, webbp, char_palette);
	destroy_bitmap(webbp);
	return 1;
}

int file_menu_save(void)
{
	int i;
	char nfn[1024] = {0};
	
	if(player_data.ap[0])
	{
		if(file_select_ex("Save Character:", filename, "ppc", 1023, 320, 240))
		{
			need_to_save = 0;
			replace_extension(filename, filename, "ppc", 1023);
			save_char(&player_data, filename);
			strcpy(cfilename, filename);
			for(i = 0; i < PP_MAX_SOUNDS; i++)
			{
				if(sound[i] && player_data.sound[i] && sound[i] != player_data.sound[i])
				{
					replace_extension(nfn, filename, "pps", 1023);
					save_sounds(&player_data, nfn);
				}
			}
		}
		key[KEY_ENTER] = 0;
	}
	logic_frames = 0;
	return 1;
}

int file_menu_exit(void)
{
	int sel;

	if(need_to_save)
	{
		sel = alert(NULL, "Quit without saving?", NULL, "Yes", "No", 0, 0);
	}
	else
	{
		sel = alert(NULL, "Sure you wanna quit?", NULL, "Yes", "No", 0, 0);
	}
	if(sel == 1)
	{
		char_done = 1;
	}
	logic_frames = 0;
	return 1;
}

int view_menu_ani(void)
{
	state = STATE_ANI;
	view_menu[0].flags = D_SELECTED;
	view_menu[1].flags = 0;
	view_menu[2].flags = 0;
	view_menu[3].flags = 0;
	return 1;
}

int view_menu_pos(void)
{
	state = STATE_POS;
	view_menu[1].flags = D_SELECTED;
	view_menu[0].flags = 0;
	view_menu[2].flags = 0;
	view_menu[3].flags = 0;
	if(selected_state >= PLAYER_STATES)
	{
		selected_state = 0;
	}
	set_item_menu();
	peg_item();
	return 1;
}

int view_menu_scratch(void)
{
	state = STATE_SCRATCH;
	view_menu[0].flags = 0;
	view_menu[1].flags = 0;
	view_menu[2].flags = D_SELECTED;
	view_menu[3].flags = 0;
	return 1;
}

int view_menu_sounds(void)
{
	state = STATE_SOUND;
	view_menu[0].flags = 0;
	view_menu[1].flags = 0;
	view_menu[2].flags = 0;
	view_menu[3].flags = D_SELECTED;
	return 1;
}

int ani_menu_new(void)
{
	int is;
	int i;
	BITMAP * bp;
	ANIMATION * tap;

	if(ap)
	{
		sprintf(wtext, "%d", ap->w);
		sprintf(htext, "%d", ap->h);
	}
	else
	{
		sprintf(wtext, "16");
		sprintf(htext, "16");
	}
	centre_dialog(new_ani_dialog);
    is = my_popup_dialog(new_ani_dialog, 0);
    if(is == 6)
    {
		need_to_save = 1;
	    bp = create_bitmap(atoi(wtext), atoi(htext));
	    clear_bitmap(bp);
	    tap = create_ani(bp->w, bp->h);
		animation_add_frame(tap, bp);
		destroy_bitmap(bp);
		if(selected_state < PLAYER_STATES)
		{
			destroy_ani(player_data.ap[selected_state]);
			player_data.ap[selected_state] = tap;
		}
		else if(selected_state < PLAYER_STATES + 4)
		{
			destroy_ani(player_data.gap[selected_state - PLAYER_STATES]);
			player_data.gap[selected_state - PLAYER_STATES] = tap;
		}
		else if(selected_state < PLAYER_STATES + 5)
		{
			destroy_ani(player_data.pap);
			player_data.pap = tap;
		}
		else
		{
			destroy_ani(player_data.ppap);
			player_data.ppap = tap;
		}
		ani_sel_bp = NULL;
		if(ani_sel_fbp)
		{
			destroy_bitmap(ani_sel_fbp);
			ani_sel_fbp = NULL;
		}
		ap = NULL;
    }
	return 1;
}

int ani_sub_menu_import(void)
{
	PALETTE pal;
	int i;

	if(file_select_ex("Load ANI:", filename, "ANI", 1023, 320, 240))
	{
		need_to_save = 1;
		if(selected_state < PLAYER_STATES)
		{
			destroy_ani(player_data.ap[selected_state]);
			player_data.ap[selected_state] = load_ani(filename, pal);
			for(i = 0; i < player_data.ap[selected_state]->f; i++)
			{
				convert_palette(player_data.ap[selected_state]->image[i], pal, char_palette);
			}
		}
		else if(selected_state < PLAYER_STATES + 4)
		{
			destroy_ani(player_data.gap[selected_state - PLAYER_STATES]);
			player_data.gap[selected_state - PLAYER_STATES] = load_ani(filename, pal);
			for(i = 0; i < player_data.gap[selected_state - PLAYER_STATES]->f; i++)
			{
				convert_palette(player_data.gap[selected_state - PLAYER_STATES]->image[i], pal, char_palette);
			}
		}
		else if(selected_state < PLAYER_STATES + 5)
		{
			destroy_ani(player_data.pap);
			player_data.pap = load_ani(filename, pal);
			for(i = 0; i < player_data.pap->f; i++)
			{
				convert_palette(player_data.pap->image[i], pal, char_palette);
			}
		}
		else
		{
			destroy_ani(player_data.ppap);
			player_data.ppap = load_ani(filename, pal);
			for(i = 0; i < player_data.ppap->f; i++)
			{
				convert_palette(player_data.ppap->image[i], pal, char_palette);
			}
		}
		ani_sel_bp = NULL;
		if(ani_sel_fbp)
		{
			destroy_bitmap(ani_sel_fbp);
			ani_sel_fbp = NULL;
		}
	}
	logic_frames = 0;
	return 1;
}

int ani_sub_menu_export(void)
{
	if(file_select_ex("Save ANI:", filename, "ANI", 1023, 320, 240))
	{
		replace_extension(filename, filename, "ani", 1023);
		if(selected_state < PLAYER_STATES)
		{
			save_ani(player_data.ap[selected_state], filename, char_palette);
		}
		else if(selected_state < PLAYER_STATES + 4)
		{
			save_ani(player_data.gap[selected_state - PLAYER_STATES], filename, char_palette);
		}
		else if(selected_state < PLAYER_STATES + 5)
		{
			save_ani(player_data.pap, filename, char_palette);
		}
		else
		{
			save_ani(player_data.ppap, filename, char_palette);
		}
	}
	logic_frames = 0;
	return 1;
}

int ani_menu_qo(void)
{
	int i;
	int dstate;

	if(selected_state < PLAYER_STATES)
	{
		need_to_save = 1;
		if(selected_state % 2 == 0)
		{
			dstate = selected_state + 1;
		}
		else
		{
			dstate = selected_state - 1;
		}
		destroy_ani(player_data.ap[dstate]);
		player_data.ap[dstate] = create_ani(player_data.ap[selected_state]->w, player_data.ap[selected_state]->h);
		for(i = 0; i < player_data.ap[selected_state]->f; i++)
		{
			animation_add_frame(player_data.ap[dstate], player_data.ap[selected_state]->image[i]);
			clear_bitmap(player_data.ap[dstate]->image[i]);
			draw_sprite_h_flip(player_data.ap[dstate]->image[i], player_data.ap[selected_state]->image[i], 0, 0);
		}
		player_data.ap[dstate]->d = player_data.ap[selected_state]->d;
	}
	return 1;
}

int ani_menu_fire(void)
{
	int i;
	int dstate;

	if(selected_state < OLD_PLAYER_STATES)
	{
		need_to_save = 1;
		dstate = selected_state + OLD_PLAYER_STATES;
		destroy_ani(player_data.ap[dstate]);
		player_data.ap[dstate] = create_ani(player_data.ap[selected_state]->w, player_data.ap[selected_state]->h);
		for(i = 0; i < player_data.ap[selected_state]->f; i++)
		{
			animation_add_frame(player_data.ap[dstate], player_data.ap[selected_state]->image[i]);
			clear_bitmap(player_data.ap[dstate]->image[i]);
			draw_sprite(player_data.ap[dstate]->image[i], player_data.ap[selected_state]->image[i], 0, 0);
		}
		player_data.ap[dstate]->d = player_data.ap[selected_state]->d;
	}
	return 1;
}

int ani_menu_fire_all(void)
{
	int i, j;
	int dstate;

	for(j = 0; j < OLD_PLAYER_STATES; j++)
	{
		need_to_save = 1;
		dstate = j + OLD_PLAYER_STATES;
		destroy_ani(player_data.ap[dstate]);
		player_data.ap[dstate] = create_ani(player_data.ap[j]->w, player_data.ap[j]->h);
		for(i = 0; i < player_data.ap[j]->f; i++)
		{
			animation_add_frame(player_data.ap[dstate], player_data.ap[j]->image[i]);
			clear_bitmap(player_data.ap[dstate]->image[i]);
			draw_sprite(player_data.ap[dstate]->image[i], player_data.ap[j]->image[i], 0, 0);
		}
		player_data.ap[dstate]->d = player_data.ap[j]->d;
	}
	return 1;
}

int ani_menu_rotate(void)
{
	int sel;
	int is;
	int speed, deg;
	BITMAP * nbp;
	int i;
	
	sel = alert(NULL, "Overwrite current animation?", NULL, "OK", "Cancel", 0, 0);
	if(sel == 2)
	{
		return 1;
	}
	sprintf(wtext, "2");
	sprintf(htext, "256");
	centre_dialog(rotate_dialog);
    is = my_popup_dialog(rotate_dialog, 0);
    if(is == 6)
    {
		need_to_save = 1;
		speed = atoi(wtext);
		if(speed == 0)
		{
			speed = 1;
		}
		deg = atoi(htext);
		if(deg < 0)
		{
			deg = -deg;
		}
//		if(rotate_dialog[3].flags & D_SELECTED)
//		{
//			speed = -speed;
//		}
	    nbp = create_bitmap(ap->w, ap->h);
	    clear_bitmap(nbp);
	    draw_sprite(nbp, ap->image[ani_frame], 0, 0);
	    while(ap->f > 0)
	    {
		    animation_delete_frame(ap, 0);
	    }
	    animation_add_frame(ap, nbp);
    	for(i = speed; i < deg && i > -deg; i += speed)
    	{
	    	animation_add_frame(ap, nbp);
	    	clear_bitmap(ap->image[ap->f - 1]);
	    	rotate_sprite(ap->image[ap->f - 1], ap->image[0], 0, 0, itofix(i));
    	}
		destroy_bitmap(nbp);
    }
	return 1;
}

int frame_menu_insert(void)
{
	need_to_save = 1;
	animation_insert_frame(ap, ani_frame, ap->image[ani_frame]);
	ani_sel_bp = NULL;
	if(ani_sel_fbp)
	{
		destroy_bitmap(ani_sel_fbp);
		ani_sel_fbp = NULL;
	}
	return 1;
}

int frame_menu_add(void)
{
	need_to_save = 1;
	animation_add_frame(ap, ap->image[ani_frame]);
	return 1;
}

int frame_menu_delete(void)
{
	need_to_save = 1;
	animation_delete_frame(ap, ani_frame);
	ani_sel_bp = NULL;
	if(ani_sel_fbp)
	{
		destroy_bitmap(ani_sel_fbp);
		ani_sel_fbp = NULL;
	}
	return 1;
}

int frame_menu_import(void)
{
	BITMAP * bp;
	BITMAP * nbp;
	PALETTE pal;

	if(file_select_ex("Import:", filename, "bmp;pcx;tga;lbm", 1023, 320, 240))
	{
		need_to_save = 1;
		bp = load_bitmap(filename, pal);
		convert_palette(bp, pal, char_palette);
		if(bp->w > ap->w || bp->h > ap->h)
		{
			grab_frame(bp, 0);
//			animation_replace_frame(ap, ani_frame, nbp);
//			destroy_bitmap(nbp);
		}
		else
		{
			animation_replace_frame(ap, ani_frame, bp);
		}
		destroy_bitmap(bp);
		ani_sel_bp = NULL;
		if(ani_sel_fbp)
		{
			destroy_bitmap(ani_sel_fbp);
			ani_sel_fbp = NULL;
		}
	}
	logic_frames = 0;
	return 1;
}

int frame_menu_import_sequence(void)
{
	BITMAP * ibp;
	BITMAP * nbp;
	ANIMATION * nap;
	PALETTE pal;
	int sel, i;

	sel = alert(NULL, "Overwrite current animation?", NULL, "OK", "Cancel", 0, 0);
	if(sel == 2)
	{
		return 1;
	}
	
	/* select the file */
	if(file_select_ex("Import:", filename, "bmp;pcx;tga;lbm", 1023, 320, 240))
	{
		
		/* keep an extra copy of animation in case user changes mind */
		nap = duplicate_ani(ap);
	
		/* erase the old animation */
		for(i = 0; i < ap->f; i++)
		{
			destroy_bitmap(ap->image[i]);
		}
		ap->f = 0;
		ap->d = 0;
	
		need_to_save = 1;
		ibp = load_bitmap(filename, pal);
		convert_palette(ibp, pal, char_palette);
		
		if(ibp->w > ap->w || ibp->h > ap->h)
		{
			/* go to grab screen for large image */
			grab_frame(ibp, 1);
			
			/* if user doesn't select any frames, keep old animation */
			if(ap->f <= 0)
			{
				destroy_ani(ap);
				ap = duplicate_ani(nap);
				destroy_ani(nap);
			}
			else
			{
				destroy_ani(nap);
			}
		}
		else
		{
			animation_replace_frame(ap, ani_frame, ibp);
		}
		destroy_bitmap(ibp);
		ani_sel_bp = NULL;
		if(ani_sel_fbp)
		{
			destroy_bitmap(ani_sel_fbp);
			ani_sel_fbp = NULL;
		}
	}
	logic_frames = 0;
	return 1;
}

int frame_menu_export(void)
{
	if(file_select_ex("Export:", filename, "pcx", 1023, 320, 240))
	{
		replace_extension(filename, filename, "pcx", 1023);
		save_pcx(filename, ap->image[ani_frame], char_palette);
	}
	logic_frames = 0;
	return 1;
}

int frame_menu_clear(void)
{
	need_to_save = 1;
	clear_bitmap(ap->image[ani_frame]);
	ani_sel_bp = NULL;
	if(ani_sel_fbp)
	{
		destroy_bitmap(ani_sel_fbp);
		ani_sel_fbp = NULL;
	}
	return 1;
}

int speed_menu_decrease(void)
{
	need_to_save = 1;
	ap->d++;
	return 1;
}

int speed_menu_increase(void)
{
	if(ap->d > 0)
	{
		need_to_save = 1;
		ap->d--;
	}
	return 1;
}

int menu_nop(void)
{
	return 1;
}

int item_menu_pf(void)
{
	int is;

	sprintf(wtext, "%i", player_data.pf);
//	sprintf(wtext, "test");
	centre_dialog(pf_dialog);
    is = my_popup_dialog(pf_dialog, 0);
    if(is == 4)
    {
		need_to_save = 1;
	    player_data.pf = atoi(wtext);
    }
	return 1;
}

int item_menu_cmap(void)
{
	item = ITEM_CMAP;
	check_item(item);
	peg_item();
	return 1;
}

int item_menu_flash(void)
{
	if(selected_state >= OLD_PLAYER_STATES)
	{
		item = ITEM_GUN;
		check_item(item);
		peg_item();
	}
	return 1;
}

int item_menu_it(void)
{
	item = ITEM_IT;
	check_item(item);
	peg_item();
	return 1;
}

int item_menu_arrow(void)
{
	item = ITEM_CENTER;
	check_item(item);
	peg_item();
	return 1;
}

int item_menu_paintball(void)
{
	if(selected_state >= OLD_PLAYER_STATES)
	{
		item = ITEM_PAINT;
		check_item(item);
		peg_item();
	}
	return 1;
}

int item_menu_lock(void)
{
	cmap_locked = 1 - cmap_locked;
	if(cmap_locked)
	{
		item_menu[7].flags = D_SELECTED;
	}
	else
	{
		item_menu[7].flags = 0;
	}
	return 1;
}

int frame_shift_left(void)
{
	BITMAP * scratch;

	need_to_save = 1;
	scratch = create_bitmap(bp->w, bp->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, bp, 0, 0);
	clear_bitmap(bp);
	draw_sprite(bp, scratch, -1, 0);
	draw_sprite(bp, scratch, scratch->w - 1, 0);
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int frame_shift_right(void)
{
	BITMAP * scratch;

	need_to_save = 1;
	scratch = create_bitmap(bp->w, bp->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, bp, 0, 0);
	clear_bitmap(bp);
	draw_sprite(bp, scratch, 1, 0);
	draw_sprite(bp, scratch, 1 - scratch->w, 0);
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int frame_shift_up(void)
{
	BITMAP * scratch;

	need_to_save = 1;
	scratch = create_bitmap(bp->w, bp->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, bp, 0, 0);
	clear_bitmap(bp);
	draw_sprite(bp, scratch, 0, -1);
	draw_sprite(bp, scratch, 0, scratch->h - 1);
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int frame_shift_down(void)
{
	BITMAP * scratch;

	need_to_save = 1;
	scratch = create_bitmap(bp->w, bp->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, bp, 0, 0);
	clear_bitmap(bp);
	draw_sprite(bp, scratch, 0, 1);
	draw_sprite(bp, scratch, 0, 1 - scratch->h);
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int ani_shift_left(void)
{
	BITMAP * scratch;
	int i;

	need_to_save = 1;
	scratch = create_bitmap(ap->w, ap->h);
	for(i = 0; i < ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, ap->image[i], 0, 0);
		clear_bitmap(ap->image[i]);
		draw_sprite(ap->image[i], scratch, -1, 0);
		draw_sprite(ap->image[i], scratch, scratch->w - 1, 0);
	}
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int ani_shift_right(void)
{
	BITMAP * scratch;
	int i;

	need_to_save = 1;
	scratch = create_bitmap(ap->w, ap->h);
	for(i = 0; i < ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, ap->image[i], 0, 0);
		clear_bitmap(ap->image[i]);
		draw_sprite(ap->image[i], scratch, 1, 0);
		draw_sprite(ap->image[i], scratch, 1 - scratch->w, 0);
	}
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int ani_shift_up(void)
{
	BITMAP * scratch;
	int i;

	need_to_save = 1;
	scratch = create_bitmap(ap->w, ap->h);
	for(i = 0; i < ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, ap->image[i], 0, 0);
		clear_bitmap(ap->image[i]);
		draw_sprite(ap->image[i], scratch, 0, -1);
		draw_sprite(ap->image[i], scratch, 0, scratch->h - 1);
	}
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int ani_shift_down(void)
{
	BITMAP * scratch;
	int i;

	need_to_save = 1;
	scratch = create_bitmap(ap->w, ap->h);
	for(i = 0; i < ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, ap->image[i], 0, 0);
		clear_bitmap(ap->image[i]);
		draw_sprite(ap->image[i], scratch, 0, 1);
		draw_sprite(ap->image[i], scratch, 0, 1 - scratch->h);
	}
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

SAMPLE * downsample(SAMPLE * sp)
{
	int i;
	SAMPLE * ret;
	
	ret = create_sample(8, 0, sp->freq, sp->len);
	for(i = 0; i < ret->len; i++)
	{
		((unsigned char *)(ret->data))[i] = ((unsigned short *)(sp->data))[i] / 256;
	}
	return ret;
}

int sound_menu_patch(void)
{
	SAMPLE * temp;
	
	if(file_select_ex("Load WAV:", nfilename, "wav", 1023, 320, 240))
	{
		temp = load_wav(nfilename);
/*		if(temp->stereo)
		{
			destroy_sample(temp);
			clear_keybuf();
			key[KEY_ENTER] = 0;
			alert(NULL, "Stereo samples not supported!", NULL, "OK", NULL, 0, 0);
		}
		if(temp->bits == 16)
		{
			clear_keybuf();
			key[KEY_ENTER] = 0;
			alert(NULL, "Converted to 8-bit!", NULL, "OK", NULL, 0, 0);
			if(player_data.sound[sound_order[selected_sound]] != sound[sound_order[selected_sound]])
			{
				destroy_sample(player_data.sound[sound_order[selected_sound]]);
			}
			player_data.sound[sound_order[selected_sound]] = downsample(temp);
			destroy_sample(temp);
			need_to_save = 1;
		}
		else
		{ */
			if(player_data.sound[sound_order[selected_sound]] != sound[sound_order[selected_sound]])
			{
				destroy_sample(player_data.sound[sound_order[selected_sound]]);
			}
			player_data.sound[sound_order[selected_sound]] = temp;
			need_to_save = 1;
//		}
	}
	key[KEY_ENTER] = 0;
	logic_frames = 0;
	return 1;
}

int sound_menu_unpatch(void)
{
	player_data.sound[sound_order[selected_sound]] = sound[sound_order[selected_sound]];
	return 1;
}

int sound_menu_blank(void)
{
	player_data.sound[sound_order[selected_sound]] = ncds_internal_sample;
	return 1;
}

int sound_menu_export(void)
{
	if(file_select_ex("Export WAV:", nfilename, "wav", 1023, 320, 240))
	{
		replace_extension(nfilename, nfilename, "wav", 1023);
		ncds_save_wav(player_data.sound[sound_order[selected_sound]], nfilename);
	}
	key[KEY_ENTER] = 0;
	logic_frames = 0;
	return 1;
}

int sound_menu_listen(void)
{
	if(player_data.sound[sound_order[selected_sound]])
	{
		play_sample(player_data.sound[sound_order[selected_sound]], 255, 128, 1000, 0);
	}
	return 1;
}
