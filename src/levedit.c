#include <allegro.h>
#include "tilemap.h"
#include "level.h"
#include "objects.h"
#include "levedit.h"
#include "itools.h"
#include "mygui.h"

#define PAL_S (6)
#define PAL_X (320 - 16 * PAL_S)
#define PAL_Y (240 - 16 * PAL_S)
#define PAL_W (PAL_S * 16)
#define PAL_H (PAL_S * 16)

/* preview data */
BITMAP * preview;
BITMAP * layer[16];
ANIMATION * ap;
BITMAP * bp;
int pw, ph;

int ani_tool = TOOL_PIXEL;
BITMAP * ani_sel_bp = NULL;
BITMAP * ani_sel_fbp = NULL;
BITMAP * ani_sel_cbp = NULL;
int ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, ani_sel_float = 0, ani_sel_v = 1;
BITMAP * ani_tool_image[9] = {NULL};

char *      level_help_text;
LEVEL * 	level = NULL;
BITMAP * 	level_screen;
BITMAP *    view_screen;
BITMAP *    layer_screen;
BITMAP *    tile_screen;
BITMAP *    level_clipboard;
PALETTE 	level_palette;
COLOR_MAP   trans_colors;
COLOR_MAP   light_colors;
char 		level_filename[256] = {0};
OBJECT      object[PP_MAX_OBJECTS];
ANIMATION * object_ani[256] = {NULL};
BITMAP * tile_list[TILEMAP_MAX_TILES] = {NULL};
int      tile_good[TILEMAP_MAX_TILES] = {0};
char wtext[256] = {0};
char htext[256] = {0};
char ltext[256] = {0};
char etext[16][256] = {0};

int selected_object = 0;
int selected_type = 0;
int selected_tile = 0;
int selected_layer = 0;
int selected_color = 0;
int selected_frame = 0;
int tile_scale = 8;
int mouse_selected = -1;
int mouse_stop = 0;
int frame = 0;
int sticky = 0;
int pox, poy, pegged = -1;
int grid_x, grid_y;
int level_quit = 0;
int mouse_hidden = 0;
int need_to_save = 0;
int my_mouse_x, my_mouse_y, my_mouse_b;
int logic_frames = 0;

#define L_VIEWS       3
#define L_VIEW_OBJECT 0
#define L_VIEW_MAP    1
#define L_VIEW_TILES  2
int level_view = L_VIEW_OBJECT;
int tool_map[3][3] = {0, 1, 6, 2, 3, 7, 4, 5, 8};

char * object_name[256] =
{
    "Player Portal",
    "",
    "Normal Ammo",
    "Splitter Ammo",
    "Mine Ammo",
    "Bouncer Ammo",
    "Seeker Ammo",
    "Cloak",
    "Jump",
    "Run",
    "Deflect",
    "",
    "Flag Portal",
    "",
    "Base Portal",
    "",
    "",
    "",
    "",
    "",
    "",
    "Gem Portal",
    "Gem Base",
    "",
    "Target Portal",
    "",
    "",
    "",
    "Fly",
    "",
    "Spring Up",
    "Spring Down",
    "Spring Left",
    "Spring Right",
    "",
    "",
    "Turbo",
    "",
    "",
    "",
    ""
};

MENU export_menu[] =
{
    {"Tileset", export_menu_tileset, NULL, 0, NULL},
    {"Tilemap", export_menu_tilemap, NULL, 0, NULL},
    {"Object Map", export_menu_objectmap, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU import_menu[] =
{
    {"Tileset", import_menu_tileset, NULL, 0, NULL},
    {"Tilemap", import_menu_tilemap, NULL, 0, NULL},
    {"Object Map", import_menu_objectmap, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU file_menu[] =
{
    {"New\tF4", file_menu_new, NULL, 0, NULL},
    {"Open\tF3", file_menu_load, NULL, 0, NULL},
    {"Save\tF2", file_menu_save, NULL, D_DISABLED, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Import", NULL, import_menu, D_DISABLED, NULL},
    {"Export", NULL, export_menu, D_DISABLED, NULL},
    {"", NULL, NULL, 0, NULL},
	{"Exit\tEsc", file_menu_exit, NULL, 0, NULL},
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

MENU view_menu[] =
{
    {"Object Map", view_menu_objectmap, NULL, D_SELECTED, NULL},
    {"Tilemap", view_menu_tilemap, NULL, 0, NULL},
    {"Tileset", view_menu_tileset, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU object_sub_menu[] =
{
    {"Next\t>", object_menu_next, NULL, 0, NULL},
    {"Previous\t<", object_menu_previous, NULL, 0, NULL},
    {"Delete\tDEL", object_menu_delete, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU layer_select_menu[] =
{
    {"1", layer_select_1, NULL, D_SELECTED, NULL},
    {"2", layer_select_2, NULL, 0, NULL},
    {"3", layer_select_3, NULL, 0, NULL},
    {"4", layer_select_4, NULL, 0, NULL},
    {"5", layer_select_5, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU layer_shift_menu[] =
{
    {"Left\tA", layer_shift_left, NULL, 0, NULL},
    {"Right\tD", layer_shift_right, NULL, 0, NULL},
    {"Up\tW", layer_shift_up, NULL, 0, NULL},
    {"Down\tS", layer_shift_down, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU layer_menu[] =
{
    {"Select", NULL, layer_select_menu, 0, NULL},
    {"Shift", NULL, layer_shift_menu, 0, NULL},
    {"Properties", layer_menu_properties, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU map_menu[] =
{
    {"Layer", NULL, layer_menu, 0, NULL},
    {"Properties\tF9", map_menu_properties, NULL, 0, NULL},
    {"Resize", map_menu_resize, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU object_menu[] =
{
    {"File", NULL, file_menu, 0, NULL},
    {"View", NULL, view_menu, D_DISABLED, NULL},
    {"Object", NULL, object_sub_menu, D_DISABLED, NULL},
    {"X", menu_nop, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU tilemap_menu[] =
{
    {"File", NULL, file_menu, 0, NULL},
    {"View", NULL, view_menu, 0, NULL},
    {"Map", NULL, map_menu, 0, NULL},
    {"X", menu_nop, NULL, 0, NULL},
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
    {"Insert", frame_menu_insert, NULL, 0, NULL},
    {"Add", frame_menu_add, NULL, 0, NULL},
    {"Delete", frame_menu_delete, NULL, 0, NULL},
    {"Import", frame_menu_import, NULL, 0, NULL},
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
    {"Left\tSh+A", ani_shift_left, NULL, 0, NULL},
    {"Right\tSh+D", ani_shift_right, NULL, 0, NULL},
    {"Up\tSh+W", ani_shift_up, NULL, 0, NULL},
    {"Down\tSh+S", ani_shift_down, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU tile_menu[] =
{
    {"Import", ani_sub_menu_import, NULL, 0, NULL},
    {"Replace", ani_sub_menu_replace, NULL, 0, NULL},
    {"Export", ani_sub_menu_export, NULL, 0, NULL},
    {"Insert", ani_sub_menu_insert, NULL, 0, NULL},
    {"Delete", ani_sub_menu_delete, NULL, 0, NULL},
    {"Frame", NULL, frame_menu, 0, NULL},
    {"Speed", NULL, speed_menu, 0, NULL},
    {"Shift", NULL, ani_shift_menu, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Properties\tF9", tile_menu_properties, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU tileset_menu[] =
{
    {"File", NULL, file_menu, 0, NULL},
    {"Edit", NULL, edit_menu, 0, NULL},
    {"View", NULL, view_menu, 0, NULL},
    {"Tile", NULL, tile_menu, 0, NULL},
    {"X", menu_nop, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

DIALOG help_dialog[] = 
{
   { d_textbox_proc, 0, 0, 640, 480, 8, 31, 0, 0, 0, 0, NULL, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG layer_properties_dialog[] =
{
   /* (proc)        (x)  (y)  (w)  (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)                (dp2) (dp3) */
   { ppgui_shadow_box_proc,    28,  48,  160, 96, 2,   23,  0,    0,      0,   0,   NULL,               NULL, NULL },
   { d_text_proc,   44,  56,  128, 8,  2,   23,  0,    0,      0,   0,   "Layer Properties", NULL, NULL },
   { d_text_proc,   48,  80,  64,  8,  2,   23,  0,    0,      0,   0,   "X Speed:",         NULL, NULL },
   { ppgui_edit_proc,   132, 80,  40,  8,  2,   23,  0,    0,      4,   0,   etext[0],           NULL, NULL },
   { d_text_proc,   48,  96,  64,  8,  2,   23,  0,    0,      0,   0,   "Y Speed:",         NULL, NULL },
   { ppgui_edit_proc,   132, 96,  40,  8,  2,   23,  0,    0,      4,   0,   etext[1],           NULL, NULL },
   { ppgui_button_proc, 36,  120, 68,  16, 2,   23,  0,    D_EXIT, 0,   0,   "OK",               NULL, NULL },
   { ppgui_button_proc, 112, 120, 68,  16, 2,   23,  0,    D_EXIT, 0,   0,   "Cancel",           NULL, NULL },
   { NULL,          0,   0,   0,   0,  2,   23,  0,    0,      0,   0,   NULL,               NULL, NULL }
};

DIALOG map_properties_dialog[] =
{
   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                 (dp2) (dp3) */
   { ppgui_shadow_box_proc,    64,  48,  196, 176, 2,   23,  0,    0,      0,   0,   NULL,                NULL, NULL },
   { d_text_proc,   108, 56,  112, 8,   2,   23,  0,    0,      0,   0,   "Map Properties",    NULL, NULL },
   { d_text_proc,   72,  160, 208, 8,   2,   23,  0,    0,      0,   0,   "XMin:",             NULL, NULL },
   { ppgui_edit_proc,   120, 160, 40,  8,   2,   23,  0,    0,      4,   0,   etext[0],            NULL, NULL },
   { d_text_proc,   164, 160, 48,  8,   2,   23,  0,    0,      0,   0,   "XMax:",             NULL, NULL },
   { ppgui_edit_proc,   212, 160, 40,  8,   2,   23,  0,    0,      4,   0,   etext[1],            NULL, NULL },
   { d_text_proc,   72,  176, 48,  8,   2,   23,  0,    0,      0,   0,   "YMin:",             NULL, NULL },
   { d_text_proc,   164, 176, 48,  8,   2,   23,  0,    0,      0,   0,   "YMax:",             NULL, NULL },
   { ppgui_edit_proc,   120, 176, 40,  8,   2,   23,  0,    0,      4,   0,   etext[2],            NULL, NULL },
   { ppgui_edit_proc,   212, 176, 40,  8,   2,   23,  0,    0,      4,   0,   etext[3],            NULL, NULL },
   { d_text_proc,   140, 80,  48,  8,   2,   23,  0,    0,      0,   0,   "ILayer",            NULL, NULL },
   { ppgui_list_proc,   120, 96,  88,  48,  2,   23,  0,    0,      0,   0,   layer_list_get_text, NULL, NULL },
   { ppgui_button_proc, 96,  200, 64,  16,  2,   23,  0,    D_EXIT, 0,   0,   "OK",                NULL, NULL },
   { ppgui_button_proc, 168, 200, 64,  16,  2,   23,  0,    D_EXIT, 0,   0,   "Cancel",            NULL, NULL },
   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                NULL, NULL }
};

DIALOG tile_properties_dialog[] =
{
   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)               (dp2) (dp3) */
   { ppgui_shadow_box_proc,    56,  64,  168, 240, 2,   23,  0,    0,      0,   0,   NULL,              NULL, NULL },
   { d_text_proc,   80,  72,  120, 8,   2,   23,  0,    0,      0,   0,   "Tile Properties", NULL, NULL },
   { ppgui_check_proc,  84,  96,  88,  8,   2,   23,  0,    0,      1,   0,   "Solid Top",       NULL, NULL },
   { ppgui_check_proc,  84,  112, 112, 8,   2,   23,  0,    0,      1,   0,   "Solid Bottom",    NULL, NULL },
   { ppgui_check_proc,  84,  128, 96,  8,   2,   23,  0,    0,      1,   0,   "Solid Left",      NULL, NULL },
   { ppgui_check_proc,  84,  144, 104, 8,   2,   23,  0,    0,      1,   0,   "Solid Right",     NULL, NULL },
   { ppgui_check_proc,  84,  160, 112, 8,   2,   23,  0,    0,      1,   0,   "Translucent",     NULL, NULL },
   { ppgui_check_proc,  84,  176, 112, 8,   2,   23,  0,    0,      1,   0,   "Slick",           NULL, NULL },
   
   { ppgui_check_proc,  84,  192, 112, 8,   2,   23,  0,    0,      1,   0,   "Light",           NULL, NULL },
   
   { d_text_proc,   88,  216, 88,  8,   2,   23,  0,    0,      0,   0,   "Convey: ",        NULL, NULL },
   { ppgui_edit_proc,   152, 216, 48,  8,   2,   23,  0,    0,      5,   0,   etext[0],          NULL, NULL },
   { d_text_proc,   64,  232, 88,  8,   2,   23,  0,    0,      0,   0,   "Change To: ",     NULL, NULL },
   { ppgui_edit_proc,   152, 232, 32,  8,   2,   23,  0,    0,      3,   0,   etext[1],          NULL, NULL },
   { d_text_proc,   96,  248, 72,  8,   2,   23,  0,    0,      0,   0,   "Delay:",          NULL, NULL },
   { ppgui_edit_proc,   152, 248, 32,  8,   2,   23,  0,    0,      3,   0,   etext[2],          NULL, NULL },
   { ppgui_button_proc, 64,  280, 72,  16,  2,   23,  0,    D_EXIT, 0,   0,   "OK",              NULL, NULL },
   { ppgui_button_proc, 144, 280, 72,  16,  2,   23,  0,    D_EXIT, 0,   0,   "Cancel",          NULL, NULL },
   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,              NULL, NULL }
};

DIALOG object_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_menu_proc, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, object_menu, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG tilemap_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_menu_proc, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, tilemap_menu, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG tileset_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_menu_proc, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, tileset_menu, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG new_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { ppgui_shadow_box_proc, 80, 60, 160, 80, 2, 23, 0, 0, 0, 0, NULL, NULL, NULL },
   { d_ctext_proc, 108, 68, 104, 8, 2, 23, 0, 0, 0, 0, "New Level", NULL, NULL },
   { d_text_proc, 88, 84, 64, 8, 2, 23, 0, 0, 0, 0, "Width :", NULL, NULL },
   { d_text_proc, 88, 100, 64, 8, 2, 23, 0, 0, 0, 0, "Height:", NULL, NULL },
   { ppgui_edit_proc, 150, 84, 32, 8, 2, 23, 0, 0, 3, 0, wtext, NULL, NULL },
   { ppgui_edit_proc, 150, 100, 32, 8, 2, 23, 0, 0, 3, 0, htext, NULL, NULL },
   { ppgui_button_proc, 92, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "OK", NULL, NULL },
   { ppgui_button_proc, 164, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "Cancel", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG resize_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { ppgui_shadow_box_proc, 80, 60, 160, 80, 2, 23, 0, 0, 0, 0, NULL, NULL, NULL },
   { d_ctext_proc, 108, 68, 104, 8, 2, 23, 0, 0, 0, 0, "Resize Level", NULL, NULL },
   { d_text_proc, 88, 84, 64, 8, 2, 23, 0, 0, 0, 0, "Width :", NULL, NULL },
   { d_text_proc, 88, 100, 64, 8, 2, 23, 0, 0, 0, 0, "Height:", NULL, NULL },
   { ppgui_edit_proc, 150, 84, 32, 8, 2, 23, 0, 0, 3, 0, wtext, NULL, NULL },
   { ppgui_edit_proc, 150, 100, 32, 8, 2, 23, 0, 0, 3, 0, htext, NULL, NULL },
   { ppgui_button_proc, 92, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "OK", NULL, NULL },
   { ppgui_button_proc, 164, 116, 64, 16, 2, 23, 0, D_EXIT, 0, 0, "Cancel", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

char * layer_list_text[5] =
{
	"Layer 1",
	"Layer 2",
	"Layer 3",
	"Layer 4",
	"Layer 5"
};

char * layer_list_get_text(int index, int * size)
{
	if(index >= 0 && index < 5)
	{
		return layer_list_text[index];
	}
	if(index < 0)
	{
		*size = 5;
	}
	return NULL;
}

void logic_timer(void)
{
	logic_frames++;
}

void lighting_blender(PALETTE pal, int x, int y, RGB *rgb)
{
	int light;
	fixed light_level;
	fixed col_level;
	int final;
	fixed left;
	
	/* calculate the total luminence of the color */
	light = (pal[x].r + pal[x].g + pal[x].b) / 3;
	
	/* do shadow blend */
	if(light == 0)
	{
		rgb->r = 0;
		rgb->g = 0;
		rgb->b = 0;
	}
	else if(light < 32)
	{
		rgb->r = fixtoi(fmul(itofix(pal[y].r), fdiv(itofix(light * 2 + 1), itofix(63))));
		rgb->g = fixtoi(fmul(itofix(pal[y].g), fdiv(itofix(light * 2 + 1), itofix(63))));
		rgb->b = fixtoi(fmul(itofix(pal[y].b), fdiv(itofix(light * 2 + 1), itofix(63))));
	}
	
	/* do light blend */
	else if(light == 32)
	{
		rgb->r = pal[y].r;
		rgb->g = pal[y].g;
		rgb->b = pal[y].b;
	}
	else
	{
		light_level = fdiv(itofix((light - 32) * 2 + 1), itofix(63));
		left = itofix(63 - pal[y].r);
		final = pal[y].r + fixtoi(fmul(left, light_level));
		rgb->r = final;
		left = itofix(63 - pal[y].g);
		final = pal[y].g + fixtoi(fmul(left, light_level));
		rgb->g = final;
		left = itofix(63 - pal[y].b);
		final = pal[y].b + fixtoi(fmul(left, light_level));
		rgb->b = final;
	}
}

int my_popup_dialog(DIALOG * dp, int n)
{
	int ret;
	
   	ret = popup_dialog(dp, n);
   	logic_frames = 0;
   	
   	return ret;
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
	if(level_clipboard)
	{
		destroy_bitmap(level_clipboard);
		level_clipboard = NULL;
	}
	ani_sel_float = 0;
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

void set_menu_frame(void)
{
	if(ap)
	{
		if(ap->f > 1)
		{
			frame_menu[2].flags = 0;
		}
		else
		{
			frame_menu[2].flags = D_DISABLED;
		}
	}
}

void set_menu_edit(void)
{
	if(!ani_sel_cbp)
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

void read_mouse(void)
{
	my_mouse_x = mouse_x;
	my_mouse_y = mouse_y;
	my_mouse_b = mouse_b;
}

void tool_select(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int * ex, * ey;
	ani_sel_bp = bp;
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
//	    fix_select();
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
    if(level_clipboard)
    {
	    destroy_bitmap(level_clipboard);
	    level_clipboard = NULL;
    }
    if(ani_sel_fbp)
    {
	    destroy_bitmap(ani_sel_fbp);
	    ani_sel_fbp = NULL;
    }
    level_clipboard = create_bitmap(ani_sel_x2 - ani_sel_x + 1, ani_sel_y2 - ani_sel_y + 1);
    clear_bitmap(level_clipboard);
    ani_sel_fbp = create_bitmap(ani_sel_bp->w, ani_sel_bp->h);
    clear_bitmap(ani_sel_fbp);
    draw_sprite(level_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
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
    draw_sprite(ani_sel_cbp, level_clipboard, 0, 0);
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
    draw_sprite(ani_sel_cbp, level_clipboard, 0, 0);
    set_menu_edit();
    return 1;
}

int tool_paste(void)
{
	if(level_clipboard)
	{
		destroy_bitmap(level_clipboard);
		level_clipboard = NULL;
	}
	if(ani_sel_fbp)
	{
		destroy_bitmap(ani_sel_fbp);
		ani_sel_fbp = NULL;
	}
	ani_sel_bp = bp;
	level_clipboard = create_bitmap(ani_sel_cbp->w, ani_sel_cbp->h);
	clear_bitmap(level_clipboard);
	draw_sprite(level_clipboard, ani_sel_cbp, 0, 0);
	ani_sel_fbp = create_bitmap(ani_sel_bp->w, ani_sel_bp->h);
	clear_bitmap(ani_sel_fbp);
	draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
	draw_sprite(ani_sel_bp, ani_sel_cbp, 0, 0);
	ani_sel_x = 0;
	ani_sel_y = 0;
	ani_sel_x2 = ani_sel_cbp->w - 1;
	ani_sel_y2 = ani_sel_cbp->h - 1;
	ani_sel_float = 1;
    ani_tool = TOOL_SELECT;
    set_menu_edit();
	return 1;
}

int tool_mirror(void)
{
   	rectfill(ani_sel_bp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
    clear_bitmap(ani_sel_fbp);
    draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
	clear_bitmap(ani_sel_bp);
	draw_sprite(ani_sel_bp, ani_sel_fbp, 0, 0);
    draw_sprite_h_flip(ani_sel_bp, level_clipboard, ani_sel_x, ani_sel_y);
    clear_bitmap(level_clipboard);
    draw_sprite(level_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
    return 1;
}

int tool_flip(void)
{
   	rectfill(ani_sel_bp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
    clear_bitmap(ani_sel_fbp);
    draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
	clear_bitmap(ani_sel_bp);
	draw_sprite(ani_sel_bp, ani_sel_fbp, 0, 0);
    draw_sprite_v_flip(ani_sel_bp, level_clipboard, ani_sel_x, ani_sel_y);
    clear_bitmap(level_clipboard);
    draw_sprite(level_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
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
		level_clipboard = turn_bitmap(level_clipboard);
		draw_sprite(ani_sel_bp, level_clipboard, ani_sel_x, ani_sel_y);
//    rotate_sprite(ani_sel_bp, level_clipboard, ani_sel_x, ani_sel_y, itofix(64));
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
//    destroy_bitmap(level_clipboard);
//    level_clipboard = create_bitmap(ani_sel_x2 - ani_sel_x + 1, ani_sel_y2 - ani_sel_y + 1);
//    clear_bitmap(level_clipboard);
//    draw_sprite(level_clipboard, ani_sel_bp, -ani_sel_x, -ani_sel_y);
    return 1;
}

void tool_move(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int px = (my_mouse_x - dx) / s;
    int py = (my_mouse_y - dy) / s;
    int nx, ny;
    int ax = ani_sel_x;
    int ay = ani_sel_y;
    BITMAP * cbp = create_bitmap(bp->w, bp->h);

	if(!ani_sel_float)
	{
    	rectfill(bp, ani_sel_x, ani_sel_y, ani_sel_x2, ani_sel_y2, 0);
	    clear_bitmap(ani_sel_fbp);
	    draw_sprite(ani_sel_fbp, ani_sel_bp, 0, 0);
    	ani_sel_float = 1;
    }
    else
    {
	    clear_bitmap(bp);
	    draw_sprite(bp, ani_sel_fbp, 0, 0);
    }
    blit(bp, cbp, 0, 0, 0, 0, bp->w, bp->h);
    ani_sel_v = 0;
    while(my_mouse_b & 1)
    {
	    read_mouse();
//	    nx = ((my_mouse_x - dx) / s - (px - ax)) % bp->w;
//	    ny = ((my_mouse_y - dy) / s - (py - ay)) % bp->h;
	    nx = ((my_mouse_x - dx) / s - (px - ax));
	    ny = ((my_mouse_y - dy) / s - (py - ay));
	    ani_sel_x2 -= ani_sel_x - nx;
	    ani_sel_y2 -= ani_sel_y - ny;
	    ani_sel_x = nx;
	    ani_sel_y = ny;
        blit(cbp, bp, 0, 0, 0, 0, bp->w, bp->h);
        draw_sprite(bp, level_clipboard, nx, ny);
//        draw_sprite(bp, level_clipboard, nx - bp->w, ny);
//        draw_sprite(bp, level_clipboard, nx + bp->w, ny);
//        draw_sprite(bp, level_clipboard, nx, ny - bp->h);
//        draw_sprite(bp, level_clipboard, nx, ny + bp->h);
        proc();
    }
    destroy_bitmap(cbp);
    ani_sel_v = 1;
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

void insert_tile(int pos, int num)
{
	int i, j;

	if(pos < 0)
	{
		pos = 0;
	}
	if(pos + num >= TILEMAP_MAX_TILES)
	{
		num = TILEMAP_MAX_TILES - pos;
	}
	for(i = TILEMAP_MAX_TILES - 1; i >= pos + num; i--)
	{
		copy_tile(level->tileset->tile[i - num], level->tileset->tile[i]);
		for(j = 0; j < level->tilemap->l; j++)
		{
			if(i - num > 0 || j == 0)
			{
				tilemap_change_tile(level->tilemap, j, i - num, i);
			}
		}
		if(level->tileset->tile[i]->flag[TILEMAP_FLAG_NEXTFRAME] >= pos)
		{
			level->tileset->tile[i]->flag[TILEMAP_FLAG_NEXTFRAME]++;
		}
	}
	for(i = pos; i < pos + num; i++)
	{
		clear_tile(level->tileset->tile[i]);
	}
	ani_sel_bp = NULL;
}

void delete_tile(int pos)
{
	int i, j;

//	destroy_tile(level->tileset->tile[TILEMAP_MAX_TILES - 1]);
	for(j = 0; j < level->tilemap->l; j++)
	{
		tilemap_change_tile(level->tilemap, j, pos, 0);
	}
	for(i = pos; i < TILEMAP_MAX_TILES - 1; i++)
	{
		copy_tile(level->tileset->tile[i + 1], level->tileset->tile[i]);
		for(j = 0; j < level->tilemap->l; j++)
		{
			tilemap_change_tile(level->tilemap, j, pos, 0);
			if(i > 1 || j == 0)
			{
				tilemap_change_tile(level->tilemap, j, i + 1, i);
			}
		}
		if(level->tileset->tile[i]->flag[TILEMAP_FLAG_NEXTFRAME] > pos)
		{
			level->tileset->tile[i]->flag[TILEMAP_FLAG_NEXTFRAME]--;
		}
	}
	clear_tile(level->tileset->tile[TILEMAP_MAX_TILES - 1]);
	ani_sel_bp = NULL;
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
	putpixel(bp, x + selected_color % 16 * s + 1, y + selected_color / 16 * s + 1, makecol(0, 0, 0));
	putpixel(bp, x + selected_color % 16 * s + 1, y + selected_color / 16 * s + 2, makecol(255, 255, 255));
}

void level_init(void)
{
    allegro_init();
    set_window_title("Paintball Party Level Builder");
//	set_window_close_button(0);
	set_display_switch_mode(SWITCH_BACKGROUND);
    set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
    mygui_init();
    install_keyboard();
    install_mouse();
    install_timer();
    level_screen = create_bitmap(640, 480);
    view_screen = create_sub_bitmap(level_screen, 0, 0, 320, 240);
    layer_screen = create_sub_bitmap(level_screen, 320, 240, 320, 240);
    tile_screen = create_sub_bitmap(level_screen, 320, 0, 320, 240);
    destroy_bitmap(load_pcx("pp.dat#images/title.pcx", level_palette));
    create_trans_table(&trans_colors, level_palette, 128, 128, 128, NULL);
	create_color_table(&light_colors, level_palette, (void *)lighting_blender, NULL);
	tilemap_trans_color_map = &trans_colors;
	tilemap_light_map = &light_colors;
	color_map = &trans_colors;
    set_palette(level_palette);
	gui_bg_color = 23;
	gui_mg_color = 12;
	gui_fg_color = 2;
//	gui_fg_color = makecol(64, 64, 64);
//	gui_mg_color = makecol(128, 128, 128);
//	gui_bg_color = makecol(255, 255, 255);
	set_mouse_sprite(NULL);
	ani_tool_image[0] = load_pcx("chared.dat#tool0.pcx", NULL);
	ani_tool_image[1] = load_pcx("chared.dat#tool1.pcx", NULL);
	ani_tool_image[2] = load_pcx("chared.dat#tool2.pcx", NULL);
	ani_tool_image[3] = load_pcx("chared.dat#tool3.pcx", NULL);
	ani_tool_image[4] = load_pcx("chared.dat#tool4.pcx", NULL);
	ani_tool_image[5] = load_pcx("chared.dat#tool5.pcx", NULL);
	ani_tool_image[6] = load_pcx("chared.dat#tool6.pcx", NULL);
	ani_tool_image[7] = load_pcx("chared.dat#tool7.pcx", NULL);
	ani_tool_image[8] = load_pcx("chared.dat#tool8.pcx", NULL);
    object_ani[PP_OBJECT_PORTAL] = load_ani("levedit.dat#portal.ani", NULL);
    object_ani[PP_OBJECT_TARGET_PORTAL] = load_ani("levedit.dat#target_portal.ani", NULL);
    object_ani[PP_OBJECT_AMMO_NORMAL] = load_ani("pp.dat#objects/ammo_normal.ani", NULL);
    object_ani[PP_OBJECT_AMMO_X] = load_ani("pp.dat#objects/ammo_x.ani", NULL);
    object_ani[PP_OBJECT_AMMO_MINE] = load_ani("pp.dat#objects/ammo_mine.ani", NULL);
    object_ani[PP_OBJECT_AMMO_BOUNCE] = load_ani("pp.dat#objects/ammo_bounce.ani", NULL);
    object_ani[PP_OBJECT_AMMO_SEEK] = load_ani("pp.dat#objects/ammo_seek.ani", NULL);
    object_ani[PP_OBJECT_POWER_CLOAK] = load_ani("pp.dat#objects/power_cloak.ani", NULL);
    object_ani[PP_OBJECT_POWER_JUMP] = load_ani("pp.dat#objects/power_jump.ani", NULL);
    object_ani[PP_OBJECT_POWER_RUN] = load_ani("pp.dat#objects/power_run.ani", NULL);
    object_ani[PP_OBJECT_POWER_DEFLECT] = load_ani("pp.dat#objects/power_deflect.ani", NULL);
    object_ani[PP_OBJECT_POWER_FLY] = load_ani("pp.dat#objects/power_fly.ani", NULL);
    object_ani[PP_OBJECT_POWER_TURBO] = load_ani("pp.dat#objects/power_turbo.ani", NULL);
    object_ani[PP_OBJECT_FLAG_PORTAL] = load_ani("levedit.dat#flag_portal.ani", NULL);
    object_ani[PP_OBJECT_BASE_PORTAL] = load_ani("pp.dat#objects/base.ani", NULL);
    object_ani[PP_OBJECT_HUNT_PORTAL] = load_ani("levedit.dat#hunt_portal.ani", NULL);
    object_ani[PP_OBJECT_HUNT_BASE] = load_ani("levedit.dat#hunt_base.ani", NULL);
    object_ani[PP_OBJECT_SPRING_UP] = load_ani("pp.dat#objects/spring_up.ani", NULL);
    object_ani[PP_OBJECT_SPRING_DOWN] = load_ani("pp.dat#objects/spring_down.ani", NULL);
    object_ani[PP_OBJECT_SPRING_LEFT] = load_ani("pp.dat#objects/spring_left.ani", NULL);
    object_ani[PP_OBJECT_SPRING_RIGHT] = load_ani("pp.dat#objects/spring_right.ani", NULL);
	level_help_text = load_text("levedit.txt");
    set_menu_frame();
	install_int_ex(logic_timer, BPS_TO_TIMER(60));
}

void level_exit(void)
{
	int i;
	
    destroy_bitmap(level_screen);
    destroy_bitmap(layer_screen);
    destroy_bitmap(tile_screen);
    destroy_level(level);
    for(i = 0; i < 9; i++)
    {
	    destroy_bitmap(ani_tool_image[i]);
    }
    free_text(level_help_text);
}

void center_to_object(int i)
{
	if(object[i].active && object_ani[object[i].type])
	{
		center_tilemap(view_screen, level->tilemap, fixtoi(object[i].x) + object_ani[object[i].type]->w / 2, fixtoi(object[i].y) + object_ani[object[i].type]->h / 2);
	}
}

void fix_clipping(void)
{
	if(level->tilemap->cx < 0)
	{
		level->tilemap->cx = 0;
	}
	if(level->tilemap->cx >= level->tilemap->w)
	{
		level->tilemap->cx = level->tilemap->w - 1;
	}
	if(level->tilemap->cr >= level->tilemap->w)
	{
		level->tilemap->cr = level->tilemap->w - 1;
	}
	if(level->tilemap->cy < 0)
	{
		level->tilemap->cy = 0;
	}
	if(level->tilemap->cy >= level->tilemap->h)
	{
		level->tilemap->cy = level->tilemap->h - 1;
	}
	if(level->tilemap->cb >= level->tilemap->h)
	{
		level->tilemap->cb = level->tilemap->h - 1;
	}
}

void global_control(void)
{
	read_mouse();
	if(key[KEY_F1])
	{
		mouse_hidden = 1;
		update();
		help_dialog[0].dp = level_help_text;
		centre_dialog(help_dialog);
    	my_popup_dialog(help_dialog, 0);
    	mouse_hidden = 0;
    	key[KEY_F1] = 0;
    	key[KEY_ESC] = 0;
    }
    if(key[KEY_F2] && level)
    {
	    file_menu_save();
        key[KEY_F2] = 0;
    }
    if(key[KEY_F3])
    {
	    file_menu_load();
        key[KEY_F3] = 0;
    }
    if(key[KEY_F4])
    {
	    file_menu_new();
    	key[KEY_F4] = 0;
    }
    if(key[KEY_TAB] && level)
    {
	    level_view++;
	    if(level_view >= L_VIEWS)
	    {
		    level_view = 0;
	    }
	    fix_view_menu();
	    key[KEY_TAB] = 0;
	}
	if(key[KEY_ESC])
	{
		mouse_hidden = 1;
		update();
		file_menu_exit();
		mouse_hidden = 0;
		key[KEY_ESC] = 0;
	}
}

void object_control(void)
{
    int i, j;

	if(!(my_mouse_b & 1) && !(my_mouse_b & 2))
	{
		if(my_mouse_x <= 0 && my_mouse_y <= 0)
		{
			mouse_hidden = 1;
			level_draw();
//        	vsync();
	        blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
			my_popup_dialog(object_dialog, 0);
			mouse_hidden = 0;
		}
	}
    if(key[KEY_LEFT])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->x -= level->tileset->w;
        }
        else
        {
            level->tilemap->x--;
        }
        if(level->tilemap->x < 0)
        {
            level->tilemap->x = 0;
        }
    }
    if(key[KEY_RIGHT])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->x += level->tileset->w;
        }
        else
        {
            level->tilemap->x++;
        }
        if(level->tilemap->x > level->tilemap->w * level->tileset->w - view_screen->w)
        {
            level->tilemap->x = level->tilemap->w * level->tileset->w - view_screen->w;
        }
    }
    if(key[KEY_UP])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->y -= level->tileset->h;
        }
        else
        {
            level->tilemap->y--;
        }
        if(level->tilemap->y < 0)
        {
            level->tilemap->y = 0;
        }
    }
    if(key[KEY_DOWN])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->y += level->tileset->h;
        }
        else
        {
            level->tilemap->y++;
        }
        if(level->tilemap->y > level->tilemap->h * level->tileset->h - view_screen->h)
        {
            level->tilemap->y = level->tilemap->h * level->tileset->h - view_screen->h;
        }
    }
    if(key[KEY_LCONTROL])
    {
	    sticky = 1;
    }
    else
    {
	    sticky = 0;
    }
    if(key[KEY_COMMA])
    {
	    object_menu_previous();
       	key[KEY_COMMA] = 0;
    }
    if(key[KEY_STOP])
    {
	    object_menu_next();
       	key[KEY_STOP] = 0;
    }
    if(key[KEY_T])
    {
	    if(object[selected_object].active)
	    {
		    if(key[KEY_LSHIFT])
		    {
	    		object[selected_object].type--;
			    while(!object_ani[object[selected_object].type])
			    {
		    		object[selected_object].type--;
		    		if(object[selected_object].type < 0)
		    		{
					    object[selected_object].type = PP_MAX_OBJECTS - 1;
		    		}
	    		}
		    }
		    else
		    {
		    	object[selected_object].type++;
			    while(!object_ani[object[selected_object].type])
			    {
			    	object[selected_object].type++;
		    		if(object[selected_object].type >= PP_MAX_OBJECTS)
		    		{
					    object[selected_object].type = 0;
		    		}
	    		}
	    	}
	    }
	    key[KEY_T] = 0;
    }
    if((my_mouse_b & 2) && !mouse_stop && object_ani[selected_type])
    {
	    if(my_mouse_x < 320 && my_mouse_y < 240)
	    {
	    	need_to_save = 1;
	    	for(i = 0; i < PP_MAX_OBJECTS; i++)
	    	{
			    if(!object[i].active)
		    	{
					object[i].type = selected_type;
		    		if(sticky)
		    		{
			       		object[i].x = itofix(((my_mouse_x + level->tilemap->x) / grid_x) * grid_x + grid_x - object_ani[object[i].type]->w / 2);
       					object[i].y = itofix(((my_mouse_y + level->tilemap->y) / grid_y) * grid_y + grid_y - object_ani[object[i].type]->h / 2);
    				}
	    			else
    				{
	       				object[i].x = itofix(my_mouse_x + level->tilemap->x - object_ani[object[i].type]->w / 2);
       					object[i].y = itofix(my_mouse_y + level->tilemap->y - object_ani[object[i].type]->h / 2);
					}
       				object[i].active = 1;
       				selected_object = i;
       				break;
		    	}
	    	}
	    }
	    else if(my_mouse_x >= 320 && my_mouse_y >= 240)
	    {
	    	need_to_save = 1;
	    	for(i = 0; i < PP_MAX_OBJECTS; i++)
	    	{
			    if(!object[i].active)
		    	{
					object[i].type = selected_type;
		    		if(sticky)
		    		{
			       		object[i].x = itofix(((my_mouse_x + level->tilemap->x - 320) / grid_x) * grid_x + grid_x - object_ani[object[i].type]->w / 2);
       					object[i].y = itofix(((my_mouse_y + level->tilemap->y - 240) / grid_y) * grid_y + grid_y - object_ani[object[i].type]->h / 2);
    				}
	    			else
    				{
	       				object[i].x = itofix(my_mouse_x + level->tilemap->x - object_ani[object[i].type]->w / 2 - 320);
       					object[i].y = itofix(my_mouse_y + level->tilemap->y - object_ani[object[i].type]->h / 2 - 240);
					}
       				object[i].active = 1;
       				selected_object = i;
       				break;
		    	}
	    	}
	    }
	    mouse_stop = 1;
    }
    if(!(my_mouse_b & 2))
    {
	    mouse_stop = 0;
    }
    if(pegged < 0)
    {
    	mouse_selected = -1;
    }
    if(level)
    {
    	for(i = 0; i < PP_MAX_OBJECTS; i++)
    	{
		    if(object_ani[object[i].type] && object[i].active)
	    	{
		    	if(my_mouse_x < 320 && my_mouse_y < 240)
		    	{
		    		if(my_mouse_x + level->tilemap->x < fixtoi(object[i].x) + object_ani[object[i].type]->w && fixtoi(object[i].x) < my_mouse_x + level->tilemap->x && my_mouse_y + level->tilemap->y < fixtoi(object[i].y) + object_ani[object[i].type]->h && fixtoi(object[i].y) < my_mouse_y + level->tilemap->y)
	    			{
			    		if(pegged < 0)
		    			{
					    	mouse_selected = i;
			    		}
				    	if(my_mouse_b & 1 && pegged < 0)
				    	{
					    	pegged = i;
       						pox = my_mouse_x + level->tilemap->x - fixtoi(object[i].x);
       						poy = my_mouse_y + level->tilemap->y - fixtoi(object[i].y);
       					}
		    			break;
	    			}
    			}
		    	else if(my_mouse_x >= 320 && my_mouse_y >= 240)
		    	{
		    		if((my_mouse_x - 320) + level->tilemap->x < fixtoi(object[i].x) + object_ani[object[i].type]->w && fixtoi(object[i].x) < (my_mouse_x - 320) + level->tilemap->x && (my_mouse_y - 240) + level->tilemap->y < fixtoi(object[i].y) + object_ani[object[i].type]->h && fixtoi(object[i].y) < (my_mouse_y - 240) + level->tilemap->y)
	    			{
			    		if(pegged < 0)
		    			{
					    	mouse_selected = i;
			    		}
				    	if(my_mouse_b & 1 && pegged < 0)
				    	{
					    	pegged = i;
       						pox = (my_mouse_x - 320) + level->tilemap->x - fixtoi(object[i].x);
       						poy = (my_mouse_y - 240) + level->tilemap->y - fixtoi(object[i].y);
       					}
		    			break;
	    			}
    			}
    		}
		}
		animate_tileset(level->tileset);
    }
    if(key[KEY_DEL])
    {
		object_menu_delete();
        key[KEY_DEL] = 0;
    }
    if(my_mouse_b & 1)
    {
	    if(my_mouse_x < 320 && my_mouse_y < 240)
	    {
	    	if(mouse_selected >= 0 && pegged >= 0)
	    	{
			    need_to_save = 1;
			    selected_object = mouse_selected;
	       		object[selected_object].x = itofix(my_mouse_x - pox + level->tilemap->x);
   				object[selected_object].y = itofix(my_mouse_y - poy + level->tilemap->y);
	    		if(sticky)
    			{
		       		object[selected_object].x = itofix(((fixtoi(object[selected_object].x) + object_ani[object[selected_object].type]->w / 2) / grid_x) * grid_x - object_ani[object[selected_object].type]->w / 2);
		       		object[selected_object].y = itofix(((fixtoi(object[selected_object].y) + object_ani[object[selected_object].type]->h / 2) / grid_y) * grid_y - object_ani[object[selected_object].type]->h / 2);
    			}
	    		else
    			{
				}
			}
		}
		else if(my_mouse_y < 240)
		{
			selected_type = (my_mouse_y / 32) * 10 + (my_mouse_x - 320) / 32;
		}
		else if(my_mouse_x >= 320 && my_mouse_y >= 240)
		{
	    	if(mouse_selected >= 0 && pegged >= 0)
	    	{
			    need_to_save = 1;
			    selected_object = mouse_selected;
	       		object[selected_object].x = itofix(my_mouse_x - pox + level->tilemap->x - 320);
   				object[selected_object].y = itofix(my_mouse_y - poy + level->tilemap->y - 240);
	    		if(sticky)
    			{
		       		object[selected_object].x = itofix(((fixtoi(object[selected_object].x) + object_ani[object[selected_object].type]->w / 2) / grid_x) * grid_x - object_ani[object[selected_object].type]->w / 2);
		       		object[selected_object].y = itofix(((fixtoi(object[selected_object].y) + object_ani[object[selected_object].type]->h / 2) / grid_y) * grid_y - object_ani[object[selected_object].type]->h / 2);
    			}
	    		else
    			{
				}
			}
		}
    }
    else if(!sticky)
    {
	    pegged = -1;
    }
}

void tilemap_control(void)
{
    int i, j;

	if(!(my_mouse_b & 1) && !(my_mouse_b & 2))
	{
		if(my_mouse_x <= 0 && my_mouse_y <= 0)
		{
			mouse_hidden = 1;
			level_draw();
//        	vsync();
	        blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
			my_popup_dialog(tilemap_dialog, 0);
			mouse_hidden = 0;
		}
	}
	if(key[KEY_F9])
	{
		map_menu_properties();
		key[KEY_F9] = 0;
	}
    if(key[KEY_LEFT])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->x -= level->tileset->w;
        }
        else
        {
            level->tilemap->x--;
        }
        if(level->tilemap->x < 0)
        {
            level->tilemap->x = 0;
        }
    }
    if(key[KEY_RIGHT])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->x += level->tileset->w;
        }
        else
        {
            level->tilemap->x++;
        }
        if(level->tilemap->x > level->tilemap->w * level->tileset->w - view_screen->w)
        {
            level->tilemap->x = level->tilemap->w * level->tileset->w - view_screen->w;
        }
    }
    if(key[KEY_UP])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->y -= level->tileset->h;
        }
        else
        {
            level->tilemap->y--;
        }
        if(level->tilemap->y < 0)
        {
            level->tilemap->y = 0;
        }
    }
    if(key[KEY_DOWN])
    {
        if(key[KEY_LSHIFT])
        {
            level->tilemap->y += level->tileset->h;
        }
        else
        {
            level->tilemap->y++;
        }
        if(level->tilemap->y > level->tilemap->h * level->tileset->h - view_screen->h)
        {
            level->tilemap->y = level->tilemap->h * level->tileset->h - view_screen->h;
        }
    }
    if(key[KEY_1])
    {
	    selected_layer = 0;
	    fix_layer_menu();
	    key[KEY_1] = 0;
    }
    if(key[KEY_2])
    {
	    selected_layer = 1;
	    fix_layer_menu();
	    key[KEY_2] = 0;
    }
    if(key[KEY_3])
    {
	    selected_layer = 2;
	    fix_layer_menu();
	    key[KEY_3] = 0;
    }
    if(key[KEY_4])
    {
	    selected_layer = 3;
	    fix_layer_menu();
	    key[KEY_4] = 0;
    }
    if(key[KEY_5])
    {
	    selected_layer = 4;
	    fix_layer_menu();
	    key[KEY_5] = 0;
    }
    if(key[KEY_A])
    {
	    layer_shift_left();
	    key[KEY_A] = 0;
    }
    if(key[KEY_D])
    {
	    layer_shift_right();
	    key[KEY_D] = 0;
    }
    if(key[KEY_W])
    {
	    layer_shift_up();
	    key[KEY_W] = 0;
    }
    if(key[KEY_S])
    {
	    layer_shift_down();
	    key[KEY_S] = 0;
    }
    if(my_mouse_b & 2)
    {
	    if(my_mouse_x < 320 && my_mouse_y < 240)
	    {
            selected_tile = level->tilemap->layer[selected_layer]->mp->line[(int)(((my_mouse_y + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h))][(int)(((my_mouse_x + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w))];
            selected_frame = 0;
		}
		else if(my_mouse_x >= 320 && my_mouse_y >= 240)
		{
            selected_tile = level->tilemap->layer[selected_layer]->mp->line[(int)(((my_mouse_y - 240 + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h))][(int)(((my_mouse_x - 320 + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w))];
            selected_frame = 0;
		}
    }
    if(my_mouse_b & 1)
    {
	    need_to_save = 1;
	    if(my_mouse_x < 320 && my_mouse_y < 240)
	    {
		    if(key[KEY_LSHIFT])
		    {
			    floodfill(level->tilemap->layer[selected_layer]->mp, (int)(((my_mouse_x + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w)), (int)(((my_mouse_y + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h)), selected_tile);
		    }
		    else
		    {
            	level->tilemap->layer[selected_layer]->mp->line[(int)(((my_mouse_y + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h))][(int)(((my_mouse_x + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w))] = selected_tile;
            }
		}
		else if(my_mouse_y < 240)
		{
			selected_tile = (my_mouse_y / 16) * 20 + (my_mouse_x - 320) / 16;
			if(selected_tile >= TILEMAP_MAX_TILES)
			{
				selected_tile = TILEMAP_MAX_TILES - 1;
			}
            selected_frame = 0;
		}
		else if(my_mouse_x >= 320)
		{
            level->tilemap->layer[selected_layer]->mp->line[(int)(((my_mouse_y - 240 + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h))][(int)(((my_mouse_x - 320 + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w))] = selected_tile;
		}
    }
    fix_clipping();
	animate_tileset(level->tileset);
}

void tileset_control(void)
{
	int tool_x, tool_y, tool_w, tool_h;
	ap = level->tileset->tile[selected_tile]->ap;
	bp = ap->image[selected_frame];
	set_menu_frame();
	
	tool_x = PAL_X - 8 - ani_tool_image[ani_tool]->w;
	tool_y = PAL_Y + PAL_H / 2 - ani_tool_image[ani_tool]->h / 2;
	tool_w = ani_tool_image[ani_tool]->w;
	tool_h = ani_tool_image[ani_tool]->h;
	if(my_mouse_x <= 0 && my_mouse_y <= 0)
	{
		mouse_hidden = 1;
		level_draw();
        blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		my_popup_dialog(tileset_dialog, 0);
		mouse_hidden = 0;
	}
	ap = level->tileset->tile[selected_tile]->ap;
	bp = ap->image[selected_frame];
    if(my_mouse_b & 1)
    {
	    need_to_save = 1;
	    if(my_mouse_x < 320 && my_mouse_y < 240)
	    {
		    if(my_mouse_x < 16 * tile_scale && my_mouse_y < 16 * tile_scale)
		    {
			    switch(ani_tool)
			    {
				    case TOOL_PIXEL:
				    {
						putpixel(bp, my_mouse_x / tile_scale, my_mouse_y / tile_scale, selected_color);
						break;
					}
				    case TOOL_FILL:
				    {
						tool_fill(bp, my_mouse_x / tile_scale, my_mouse_y / tile_scale, selected_color);
						break;
					}
				    case TOOL_REPLACE:
				    {
						tool_replace(bp, my_mouse_x / tile_scale, my_mouse_y / tile_scale, selected_color);
						break;
					}
				    case TOOL_LINE:
				    {
						tool_line(bp, 1, 0, 0, 0, 0, tile_scale, selected_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_RECT:
				    {
						tool_rect(bp, 1, 0, 0, 0, 0, tile_scale, selected_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_FRECT:
				    {
						tool_frect(bp, 1, 0, 0, 0, 0, tile_scale, selected_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_CIRCLE:
				    {
						tool_circle(bp, 1, 0, 0, 0, 0, tile_scale, selected_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_FCIRCLE:
				    {
						tool_fcircle(bp, 1, 0, 0, 0, 0, tile_scale, selected_color, update);
						logic_frames = 0;
						break;
					}
				    case TOOL_SELECT:
				    {
					    if(ani_sel_bp && ani_sel_bp == bp && my_mouse_x >= ani_sel_x * tile_scale && ani_sel_x2 * tile_scale + tile_scale && my_mouse_y >= ani_sel_y * tile_scale && my_mouse_y <= ani_sel_y2 * tile_scale + tile_scale)
					    {
							tool_move(bp, 1, 0, 0, 0, 0, tile_scale, selected_color, update);
					    }
					    else
					    {
							tool_select(bp, 1, 0, 0, 0, 0, tile_scale, selected_color, update);
						}
						logic_frames = 0;
						break;
					}
			    }
		    }
		    else if(my_mouse_x >= 224 && my_mouse_y >= 144)
		    {
			    selected_color = (my_mouse_x - 224) / 6 + ((my_mouse_y - 144) / 6) * 16;
		    }
			else if(my_mouse_x >= tool_x && my_mouse_x < tool_x + tool_w && my_mouse_y >= tool_y && my_mouse_y < tool_y + tool_h)
			{
				ani_tool = tool_map[(my_mouse_y - tool_y) / 16][(my_mouse_x - tool_x) / 16];
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
		}
		else if(my_mouse_y < 240)
		{
			selected_tile = (my_mouse_y / 16) * 20 + (my_mouse_x - 320) / 16;
			if(selected_tile >= TILEMAP_MAX_TILES)
			{
				selected_tile = TILEMAP_MAX_TILES - 1;
			}
            selected_frame = 0;
		}
		set_menu_edit();
    }
    if(my_mouse_b & 2)
    {
	    if(my_mouse_x < 16 * tile_scale && my_mouse_y < 16 * tile_scale)
	    {
		    if(ani_tool == TOOL_SELECT)
	    	{
			    ani_sel_bp = NULL;
				if(ani_sel_fbp)
				{
					destroy_bitmap(ani_sel_fbp);
					ani_sel_fbp = NULL;
				}
		    	ani_tool = TOOL_PIXEL;
	    	}
	    	else
	    	{
	            selected_color = level->tileset->tile[selected_tile]->ap->image[selected_frame]->line[my_mouse_y / tile_scale][my_mouse_x / tile_scale];
	    	}
	    }
		set_menu_edit();
    }
	if(key[KEY_F9])
	{
		tile_menu_properties();
		key[KEY_F9] = 0;
	}
    if(key[KEY_A])
    {
	    need_to_save = 1;
	    if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
	    {
	    	ani_shift_left();
	    }
	    else
	    {
	    	frame_shift_left();
    	}
	    key[KEY_A] = 0;
    }
    else if(key[KEY_D])
    {
	    need_to_save = 1;
	    if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
	    {
		    ani_shift_left();
		}
		else
		{
	    	frame_shift_right();
    	}
	    key[KEY_D] = 0;
    }
    else if(key[KEY_W])
    {
	    need_to_save = 1;
   	    if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
	    {
		    ani_shift_left();
		}
		else
		{
			frame_shift_up();
		}
	    key[KEY_W] = 0;
    }
    else if(key[KEY_S])
    {
	    need_to_save = 1;
	    if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
	    {
		    ani_shift_left();
	    }
	    else
	    {
	    	frame_shift_down();
    	}
	    key[KEY_S] = 0;
    }
    else if(key[KEY_UP])
    {
	    selected_frame--;
	    if(selected_frame < 0)
	    {
		    selected_frame = level->tileset->tile[selected_tile]->ap->f - 1;
	    }
	    key[KEY_UP] = 0;
    }
    else if(key[KEY_DOWN])
    {
	    selected_frame++;
	    if(selected_frame >= level->tileset->tile[selected_tile]->ap->f)
	    {
		    selected_frame = 0;
	    }
	    key[KEY_DOWN] = 0;
    }
	else if((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && key[KEY_C] && ani_sel_bp)
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
	else if(key[KEY_H] && ani_sel_bp)
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
	else if(key[KEY_COMMA])
	{
		ap->d++;
		key[KEY_COMMA] = 0;
	}
	else if(key[KEY_STOP])
	{
		if(ap->d > 1)
		{
			ap->d--;
		}
		key[KEY_STOP] = 0;
	}
	animate_tileset(level->tileset);
}

void level_control(void)
{
	int i;

	switch(level_view)
	{
		case L_VIEW_OBJECT:
		{
			object_control();
			break;
		}
		case L_VIEW_MAP:
		{
			tilemap_control();
			break;
		}
		case L_VIEW_TILES:
		{
			tileset_control();
			break;
		}
	}
	if(key[KEY_F12])
	{
		pw = (level->tilemap->cr - level->tilemap->cx + 1) * level->tileset->w;
		ph = (level->tilemap->cb - level->tilemap->cy + 1) * level->tileset->h;
		preview = create_bitmap(pw, ph);
		clear_bitmap(preview);
		center_tilemap(preview, level->tilemap, 0, 0);
		for(i = 0; i < level->tilemap->l; i++)
		{
			layer[i] = create_bitmap(320 + (pw - 320) * level->tilemap->layer[i]->xm, 240 + (ph - 240) * level->tilemap->layer[i]->ym);
			clear_bitmap(layer[i]);
			draw_tilemap(layer[i], level->tilemap, i);
		}
		set_palette(level->tileset->palette);
		for(i = 0; i < level->tilemap->l; i++)
		{
			stretch_sprite(preview, layer[i], 0, 0, preview->w, preview->h);
		}
   		save_pcx("mpreview.pcx", preview, level->tileset->palette);
    	destroy_bitmap(preview);
		key[KEY_F12] = 0;
	}
	frame++;
}

void vobject_draw(void)
{
    int i;
    clear_bitmap(level_screen);
    if(level && level->tilemap && level->tilemap->ts)
    {

	    /* draw the level */
        draw_tilemap_bg(view_screen, level->tilemap);
        draw_tilemap_bg(layer_screen, level->tilemap);
        for(i = 0; i < PP_MAX_OBJECTS; i++)
        {
            if(object[i].active && object_ani[object[i].type])
            {
                draw_ani(view_screen, object_ani[object[i].type], fixtoi(object[i].x) - level->tilemap->x, fixtoi(object[i].y) - level->tilemap->y, frame);
                draw_ani(layer_screen, object_ani[object[i].type], fixtoi(object[i].x) - level->tilemap->x, fixtoi(object[i].y) - level->tilemap->y, frame);
                if(i == selected_object && frame % 2 && object_ani[object[i].type])
                {
	                if(my_mouse_x < 320 && my_mouse_y < 240)
	                {
                		rect(view_screen, fixtoi(object[i].x) - level->tilemap->x, fixtoi(object[i].y) - level->tilemap->y, fixtoi(object[i].x) + object_ani[object[i].type]->w - 1 - level->tilemap->x, fixtoi(object[i].y) + object_ani[object[i].type]->h - 1 - level->tilemap->y, makecol(255, 255, 255));
                	}
                	else if(my_mouse_x >= 320 && my_mouse_y >= 240)
                	{
	                	rect(layer_screen, fixtoi(object[i].x) - level->tilemap->x, fixtoi(object[i].y) - level->tilemap->y, fixtoi(object[i].x) + object_ani[object[i].type]->w - 1 - level->tilemap->x, fixtoi(object[i].y) + object_ani[object[i].type]->h - 1 - level->tilemap->y, makecol(255, 255, 255));
                	}
                }
                else if(i == mouse_selected && frame % 2 && object_ani[object[i].type])
                {
	                if(my_mouse_x < 320 && my_mouse_y < 240)
	                {
	                	rect(view_screen, fixtoi(object[i].x) - level->tilemap->x, fixtoi(object[i].y) - level->tilemap->y, fixtoi(object[i].x) + object_ani[object[i].type]->w - 1 - level->tilemap->x, fixtoi(object[i].y) + object_ani[object[i].type]->h - 1 - level->tilemap->y, makecol(0, 255, 0));
                	}
                	else if(my_mouse_x >= 320 && my_mouse_y >= 240)
                	{
	                	rect(layer_screen, fixtoi(object[i].x) - level->tilemap->x, fixtoi(object[i].y) - level->tilemap->y, fixtoi(object[i].x) + object_ani[object[i].type]->w - 1 - level->tilemap->x, fixtoi(object[i].y) + object_ani[object[i].type]->h - 1 - level->tilemap->y, makecol(0, 255, 0));
                	}
                }
                draw_ani(layer_screen, object_ani[object[i].type], fixtoi(object[i].x) - level->tilemap->x, fixtoi(object[i].y) - level->tilemap->y, 0);
            }
        }
        draw_tilemap_fg(layer_screen, level->tilemap);
	    rect(view_screen, level->tilemap->cx * level->tileset->w - level->tilemap->x, level->tilemap->cy * level->tileset->h - level->tilemap->y, (level->tilemap->cr + 1) * level->tileset->w - level->tilemap->x - 1, (level->tilemap->cb + 1) * level->tileset->h - level->tilemap->y - 1, makecol(255, 0, 0));
	    rect(layer_screen, level->tilemap->cx * level->tileset->w - level->tilemap->x, level->tilemap->cy * level->tileset->h - level->tilemap->y, (level->tilemap->cr + 1) * level->tileset->w - level->tilemap->x - 1, (level->tilemap->cb + 1) * level->tileset->h - level->tilemap->y - 1, makecol(255, 0, 0));

       	if(object[selected_object].active && object_ani[object[selected_object].type])
       	{
	        if(my_mouse_x < 320 && my_mouse_y < 240)
        	{
	        	textprintf_ex(level_screen, font, 0, 232, makecol(255, 255, 255), -1, "Object[%d].type == %s", selected_object, object_name[object[selected_object].type]);
        	}
	        else if(my_mouse_x >= 320 && my_mouse_y >= 240)
        	{
	        	textprintf_ex(layer_screen, font, 0, 232, makecol(255, 255, 255), -1, "Object[%d].type == %s", selected_object, object_name[object[selected_object].type]);
        	}
        }
       	if(object_ani[selected_type])
       	{
	     	textprintf_ex(tile_screen, font, 0, 232, makecol(255, 255, 255), -1, "Type == %s", object_name[selected_type]);
       	}
	    for(i = 0; i < PP_MAX_OBJECTS; i++)
	    {
		    if(object_ani[i])
		    {
		    	stretch_sprite(tile_screen, get_ani(object_ani[i], frame), (i % 10) * 32, (i / 10) * 32, 32, 32);
	    	}
	    	if(frame % 2)
	    	{
	    		rect(tile_screen, (selected_type % 10) * 32, (selected_type / 10) * 32, (selected_type % 10) * 32 + 31, (selected_type / 10) * 32 + 31, 31);
    		}
	    }
    }
    else
    {
        clear_bitmap(level_screen);
    }
    if(!mouse_hidden)
    {
    	draw_sprite(level_screen, mouse_sprite, my_mouse_x, my_mouse_y);
    }
}

void tilemap_draw(void)
{
    int i;
    clear_bitmap(level_screen);
    if(level && level->tileset && level->tilemap)
    {

	    /* draw the level */
        draw_tilemap(view_screen, level->tilemap, selected_layer);
        draw_tilemap_bg(layer_screen, level->tilemap);
        draw_tilemap_fg(layer_screen, level->tilemap);
	    rect(view_screen, level->tilemap->cx * level->tileset->w - level->tilemap->x, level->tilemap->cy * level->tileset->h - level->tilemap->y, (level->tilemap->cr + 1) * level->tileset->w - level->tilemap->x - 1, (level->tilemap->cb + 1) * level->tileset->h - level->tilemap->y - 1, makecol(255, 0, 0));
        if(my_mouse_x < 320 && my_mouse_y < 240)
        {
	        if(frame % 2)
	        {
				rect(view_screen, (int)(((my_mouse_x + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w)) * level->tileset->w - level->tilemap->x * level->tilemap->layer[selected_layer]->xm, (int)((my_mouse_y + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h) * level->tileset->h - level->tilemap->y * level->tilemap->layer[selected_layer]->ym, (int)(((my_mouse_x + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w)) * level->tileset->w - level->tilemap->x * level->tilemap->layer[selected_layer]->xm + level->tileset->w - 1, (int)((my_mouse_y + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h) * level->tileset->h - level->tilemap->y * level->tilemap->layer[selected_layer]->ym + level->tileset->h - 1, makecol(255, 255, 255));
			}
	        textprintf_ex(view_screen, font, 0, 240 - 8, 31, -1, "Pos(%d,%d,%d)", level->tilemap->x, level->tilemap->y, selected_layer);
        	textprintf_ex(view_screen, font, 0, 240 - 16, 31, -1, "(%d,%d)", (my_mouse_x + level->tilemap->x) / level->tileset->w, (my_mouse_y + level->tilemap->y) / level->tileset->h);
        	textprintf_ex(view_screen, font, 0, 240 - 24, 31, -1, "(%d,%d)", my_mouse_x + level->tilemap->x, my_mouse_y + level->tilemap->y);
        }
        if(my_mouse_x >= 320 && my_mouse_y >= 240)
        {
	        if(frame % 2)
	        {
				rect(layer_screen, (int)(((my_mouse_x - 320 + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w)) * level->tileset->w - level->tilemap->x * level->tilemap->layer[selected_layer]->xm, (int)((my_mouse_y - 240 + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h) * level->tileset->h - level->tilemap->y * level->tilemap->layer[selected_layer]->ym, (int)(((my_mouse_x - 320 + level->tilemap->x * level->tilemap->layer[selected_layer]->xm) / level->tileset->w)) * level->tileset->w - level->tilemap->x * level->tilemap->layer[selected_layer]->xm + level->tileset->w - 1, (int)((my_mouse_y - 240 + level->tilemap->y * level->tilemap->layer[selected_layer]->ym) / level->tileset->h) * level->tileset->h - level->tilemap->y * level->tilemap->layer[selected_layer]->ym + level->tileset->h - 1, makecol(255, 255, 255));
			}
        	textprintf_ex(layer_screen, font, 0, 240 - 8, 31, -1, "Pos(%d,%d,%d)", level->tilemap->x, level->tilemap->y, selected_layer);
        	textprintf_ex(layer_screen, font, 0, 240 - 16, 31, -1, "(%d,%d)", (my_mouse_x - 320 + level->tilemap->x) / level->tileset->w, (my_mouse_y - 240 + level->tilemap->y) / level->tileset->h);
	        textprintf_ex(layer_screen, font, 0, 240 - 24, 31, -1, "(%d,%d)", my_mouse_x - 320 + level->tilemap->x, my_mouse_y - 240 + level->tilemap->y);
        }
        for(i = 0; i < TILEMAP_MAX_TILES; i++)
        {
	        draw_ani(tile_screen, level->tileset->tile[i]->ap, (i % 20) * 16, (i / 20) * 16, frame);
        }
    	if(frame % 2)
    	{
    		rect(tile_screen, (selected_tile % 20) * 16, (selected_tile / 20) * 16, (selected_tile % 20) * 16 + 15, (selected_tile / 20) * 16 + 15, 31);
   		}
    }
    else
    {
        clear_bitmap(level_screen);
    }
    if(!mouse_hidden)
    {
    	draw_sprite(level_screen, mouse_sprite, my_mouse_x, my_mouse_y);
    }
}

void tileset_draw(void)
{
    int i, j;
    clear_bitmap(level_screen);
    if(level && level->tileset)
    {
		ap = level->tileset->tile[selected_tile]->ap;
		bp = ap->image[selected_frame];
        for(i = 0; i < TILEMAP_MAX_TILES; i++)
        {
	        draw_ani(tile_screen, level->tileset->tile[i]->ap, (i % 20) * 16, (i / 20) * 16, frame);
        }
    	if(frame % 2)
    	{
    		rect(tile_screen, (selected_tile % 20) * 16, (selected_tile / 20) * 16, (selected_tile % 20) * 16 + 15, (selected_tile / 20) * 16 + 15, 31);
   		}
   		textprintf_ex(tile_screen, font, 0, 232, 31, -1, "Selected Tile == %d", selected_tile);
   		good_scale(view_screen, level->tileset->tile[selected_tile]->ap->image[selected_frame], 0, 0, tile_scale);
   		textprintf_ex(view_screen, font, 16 * tile_scale, 0, 31, -1, "Tile  == %d", selected_tile);
   		textprintf_ex(view_screen, font, 16 * tile_scale, 8, 31, -1, "Frame == %d", selected_frame);
   		for(i = 0; i < 15; i++)
   		{
	   		for(j = 0; j < 20; j++)
	   		{
		   		draw_sprite(layer_screen, level->tileset->tile[selected_tile]->ap->image[selected_frame], j * 16, i * 16);
	   		}
   		}
   		draw_pal(view_screen, level_palette, PAL_X, PAL_Y, PAL_S);
		draw_sprite(level_screen, ani_tool_image[ani_tool], PAL_X - (ani_tool_image[ani_tool]->w + 8), 240 - (PAL_H) / 2 - (ani_tool_image[ani_tool]->h / 2));
		if(frame % 2 == 0 && bp == ani_sel_bp && bp != NULL && ani_sel_v)
		{
			rect(level_screen, ani_sel_x < ani_sel_x2 ? ani_sel_x * tile_scale : ani_sel_x2 * tile_scale, ani_sel_y < ani_sel_y2 ? ani_sel_y * tile_scale : ani_sel_y2 * tile_scale, ani_sel_x2 > ani_sel_x ? ani_sel_x2 * tile_scale + tile_scale - 1 : ani_sel_x * tile_scale + tile_scale - 1, ani_sel_y2 > ani_sel_y ? ani_sel_y2 * tile_scale + tile_scale - 1 : ani_sel_y * tile_scale + tile_scale - 1, makecol(255, 255, 255));
		}
    }
    else
    {
        clear_bitmap(level_screen);
    }
    if(!mouse_hidden)
    {
    	draw_sprite(level_screen, mouse_sprite, my_mouse_x, my_mouse_y);
    }
}

void level_draw(void)
{
	switch(level_view)
	{
		case L_VIEW_OBJECT:
		{
			vobject_draw();
			break;
		}
		case L_VIEW_MAP:
		{
			tilemap_draw();
			break;
		}
		case L_VIEW_TILES:
		{
			tileset_draw();
			break;
		}
	}
}

void update(void)
{
	level_draw();
//	vsync();
	blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void level_main(void)
{
	int i;
	
    while(!level_quit)
    {
	    global_control();
		for(i = 0; i < logic_frames; i++)
		{
	        level_control();
        }
		logic_frames = 0;
        level_draw();

//        vsync();
        blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
//        frame++;
		rest(0);
    }
}

int main(void)
{
    level_init();
    level_main();
    level_exit();

    return 0;
}
END_OF_MAIN()

int menu_nop(void)
{
	return 1;
}

int file_menu_new(void)
{
	int i, j;
	int is;
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
		sprintf(wtext, "100");
		sprintf(htext, "100");
		sprintf(ltext, "5");
		centre_dialog(new_dialog);
    	is = my_popup_dialog(new_dialog, 0);
    	if(is == 6)
    	{
		    level = create_level(level_palette, atoi(wtext), atoi(htext), 16, 16, 5);
		    if(level)
		    {
	    		grid_x = level->tileset->w / 2;
	    		grid_y = level->tileset->h / 2;
	    		object_menu[1].flags = 0;
	    		object_menu[2].flags = 0;
	    		file_menu[2].flags = 0;
	    		file_menu[4].flags = 0;
	    		file_menu[5].flags = 0;
	    		need_to_save = 0;
				for(i = 0; i < PP_MAX_OBJECTS; i++)
				{
					object[i].type = 0;
					object[i].x = 0;
					object[i].y;
					object[i].active = 0;
					for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
					{
						object[i].flag[j] = 0;
					}
				}
			}
    	}
    }
    clear_clipboard();
    set_menu_edit();
	logic_frames = 0;
	return 1;
}

int file_menu_load(void)
{
	int i, j;

    if(file_select_ex("Load Level", level_filename, "ppl", 1024, 320, 240))
    {
        destroy_level(level);
        level = load_level(level_filename);
        level->tilemap->x = level->tilemap->cx;
        level->tilemap->y = level->tilemap->cy;
        for(i = 0; i < PP_MAX_OBJECTS; i++)
        {
	        if(level->object_type[i] >= 0)
	        {
		        if(object_ani[level->object_type[i]])
		        {
	        		object[i].type = level->object_type[i];
	        		object[i].x = itofix(level->object_x[i]);
	        		object[i].y = itofix(level->object_y[i]);
	        		object[i].active = level->object_active[i];
	        		for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
	        		{
				        object[i].flag[j] = level->object_flag[i][j];
	        		}
        		}
        		else
        		{
		        	object[i].active = 0;
        		}
        	}
        	else
        	{
	        	object[i].active = 0;
        	}
        }
        center_to_object(0);
        grid_x = level->tileset->w / 2;
        grid_y = level->tileset->h / 2;
        object_menu[1].flags = 0;
        object_menu[2].flags = 0;
        file_menu[2].flags = 0;
        file_menu[4].flags = 0;
        file_menu[5].flags = 0;
        need_to_save = 0;
    }
    clear_clipboard();
    set_menu_edit();
	logic_frames = 0;
	return 1;
}

int file_menu_save(void)
{
	int i, j;

    if(file_select_ex("Save Level", level_filename, "ppl", 1024, 320, 240))
    {
	    replace_extension(level_filename, level_filename, "ppl", 128);
        for(i = 0; i < PP_MAX_OBJECTS; i++)
        {
	        level->object_type[i] = object[i].type;
	        level->object_x[i] = fixtoi(object[i].x);
	        level->object_y[i] = fixtoi(object[i].y);
	        level->object_active[i] = object[i].active;
	        for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
	        {
		        level->object_flag[i][j] = object[i].flag[j];
	        }
        }
        save_level(level, level_filename);
        need_to_save = 0;
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
		level_quit = 1;
	}
	logic_frames = 0;
	return 1;
}

int import_menu_tileset(void)
{
	int sel;

	sel = alert(NULL, "Overwrite old tiles?", NULL, "Yes", "No", 0, 0);
	if(sel == 1)
	{
    	if(file_select_ex("Import Tiles", level_filename, "ts", 1024, 320, 240))
    	{
		    need_to_save = 1;
	    	destroy_tileset(level->tileset);
	    	level->tileset = load_tileset(level_filename);
	    	initialize_tilemap(level->tilemap, level->tileset);
   		}
    	ani_sel_bp = NULL;
    	set_menu_edit();
    }
	logic_frames = 0;
	return 1;
}

int import_menu_tilemap(void)
{
	int sel;

	
	sel = alert(NULL, "Overwrite old map?", NULL, "Yes", "No", 0, 0);
	if(sel == 1)
	{
		if(file_select_ex("Import Map", level_filename, "tm", 1024, 320, 240))
    	{
		    need_to_save = 1;
	    	destroy_tilemap(level->tilemap);
	    	level->tilemap = load_tilemap(level_filename);
	    	initialize_tilemap(level->tilemap, level->tileset);
   		}
   	}
	logic_frames = 0;
	return 1;
}

int import_menu_objectmap(void)
{
	int i, j;
	int sel;

	sel = alert(NULL, "Overwrite old objects?", NULL, "Yes", "No", 0, 0);
	if(sel == 1)
	{
    	if(file_select_ex("Import Data", level_filename, "ld", 1024, 320, 240))
    	{
		    need_to_save = 1;
    		load_level_data(level, level_filename);
        	for(i = 0; i < PP_MAX_OBJECTS; i++)
        	{
		        object[i].type = level->object_type[i];
	        	object[i].x = itofix(level->object_x[i]);
	        	object[i].y = itofix(level->object_y[i]);
	        	object[i].active = level->object_active[i];
	        	for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
	        	{
			        object[i].flag[j] = level->object_flag[i][j];
	        	}
        	}
        }
   	}
	logic_frames = 0;
	return 1;
}

int export_menu_tileset(void)
{
    if(file_select_ex("Export Tiles", level_filename, "ts", 1024, 320, 240))
    {
	    replace_extension(level_filename, level_filename, "ts", 128);
	    save_tileset(level->tileset, level_filename);
   	}
	return 1;
}

int export_menu_tilemap(void)
{
    if(file_select_ex("Export Map", level_filename, "tm", 1024, 320, 240))
    {
	    replace_extension(level_filename, level_filename, "tm", 128);
	    save_tilemap(level->tilemap, level_filename);
   	}
	logic_frames = 0;
	return 1;
}

int export_menu_objectmap(void)
{
	int i, j;

    if(file_select_ex("Export Data", level_filename, "ld", 1024, 320, 240))
    {
        for(i = 0; i < PP_MAX_OBJECTS; i++)
        {
	        level->object_type[i] = object[i].type;
	        level->object_x[i] = fixtoi(object[i].x);
	        level->object_y[i] = fixtoi(object[i].y);
	        level->object_active[i] = object[i].active;
	        for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
	        {
		        level->object_flag[i][j] = object[i].flag[j];
	        }
        }
    	save_level_data(level, level_filename);
   	}
	logic_frames = 0;
	return 1;
}

void fix_view_menu(void)
{
	int i;

	for(i = 0; i < L_VIEWS; i++)
	{
		if(level_view == i)
		{
			view_menu[i].flags = D_SELECTED;
		}
		else
		{
			view_menu[i].flags = 0;
		}
	}
}

int view_menu_tilemap(void)
{
	level_view = L_VIEW_MAP;
	view_menu[L_VIEW_MAP].flags = D_SELECTED;
	view_menu[L_VIEW_TILES].flags = 0;
	view_menu[L_VIEW_OBJECT].flags = 0;
	return 1;
}

int view_menu_tileset(void)
{
	level_view = L_VIEW_TILES;
	view_menu[L_VIEW_MAP].flags = 0;
	view_menu[L_VIEW_TILES].flags = D_SELECTED;
	view_menu[L_VIEW_OBJECT].flags = 0;
	return 1;
}

int view_menu_objectmap(void)
{
	level_view = L_VIEW_OBJECT;
	view_menu[L_VIEW_MAP].flags = 0;
	view_menu[L_VIEW_TILES].flags = 0;
	view_menu[L_VIEW_OBJECT].flags = D_SELECTED;
	return 1;
}

int object_menu_previous(void)
{
	int old = selected_object;

    selected_object--;
    if(selected_object < 0)
    {
	    selected_object = PP_MAX_OBJECTS - 1;
    }
    while(!object[selected_object].active && selected_object != old)
    {
	    selected_object--;
	    if(selected_object < 0)
	    {
	    	selected_object = PP_MAX_OBJECTS - 1;
    	}
    }
	center_to_object(selected_object);

	return 1;
}

int object_menu_next(void)
{
	int old = selected_object;

    selected_object++;
    if(selected_object >= PP_MAX_OBJECTS)
    {
	    selected_object = 0;
    }
    while(!object[selected_object].active && selected_object != old)
    {
	    selected_object++;
    	if(selected_object >= PP_MAX_OBJECTS)
    	{
	    	selected_object = 0;
	    }
    }
	center_to_object(selected_object);

	return 1;
}

int object_menu_delete(void)
{
    need_to_save = 1;
	object[selected_object].active = 0;
	object_menu_next();
}

void fix_layer_menu(void)
{
	int i;

	for(i = 0; i < 5; i++)
	{
		if(i == selected_layer)
		{
			layer_select_menu[i].flags = D_SELECTED;
		}
		else
		{
			layer_select_menu[i].flags = 0;
		}
	}
}

int layer_select_1(void)
{
	selected_layer = 0;
	fix_layer_menu();

	return 1;
}

int layer_select_2(void)
{
	selected_layer = 1;
	fix_layer_menu();

	return 1;
}

int layer_select_3(void)
{
	selected_layer = 2;
	fix_layer_menu();

	return 1;
}

int layer_select_4(void)
{
	selected_layer = 3;
	fix_layer_menu();

	return 1;
}

int layer_select_5(void)
{
	selected_layer = 4;
	fix_layer_menu();

	return 1;
}

int layer_shift_left(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tilemap->layer[selected_layer]->mp->w, level->tilemap->layer[selected_layer]->mp->w);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tilemap->layer[selected_layer]->mp, 0, 0);
	clear_bitmap(level->tilemap->layer[selected_layer]->mp);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, -1, 0);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, scratch->w - 1, 0);
	destroy_bitmap(scratch);

	return 1;
}

int layer_shift_right(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tilemap->layer[selected_layer]->mp->w, level->tilemap->layer[selected_layer]->mp->w);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tilemap->layer[selected_layer]->mp, 0, 0);
	clear_bitmap(level->tilemap->layer[selected_layer]->mp);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, 1, 0);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, 1 - scratch->w, 0);
	destroy_bitmap(scratch);

	return 1;
}

int layer_shift_up(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tilemap->layer[selected_layer]->mp->w, level->tilemap->layer[selected_layer]->mp->w);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tilemap->layer[selected_layer]->mp, 0, 0);
	clear_bitmap(level->tilemap->layer[selected_layer]->mp);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, 0, -1);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, 0, scratch->h - 1);
	destroy_bitmap(scratch);

	return 1;
}

int layer_shift_down(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tilemap->layer[selected_layer]->mp->w, level->tilemap->layer[selected_layer]->mp->w);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tilemap->layer[selected_layer]->mp, 0, 0);
	clear_bitmap(level->tilemap->layer[selected_layer]->mp);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, 0, 1);
	draw_sprite(level->tilemap->layer[selected_layer]->mp, scratch, 0, 1 - scratch->h);
	destroy_bitmap(scratch);

	return 1;
}

int map_menu_properties(void)
{
	int is;

	mouse_hidden = 1;
	level_draw();
//   	vsync();
    blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	map_properties_dialog[11].d1 = level->tilemap->il;
	sprintf(etext[0], "%d", level->tilemap->cx);
	sprintf(etext[1], "%d", level->tilemap->cr);
	sprintf(etext[2], "%d", level->tilemap->cy);
	sprintf(etext[3], "%d", level->tilemap->cb);
	centre_dialog(map_properties_dialog);
	is = my_popup_dialog(map_properties_dialog, 0);
	if(is == 12)
	{
	    need_to_save = 1;
		level->tilemap->cx = atoi(etext[0]);
		if(level->tilemap->cx < 0)
		{
			level->tilemap->cx = 0;
		}
		level->tilemap->cr = atoi(etext[1]);
		if(level->tilemap->cr > level->tilemap->w)
		{
			level->tilemap->cr = level->tilemap->w;
		}
		level->tilemap->cy = atoi(etext[2]);
		if(level->tilemap->cy < 0)
		{
			level->tilemap->cy = 0;
		}
		level->tilemap->cb = atoi(etext[3]);
		if(level->tilemap->cb > level->tilemap->h)
		{
			level->tilemap->cb = level->tilemap->h;
		}
		level->tilemap->il = map_properties_dialog[11].d1;
	}
	mouse_hidden = 0;
	return 1;
}

int map_menu_resize(void)
{
	int is;
	int i;
	BITMAP * scratch;

	sprintf(wtext, "%d", level->tilemap->w);
	sprintf(htext, "%d", level->tilemap->h);
	centre_dialog(resize_dialog);
    is = my_popup_dialog(resize_dialog, 0);
    if(is == 6)
    {
	    need_to_save = 1;
		scratch = create_bitmap(level->tilemap->layer[0]->mp->w, level->tilemap->layer[0]->mp->h);
	    for(i = 0; i < level->tilemap->l; i++)
	    {
		    clear_bitmap(scratch);
		    draw_sprite(scratch, level->tilemap->layer[i]->mp, 0, 0);
		    destroy_bitmap(level->tilemap->layer[i]->mp);
		    level->tilemap->layer[i]->mp = create_bitmap(atoi(wtext) * 2, atoi(htext) * 2);
		    clear_bitmap(level->tilemap->layer[i]->mp);
		    draw_sprite(level->tilemap->layer[i]->mp, scratch, 0, 0);
	    }
	    level->tilemap->w = atoi(wtext);
	    level->tilemap->h = atoi(htext);
	    destroy_bitmap(scratch);
	    center_tilemap(view_screen, level->tilemap, 0, 0);
    }
	return 1;
}

int layer_menu_properties(void)
{
	int is;

	mouse_hidden = 1;
	level_draw();
//   	vsync();
    blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	sprintf(etext[0], "%f", level->tilemap->layer[selected_layer]->xm);
	if(strlen(etext[0]) > 4)
	{
		etext[0][4] = '\0';
	}
	sprintf(etext[1], "%f", level->tilemap->layer[selected_layer]->ym);
	if(strlen(etext[1]) > 4)
	{
		etext[1][4] = '\0';
	}
	centre_dialog(layer_properties_dialog);
	is = my_popup_dialog(layer_properties_dialog, 0);
	if(is == 6)
	{
	    need_to_save = 1;
		level->tilemap->layer[selected_layer]->xm = atof(etext[0]);
		if(level->tilemap->layer[selected_layer]->xm < 0)
		{
			level->tilemap->layer[selected_layer]->xm = 0;
		}
		if(level->tilemap->layer[selected_layer]->xm > 2)
		{
			level->tilemap->layer[selected_layer]->xm = 2;
		}
		level->tilemap->layer[selected_layer]->ym = atof(etext[1]);
		if(level->tilemap->layer[selected_layer]->ym < 0)
		{
			level->tilemap->layer[selected_layer]->ym = 0;
		}
		if(level->tilemap->layer[selected_layer]->ym > 2)
		{
			level->tilemap->layer[selected_layer]->ym = 2;
		}
	}
	mouse_hidden = 0;
	return 1;
}

int tile_menu_properties(void)
{
	int is;

	mouse_hidden = 1;
	level_draw();
//   	vsync();
    blit(level_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	tile_properties_dialog[2].flags = level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDT] ? 2 : 0;
	tile_properties_dialog[3].flags = level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDB] ? 2 : 0;
	tile_properties_dialog[4].flags = level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDL] ? 2 : 0;
	tile_properties_dialog[5].flags = level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDR] ? 2 : 0;
	tile_properties_dialog[6].flags = level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_TRANSPARENT] ? 2 : 0;
	tile_properties_dialog[7].flags = level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SLICK] ? 2 : 0;
	tile_properties_dialog[8].flags = level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_LIGHT] ? 2 : 0;
	sprintf(etext[0], "%f", fixtof(level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_CONVEY]));
	etext[0][5] = '\0';
	sprintf(etext[1], "%d", level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_NEXTFRAME]);
	sprintf(etext[2], "%d", level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_NEXTWAIT]);
	centre_dialog(tile_properties_dialog);
	is = my_popup_dialog(tile_properties_dialog, 0);
	if(is == 15)
	{
	    need_to_save = 1;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDT] = tile_properties_dialog[2].flags;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDB] = tile_properties_dialog[3].flags;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDL] = tile_properties_dialog[4].flags;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SOLIDR] = tile_properties_dialog[5].flags;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_TRANSPARENT] = tile_properties_dialog[6].flags;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_SLICK] = tile_properties_dialog[7].flags;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_LIGHT] = tile_properties_dialog[8].flags;
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_CONVEY] = ftofix(atof(etext[0]));
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_NEXTFRAME] = atoi(etext[1]);
		level->tileset->tile[selected_tile]->flag[TILEMAP_FLAG_NEXTWAIT] = atoi(etext[2]);
	}
	mouse_hidden = 0;
	return 1;
}

int frame_shift_left(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]->w, level->tileset->tile[selected_tile]->ap->image[selected_frame]->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[selected_frame], 0, 0);
	clear_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, -1, 0);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, scratch->w - 1, 0);
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int frame_shift_right(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]->w, level->tileset->tile[selected_tile]->ap->image[selected_frame]->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[selected_frame], 0, 0);
	clear_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, 1, 0);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, 1 - scratch->w, 0);
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int frame_shift_up(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]->w, level->tileset->tile[selected_tile]->ap->image[selected_frame]->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[selected_frame], 0, 0);
	clear_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, 0, -1);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, 0, scratch->h - 1);
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int frame_shift_down(void)
{
	BITMAP * scratch;

    need_to_save = 1;
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]->w, level->tileset->tile[selected_tile]->ap->image[selected_frame]->h);
	clear_bitmap(scratch);
	draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[selected_frame], 0, 0);
	clear_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, 0, 1);
	draw_sprite(level->tileset->tile[selected_tile]->ap->image[selected_frame], scratch, 0, 1 - scratch->h);
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
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[0]->w, level->tileset->tile[selected_tile]->ap->image[0]->h);
	for(i = 0; i < level->tileset->tile[selected_tile]->ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[i], 0, 0);
		clear_bitmap(level->tileset->tile[selected_tile]->ap->image[i]);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, -1, 0);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, scratch->w - 1, 0);
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
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[0]->w, level->tileset->tile[selected_tile]->ap->image[0]->h);
	for(i = 0; i < level->tileset->tile[selected_tile]->ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[i], 0, 0);
		clear_bitmap(level->tileset->tile[selected_tile]->ap->image[i]);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, 1, 0);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, 1 - scratch->w, 0);
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
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[0]->w, level->tileset->tile[selected_tile]->ap->image[0]->h);
	for(i = 0; i < level->tileset->tile[selected_tile]->ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[i], 0, 0);
		clear_bitmap(level->tileset->tile[selected_tile]->ap->image[i]);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, 0, -1);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, 0, scratch->h - 1);
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
	scratch = create_bitmap(level->tileset->tile[selected_tile]->ap->image[0]->w, level->tileset->tile[selected_tile]->ap->image[0]->h);
	for(i = 0; i < level->tileset->tile[selected_tile]->ap->f; i++)
	{
		clear_bitmap(scratch);
		draw_sprite(scratch, level->tileset->tile[selected_tile]->ap->image[i], 0, 0);
		clear_bitmap(level->tileset->tile[selected_tile]->ap->image[i]);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, 0, 1);
		draw_sprite(level->tileset->tile[selected_tile]->ap->image[i], scratch, 0, 1 - scratch->h);
	}
	destroy_bitmap(scratch);
	ani_sel_bp = NULL;
	set_menu_edit();

	return 1;
}

int ani_sub_menu_import(void)
{
	char filename[1024] = {0};
	int i;
	PALETTE pal;

	if(file_select_ex("Import Tile", filename, "pcx;bmp;lbm;tga;ani", 1024, 320, 240))
	{
	    need_to_save = 1;
	    get_tiles(filename, 0);
//		destroy_ani(level->tileset->tile[selected_tile]->ap);
//		level->tileset->tile[selected_tile]->ap = load_ani(filename, NULL);
//		for(i = 0; i < level->tileset->tile[selected_tile]->ap->f; i++)
//		{
//			convert_palette(level->tileset->tile[selected_tile]->ap->image[i], pal, level_palette);
//		}
	}
	ani_sel_bp = NULL;
	set_menu_edit();
	logic_frames = 0;
	return 1;
}

int ani_sub_menu_replace(void)
{
	char filename[1024] = {0};
	int i;
	PALETTE pal;

	if(file_select_ex("Import Tile", filename, "pcx;bmp;lbm;tga;ani", 1024, 320, 240))
	{
	    need_to_save = 1;
	    get_tiles(filename, 1);
//		destroy_ani(level->tileset->tile[selected_tile]->ap);
//		level->tileset->tile[selected_tile]->ap = load_ani(filename, NULL);
//		for(i = 0; i < level->tileset->tile[selected_tile]->ap->f; i++)
//		{
//			convert_palette(level->tileset->tile[selected_tile]->ap->image[i], pal, level_palette);
//		}
	}
	ani_sel_bp = NULL;
	set_menu_edit();
	logic_frames = 0;
	return 1;
}

int ani_sub_menu_export(void)
{
	char filename[1024] = {0};

	if(file_select_ex("Save ANI", filename, "ANI", 1024, 320, 240))
	{
		replace_extension(filename, filename, "ani", 240);
		save_ani(level->tileset->tile[selected_tile]->ap, filename, level_palette);
	}
	logic_frames = 0;
	return 1;
}

int ani_sub_menu_insert(void)
{
    insert_tile(selected_tile, 1);
    ani_sel_bp = NULL;
    set_menu_edit();
    return 1;
}

int ani_sub_menu_delete(void)
{
	int sel = 0;
	
	sel = alert(NULL, "Delete this tile?", NULL, "Yes", "No", 0, 0);
	if(sel == 1)
	{
		delete_tile(selected_tile);
		ani_sel_bp = NULL;
		set_menu_edit();
	}
	logic_frames = 0;
	return 1;
}

int frame_menu_insert(void)
{
    need_to_save = 1;
	animation_insert_frame(level->tileset->tile[selected_tile]->ap, selected_frame, level->tileset->tile[selected_tile]->ap->image[selected_frame]);
	ani_sel_bp = NULL;
	set_menu_edit();
	return 1;
}

int frame_menu_add(void)
{
    need_to_save = 1;
	animation_add_frame(level->tileset->tile[selected_tile]->ap, level->tileset->tile[selected_tile]->ap->image[selected_frame]);
	return 1;
}

int frame_menu_delete(void)
{
	if(level->tileset->tile[selected_tile]->ap->f > 1)
	{
    	need_to_save = 1;
		animation_delete_frame(level->tileset->tile[selected_tile]->ap, selected_frame);
		ani_sel_bp = NULL;
		set_menu_edit();
		if(selected_frame >= level->tileset->tile[selected_tile]->ap->f)
		{
			selected_frame = 0;
		}
	}
	return 1;
}

BITMAP * grab_frame(BITMAP * sp)
{
	BITMAP * dp;
	PALETTE spal;
	int sx = 0, sy = 0;
	int ox = 0, oy = 0;
	int step = 0;
	int w = 0;
	
	scare_mouse();
	dp = create_bitmap(level->tileset->w, level->tileset->h);
	clear_bitmap(dp);
	if(sp->w <= level_screen->w && sp->h <= level_screen->h)
	{
		set_mouse_range(0, 0, sp->w - dp->w, sp->h - dp->h);
		step = 1;
	}
	else
	{
		set_mouse_range(0, 0, sp->w - level_screen->w, sp->h - level_screen->h);
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
				if((my_mouse_b & 1) && !w)
				{
					w = 1;
					step = 1;
					set_mouse_range(0, 0, sp->w - dp->w, sp->h - dp->h);
				}
				break;
			}
			case 1:
			{
				sx = my_mouse_x;
				sy = my_mouse_y;
				if(sx + level->tileset->w > sp->w)
				{
					sx = sp->w - level->tileset->w;
				}
				if(sy + level->tileset->h > sp->h)
				{
					sy = sp->h - level->tileset->h;
				}
				position_mouse(sx, sy);
				if((my_mouse_b & 1) && !w)
				{
					w = 1;
					step = 2;
				}
				break;
			}
		}
		if(!(my_mouse_b & 1))
		{
			w = 0;
		}
		
		clear_to_color(level_screen, makecol(0, 64, 128));
//		blit(sp, char_screen, 0, 0, -ox, -oy, char_screen->w, char_screen->h);
		blit(sp, level_screen, 0, 0, -ox, -oy, sp->w, sp->h);
		if(frame % 2 == 0)
		{
			rect(level_screen, sx - ox, sy - oy, sx - ox + level->tileset->w - 1, sy - oy + level->tileset->h - 1, makecol(255, 255, 255));
		}
//		vsync();
		blit(level_screen, screen, 0, 0, 0, 0, 640, 480);
//		frame++;
	}
	draw_sprite(dp, sp, -sx, -sy);
	set_mouse_range(0, 0, SCREEN_W, SCREEN_H);
	return dp;
}

BITMAP * grab_bitmap(BITMAP * sp)
{
	BITMAP * dp;
	PALETTE spal;
	int sx = 0, sy = 0;
	int ox = 0, oy = 0;
	int step = 0;
	int w = 0;
	int tw = 16, th = 16;
	
	scare_mouse();
//	dp = create_bitmap(level->tileset->w, level->tileset->h);
//	clear_bitmap(dp);
	if(sp->w <= level_screen->w && sp->h <= level_screen->h)
	{
//		set_mouse_range(0, 0, sp->w - dp->w, sp->h - dp->h);
		set_mouse_range(0, 0, level_screen->w, level_screen->h);
		step = 1;
	}
	else
	{
		set_mouse_range(0, 0, sp->w - level_screen->w, sp->h - level_screen->h);
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
				if((my_mouse_b & 1) && !w)
				{
					w = 1;
					step = 1;
					set_mouse_range(0, 0, sp->w - dp->w, sp->h - dp->h);
				}
				break;
			}
			case 1:
			{
				sx = my_mouse_x;
				sy = my_mouse_y;
				if((my_mouse_b & 1) && !w)
				{
					w = 1;
					step = 2;
				}
				if(key[KEY_UP])
				{
					if(th > 16)
					{
						th -= 16;
					}
					key[KEY_UP] = 0;
				}
				if(key[KEY_DOWN])
				{
					if(th < (sp->h / 16) * 16)
					{
						th += 16;
					}
					key[KEY_DOWN] = 0;
				}
				if(key[KEY_LEFT])
				{
					if(tw > 16)
					{
						tw -= 16;
					}
					key[KEY_LEFT] = 0;
				}
				if(key[KEY_RIGHT])
				{
					if(tw < (sp->w / 16) * 16)
					{
						tw += 16;
					}
					key[KEY_RIGHT] = 0;
				}
				if(sx + tw > sp->w)
				{
					sx = sp->w - tw;
				}
				if(sy + th > sp->h)
				{
					sy = sp->h - th;
				}
				position_mouse(sx, sy);
				break;
			}
		}
		if(!(my_mouse_b & 1))
		{
			w = 0;
		}
		
		clear_to_color(level_screen, makecol(0, 64, 128));
//		blit(sp, char_screen, 0, 0, -ox, -oy, char_screen->w, char_screen->h);
		blit(sp, level_screen, 0, 0, -ox, -oy, sp->w, sp->h);
		if(frame % 2 == 0)
		{
			rect(level_screen, sx - ox, sy - oy, sx - ox + tw - 1, sy - oy + th - 1, makecol(255, 255, 255));
		}
//		vsync();
		blit(level_screen, screen, 0, 0, 0, 0, 640, 480);
//		frame++;
	}
	dp = create_bitmap(tw, th);
	clear_bitmap(dp);
	draw_sprite(dp, sp, -sx, -sy);
	set_mouse_range(0, 0, SCREEN_W, SCREEN_H);
	return dp;
}

int frame_menu_import(void)
{
	BITMAP * bp;
	BITMAP * nbp;
	PALETTE pal;
	char filename[1024] = {0};

	if(file_select_ex("Import", filename, "bmp;pcx;tga;lbm", 1024, 320, 240))
	{
	    need_to_save = 1;
		bp = load_bitmap(filename, pal);
		convert_palette(bp, pal, level_palette);
		if(bp->w > level->tileset->w || bp->h > level->tileset->h)
		{
			nbp = grab_frame(bp);
			animation_replace_frame(ap, selected_frame, nbp);
			destroy_bitmap(nbp);
		}
		else
		{
			animation_replace_frame(level->tileset->tile[selected_tile]->ap, selected_frame, bp);
		}
//		convert_palette(bp, pal, level_palette);
		destroy_bitmap(bp);
	}
	ani_sel_bp = NULL;
	if(ani_sel_fbp)
	{
		destroy_bitmap(ani_sel_fbp);
		ani_sel_fbp = NULL;
	}
	set_menu_edit();
	logic_frames = 0;
	return 1;
}

int frame_menu_clear(void)
{
    need_to_save = 1;
	clear_bitmap(level->tileset->tile[selected_tile]->ap->image[selected_frame]);
	ani_sel_bp = NULL;
	set_menu_edit();
	return 1;
}

int speed_menu_decrease(void)
{
    need_to_save = 1;
	level->tileset->tile[selected_tile]->ap->d++;
	return 1;
}

int speed_menu_increase(void)
{
	if(level->tileset->tile[selected_tile]->ap->d > 0)
	{
	    need_to_save = 1;
		level->tileset->tile[selected_tile]->ap->d--;
	}
	return 1;
}

int bitmap_equal(BITMAP * bp1, BITMAP * bp2)
{
    int i, j;

    if(bp1->w != bp2->w || bp1->h != bp2->h)
    {
        return 0;
    }
    for(i = 0; i < bp1->h; i++)
    {
        for(j = 0; j < bp1->w; j++)
        {
            if(bp1->line[i][j] != bp2->line[i][j])
            {
                return 0;
            }
        }
    }
    return 1;
}

int tiles_consumed(BITMAP * bp, TILESET * tp)
{
    return (bp->w / tp->w) * (bp->h / tp->h);
}

void split_bitmap(BITMAP * bp, TILESET * tp, int start, int f, int d)
{
    int i, j, k;
    BITMAP * tbp;

    tbp = create_bitmap(tp->w, tp->h);
    for(i = 0; i * tp->h < bp->h && start < TILEMAP_MAX_TILES; i++)
    {
        for(j = 0; j * tp->w < bp->w && start < TILEMAP_MAX_TILES; j++)
        {
            if(!f)
            {
                destroy_ani(tp->tile[start]->ap);
                tp->tile[start]->ap = create_ani(tp->w, tp->h);
                tp->tile[start]->ap->d = d;
            }
            clear_bitmap(tbp);
            blit(bp, tbp, j * tp->w, i * tp->h, 0, 0, tp->w, tp->h);
            while(f >= tp->tile[start]->ap->f)
            {
                animation_add_frame(tp->tile[start]->ap, tbp);
            }
            blit(bp, tp->tile[start]->ap->image[f], j * tp->w, i * tp->h, 0, 0, tp->w, tp->h);
            for(k = 0; k < TILEMAP_MAX_FLAGS; k++)
            {
                tp->tile[start]->flag[k] = 0;
            }
            start++;
        }
    }
    destroy_bitmap(tbp);
}

void split_ani(ANIMATION * ap, TILESET * tp, int start)
{
    int i, j, k;

    for(i = 0; i < ap->f; i++)
    {
        split_bitmap(ap->image[i], tp, start, i, ap->d);
    }
}

/* grab tile images from image files */
void get_tiles(char * fn, int ow)
{
    BITMAP * bp, *dbp;
    ANIMATION * ap;
    int i, j, k;
//    int start = ts_selected;
    PALETTE pal;
    char nfn[1024];

    strcpy(nfn, fn);
    for(i = 0; i < strlen(get_extension(nfn)); i++)
    {
	    tolower(get_extension(nfn)[i]);
    }

	if(!strcmp("ani", get_extension(nfn)))
	{
		ap = load_ani(fn, pal);
		for(i = 0; i < ap->f; i++)
		{
			convert_palette(ap->image[i], pal, level_palette);
		}
		if(!ow)
		{
			insert_tile(selected_tile, (ap->w / level->tileset->w + (ap->w % level->tileset->w ? 1 : 0)) * (ap->h / level->tileset->h + (ap->h % level->tileset->h ? 1 : 0)));
		}
		split_ani(ap, level->tileset, selected_tile);
		destroy_ani(ap);
	}
	else
	{
	    bp = load_bitmap(fn, pal);
       	convert_palette(bp, pal, level_palette);
	    if(bp->w <= level->tileset->w && bp->h <= level->tileset->h)
	    {
		    dbp = bp;
        }
        else
        {
		    dbp = grab_bitmap(bp);
        }
		if(!ow)
		{
			insert_tile(selected_tile, (dbp->w / level->tileset->w + (dbp->w % level->tileset->w ? 1 : 0)) * (dbp->h / level->tileset->h + (dbp->h % level->tileset->h ? 1 : 0)));
		}
        split_bitmap(dbp, level->tileset, selected_tile, 0, 0);
        if(bp == dbp)
        {
        	destroy_bitmap(bp);
       	}
        else
        {	
        	destroy_bitmap(bp);
        	destroy_bitmap(dbp);
        }
	}
}
