#include <stdio.h>
#include "includes.h"

#define PP_NEXT_FRAMES 2
#define PP_FLASH_FRAMES 8

char * pp_menu_logic_text[3] = {"Smooth", "Timed", "Fast"};
char * pp_menu_vsync_text[2] = {"Off", "On"};
char * pp_menu_game_type_text[2] = {"Normal", "Timed"};
char * pp_menu_split_mode_text[PP_SCREEN_MODES] = {"Quarter", "Half Vertical", "Half Horizontal"};
char pp_menu_ammo_respawn_text[128] = {0};
char pp_menu_power_respawn_text[128] = {0};
char pp_menu_weapon_normal_text[128] = {0};
char pp_menu_weapon_x_text[128] = {0};
char pp_menu_weapon_mine_text[128] = {0};
char pp_menu_weapon_bounce_text[128] = {0};
char pp_menu_weapon_seek_text[128] = {0};
char pp_menu_game_length_text[128] = {0};
char pp_menu_tag_length_text[128] = {0};
char pp_menu_flag_length_text[128] = {0};
char pp_menu_hunter_length_text[128] = {0};
char pp_menu_mvol_text[128] = {0};
char pp_menu_svol_text[128] = {0};
int pp_menu_bg_x = 0;
int pp_menu_bg_y = 0;
int pp_menu_idle_time = 0;

char menu_out_text[16][256] = {0};

int pp_next_menu;
int pp_next_frame;
int pp_next_reset;
int pp_next_done;
void (*pp_next_proc)() = NULL;
int pp_title_done = 0;

int pp_start_time;
//int pp_entered[4];
//int pp_started[4];
//int pp_datad[4];
int pp_waiting;
//int pp_name[4];
//int pp_character[4];
int pp_game_type;
int pp_slev = 0;
int no_entry = 0;


/*int my_popup_dialog(DIALOG * dialog)
{
	DIALOG_PLAYER * player = init_dialog(dialog, 0);

	show_mouse(screen);
	while(update_dialog(player))
	{
		ncds_update_music();
	}
	show_mouse(NULL);
	return shutdown_dialog(player);
} */

/*char * music_list_get_text(int index, int * size)
{
	if(index >= 0 && index < 1024)
	{
		return get_filename(music_file[index]);
	}
	if(index < 0)
	{
		*size = music_index;
	}
	return NULL;
} */

//DIALOG playlist_dialog[] = 
//{
//   { d_shadow_box_proc, 160, 120, 320, 240, 8, 31, 0, 0, 0, 0, NULL, NULL, NULL },
//   { d_ctext_proc, 320, 128, 320, 8, 8, 31, 0, 0, 0, 0, "Music Playlist", NULL, NULL },
//   { d_list_proc, 168, 144, 304, 224 - 32 - 40, 8, 31, 0, 0, 0, 0, music_list_get_text, music_flag, NULL },
//   { d_button_proc, 168, 144, 304, 224 - 32 - 40, 8, 31, 0, 0, 0, 0, music_list_get_text, music_flag, NULL },
//   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
//};

void pp_menu_net_type_left(void)
{
	pp_config[PP_CONFIG_NET_THROTTLE] = 1 - pp_config[PP_CONFIG_NET_THROTTLE];
}

void pp_menu_net_type_right(void)
{
	pp_config[PP_CONFIG_NET_THROTTLE] = 1 - pp_config[PP_CONFIG_NET_THROTTLE];
}

int level_supports(LEVEL * lp, int mode)
{
	int i;
	int hunt_bases = 0;
	int portals = 0, flags = 0, bases = 0, targets = 0, hunts = 0;

	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		if(lp->object_active[i])
		{
			if(lp->object_type[i] == PP_OBJECT_PORTAL)
			{
				portals++;
			}
			else if(lp->object_type[i] == PP_OBJECT_FLAG_PORTAL)
			{
				flags++;
			}
			else if(lp->object_type[i] == PP_OBJECT_BASE_PORTAL)
			{
				bases++;
			}
			else if(lp->object_type[i] == PP_OBJECT_TARGET_PORTAL)
			{
				targets++;
			}
			else if(lp->object_type[i] == PP_OBJECT_HUNT_PORTAL)
			{
				hunts++;
			}
			else if(lp->object_type[i] == PP_OBJECT_HUNT_BASE)
			{
				hunt_bases++;
			}
		}
	}
	switch(mode)
	{
		case PP_MODE_SCAVENGER:
		{
			if(hunt_bases && hunts >= 5)
			{
				return 1;
			}
			return 0;
		}
		case PP_MODE_TARGET:
		{
			if(targets > 1)
			{
				return 1;
			}
			return 0;
		}
		case PP_MODE_SPLAT:
		case PP_MODE_HUNTER:
		case PP_MODE_TAG:
		{
			if(portals >= pp_players_in())
			{
				return 1;
			}
			return 0;
		}
		case PP_MODE_FLAG:
		{
			if(portals >= pp_players_in() && flags > 0 && bases > 0)
			{
				return 1;
			}
			return 0;
		}
		default:
		{
			return 0;
		}
	}
}

void pp_create_menu(PP_MENU * mp, char * title, int x, int y, int parent)
{
	mp->items = 0;
	mp->x = x;
	mp->y = y;
	mp->parent = parent;
	mp->selected = 0;
	mp->title = title;
}

void pp_add_menu_item(PP_MENU * mp, FONT * fp, int ox, int oy, char * name, void(*left_proc)(), void(*right_proc)(), void(*enter_proc)())
{
	mp->item[mp->items].type = PP_MENU_ITEM_TYPE_NORMAL;
	mp->item[mp->items].name = name;
	mp->item[mp->items].ox = ox;
	mp->item[mp->items].oy = oy;
	mp->item[mp->items].dp = NULL;
	mp->item[mp->items].dp2 = NULL;
	mp->item[mp->items].left_proc = left_proc;
	mp->item[mp->items].right_proc = right_proc;
	mp->item[mp->items].enter_proc = enter_proc;
	mp->item[mp->items].fp = fp;
	mp->items++;
}

void pp_add_menu_flag(PP_MENU * mp, FONT * fp, int ox, int oy, char * name, int * dp, void(*left_proc)(), void(*right_proc)(), void(*enter_proc)())
{
	mp->item[mp->items].type = PP_MENU_ITEM_TYPE_FLAG;
	mp->item[mp->items].name = name;
	mp->item[mp->items].ox = ox;
	mp->item[mp->items].oy = oy;
	mp->item[mp->items].dp = dp;
	mp->item[mp->items].dp2 = NULL;
	mp->item[mp->items].left_proc = left_proc;
	mp->item[mp->items].right_proc = right_proc;
	mp->item[mp->items].enter_proc = enter_proc;
	mp->item[mp->items].fp = fp;
	mp->items++;
}

void pp_add_menu_header(PP_MENU * mp, FONT * fp, int ox, int oy, char * name)
{
	mp->item[mp->items].type = PP_MENU_ITEM_TYPE_HEADER;
	mp->item[mp->items].ox = ox;
	mp->item[mp->items].oy = oy;
	mp->item[mp->items].dp = NULL;
	mp->item[mp->items].dp2 = NULL;
	mp->item[mp->items].name = name;
	mp->item[mp->items].left_proc = NULL;
	mp->item[mp->items].right_proc = NULL;
	mp->item[mp->items].enter_proc = NULL;
	mp->item[mp->items].fp = fp;
	mp->items++;
}

void pp_add_menu_variable(PP_MENU * mp, FONT * fp, int ox, int oy, int * dp, char * dp2[])
{
	mp->item[mp->items].type = PP_MENU_ITEM_TYPE_VARIABLE;
	mp->item[mp->items].name = NULL;
	mp->item[mp->items].ox = ox;
	mp->item[mp->items].oy = oy;
	mp->item[mp->items].dp = dp;
	mp->item[mp->items].dp2 = dp2;
	mp->item[mp->items].left_proc = NULL;
	mp->item[mp->items].right_proc = NULL;
	mp->item[mp->items].enter_proc = NULL;
	mp->item[mp->items].fp = fp;
	mp->items++;
}

void pp_add_menu_string(PP_MENU * mp, FONT * fp, int ox, int oy, char * string)
{
	mp->item[mp->items].type = PP_MENU_ITEM_TYPE_STRING;
	mp->item[mp->items].name = string;
	mp->item[mp->items].ox = ox;
	mp->item[mp->items].oy = oy;
	mp->item[mp->items].dp = NULL;
	mp->item[mp->items].dp2 = NULL;
	mp->item[mp->items].left_proc = NULL;
	mp->item[mp->items].right_proc = NULL;
	mp->item[mp->items].enter_proc = NULL;
	mp->item[mp->items].fp = fp;
	mp->items++;
}

void pp_add_menu_image(PP_MENU * mp, FONT * fp, int ox, int oy, BITMAP * bp)
{
	mp->item[mp->items].type = PP_MENU_ITEM_TYPE_IMAGE;
	mp->item[mp->items].bp = bp;
	mp->item[mp->items].ox = ox;
	mp->item[mp->items].oy = oy;
	mp->item[mp->items].dp = NULL;
	mp->item[mp->items].dp2 = NULL;
	mp->item[mp->items].left_proc = NULL;
	mp->item[mp->items].right_proc = NULL;
	mp->item[mp->items].enter_proc = NULL;
	mp->item[mp->items].fp = fp;
	mp->items++;
}

void pp_center_menu(PP_MENU * mp, BITMAP * bp, int ox, int oy)
{
	int i;
	int tw;
	int lowest = mp->item[0].oy;
	int highest = mp->item[0].oy;
	int xlowest = mp->item[0].ox;
	int xhighest;

//	xhighest = xlowest + strlen(mp->item[0].name) * pp_font[PP_FONT_MENU_ITEM]->w + pp_font[PP_FONT_MENU_ITEM]->w;
	for(i = 0; i < mp->items; i++)
	{
		if(mp->item[i].oy < lowest)
		{
			lowest = mp->item[i].oy;
		}
		else if(mp->item[i].oy > highest)
		{
			highest = mp->item[i].oy + 16;
		}
	}
//	mp->x = ox;
//	mp->x = SCREEN_W / 2 - (xhighest - xlowest) / 2;
	mp->y = bp->h / 2 - (highest - lowest) / 2 - lowest;
}

void pp_menu_logic_left(void)
{
	if(pp_config[PP_CONFIG_LOGIC] > 0)
	{
		pp_config[PP_CONFIG_LOGIC]--;
		if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_TIMED)
		{
			gametime_init(60);
			logic_switch = 1;
		}
		else if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_FAST)
		{
			gametime_init(75);
			logic_switch = 1;
		}
		else
		{
			pp_config[PP_CONFIG_LOGIC] = PP_CONFIG_LOGIC_SMOOTH;
		}
	}
}

void pp_menu_logic_right(void)
{
	if(pp_config[PP_CONFIG_LOGIC] < 2)
	{
		pp_config[PP_CONFIG_LOGIC]++;
		if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_TIMED)
		{
			gametime_init(60);
			logic_switch = 1;
		}
		else if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_FAST)
		{
			gametime_init(75);
			logic_switch = 1;
		}
		else
		{
			pp_config[PP_CONFIG_LOGIC] = PP_CONFIG_LOGIC_SMOOTH;
		}
	}
}

void pp_menu_split_left(void)
{
	if(pp_config[PP_CONFIG_SPLIT_MODE] > 0)
	{
		pp_config[PP_CONFIG_SPLIT_MODE]--;
	}
}

void pp_menu_split_right(void)
{
	if(pp_config[PP_CONFIG_SPLIT_MODE] < PP_SCREEN_MODES - 1)
	{
		pp_config[PP_CONFIG_SPLIT_MODE]++;
	}
}

void pp_menu_mvol_left(void)
{
	if(pp_config[PP_CONFIG_MUSIC_VOLUME] > 0)
	{
		pp_config[PP_CONFIG_MUSIC_VOLUME] -= 5;
		ncds_set_music_volume(pp_config[PP_CONFIG_MUSIC_VOLUME]);
	}
}

void pp_menu_mvol_right(void)
{
	if(pp_config[PP_CONFIG_MUSIC_VOLUME] < 100)
	{
		pp_config[PP_CONFIG_MUSIC_VOLUME] += 5;
		ncds_set_music_volume(pp_config[PP_CONFIG_MUSIC_VOLUME]);
	}
}

void pp_menu_svol_left(void)
{
	if(pp_config[PP_CONFIG_SOUND_VOLUME] > 0)
	{
		pp_config[PP_CONFIG_SOUND_VOLUME] -= 5;
		ncds_set_sound_volume(pp_config[PP_CONFIG_SOUND_VOLUME]);
	}
}

void pp_menu_svol_right(void)
{
	if(pp_config[PP_CONFIG_SOUND_VOLUME] < 100)
	{
		pp_config[PP_CONFIG_SOUND_VOLUME] += 5;
		ncds_set_sound_volume(pp_config[PP_CONFIG_SOUND_VOLUME]);
	}
}

void pp_menu_game_type_left(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] > 0)
	{
		pp_option[PP_OPTION_GAME_TYPE]--;
	}
}

void pp_menu_game_type_right(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] < 1)
	{
		pp_option[PP_OPTION_GAME_TYPE]++;
	}
}

void pp_menu_game_length_left(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_SF_HITS] > 1)
		{
			pp_option[PP_OPTION_SF_HITS]--;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] > 3600)
		{
			pp_option[PP_OPTION_GAME_TIME] -= 3600;
		}
	}
}

void pp_menu_game_length_right(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_SF_HITS] < 100)
		{
			pp_option[PP_OPTION_SF_HITS]++;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] < 108000)
		{
			pp_option[PP_OPTION_GAME_TIME] += 3600;
		}
	}
}

void pp_menu_tag_length_left(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_T_TIME] > 3600)
		{
			pp_option[PP_OPTION_T_TIME] -= 3600;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] > 3600)
		{
			pp_option[PP_OPTION_GAME_TIME] -= 3600;
		}
	}
}

void pp_menu_tag_length_right(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_T_TIME] < 108000)
		{
			pp_option[PP_OPTION_T_TIME] += 3600;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] < 108000)
		{
			pp_option[PP_OPTION_GAME_TIME] += 3600;
		}
	}
}

void pp_menu_flag_length_left(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_F_FLAGS] > 1)
		{
			pp_option[PP_OPTION_F_FLAGS]--;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] > 3600)
		{
			pp_option[PP_OPTION_GAME_TIME] -= 3600;
		}
	}
}

void pp_menu_flag_length_right(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_F_FLAGS] < 99)
		{
			pp_option[PP_OPTION_F_FLAGS]++;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] < 108000)
		{
			pp_option[PP_OPTION_GAME_TIME] += 3600;
		}
	}
}

void pp_menu_hunter_length_left(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_H_HITS] > 1)
		{
			pp_option[PP_OPTION_H_HITS]--;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] > 3600)
		{
			pp_option[PP_OPTION_GAME_TIME] -= 3600;
		}
	}
}

void pp_menu_hunter_length_right(void)
{
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		if(pp_option[PP_OPTION_H_HITS] < 99)
		{
			pp_option[PP_OPTION_H_HITS]++;
		}
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] < 108000)
		{
			pp_option[PP_OPTION_GAME_TIME] += 3600;
		}
	}
}

void pp_menu_weapon_normal_left(void)
{
	if(pp_option[PP_OPTION_AMMO_NORMAL] > 0)
	{
		pp_option[PP_OPTION_AMMO_NORMAL]--;
	}
}

void pp_menu_weapon_normal_right(void)
{
	if(pp_option[PP_OPTION_AMMO_NORMAL] < 99)
	{
		pp_option[PP_OPTION_AMMO_NORMAL]++;
	}
}

void pp_menu_weapon_x_left(void)
{
	if(pp_option[PP_OPTION_AMMO_X] > 0)
	{
		pp_option[PP_OPTION_AMMO_X]--;
	}
}

void pp_menu_weapon_x_right(void)
{
	if(pp_option[PP_OPTION_AMMO_X] < 99)
	{
		pp_option[PP_OPTION_AMMO_X]++;
	}
}

void pp_menu_weapon_mine_left(void)
{
	if(pp_option[PP_OPTION_AMMO_MINE] > 0)
	{
		pp_option[PP_OPTION_AMMO_MINE]--;
	}
}

void pp_menu_weapon_mine_right(void)
{
	if(pp_option[PP_OPTION_AMMO_MINE] < 99)
	{
		pp_option[PP_OPTION_AMMO_MINE]++;
	}
}

void pp_menu_weapon_bounce_left(void)
{
	if(pp_option[PP_OPTION_AMMO_BOUNCE] > 0)
	{
		pp_option[PP_OPTION_AMMO_BOUNCE]--;
	}
}

void pp_menu_weapon_bounce_right(void)
{
	if(pp_option[PP_OPTION_AMMO_BOUNCE] < 99)
	{
		pp_option[PP_OPTION_AMMO_BOUNCE]++;
	}
}

void pp_menu_weapon_seek_left(void)
{
	if(pp_option[PP_OPTION_AMMO_SEEK] > 0)
	{
		pp_option[PP_OPTION_AMMO_SEEK]--;
	}
}

void pp_menu_weapon_seek_right(void)
{
	if(pp_option[PP_OPTION_AMMO_SEEK] < 99)
	{
		pp_option[PP_OPTION_AMMO_SEEK]++;
	}
}

void pp_select_menu(int num)
{
	pp_next_menu = num;
	pp_next_frame = 0;
	pp_next_reset = 1;
	pp_next_done = 0;
}

int pp_set_game_in(void)
{
	int p[2], pc;
	int i, j;
	
//	if(pp_game_data.mode != PP_MODE_1P_SCAVENGER && pp_game_data.mode != PP_MODE_1P_TARGET && pp_game_data.mode != PP_MODE_1P_TEST)
	if(pp_game_data.mode != PP_MODE_SCAVENGER && pp_game_data.mode != PP_MODE_TARGET)
	{
		if(pp_players_in() > 2 || pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_QUARTER)
		{
			pp_state = PP_STATE_GAME_IN;
			pp_game_data.player[0].screen.x = -320;
			pp_game_data.player[0].screen.y = -240;
			pp_game_data.player[0].screen.bp = pp_game_data.player[0].screen.b[PP_SCREEN_MODE_QUARTER];
			if(!player_generate_screen(&pp_game_data.player[0], pp_game_data.level))
			{
				ncds_pause_music();
				alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return 0;
			}
			pp_game_data.player[0].screen.w = 320;
			pp_game_data.player[0].screen.h = 240;
			pp_game_data.player[1].screen.x = 640;
			pp_game_data.player[1].screen.y = 480;
			pp_game_data.player[1].screen.bp = pp_game_data.player[1].screen.b[PP_SCREEN_MODE_QUARTER];
			if(!player_generate_screen(&pp_game_data.player[1], pp_game_data.level))
			{
				ncds_pause_music();
				alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return 0;
			}
			pp_game_data.player[1].screen.w = 320;
			pp_game_data.player[1].screen.h = 240;
			pp_game_data.player[2].screen.x = 640;
			pp_game_data.player[2].screen.y = -240;
			pp_game_data.player[2].screen.bp = pp_game_data.player[2].screen.b[PP_SCREEN_MODE_QUARTER];
			if(!player_generate_screen(&pp_game_data.player[2], pp_game_data.level))
			{
				ncds_pause_music();
				alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return 0;
			}
			pp_game_data.player[2].screen.w = 320;
			pp_game_data.player[2].screen.h = 240;
			pp_game_data.player[3].screen.x = -320;
			pp_game_data.player[3].screen.y = 480;
			pp_game_data.player[3].screen.bp = pp_game_data.player[3].screen.b[PP_SCREEN_MODE_QUARTER];
			if(!player_generate_screen(&pp_game_data.player[3], pp_game_data.level))
			{
				ncds_pause_music();
				alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return 0;
			}
			pp_game_data.player[3].screen.w = 320;
			pp_game_data.player[3].screen.h = 240;
		}
		else
		{
			pc = 0;
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].entered == 3)
				{
					p[pc] = i;
					pc++;
				}
				else
				{
					pp_game_data.player[i].screen.x = -320;
					pp_game_data.player[i].screen.y = -240;
				}
			}
			if(pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_VERTICAL)
			{
				pp_game_data.player[p[0]].screen.x = -320;
				pp_game_data.player[p[0]].screen.y = 0;
				pp_game_data.player[p[0]].screen.bp = pp_game_data.player[p[0]].screen.b[PP_SCREEN_MODE_VERTICAL];
				if(!player_generate_screen(&pp_game_data.player[p[0]], pp_game_data.level))
				{
					ncds_pause_music();
					alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return 0;
				}
				pp_game_data.player[p[0]].screen.w = 320;
				pp_game_data.player[p[0]].screen.h = 480;
				pp_game_data.player[p[1]].screen.x = 640;
				pp_game_data.player[p[1]].screen.y = 0;
				pp_game_data.player[p[1]].screen.bp = pp_game_data.player[p[1]].screen.b[PP_SCREEN_MODE_VERTICAL];
				if(!player_generate_screen(&pp_game_data.player[p[1]], pp_game_data.level))
				{
					ncds_pause_music();
					alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return 0;
				}
				pp_game_data.player[p[1]].screen.w = 320;
				pp_game_data.player[p[1]].screen.h = 480;
			}
			else
			{
				pp_game_data.player[p[0]].screen.x = 0;
				pp_game_data.player[p[0]].screen.y = -240;
				pp_game_data.player[p[0]].screen.bp = pp_game_data.player[p[0]].screen.b[PP_SCREEN_MODE_HORIZONTAL];
				if(!player_generate_screen(&pp_game_data.player[p[0]], pp_game_data.level))
				{
					ncds_pause_music();
					alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return 0;
				}
				pp_game_data.player[p[0]].screen.w = 640;
				pp_game_data.player[p[0]].screen.h = 240;
				pp_game_data.player[p[1]].screen.x = 0;
				pp_game_data.player[p[1]].screen.y = 480;
				pp_game_data.player[p[1]].screen.bp = pp_game_data.player[p[1]].screen.b[PP_SCREEN_MODE_HORIZONTAL];
				if(!player_generate_screen(&pp_game_data.player[p[1]], pp_game_data.level))
				{
					ncds_pause_music();
					alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return 0;
				}
				pp_game_data.player[p[1]].screen.w = 640;
				pp_game_data.player[p[1]].screen.h = 240;
			}
			pp_state = PP_STATE_GAME_IN;
		}
	}
	else
	{
		pp_game_data.player[0].screen.x = 0;
		pp_game_data.player[0].screen.y = 0;
		pp_game_data.player[0].screen.bp = pp_game_data.player[0].screen.b[PP_SCREEN_MODE_QUARTER];
		if(!player_generate_screen(&pp_game_data.player[0], pp_game_data.level))
		{
			ncds_pause_music();
			alert(NULL, "Error initializing player!", NULL, "OK", NULL, 0, 0);
			ncds_resume_music();
			pp_select_menu(PP_MENU_MAIN);
			pp_state = PP_STATE_TITLE;
			gametime_reset();
			logic_switch = 1;
			return 0;
		}
		pp_state = PP_STATE_GAME;
	}
	return 1;
}

void pp_level_ok(void)
{
	if(!pp_game_init(PP_MODE_SPLAT))
	{
		ncds_pause_music();
		alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
		ncds_resume_music();
		pp_select_menu(PP_MENU_MAIN);
		pp_state = PP_STATE_TITLE;
		gametime_reset();
		logic_switch = 1;
		return;
	}
	if(!pp_set_game_in())
	{
		set_palette(pp_palette);
		ncds_pause_music();
		alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
		ncds_resume_music();
		pp_select_menu(PP_MENU_MAIN);
		pp_state = PP_STATE_TITLE;
		gametime_reset();
		logic_switch = 1;
		return;
	}
	clear_bitmap(screen);
	pp_update();
	pp_draw();
	pp_start_fade(black_palette, pp_palette, 2);
	gametime_reset();
	logic_switch = 1;
}

void pp_menu_main_play(void)
{
	int i;
	int c = 1;
	
	pp_select_menu(PP_MENU_PLAY);
}

	void pp_menu_play_1p_char(void)
	{
		int i;
		
		pp_state = PP_STATE_CHAR;
		single_player = 1;
		pp_game_data.player[0].entered = 1;
		pp_game_data.player[1].entered = 0;
		pp_game_data.player[2].entered = 0;
		pp_game_data.player[3].entered = 0;
		pp_game_data.player[0].data_pick = pp_config[PP_CONFIG_P_P1];
		pp_game_data.frame = 0;
		pp_game_data.player[0].screen.x = 160;
		pp_game_data.player[0].screen.y = 120;
		pp_game_data.player[0].screen.w = 320;
		pp_game_data.player[0].screen.h = 240;
		for(i = 0; i < 4; i++)
		{
			if(pp_game_data.player[i].char_pick >= char_entries)
			{
				if(exists(pp_title_char_name[i]))
				{
					if(!load_char_ns(&pp_game_data.player[i], pp_title_char_name[i]))
					{
						ncds_pause_music();
						alert(NULL, "Cannot load character!", NULL, "OK", NULL, 0, 0);
						ncds_resume_music();
						gametime_reset();
						logic_switch = 1;
					}
					else
					{
						pp_game_data.frame = 0;
						if(pp_game_data.player[i].char_pick >= char_entries)
						{
							pp_game_data.player[i].char_pick = char_entries + 1;
						}
						strcpy(pp_game_data.player[i].name, get_filename(pp_title_char_name[i]));
						delete_extension(pp_game_data.player[i].name);
					}
				}
				else
				{
					pp_game_data.player[i].char_pick = i;
				}
			}
		}
	}

	void pp_menu_play_1p(void)
	{
//		pp_select_menu(PP_MENU_PLAY_1P);
		pp_select_menu(PP_MENU_BLANK);
		pp_next_proc = pp_menu_play_1p_char;
	}

		void pp_menu_play_1p_scavenger_proc(void)
		{
			if(!pp_game_init(PP_MODE_SCAVENGER))
			{
				ncds_pause_music();
				alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return;
			}
			if(!pp_set_game_in())
			{
				set_palette(pp_palette);
				ncds_pause_music();
				alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return;
			}
			clear_bitmap(screen);
			pp_update();
			pp_draw();
			pp_start_fade(black_palette, pp_palette, 2);
			gametime_reset();
			logic_switch = 1;
		}

		void pp_menu_play_1p_scavenger(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_SCAVENGER))
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_1p_scavenger_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

		void pp_menu_play_1p_target_proc(void)
		{
			if(!pp_game_init(PP_MODE_TARGET))
			{
				ncds_pause_music();
				alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return;
			}
			set_default_game_options();
			if(!pp_set_game_in())
			{
				set_palette(pp_palette);
				ncds_pause_music();
				alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return;
			}
			clear_bitmap(screen);
			pp_update();
			pp_draw();
			pp_start_fade(black_palette, pp_palette, 2);
			gametime_reset();
			logic_switch = 1;
		}

		void pp_menu_play_1p_target(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_TARGET))
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_1p_target_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

		void pp_menu_play_1p_test_proc(void)
		{
			pp_game_data.player[1].entered = 3;
//			pp_started[1] = 1;
			strcpy(pp_title_char_name[1], char_entry[rand() % char_entries].file);
			load_char(&pp_game_data.player[1], pp_title_char_name[1]);
//			pp_game_data.player[1].ai_flag = 1;
			pp_game_data.frame = 0;
			if(!pp_game_init(PP_MODE_HUNTER))
			{
				ncds_pause_music();
				alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return;
			}
			set_default_game_options();
			if(!pp_set_game_in())
			{
				set_palette(pp_palette);
				ncds_pause_music();
				alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				pp_select_menu(PP_MENU_MAIN);
				pp_state = PP_STATE_TITLE;
				gametime_reset();
				logic_switch = 1;
				return;
			}
			clear_bitmap(screen);
			pp_update();
			pp_draw();
			pp_start_fade(black_palette, pp_palette, 2);
			gametime_reset();
			logic_switch = 1;
		}

		void pp_menu_play_1p_test(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_SPLAT))
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_1p_test_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

	void pp_menu_play_2p_char(void)
	{
		int i;
		
		pp_state = PP_STATE_CHAR;
		single_player = 0;
		pp_game_data.player[0].entered = 0;
		pp_game_data.player[1].entered = 0;
		pp_game_data.player[2].entered = 0;
		pp_game_data.player[3].entered = 0;
//		pp_entered[0] = 0;
//		pp_entered[1] = 0;
//		pp_entered[2] = 0;
//		pp_entered[3] = 0;
//		pp_started[0] = 0;
//		pp_started[1] = 0;
//		pp_started[2] = 0;
//		pp_started[3] = 0;
//		pp_datad[0] = 0;
//		pp_datad[1] = 0;
//		pp_datad[2] = 0;
//		pp_datad[3] = 0;
//		pp_character[0] = 0;
//		pp_character[1] = 1;
//		pp_character[2] = 2;
//		pp_character[3] = 3;
		pp_game_data.player[0].data_pick = pp_config[PP_CONFIG_P_P1];
		pp_game_data.player[1].data_pick = pp_config[PP_CONFIG_P_P2];
		pp_game_data.player[2].data_pick = pp_config[PP_CONFIG_P_P3];
		pp_game_data.player[3].data_pick = pp_config[PP_CONFIG_P_P4];
		pp_start_time = 301;
		pp_game_data.frame = 0;
		pp_game_data.player[0].screen.x = 0;
		pp_game_data.player[0].screen.y = 0;
		pp_game_data.player[0].screen.w = 320;
		pp_game_data.player[0].screen.h = 240;
		pp_game_data.player[1].screen.x = 320;
		pp_game_data.player[1].screen.y = 240;
		pp_game_data.player[1].screen.w = 320;
		pp_game_data.player[1].screen.h = 240;
		pp_game_data.player[2].screen.x = 320;
		pp_game_data.player[2].screen.y = 0;
		pp_game_data.player[2].screen.w = 320;
		pp_game_data.player[2].screen.h = 240;
		pp_game_data.player[3].screen.x = 0;
		pp_game_data.player[3].screen.y = 240;
		pp_game_data.player[3].screen.w = 320;
		pp_game_data.player[3].screen.h = 240;
		for(i = 0; i < 4; i++)
		{
			if(pp_game_data.player[i].char_pick >= char_entries)
			{
				if(exists(pp_title_char_name[i]))
				{
					if(!load_char_ns(&pp_game_data.player[i], pp_title_char_name[i]))
					{
						ncds_pause_music();
						alert(NULL, "Cannot load level!", NULL, "OK", NULL, 0, 0);
						ncds_resume_music();
						gametime_reset();
						logic_switch = 1;
					}
					else
					{
						pp_game_data.frame = 0;
						pp_game_data.player[i].char_pick = char_entries + 1;
						strcpy(pp_game_data.player[i].name, get_filename(pp_title_char_name[i]));
						delete_extension(pp_game_data.player[i].name);
					}
				}
				else
				{
					pp_game_data.player[i].char_pick = i;
				}
			}
		}
	}

	void pp_menu_play_2p(void)
	{
		pp_select_menu(PP_MENU_BLANK);
		pp_next_proc = pp_menu_play_2p_char;
	}

		void pp_menu_play_2p_splat(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_SPLAT))
			{
				pp_select_menu(PP_MENU_PLAY_2P_SPLAT_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_2p_splat_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_2P_SPLAT_SETUP);
			}

			void pp_menu_play_2p_splat_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_SPLAT))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_2p_splat_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_2p_splat_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_2p_hunter(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_HUNTER))
			{
				pp_select_menu(PP_MENU_PLAY_2P_HUNTER_SETUP);
				pp_option[PP_OPTION_GAME_TYPE] = PP_OPTION_GAME_NORMAL;
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_2p_hunter_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_2P_HUNTER_SETUP);
			}

			void pp_menu_play_2p_hunter_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_HUNTER))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_2p_hunter_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_2p_hunter_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_2p_tag(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_TAG))
			{
				pp_select_menu(PP_MENU_PLAY_2P_TAG_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_2p_tag_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_2P_TAG_SETUP);
			}

			void pp_menu_play_2p_tag_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_TAG))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_2p_tag_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_2p_tag_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_2p_flag(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_FLAG))
			{
				pp_select_menu(PP_MENU_PLAY_2P_FLAG_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_2p_flag_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_2P_FLAG_SETUP);
			}

			void pp_menu_play_2p_flag_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_FLAG))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_2p_flag_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_2p_flag_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

	void pp_menu_play_3p(void)
	{
		pp_select_menu(PP_MENU_PLAY_3P);
	}

		void pp_menu_play_3p_splat(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_SPLAT))
			{
				pp_select_menu(PP_MENU_PLAY_3P_SPLAT_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_3p_splat_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_3P_SPLAT_SETUP);
			}

			void pp_menu_play_3p_splat_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_SPLAT))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_3p_splat_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_3p_splat_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_3p_hunter(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_HUNTER))
			{
				pp_select_menu(PP_MENU_PLAY_3P_HUNTER_SETUP);
				pp_option[PP_OPTION_GAME_TYPE] = PP_OPTION_GAME_NORMAL;
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_3p_hunter_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_3P_HUNTER_SETUP);
			}

			void pp_menu_play_3p_hunter_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_HUNTER))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_3p_hunter_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_3p_hunter_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_3p_tag(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_TAG))
			{
				pp_select_menu(PP_MENU_PLAY_3P_TAG_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_3p_tag_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_3P_TAG_SETUP);
			}

			void pp_menu_play_3p_tag_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_TAG))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_3p_tag_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_3p_tag_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_3p_flag(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_FLAG))
			{
				pp_select_menu(PP_MENU_PLAY_3P_FLAG_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_3p_flag_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_3P_FLAG_SETUP);
			}

			void pp_menu_play_3p_flag_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_FLAG))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_3p_flag_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_3p_flag_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

	void pp_menu_play_4p(void)
	{
		pp_select_menu(PP_MENU_PLAY_4P);
	}

		void pp_menu_play_4p_splat(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_SPLAT))
			{
				pp_select_menu(PP_MENU_PLAY_4P_SPLAT_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_4p_splat_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_4P_SPLAT_SETUP);
			}

			void pp_menu_play_4p_splat_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_SPLAT))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_4p_splat_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_4p_splat_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_4p_hunter(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_HUNTER))
			{
				pp_select_menu(PP_MENU_PLAY_4P_HUNTER_SETUP);
				pp_option[PP_OPTION_GAME_TYPE] = PP_OPTION_GAME_NORMAL;
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_4p_hunter_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_4P_HUNTER_SETUP);
			}

			void pp_menu_play_4p_hunter_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_HUNTER))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_4p_hunter_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_4p_hunter_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_4p_tag(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_TAG))
			{
				pp_select_menu(PP_MENU_PLAY_4P_TAG_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_4p_tag_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_4P_TAG_SETUP);
			}

			void pp_menu_play_4p_tag_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_TAG))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_4p_tag_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_4p_tag_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}

		void pp_menu_play_4p_flag(void)
		{
			if(level_supports(pp_game_data.level, PP_MODE_FLAG))
			{
				pp_select_menu(PP_MENU_PLAY_4P_FLAG_SETUP);
			}
			else
			{
				ncds_pause_music();
				alert(NULL, "Mode not supported!", NULL, "OK", NULL, 0, 0);
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
			}
		}

			void pp_menu_play_4p_flag_setup(void)
			{
				pp_select_menu(PP_MENU_PLAY_4P_FLAG_SETUP);
			}

			void pp_menu_play_4p_flag_start_proc(void)
			{
				if(!pp_game_init(PP_MODE_FLAG))
				{
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				if(!pp_set_game_in())
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Game could not be started!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					pp_select_menu(PP_MENU_MAIN);
					pp_state = PP_STATE_TITLE;
					gametime_reset();
					logic_switch = 1;
					return;
				}
				clear_bitmap(screen);
				pp_update();
				pp_draw();
//				set_palette(pp_palette);
				pp_start_fade(black_palette, pp_palette, 2);
				gametime_reset();
				logic_switch = 1;
			}

			void pp_menu_play_4p_flag_start(void)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_menu_play_4p_flag_start_proc;
				pp_start_fade(pp_palette, black_palette, 2);
			}


	void pp_menu_start_replay_proc(void)
	{
		if(pp_replay_start_play(pp_replay_name, 0))
		{
			pp_state = PP_STATE_REPLAY;
			pp_update();
			pp_draw();
			pp_start_fade(black_palette, pp_palette, 2);
		}
		else
		{
			set_palette(pp_palette);
			ncds_pause_music();
			alert(NULL, "Cannot play replay!", NULL, "OK", NULL, 0, 0);
			ncds_resume_music();
			pp_select_menu(PP_MENU_PLAY);
			pp_next_reset = 0;
			gametime_reset();
			logic_switch = 1;
		}
	}

	void pp_menu_play_replay_proc(void)
	{
		ncds_pause_music();
		if(file_select_ex("Choose Replay", pp_replay_name, "ppr", 1024, 320, 240))
		{
			ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
			pp_select_menu(PP_MENU_BLANK);
//			pp_next_proc = pp_menu_start_replay_proc;
			pp_fade(pp_palette, black_palette, 2);
			pp_menu_start_replay_proc();
		}
		else
		{
			ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
			pp_select_menu(PP_MENU_PLAY);
			pp_next_reset = 0;
			ncds_resume_music();
		}
		gametime_reset();
		logic_switch = 1;
		key[KEY_ENTER] = 0;
		key[KEY_ESC] = 0;
	}

	void pp_menu_play_replay(void)
	{
//		pp_select_menu(PP_MENU_PLAY_2P);
		pp_select_menu(PP_MENU_BLANK);
		pp_next_proc = pp_menu_play_replay_proc;
	}
	void pp_menu_play_stats_proc(void)
	{
		pp_state = PP_STATE_MENU_STATS;
		pp_stat = -1;
	}

	void pp_menu_play_stats(void)
	{
		pp_select_menu(PP_MENU_BLANK);
		pp_next_proc = pp_menu_play_stats_proc;
	}
	
	void pp_menu_new_player_proc(void)
	{
		pp_state = PP_STATE_NEW_PLAYER;
		strcpy(pp_new_data_text, "");
		pp_new_data_pos = 0;
		pp_new_data_key = 0x0;
		clear_keybuf();
	}
	
	void pp_menu_new_player(void)
	{
		pp_select_menu(PP_MENU_BLANK);
		pp_next_proc = pp_menu_new_player_proc;
		pp_state = PP_STATE_TITLE;
	}

void pp_menu_main_options(void)
{
	pp_select_menu(PP_MENU_OPTIONS);
}

	void pp_menu_options_graphics(void)
	{
		pp_select_menu(PP_MENU_OPTIONS_GRAPHICS);
	}

	void pp_menu_options_audio(void)
	{
		pp_select_menu(PP_MENU_OPTIONS_AUDIO);
	}

	void pp_menu_options_controls(void)
	{
		pp_select_menu(PP_MENU_CONTROLS);
	}

		void pp_menu_options_controls_custom(void)
		{
			pp_select_menu(PP_MENU_CONTROLS_CUSTOM);
		}

			void pp_menu_options_joy_check(void)
			{
				if(pp_config[PP_CONFIG_JOY])
				{
					ncd_joy_install();
				}
				else
				{
					ncd_joy_remove();
				}
				pp_set_controllers();
			}

			void pp_menu_options_controls_custom_p1(void)
			{
				pp_select_menu(PP_MENU_CONTROLS_CUSTOM_P1);
			}

			void pp_menu_options_controls_custom_p2(void)
			{
				pp_select_menu(PP_MENU_CONTROLS_CUSTOM_P2);
			}

			void pp_menu_options_controls_custom_p3(void)
			{
				pp_select_menu(PP_MENU_CONTROLS_CUSTOM_P3);
			}

			void pp_menu_options_controls_custom_p4(void)
			{
				pp_select_menu(PP_MENU_CONTROLS_CUSTOM_P4);
			}

				/* menu variable must be added immediately after menu item for this to work */
				void pp_menu_options_controls_custom_set(void)
				{
					int new_key;
					int old_key = *pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected + 1].dp;

					*pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected + 1].dp = 0;
					pp_update();
					pp_draw();
					new_key = get_key_all();
					if(new_key != KEY_ESC)
					{
						*pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected + 1].dp = new_key;
					}
					else
					{
						*pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected + 1].dp = old_key;
					}
					junk_controller(&pp_game_data.player[0].controller);
					gametime_reset();
					logic_switch = 1;
				}

	void pp_menu_options_misc(void)
	{
		pp_select_menu(PP_MENU_MISC);
	}

void pp_menu_weapons(void)
{
	pp_menu[PP_MENU_WEAPONS].parent = pp_current_menu;
	pp_select_menu(PP_MENU_WEAPONS);
}

void pp_menu_powerups(void)
{
	pp_menu[PP_MENU_POWERUPS].parent = pp_current_menu;
	pp_select_menu(PP_MENU_POWERUPS);
}

	void pp_menu_powerups_ammo(void)
	{
//		pp_menu[PP_MENU_POWERUPS].parent = pp_current_menu;
		pp_select_menu(PP_MENU_AMMO);
	}

	void pp_menu_powerups_special(void)
	{
//		pp_menu[PP_MENU_POWERUPS].parent = pp_current_menu;
		pp_select_menu(PP_MENU_SPECIAL);
	}

void pp_menu_ammo_time_left(void)
{
	if(pp_option[PP_OPTION_AMMO_TIME] > 0)
	{
		pp_option[PP_OPTION_AMMO_TIME] -= 900;
	}
}

void pp_menu_ammo_time_right(void)
{
	if(pp_option[PP_OPTION_AMMO_TIME] < 1800)
	{
		pp_option[PP_OPTION_AMMO_TIME] += 900;
	}
}

void pp_menu_power_time_left(void)
{
	if(pp_option[PP_OPTION_POWER_TIME] > 0)
	{
		pp_option[PP_OPTION_POWER_TIME] -= 900;
	}
}

void pp_menu_power_time_right(void)
{
	if(pp_option[PP_OPTION_POWER_TIME] < 1800)
	{
		pp_option[PP_OPTION_POWER_TIME] += 900;
	}
}

void pp_menu_main_quit_proc(void)
{
	pp_quit = 1;
//	pp_fade(pp_palette, black_palette, 2);
	pp_start_fade(pp_palette, black_palette, 2);
}

	void pp_menu_main_quit(void)
	{
		pp_select_menu(PP_MENU_BLANK);
		pp_next_proc = pp_menu_main_quit_proc;
	}
	
void pp_create_done_menu(void)
{
	pp_create_menu(&pp_menu[PP_MENU_DONE], "Choice", 0, 0, PP_MENU_DONE);
	pp_add_menu_item(&pp_menu[PP_MENU_DONE], al_font[1], 16, 0, "Restart", NULL, NULL, pp_done_no);
	if(pp_config[PP_CONFIG_REPLAY])
	{
		pp_add_menu_item(&pp_menu[PP_MENU_DONE], al_font[1], 16, 32, "Save Replay", NULL, NULL, pp_instant_replay);
		pp_add_menu_item(&pp_menu[PP_MENU_DONE], al_font[1], 16, 64, "Quit", NULL, NULL, pp_quit_yes);
	}
	else
	{
		pp_add_menu_item(&pp_menu[PP_MENU_DONE], al_font[1], 16, 32, "Quit", NULL, NULL, pp_quit_yes);
	}
	pp_center_menu(&pp_menu[PP_MENU_DONE], pp_screen, 16, 0);
}

/*void pp_menu_playlist(void)
{
	ncds_pause_music();
	my_popup_dialog(playlist_dialog);
	ncds_resume_music();
} */

void pp_title_init(void)
{
	int i;

	gui_bg_color = 23;
	gui_mg_color = 12;
	gui_fg_color = 2;

	pp_create_menu(&pp_menu[PP_MENU_BLANK], "", 0, 0, PP_MENU_BLANK);

	/* create main menu */
	pp_create_menu(&pp_menu[PP_MENU_MAIN], "Paintball Party", 0, 0, PP_MENU_MAIN);
	pp_add_menu_item(&pp_menu[PP_MENU_MAIN], al_font[1], 16 + 16, 0, "Play", NULL, NULL, pp_menu_main_play);
	pp_add_menu_item(&pp_menu[PP_MENU_MAIN], al_font[1], 16 + 16, 32, "Options", NULL, NULL, pp_menu_main_options);
	pp_add_menu_item(&pp_menu[PP_MENU_MAIN], al_font[1], 16 + 16, 64, "Quit", NULL, NULL, pp_menu_main_quit);

	/* create play menu */
	pp_create_menu(&pp_menu[PP_MENU_PLAY], "Play", 0, 0, PP_MENU_MAIN);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY], al_font[1], 16, 0, "Single Player", NULL, NULL, pp_menu_play_1p);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY], al_font[1], 16, 32, "Multi-Player", NULL, NULL, pp_menu_play_2p);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY], al_font[1], 16, 64, "Replay", NULL, NULL, pp_menu_play_replay);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY], al_font[1], 16, 96, "Stats", NULL, NULL, pp_menu_play_stats);
	
	/* create 1 player menu */
	pp_create_menu(&pp_menu[PP_MENU_PLAY_1P], "1 Player", 0, 0, PP_MENU_PLAY);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_1P], al_font[1], 16, 0, "Scavenger Hunt", NULL, NULL, pp_menu_play_1p_scavenger);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_1P], al_font[1], 16, 32, "Target Practice", NULL, NULL, pp_menu_play_1p_target);
//	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_1P], al_font[1], 16, 64, "AI Test", NULL, NULL, pp_menu_play_1p_test);

	/* create 2 player menu */
	pp_create_menu(&pp_menu[PP_MENU_PLAY_2P], "2 Players", 0, 0, PP_MENU_PLAY);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P], al_font[1], 16, 0, "Splat Fest", NULL, NULL, pp_menu_play_2p_splat);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P], al_font[1], 16, 32, "Eliminator", NULL, NULL, pp_menu_play_2p_hunter);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P], al_font[1], 16, 64, "Paint Tag", NULL, NULL, pp_menu_play_2p_tag);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P], al_font[1], 16, 96, "Capture the Flag", NULL, NULL, pp_menu_play_2p_flag);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], "Splat Fest", 0, 0, PP_MENU_PLAY_2P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], al_font[1], 16, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_game_length_left, pp_menu_game_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], al_font[1], 32, 192, pp_menu_game_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_SPLAT_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_2p_splat_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_2P_HUNTER_SETUP], "Eliminator", 0, 0, PP_MENU_PLAY_2P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_HUNTER_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_HUNTER_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_HUNTER_SETUP], al_font[1], 16, 80, "Game Length", pp_menu_hunter_length_left, pp_menu_hunter_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_2P_HUNTER_SETUP], al_font[1], 32, 112, pp_menu_hunter_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_HUNTER_SETUP], al_font[1], 32, 160, "Start", NULL, NULL, pp_menu_play_2p_hunter_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], "Paint Tag", 0, 0, PP_MENU_PLAY_2P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_tag_length_left, pp_menu_tag_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], al_font[1], 32, 192, pp_menu_tag_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_TAG_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_2p_tag_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], "Capture the Flag", 0, 0, PP_MENU_PLAY_2P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_flag_length_left, pp_menu_flag_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], al_font[1], 32, 192, pp_menu_flag_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_2P_FLAG_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_2p_flag_start);

	/* create 3 player menu */
	pp_create_menu(&pp_menu[PP_MENU_PLAY_3P], "3 Players", 0, 0, PP_MENU_PLAY);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P], al_font[1], 16, 0, "Splat Fest", NULL, NULL, pp_menu_play_3p_splat);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P], al_font[1], 16, 32, "Eliminator", NULL, NULL, pp_menu_play_3p_hunter);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P], al_font[1], 16, 64, "Paint Tag", NULL, NULL, pp_menu_play_3p_tag);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P], al_font[1], 16, 96, "Capture the Flag", NULL, NULL, pp_menu_play_3p_flag);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], "Splat Fest", 0, 0, PP_MENU_PLAY_3P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_game_length_left, pp_menu_game_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], al_font[1], 32, 192, pp_menu_game_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_SPLAT_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_3p_splat_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_3P_HUNTER_SETUP], "Eliminator", 0, 0, PP_MENU_PLAY_3P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_HUNTER_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_HUNTER_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_HUNTER_SETUP], al_font[1], 16, 80, "Game Length", pp_menu_hunter_length_left, pp_menu_hunter_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_3P_HUNTER_SETUP], al_font[1], 32, 112, pp_menu_hunter_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_HUNTER_SETUP], al_font[1], 32, 160, "Start", NULL, NULL, pp_menu_play_3p_hunter_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], "Paint Tag", 0, 0, PP_MENU_PLAY_3P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_tag_length_left, pp_menu_tag_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], al_font[1], 32, 192, pp_menu_tag_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_TAG_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_3p_tag_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], "Capture the Flag", 0, 0, PP_MENU_PLAY_3P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_flag_length_left, pp_menu_flag_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], al_font[1], 32, 192, pp_menu_flag_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_3P_FLAG_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_3p_flag_start);

	/* create 4 player menu */
	pp_create_menu(&pp_menu[PP_MENU_PLAY_4P], "4 Players", 0, 0, PP_MENU_PLAY);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P], al_font[1], 16, 0, "Splat Fest", NULL, NULL, pp_menu_play_4p_splat);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P], al_font[1], 16, 32, "Eliminator", NULL, NULL, pp_menu_play_4p_hunter);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P], al_font[1], 16, 64, "Paint Tag", NULL, NULL, pp_menu_play_4p_tag);
	pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P], al_font[1], 16, 96, "Capture the Flag", NULL, NULL, pp_menu_play_4p_flag);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], "Splat Fest", 0, 0, PP_MENU_PLAY_4P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_game_length_left, pp_menu_game_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], al_font[1], 32, 192, pp_menu_game_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_SPLAT_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_4p_splat_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_4P_HUNTER_SETUP], "Eliminator", 0, 0, PP_MENU_PLAY_4P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_HUNTER_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_HUNTER_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_HUNTER_SETUP], al_font[1], 16, 80, "Game Length", pp_menu_hunter_length_left, pp_menu_hunter_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_4P_HUNTER_SETUP], al_font[1], 32, 112, pp_menu_hunter_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_HUNTER_SETUP], al_font[1], 32, 160, "Start", NULL, NULL, pp_menu_play_4p_hunter_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], "Paint Tag", 0, 0, PP_MENU_PLAY_4P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_tag_length_left, pp_menu_tag_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], al_font[1], 32, 192, pp_menu_tag_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_TAG_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_4p_tag_start);

		pp_create_menu(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], "Capture the Flag", 0, 0, PP_MENU_PLAY_4P);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], al_font[1], 16, 0, "Stash", NULL, NULL, pp_menu_weapons);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], al_font[1], 16, 32, "Powerups", NULL, NULL, pp_menu_powerups);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], al_font[1], 16, 80, "Game Type", pp_menu_game_type_left, pp_menu_game_type_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], al_font[1], 32, 112, &pp_option[PP_OPTION_GAME_TYPE], pp_menu_game_type_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], al_font[1], 16, 160, "Game Length", pp_menu_flag_length_left, pp_menu_flag_length_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], al_font[1], 32, 192, pp_menu_flag_length_text);
		pp_add_menu_item(&pp_menu[PP_MENU_PLAY_4P_FLAG_SETUP], al_font[1], 32, 240, "Start", NULL, NULL, pp_menu_play_4p_flag_start);

	pp_create_menu(&pp_menu[PP_MENU_OPTIONS], "Options", 0, 0, PP_MENU_MAIN);
	pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS], al_font[1], 16, 0, "Graphics", NULL, NULL, pp_menu_options_graphics);
	pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS], al_font[1], 16, 32, "Audio", NULL, NULL, pp_menu_options_audio);
	pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS], al_font[1], 16, 64, "Controls", NULL, NULL, pp_menu_options_controls);
	pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS], al_font[1], 16, 96, "Miscellaneous", NULL, NULL, pp_menu_options_misc);

		pp_create_menu(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], "Graphics", 0, 0, PP_MENU_OPTIONS);
		pp_add_menu_flag(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 16, 0, "Allow 320x240", &pp_config[PP_CONFIG_LOWRES], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 16, 32, "Translucency", &pp_config[PP_CONFIG_TRANS], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 16, 64, "Vsync", &pp_config[PP_CONFIG_VSYNC], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 16, 96, "Icons", &pp_config[PP_CONFIG_ICONS], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 16, 128, "Radar", &pp_config[PP_CONFIG_RADAR], NULL, NULL, NULL);
		pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 16, 176, "Logic Method", pp_menu_logic_left, pp_menu_logic_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 32, 208, &pp_config[PP_CONFIG_LOGIC], pp_menu_logic_text);
		pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 16, 256, "2P Split Screen", pp_menu_split_left, pp_menu_split_right, NULL);
		pp_add_menu_variable(&pp_menu[PP_MENU_OPTIONS_GRAPHICS], al_font[1], 32, 288, &pp_config[PP_CONFIG_SPLIT_MODE], pp_menu_split_mode_text);

		pp_create_menu(&pp_menu[PP_MENU_OPTIONS_AUDIO], "Audio", 0, 0, PP_MENU_OPTIONS);
		pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS_AUDIO], al_font[1], 16, 0, "Music Volume", pp_menu_mvol_left, pp_menu_mvol_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_OPTIONS_AUDIO], al_font[1], 32, 32, pp_menu_mvol_text);
		pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS_AUDIO], al_font[1], 16, 80, "Sound Volume", pp_menu_svol_left, pp_menu_svol_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_OPTIONS_AUDIO], al_font[1], 32, 112, pp_menu_svol_text);
//		pp_add_menu_item(&pp_menu[PP_MENU_OPTIONS_AUDIO], al_font[1], 16, 144, "Playlist", NULL, NULL, pp_menu_playlist);

		pp_create_menu(&pp_menu[PP_MENU_CONTROLS], "Controls", 0, 0, PP_MENU_OPTIONS);
		pp_add_menu_flag(&pp_menu[PP_MENU_CONTROLS], al_font[1], 16, 0, "Joystick", &pp_config[PP_CONFIG_JOY], NULL, NULL, pp_menu_options_joy_check);
		pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS], al_font[1], 16, 48, "Player 1", NULL, NULL, pp_menu_options_controls_custom_p1);
		pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS], al_font[1], 16, 80, "Player 2", NULL, NULL, pp_menu_options_controls_custom_p2);
		pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS], al_font[1], 16, 112, "Player 3", NULL, NULL, pp_menu_options_controls_custom_p3);
		pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS], al_font[1], 16, 144, "Player 4", NULL, NULL, pp_menu_options_controls_custom_p4);

			pp_create_menu(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], "Customize", 0, 0, PP_MENU_CONTROLS);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 0, "Move Left", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 20, &pp_game_data.player[0].controller.left_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 50, "Move Right", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 70, &pp_game_data.player[0].controller.right_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 100, "Look Up", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 120, &pp_game_data.player[0].controller.up_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 150, "Duck", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 170, &pp_game_data.player[0].controller.down_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 200, "Fire", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 220, &pp_game_data.player[0].controller.fire_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 250, "Jump", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 270, &pp_game_data.player[0].controller.jump_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 300, "Select Weapon", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 320, &pp_game_data.player[0].controller.select_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 16, 350, "Option", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P1], al_font[2], 32, 370, &pp_game_data.player[0].controller.option_key, key_names);

			pp_create_menu(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], "Customize", 0, 0, PP_MENU_CONTROLS);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 0, "Move Left", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 20, &pp_game_data.player[1].controller.left_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 50, "Move Right", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 70, &pp_game_data.player[1].controller.right_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 100, "Look Up", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 120, &pp_game_data.player[1].controller.up_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 150, "Duck", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 170, &pp_game_data.player[1].controller.down_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 200, "Fire", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 220, &pp_game_data.player[1].controller.fire_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 250, "Jump", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 270, &pp_game_data.player[1].controller.jump_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 300, "Select Weapon", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 320, &pp_game_data.player[1].controller.select_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 16, 350, "Option", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P2], al_font[2], 32, 370, &pp_game_data.player[1].controller.option_key, key_names);

			pp_create_menu(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], "Customize", 0, 0, PP_MENU_CONTROLS);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 0, "Move Left", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 20, &pp_game_data.player[2].controller.left_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 50, "Move Right", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 70, &pp_game_data.player[2].controller.right_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 100, "Look Up", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 120, &pp_game_data.player[2].controller.up_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 150, "Duck", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 170, &pp_game_data.player[2].controller.down_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 200, "Fire", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 220, &pp_game_data.player[2].controller.fire_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 250, "Jump", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 270, &pp_game_data.player[2].controller.jump_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 300, "Select Weapon", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 320, &pp_game_data.player[2].controller.select_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 16, 350, "Option", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P3], al_font[2], 32, 370, &pp_game_data.player[2].controller.option_key, key_names);

			pp_create_menu(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], "Customize", 0, 0, PP_MENU_CONTROLS);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 0, "Move Left", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 20, &pp_game_data.player[3].controller.left_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 50, "Move Right", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 70, &pp_game_data.player[3].controller.right_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 100, "Look Up", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 120, &pp_game_data.player[3].controller.up_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 150, "Duck", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 170, &pp_game_data.player[3].controller.down_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 200, "Fire", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 220, &pp_game_data.player[3].controller.fire_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 250, "Jump", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 270, &pp_game_data.player[3].controller.jump_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 300, "Select Weapon", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 320, &pp_game_data.player[3].controller.select_key, key_names);
			pp_add_menu_item(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 16, 350, "Option", NULL, NULL, pp_menu_options_controls_custom_set);
			pp_add_menu_variable(&pp_menu[PP_MENU_CONTROLS_CUSTOM_P4], al_font[2], 32, 370, &pp_game_data.player[3].controller.option_key, key_names);

		pp_create_menu(&pp_menu[PP_MENU_MISC], "Miscellaneous", 0, 0, PP_MENU_OPTIONS);
		pp_add_menu_flag(&pp_menu[PP_MENU_MISC], al_font[1], 32, 0, "Random Music", &pp_config[PP_CONFIG_RANDOM_MUSIC], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_MISC], al_font[1], 32, 32, "Allow Replays", &pp_config[PP_CONFIG_REPLAY], NULL, NULL, pp_create_done_menu);
		pp_add_menu_flag(&pp_menu[PP_MENU_MISC], al_font[1], 32, 64, "Default Music", &pp_config[PP_CONFIG_DMUSIC], NULL, NULL, pp_build_music_list);
		pp_add_menu_flag(&pp_menu[PP_MENU_MISC], al_font[1], 32, 96, "Auto Targeting", &pp_option[PP_OPTION_AUTO_TARGET], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_MISC], al_font[1], 32, 128, "Character Themes", &pp_config[PP_CONFIG_CHAR_THEMES], NULL, NULL, pp_build_music_list);

	/* weapons menu */
	pp_create_menu(&pp_menu[PP_MENU_WEAPONS], "Ammo Stash", 0, 0, PP_MENU_MAIN);
	pp_add_menu_item(&pp_menu[PP_MENU_WEAPONS], al_font[1], 16, 0, "Normal", pp_menu_weapon_normal_left, pp_menu_weapon_normal_right, NULL);
	pp_add_menu_variable(&pp_menu[PP_MENU_WEAPONS], al_font[1], 32, 32, &pp_option[PP_OPTION_AMMO_NORMAL], NULL);
	pp_add_menu_item(&pp_menu[PP_MENU_WEAPONS], al_font[1], 16, 80, "Splitter", pp_menu_weapon_x_left, pp_menu_weapon_x_right, NULL);
	pp_add_menu_variable(&pp_menu[PP_MENU_WEAPONS], al_font[1], 32, 112, &pp_option[PP_OPTION_AMMO_X], NULL);
	pp_add_menu_item(&pp_menu[PP_MENU_WEAPONS], al_font[1], 16, 160, "Mines", pp_menu_weapon_mine_left, pp_menu_weapon_mine_right, NULL);
	pp_add_menu_variable(&pp_menu[PP_MENU_WEAPONS], al_font[1], 32, 192, &pp_option[PP_OPTION_AMMO_MINE], NULL);
	pp_add_menu_item(&pp_menu[PP_MENU_WEAPONS], al_font[1], 16, 240, "Bouncer", pp_menu_weapon_bounce_left, pp_menu_weapon_bounce_right, NULL);
	pp_add_menu_variable(&pp_menu[PP_MENU_WEAPONS], al_font[1], 32, 272, &pp_option[PP_OPTION_AMMO_BOUNCE], NULL);
	pp_add_menu_item(&pp_menu[PP_MENU_WEAPONS], al_font[1], 16, 320, "Seeker", pp_menu_weapon_seek_left, pp_menu_weapon_seek_right, NULL);
	pp_add_menu_variable(&pp_menu[PP_MENU_WEAPONS], al_font[1], 32, 352, &pp_option[PP_OPTION_AMMO_SEEK], NULL);

	pp_create_menu(&pp_menu[PP_MENU_POWERUPS], "Powerups", 0, 0, PP_MENU_MAIN);
	pp_add_menu_item(&pp_menu[PP_MENU_POWERUPS], al_font[1], 32, 0, "Ammo", NULL, NULL, pp_menu_powerups_ammo);
	pp_add_menu_item(&pp_menu[PP_MENU_POWERUPS], al_font[1], 32, 32, "Special", NULL, NULL, pp_menu_powerups_special);
	pp_add_menu_flag(&pp_menu[PP_MENU_POWERUPS], al_font[1], 32, 80, "Random", &pp_option[PP_OPTION_POWER_RAND], NULL, NULL, NULL);

		pp_create_menu(&pp_menu[PP_MENU_AMMO], "Ammo Settings", 0, 0, PP_MENU_POWERUPS);
		pp_add_menu_flag(&pp_menu[PP_MENU_AMMO], al_font[1], 32, 0, "Normal", &pp_option[PP_OPTION_AMMO_NORMAL_FLAG], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_AMMO], al_font[1], 32, 32, "Splitter", &pp_option[PP_OPTION_AMMO_X_FLAG], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_AMMO], al_font[1], 32, 64, "Mine", &pp_option[PP_OPTION_AMMO_MINE_FLAG], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_AMMO], al_font[1], 32, 96, "Bouncer", &pp_option[PP_OPTION_AMMO_BOUNCE_FLAG], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_AMMO], al_font[1], 32, 128, "Seeker", &pp_option[PP_OPTION_AMMO_SEEK_FLAG], NULL, NULL, NULL);
		pp_add_menu_item(&pp_menu[PP_MENU_AMMO], al_font[1], 32, 176, "Respawn", pp_menu_ammo_time_left, pp_menu_ammo_time_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_AMMO], al_font[1], 48, 208, pp_menu_ammo_respawn_text);

		pp_create_menu(&pp_menu[PP_MENU_SPECIAL], "Specials", 0, 0, PP_MENU_POWERUPS);
		pp_add_menu_flag(&pp_menu[PP_MENU_SPECIAL], al_font[1], 32, 0, "Cloak", &pp_option[PP_OPTION_POWER_CLOAK], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_SPECIAL], al_font[1], 32, 32, "Deflect", &pp_option[PP_OPTION_POWER_DEFLECT], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_SPECIAL], al_font[1], 32, 64, "Run", &pp_option[PP_OPTION_POWER_RUN], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_SPECIAL], al_font[1], 32, 96, "Jump", &pp_option[PP_OPTION_POWER_JUMP], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_SPECIAL], al_font[1], 32, 128, "Fly", &pp_option[PP_OPTION_POWER_FLY], NULL, NULL, NULL);
		pp_add_menu_flag(&pp_menu[PP_MENU_SPECIAL], al_font[1], 32, 160, "Turbo", &pp_option[PP_OPTION_POWER_TURBO], NULL, NULL, NULL);
		pp_add_menu_item(&pp_menu[PP_MENU_SPECIAL], al_font[1], 32, 204, "Respawn", pp_menu_power_time_left, pp_menu_power_time_right, NULL);
		pp_add_menu_string(&pp_menu[PP_MENU_SPECIAL], al_font[1], 48, 236, pp_menu_power_respawn_text);

	pp_current_menu = PP_MENU_MAIN;
	pp_next_menu = PP_MENU_MAIN;
	pp_menu_fix_item(&pp_menu[PP_MENU_MAIN]);
	pp_next_frame = 0;
	pp_next_reset = 1;
	pp_next_done = 0;
	pp_menu[pp_current_menu].selected = 0;

	/* set up in-game menus */
	pp_create_menu(&pp_menu[PP_MENU_EXIT], "Choice", 0, 0, PP_MENU_EXIT);
	pp_add_menu_item(&pp_menu[PP_MENU_EXIT], al_font[1], 16, 0, "Continue", NULL, NULL, pp_quit_no);
	pp_add_menu_item(&pp_menu[PP_MENU_EXIT], al_font[1], 16, 32, "Restart", NULL, NULL, pp_done_no);
	pp_add_menu_item(&pp_menu[PP_MENU_EXIT], al_font[1], 16, 64, "Quit", NULL, NULL, pp_quit_yes);

	pp_create_done_menu();
//	pp_create_menu(&pp_menu[PP_MENU_DONE], "Choice", 0, 0, PP_MENU_DONE);
//	pp_add_menu_item(&pp_menu[PP_MENU_DONE], al_font[1], 16, 0, "Restart", NULL, NULL, pp_done_no);
//	pp_add_menu_item(&pp_menu[PP_MENU_DONE], al_font[1], 16, 32, "Save Replay", NULL, NULL, pp_instant_replay);
//	pp_add_menu_item(&pp_menu[PP_MENU_DONE], al_font[1], 16, 64, "Quit", NULL, NULL, pp_quit_yes);

	for(i = 0; i < MAX_MENUS; i++)
	{
		pp_center_menu(&pp_menu[i], pp_screen, 16, 0);
	}
}

void pp_menu_fix_item(PP_MENU * mp)
{
	int start = mp->selected;

	while(mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_HEADER || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_VARIABLE || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_STRING || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_IMAGE)
	{
		mp->selected++;
		if(mp->selected >= mp->items)
		{
			mp->selected = 0;
		}
		if(mp->selected == start)
		{
			break;
		}
	}
}

void pp_menu_next_item(PP_MENU * mp)
{
	int start = mp->selected;

	mp->selected++;
	if(mp->selected >= mp->items)
	{
		mp->selected = 0;
	}
	while(mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_HEADER || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_VARIABLE || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_STRING || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_IMAGE)
	{
		mp->selected++;
		if(mp->selected >= mp->items)
		{
			mp->selected = 0;
		}
		if(mp->selected == start)
		{
			break;
		}
	}
}

void pp_menu_prev_item(PP_MENU * mp)
{
	int start = mp->selected;

	mp->selected--;
	if(mp->selected < 0)
	{
		mp->selected = mp->items - 1;
	}
	while(mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_HEADER || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_VARIABLE || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_STRING || mp->item[mp->selected].type == PP_MENU_ITEM_TYPE_IMAGE)
	{
		mp->selected--;
		if(mp->selected < 0)
		{
			mp->selected = mp->items - 1;
		}
		if(mp->selected == start)
		{
			break;
		}
	}
}

/* renew some of the menu variables with their current values */
void pp_update_variables(void)
{
	if(pp_option[PP_OPTION_AMMO_TIME] == 0)
	{
		sprintf(pp_menu_ammo_respawn_text, "Never");
	}
	else if(pp_option[PP_OPTION_AMMO_TIME] == 1800)
	{
		sprintf(pp_menu_ammo_respawn_text, "Slow");
	}
	else if(pp_option[PP_OPTION_AMMO_TIME] == 900)
	{
		sprintf(pp_menu_ammo_respawn_text, "Fast");
	}
	if(pp_option[PP_OPTION_POWER_TIME] == 0)
	{
		sprintf(pp_menu_power_respawn_text, "Never");
	}
	else if(pp_option[PP_OPTION_POWER_TIME] == 1800)
	{
		sprintf(pp_menu_power_respawn_text, "Slow");
	}
	else if(pp_option[PP_OPTION_POWER_TIME] == 900)
	{
		sprintf(pp_menu_power_respawn_text, "Fast");
	}
	sprintf(pp_menu_mvol_text, "%d%%", pp_config[PP_CONFIG_MUSIC_VOLUME]);
	sprintf(pp_menu_svol_text, "%d%%", pp_config[PP_CONFIG_SOUND_VOLUME]);
	sprintf(pp_menu_weapon_normal_text, "Start with %d", pp_option[PP_OPTION_AMMO_NORMAL]);
	sprintf(pp_menu_weapon_x_text, "Start with %d", pp_option[PP_OPTION_AMMO_X]);
	sprintf(pp_menu_weapon_mine_text, "Start with %d", pp_option[PP_OPTION_AMMO_MINE]);
	sprintf(pp_menu_weapon_bounce_text, "Start with %d", pp_option[PP_OPTION_AMMO_BOUNCE]);
	sprintf(pp_menu_weapon_seek_text, "Start with %d", pp_option[PP_OPTION_AMMO_SEEK]);
	if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
	{
		sprintf(pp_menu_game_length_text, "%d to Win", pp_option[PP_OPTION_SF_HITS]);
		sprintf(pp_menu_tag_length_text, "%d Minute Elimination", pp_option[PP_OPTION_T_TIME] / 3600);
		sprintf(pp_menu_flag_length_text, "%d to Win", pp_option[PP_OPTION_F_FLAGS]);
	}
	else
	{
		if(pp_option[PP_OPTION_GAME_TIME] <= 3600)
		{
			sprintf(pp_menu_game_length_text, "1 Minute");
			sprintf(pp_menu_tag_length_text, "1 Minute");
			sprintf(pp_menu_flag_length_text, "1 Minute");
//			sprintf(pp_menu_hunter_length_text, "1 Minute");
		}
		else
		{
			sprintf(pp_menu_game_length_text, "%d Minutes", pp_option[PP_OPTION_GAME_TIME] / 3600);
			sprintf(pp_menu_tag_length_text, "%d Minutes", pp_option[PP_OPTION_GAME_TIME] / 3600);
			sprintf(pp_menu_flag_length_text, "%d Minutes", pp_option[PP_OPTION_GAME_TIME] / 3600);
			sprintf(pp_menu_hunter_length_text, "%d Minutes", pp_option[PP_OPTION_GAME_TIME] / 3600);
		}
	}
	sprintf(pp_menu_hunter_length_text, "%d Hit Elimination", pp_option[PP_OPTION_H_HITS]);
}

void pp_title_transition(void)
{
	if(pp_next_frame % PP_NEXT_FRAMES == 0)
	{
		if(pp_next_menu == pp_current_menu)
		{
			if(pp_next_frame / PP_NEXT_FRAMES >= pp_menu[pp_current_menu].items)
			{
				pp_next_done = 1;
			}
			else
			{
				pp_menu[pp_current_menu].item[pp_next_frame / PP_NEXT_FRAMES].vis = 1;
				ncds_play_sample(pp_sound[PP_SOUND_MENU_NEXT], 128, -1, -1);
			}
		}
		else
		{
			if(pp_next_frame / PP_NEXT_FRAMES >= pp_menu[pp_current_menu].items)
			{
				pp_current_menu = pp_next_menu;
				if(pp_next_reset)
				{
					pp_menu[pp_current_menu].selected = 0;
				}
				pp_menu_fix_item(&pp_menu[pp_current_menu]);
				pp_next_frame = 0;
				return;
			}
			else
			{
				pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].items - 1 - pp_next_frame / PP_NEXT_FRAMES].vis = 0;
				ncds_play_sample(pp_sound[PP_SOUND_MENU_NEXT], 128, -1, -1);
			}
		}
	}
	pp_next_frame++;
}

void pp_level_select(void)
{
	pp_state = PP_STATE_LEVEL;
	if(!level_is_available(pp_slev))
	{
		get_first_level();
	}
	key[KEY_ENTER] = 0;
}

void pp_idle_proc(void)
{
	pp_state = PP_STATE_LOGO;
	pp_start_song("title.xm", PP_MUSIC_FILE_INTRO);
	pp_update();
	pp_draw();
	pp_fade(black_palette, logo_palette, 8);
	gametime_reset();
	logic_switch = 1;
	logo_step = 0;
	intro_times = 0;
}

void pp_menu_bg_draw(void)
{
	int i, j;

	for(i = 0; i < SCREEN_H / pp_image[PP_IMAGE_MENU]->h + 2; i++)
	{
		for(j = 0; j < SCREEN_W / pp_image[PP_IMAGE_MENU]->w + 2; j++)
		{
			blit(pp_image[PP_IMAGE_MENU], pp_screen, 0, 0, j * pp_image[PP_IMAGE_MENU]->w + pp_menu_bg_x % pp_image[PP_IMAGE_MENU]->w, i * pp_image[PP_IMAGE_MENU]->h + pp_menu_bg_y % pp_image[PP_IMAGE_MENU]->h, SCREEN_W, SCREEN_H);
		}
	}
}

void pp_new_player_logic(void)
{
	if(keypressed())
	{
		pp_new_data_key = readkey() & 0xFF;
		switch(pp_new_data_key)
		{
			/* enter */
			case 0xD:
			{
//	            pp_new_data_text[pp_new_data_pos] = pp_new_data_key;
//            	pp_new_data_pos++;
            	pp_new_data_text[pp_new_data_pos] = '\0';
            	pp_add_player_data(pp_new_data_text);
            	pp_game_data.player[pp_new_one].data_pick = pp_players - 1;
				pp_state = PP_STATE_CHAR;
				ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
				key[KEY_ENTER] = 0;
				break;
			}
			
			/* escape */
			case 0x1B:
			{
				pp_select_menu(PP_MENU_PLAY);
				pp_state = PP_STATE_CHAR;
				ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
				key[KEY_ESC] = 0;
				break;
			}
			
			/* backspace */
			case 0x8:
			{
	            pp_new_data_text[pp_new_data_pos] = '\0';
            	if(pp_new_data_pos > 0)
            	{
	                pp_new_data_pos--;
                	pp_new_data_text[pp_new_data_pos] = '\0';
            	}
				break;
			}
			
			/* normal key */
			default:
			{
	            pp_new_data_text[pp_new_data_pos] = pp_new_data_key;
            	pp_new_data_pos++;
            	pp_new_data_text[pp_new_data_pos] = '\0';
				break;
			}
		}
	}
	pp_menu_bg_x--;
	pp_menu_bg_y--;
	pp_game_data.frame++;
}

void pp_new_player_update(void)
{
	pp_menu_bg_draw();
	#ifdef USE_ALLEGTTF
		aatextprintf_center(pp_screen, al_font[1], SCREEN_W / 2 + rand() % 5 - 2, 208 + rand() % 5 - 2, 236, "Enter Name");
		aatextprintf_center(pp_screen, al_font[1], SCREEN_W / 2, 208, 239, "Enter Name");
		aatextprintf_center(pp_screen, al_font[1], SCREEN_W / 2, 240, 231, "%s", pp_new_data_text);
	#else
		textprintf_centre(pp_screen, al_font[1], SCREEN_W / 2 + rand() % 5 - 2, 208 + rand() % 5 - 2, 236, "Enter Name");
		textprintf_centre(pp_screen, al_font[1], SCREEN_W / 2, 208, 239, "Enter Name");
		textprintf_centre(pp_screen, al_font[1], SCREEN_W / 2, 240, 231, "%s", pp_new_data_text);
	#endif
}

void pp_new_player_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, 640, 480);
}

void pp_title_logic(void)
{
	int i;

//	read_joy_keys();
	ncd_joy_poll();
	read_controller(&pp_game_data.player[0].controller);
	if(pp_next_done)
	{
		/* see if there's a procedure waiting to be run */
		if(pp_next_proc)
		{
			if(!pp_fading)
			{
				pp_next_proc();
				pp_next_proc = NULL;
			}
		}
		if(pp_game_data.player[0].controller.up == 1 || key[KEY_UP])
		{
			pp_menu_prev_item(&pp_menu[pp_current_menu]);
			ncds_play_sample(pp_sound[PP_SOUND_MENU_MOVE], 128, -1, -1);
			pp_menu_idle_time = 0;
			key[KEY_UP] = 0;
		}
		if(pp_game_data.player[0].controller.down == 1 || key[KEY_DOWN])
		{
			pp_menu_next_item(&pp_menu[pp_current_menu]);
			ncds_play_sample(pp_sound[PP_SOUND_MENU_MOVE], 128, -1, -1);
			pp_menu_idle_time = 0;
			key[KEY_DOWN] = 0;
		}
		if(pp_game_data.player[0].controller.jump || key[KEY_ESC])
		{
			if(pp_menu[pp_current_menu].parent == pp_current_menu)
			{
				pp_menu[pp_current_menu].selected = pp_menu[pp_current_menu].items - 1;
			}
			else if(pp_menu[pp_current_menu].parent == PP_MENU_PLAY)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_level_select;
				ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
			}
			else
			{
				pp_next_menu = pp_menu[pp_current_menu].parent;
				pp_next_reset = 0;
				pp_next_frame = 0;
				pp_next_done = 0;
				ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
			}
			pp_menu_idle_time = 0;
			key[KEY_ESC] = 0;
		}
		switch(pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].type)
		{
			case PP_MENU_ITEM_TYPE_FLAG:
			{
				if((pp_game_data.player[0].controller.fire == 1 || key[KEY_ENTER]) && pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].dp)
				{
					if(*pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].dp)
					{
						*pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].dp = 0;
					}
					else
					{
						*pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].dp = 1;
					}
					ncds_play_sample(pp_sound[PP_SOUND_MENU_FLAG], 128, -1, -1);
					key[KEY_ENTER] = 0;
					pp_menu_idle_time = 0;
				}
				break;
			}
		}
		if(pp_game_data.player[0].controller.left == 1 || key[KEY_LEFT])
		{
			if(pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].left_proc)
			{
				pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].left_proc();
				ncds_play_sample(pp_sound[PP_SOUND_MENU_LEFT], 128, -1, -1);
			}
			pp_menu_idle_time = 0;
			key[KEY_LEFT] = 0;
		}
		if(pp_game_data.player[0].controller.right == 1 || key[KEY_RIGHT])
		{
			if(pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].right_proc)
			{
				pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].right_proc();
				ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
			}
			pp_menu_idle_time = 0;
			key[KEY_RIGHT] = 0;
		}
		if(pp_game_data.player[0].controller.fire == 1 || key[KEY_ENTER])
		{
			if(pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].enter_proc)
			{
				pp_menu[pp_current_menu].item[pp_menu[pp_current_menu].selected].enter_proc();
				ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
			}
			pp_menu_idle_time = 0;
			key[KEY_ENTER] = 0;
		}
		if(!pp_fading && pp_current_menu == PP_MENU_MAIN)
		{
			pp_menu_idle_time++;
			if(pp_menu_idle_time > 900)
			{
				pp_select_menu(PP_MENU_BLANK);
				pp_next_proc = pp_idle_proc;
				pp_menu_idle_time = 0;
				pp_start_fade(pp_palette, black_palette, 2);
//				pp_fade(pp_palette, black_palette, 2);
			}
		}
	}
	pp_update_variables();
	pp_menu_bg_x--;
	pp_menu_bg_y--;
	if(!pp_next_done && !pp_title_done)
	{
		pp_title_transition();
	}
	pp_game_data.frame++;
}

void pp_draw_menu(BITMAP * bp, PP_MENU * mp)
{
	int i, y, j;
	char temp_string[128];

	for(i = 0; i < mp->items; i++)
	{
		if(mp->item[i].vis)
		{
			switch(mp->item[i].type)
			{
				case PP_MENU_ITEM_TYPE_NORMAL:
				{
					if(i == mp->selected)
					{
						#ifdef USE_ALLEGTTF
							aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2 + rand() % 5 - 2, mp->y + mp->item[i].oy + rand() % 5 - 2, 236, "%s", mp->item[i].name);
							aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 239, "%s", mp->item[i].name);
						#else
							textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2 + rand() % 5 - 2, mp->y + mp->item[i].oy + rand() % 5 - 2, 236, "%s", mp->item[i].name);
							textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 239, "%s", mp->item[i].name);
						#endif
					}
					else
					{
						#ifdef USE_ALLEGTTF
							aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 238, "%s", mp->item[i].name);
						#else
							textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 238, "%s", mp->item[i].name);
						#endif
					}
					break;
				}
				case PP_MENU_ITEM_TYPE_FLAG:
				{
					if(mp->item[i].dp)
					{
						if(mp->item[i].dp)
						{
							if(i == mp->selected)
							{
								if(*mp->item[i].dp)
								{
									#ifdef USE_ALLEGTTF
										aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2 + rand() % 5 - 2, mp->y + mp->item[i].oy + rand() % 5 - 2, 236, "%s", mp->item[i].name);
										aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 239, "%s", mp->item[i].name);
									#else
										textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2 + rand() % 5 - 2, mp->y + mp->item[i].oy + rand() % 5 - 2, 236, "%s", mp->item[i].name);
										textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 239, "%s", mp->item[i].name);
									#endif
								}
								else
								{
									#ifdef USE_ALLEGTTF
										aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2 + rand() % 5 - 2, mp->y + mp->item[i].oy + rand() % 5 - 2, 234, "%s", mp->item[i].name);
										aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 237, "%s", mp->item[i].name);
									#else
										textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2 + rand() % 5 - 2, mp->y + mp->item[i].oy + rand() % 5 - 2, 234, "%s", mp->item[i].name);
										textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 237, "%s", mp->item[i].name);
									#endif
								}
							}
							else
							{
								if(*mp->item[i].dp)
								{
									#ifdef USE_ALLEGTTF
										aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 238, "%s", mp->item[i].name);
									#else
										textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 238, "%s", mp->item[i].name);
									#endif
								}
								else
								{
									#ifdef USE_ALLEGTTF
										aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 236, "%s", mp->item[i].name);
									#else
										textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 236, "%s", mp->item[i].name);
									#endif
								}
							}
							if(*mp->item[i].dp)
							{
								for(j = 0; j < strlen(mp->item[i].name) + 3; j++)
								{
									temp_string[j] = ' ';
								}
								temp_string[strlen(mp->item[i].name) + 3] = 0;
								temp_string[1] = 'X';
							}
						}
					}
					break;
				}
				case PP_MENU_ITEM_TYPE_HEADER:
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 239, "%s", mp->item[i].name);
					#else
						textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 239, "%s", mp->item[i].name);
					#endif
					break;
				}
				case PP_MENU_ITEM_TYPE_VARIABLE:
				{
					if(mp->item[i].dp && mp->item[i].dp2)
					{
						#ifdef USE_ALLEGTTF
							aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 231, "%s", mp->item[i].dp2[*mp->item[i].dp]);
						#else
							textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 231, "%s", mp->item[i].dp2[*mp->item[i].dp]);
						#endif
					}
					else if(mp->item[i].dp)
					{
						#ifdef USE_ALLEGTTF
							aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 231, "%d", *mp->item[i].dp);
						#else
							textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 231, "%d", *mp->item[i].dp);
						#endif
					}
					break;
				}
				case PP_MENU_ITEM_TYPE_STRING:
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 231, "%s", mp->item[i].name);
					#else
						textprintf_centre(bp, mp->item[i].fp, SCREEN_W / 2, mp->y + mp->item[i].oy, 231, "%s", mp->item[i].name);
					#endif
					break;
				}
				case PP_MENU_ITEM_TYPE_IMAGE:
				{
					draw_sprite(pp_screen, mp->item[i].bp, mp->x + mp->item[i].ox, mp->y + mp->item[i].oy);
					break;
				}
			}
		}
	}
}

void pp_title_update(void)
{
	int i;

	pp_menu_bg_draw();
	if(pp_next_done || pp_next_frame)
	{
		#ifdef USE_ALLEGTTF
			aatextout_center(pp_screen, al_font[0], pp_menu[pp_current_menu].title, 320 + rand() % 5 - 2, pp_menu[pp_current_menu].y - 64 + rand() % 5 - 2, 220);
			aatextout_center(pp_screen, al_font[0], pp_menu[pp_current_menu].title, 320, pp_menu[pp_current_menu].y - 64, 223);
		#else
			textout_centre(pp_screen, al_font[0], pp_menu[pp_current_menu].title, 320 + rand() % 5 - 2, pp_menu[pp_current_menu].y - 64 + rand() % 5 - 2, 220);
			textout_centre(pp_screen, al_font[0], pp_menu[pp_current_menu].title, 320, pp_menu[pp_current_menu].y - 64, 223);
		#endif
	}
	pp_draw_menu(pp_screen, &pp_menu[pp_current_menu]);
}

void pp_title_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, 640, 480);
}

int char_is_available(int n)
{
	int i;
	
	return 1;
}

int pp_data_ok(int p)
{
	int i;
	
	for(i = 0; i < 4; i++)
	{
		if(i != p && pp_game_data.player[i].entered > 1 && pp_game_data.player[i].data_pick == pp_game_data.player[p].data_pick)
		{
			return 0;
		}
	}
	return 1;
}

void pp_char_logic(void)
{
	int i, j;
	int count, selecting;
	int okay;
	int cmax = 4;

	if(key[KEY_ESC])
	{
		pp_select_menu(PP_MENU_PLAY);
		pp_next_reset = 0;
		pp_state = PP_STATE_TITLE;
		key[KEY_ESC] = 0;
		ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
	}
	/* players select characters */
	ncd_joy_poll();
	read_joy_keys();
	if(single_player)
	{
		cmax = 1;
	}
//	else
//	{
		for(i = 0; i < cmax; i++)
		{
			read_controller(&pp_game_data.player[i].controller);
			if(pp_game_data.player[i].controller.fire == 1)
			{
				if(pp_game_data.player[i].entered == 0)
				{
					pp_game_data.player[i].entered = 1;
				}
				else if(pp_game_data.player[i].entered < 2)
				{
					if(pp_game_data.player[i].data_pick < 0)
					{
						pp_new_one = i;
						pp_menu_new_player();
						pp_config[PP_CONFIG_P_P1 + i] = pp_game_data.player[i].data_pick;
						ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
					}
					else
					{
						if(pp_data_ok(i))
						{
							pp_config[PP_CONFIG_P_P1 + i] = pp_game_data.player[i].data_pick;
							pp_game_data.player[i].entered = 2;
							ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
						}
					}
				}
				else if(pp_game_data.player[i].entered != 3)
				{
					if(pp_game_data.player[i].char_pick == char_entries)
					{
						ncds_pause_music();
						pp_title_char_name[i][0] = '\0';
						if(file_select_ex("Choose Character:", pp_title_char_name[i], "ppc", 1024, 320, 240))
						{
							if(!load_char(&pp_game_data.player[i], pp_title_char_name[i]))
							{
								ncds_pause_music();
								alert(NULL, "Cannot load character!", NULL, "OK", NULL, 0, 0);
								ncds_resume_music();
								gametime_reset();
								logic_switch = 1;
							}
							else
							{
								ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_ENTER], 128, -1, -1);
								pp_game_data.frame = 0;
								strcpy(pp_game_data.player[i].name, get_filename(pp_title_char_name[i]));
								delete_extension(pp_game_data.player[i].name);
								pp_game_data.player[i].char_pick = char_entries + 1;
								pp_game_data.player[i].entered = 3;
								if(single_player)
								{
									pp_state = PP_STATE_LEVEL;
									if(!level_is_available(pp_slev))
									{
										get_first_level();
									}
									pp_config[PP_CONFIG_P_P1] = pp_game_data.player[i].data_pick;
								}
							}
						}
						else
						{
							ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
						}
						ncds_resume_music();
						gametime_reset();
						logic_switch = 1;
						key[KEY_ENTER] = 0;
						key[KEY_ESC] = 0;
					}
					else
					{
						if(pp_game_data.player[i].char_pick < char_entries)
						{
							if(!load_char(&pp_game_data.player[i], char_entry[pp_game_data.player[i].char_pick].file))
							{
								ncds_pause_music();
								alert(NULL, "Cannot load character!", NULL, "OK", NULL, 0, 0);
								ncds_resume_music();
								gametime_reset();
								logic_switch = 1;
							}
							else
							{
								pp_game_data.frame = 0;
								strcpy(pp_title_char_name[i], char_entry[pp_game_data.player[i].char_pick].file);
								strcpy(pp_game_data.player[i].name, char_entry[pp_game_data.player[i].char_pick].name);
							}
						}
						pp_select_menu(PP_MENU_PLAY_1P);
						ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_ENTER], 128, -1, -1);
						pp_game_data.player[i].entered = 3;
						if(single_player)
						{
							pp_state = PP_STATE_LEVEL;
							if(!level_is_available(pp_slev))
							{
								get_first_level();
							}
							pp_config[PP_CONFIG_P_P1] = pp_game_data.player[i].data_pick;
						}
					}
				}
				else if(pp_start_time <= 300)
				{
					if(pp_start_time % 60)
					{
						pp_start_time -= pp_start_time % 60;
					}
					else
					{
						pp_start_time -= 60;
					}
					if(pp_start_time < 0)
					{
						pp_start_time = 0;
					}
				}
//				pp_game_data.player[i].controller.fire++;
				key[KEY_ENTER] = 0;
			}
			if(pp_game_data.player[i].controller.jump)
			{
				if(pp_game_data.player[i].entered > 0)
				{
					pp_game_data.player[i].entered--;
					ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
					if(single_player && i == 0 && pp_game_data.player[i].entered == 0)
					{
						pp_select_menu(PP_MENU_PLAY);
						pp_next_reset = 0;
						pp_state = PP_STATE_TITLE;
					}
				}
				else
				{
					if(i == 0)
					{
						pp_select_menu(PP_MENU_PLAY);
						pp_next_reset = 0;
						pp_state = PP_STATE_TITLE;
						ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
					}
				}
			}
			if(pp_game_data.player[i].entered < 2 && pp_game_data.player[i].controller.select == 1)
			{
				if(pp_game_data.player[i].data_pick >= 4)
				{
					ncds_pause_music();
					if(alert(NULL, "Delete this player?", NULL, "Yes", "No", 0, 0) == 1)
					{
						pp_delete_player_data(pp_game_data.player[i].data_pick);
						if(pp_game_data.player[i].data_pick >= pp_players)
						{
							pp_game_data.player[i].data_pick--;
						}
						ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
					}
					ncds_resume_music();
					pp_game_data.player[i].controller.fire = 1;
					gametime_reset();
					logic_switch = 1;
				}
			}
			if(pp_game_data.player[i].controller.left == 1 && pp_game_data.player[i].entered)
			{
				if(pp_game_data.player[i].entered < 2)
				{
					pp_game_data.player[i].data_pick--;
					if(pp_game_data.player[i].data_pick < -1)
					{
						pp_game_data.player[i].data_pick = pp_players - 1;
					}
					ncds_play_sample(pp_sound[PP_SOUND_MENU_LEFT], 128, -1, -1);
				}
				else if(pp_game_data.player[i].entered != 3)
				{
					pp_game_data.player[i].char_pick--;
					if(pp_game_data.player[i].char_pick < 0)
					{
						pp_game_data.player[i].char_pick = char_entries;
					}
					ncds_play_sample(pp_sound[PP_SOUND_MENU_LEFT], 128, -1, -1);
				}
			}
			if(pp_game_data.player[i].controller.right == 1 && pp_game_data.player[i].entered)
			{
				if(pp_game_data.player[i].entered < 2)
				{
					pp_game_data.player[i].data_pick++;
					if(pp_game_data.player[i].data_pick > pp_players - 1)
					{
						pp_game_data.player[i].data_pick = -1;
					}
					ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
				}
				else if(pp_game_data.player[i].entered != 3)
				{
					pp_game_data.player[i].char_pick++;
					if(pp_game_data.player[i].char_pick > char_entries)
					{
						pp_game_data.player[i].char_pick = 0;
					}
					ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
				}
			}
			if(pp_game_data.player[i].controller.up == 1 && pp_game_data.player[i].entered)
			{
				if(pp_game_data.player[i].entered < 2)
				{
					pp_game_data.player[i].data_pick = -1;
					ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
				}
				else if(pp_game_data.player[i].entered != 3)
				{
					pp_game_data.player[i].char_pick = char_entries;
					ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
				}
			}
			if(pp_game_data.player[i].controller.down == 1 && pp_game_data.player[i].entered)
			{
				if(pp_game_data.player[i].entered == 2)
				{
					pp_game_data.player[i].char_pick = rand() % char_entries;
					ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
				}
			}
		}
//	}
	
	/* see if there's any players still selecting */
	selecting = 0;
	for(i = 0; i < 4; i++)
	{

		/* if there's a player still selecting, don't start timer */
		if(pp_game_data.player[i].entered && pp_game_data.player[i].entered != 3)
		{
			pp_start_time = 301; /* don't show time when it's this number */
			selecting = 1;
			break;
		}
	}

	/* if there are none selecting and at least 2 have selected, start timer */
	if(!selecting)
	{
		count = 0;
		for(i = 0; i < 4; i++)
		{
			if(pp_game_data.player[i].entered == 3)
			{
				count++;
				if(count >= 2)
				{
					if(pp_start_time == 301)
					{
						pp_start_time = 300;
					}
				}
			}
		}
		if(count >= 2 && pp_start_time <= 300)
		{
			pp_start_time--;
			if(pp_start_time <= 0 || count >= 4)
			{
				switch(count)
				{
					case 2:
					{
						pp_select_menu(PP_MENU_PLAY_2P);
						break;
					}
					case 3:
					{
						pp_select_menu(PP_MENU_PLAY_3P);
						break;
					}
					case 4:
					{
						pp_select_menu(PP_MENU_PLAY_4P);
						break;
					}
				}
				pp_state = PP_STATE_LEVEL;
				if(!level_is_available(pp_slev))
				{
					get_first_level();
				}
				if(count < 4)
				{
					ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
				}
				key[KEY_ENTER] = 0;
			}
		}
	}
	pp_menu_bg_x--;
	pp_menu_bg_y--;
	pp_game_data.frame++;
}

void delete_extension(char * text)
{
	int i;
	
	for(i = strlen(text) - 1; i >= 0; i--)
	{
		if(text[i] == '.')
		{
			text[i] = '\0';
			break;
		}
	}
}

void pp_char_update(void)
{
	int i;
	int cx[4], cy[4];
//	char cname[64] = {0};
	int cmax = 4;
	int cp;
	int color1, color2;

	for(i = 0; i < 4; i++)
	{
		if(i > 0)
		{
			if(single_player)
			{
				cx[i] = 640;
				cy[i] = 640;
			}
			else
			{
				cx[i] = pp_game_data.player[i].screen.x + pp_game_data.player[i].screen.b[PP_SCREEN_MODE_QUARTER]->w / 2;
				cy[i] = pp_game_data.player[i].screen.y + pp_game_data.player[i].screen.b[PP_SCREEN_MODE_QUARTER]->h / 2;
			}
		}
		else
		{
			cx[i] = pp_game_data.player[i].screen.x + pp_game_data.player[i].screen.b[PP_SCREEN_MODE_QUARTER]->w / 2;
			cy[i] = pp_game_data.player[i].screen.y + pp_game_data.player[i].screen.b[PP_SCREEN_MODE_QUARTER]->h / 2;
		}
	}

	pp_menu_bg_draw();
	if(single_player)
	{
		#ifdef USE_ALLEGTTF
			aatextout_center(pp_screen, al_font[0], "Single Player", 320 + rand() % 5 - 2, 0 + rand() % 5 - 2, 220);
			aatextout_center(pp_screen, al_font[0], "Single Player", 320, 0, 223);
		#else
			textout_centre(pp_screen, al_font[0], "Single Player", 320 + rand() % 5 - 2, 0 + rand() % 5 - 2, 220);
			textout_centre(pp_screen, al_font[0], "Single Player", 320, 0, 223);
		#endif
		cmax = 1;
	}
	else
	{
		#ifdef USE_ALLEGTTF
			aatextout_center(pp_screen, al_font[0], "Multi-Player", 320 + rand() % 5 - 2, 0 + rand() % 5 - 2, 220);
			aatextout_center(pp_screen, al_font[0], "Multi-Player", 320, 0, 223);
		#else
			textout_centre(pp_screen, al_font[0], "Multi-Player", 320 + rand() % 5 - 2, 0 + rand() % 5 - 2, 220);
			textout_centre(pp_screen, al_font[0], "Multi-Player", 320, 0, 223);
		#endif
		cmax = 4;
	}
	for(i = 0; i < cmax; i++)
	{
		cp = pp_game_data.player[i].char_pick;
		if(!pp_game_data.player[i].entered)
		{
			if((pp_game_data.frame / 30) % 2 == 0)
			{
				#ifdef USE_ALLEGTTF
					aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] - 20, 239, "Press Fire");
					aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i], 239, "to Join");
				#else
					textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] - 20, 239, "Press Fire");
					textprintf_centre(pp_screen, al_font[2], cx[i], cy[i], 239, "to Join");
				#endif
			}
		}
		else
		{
			if(pp_game_data.player[i].char_pick < char_entries)
			{
				stretch_sprite(pp_screen, get_ani(char_entry[cp].ap, pp_game_data.frame), cx[i] - char_entry[cp].ap->w, cy[i] - char_entry[cp].ap->h + 16, char_entry[cp].ap->w * 2, char_entry[cp].ap->h * 2);
			}
			else
			{
//				if(pp_started[i])
				if(pp_game_data.player[i].character.ap[0] && cp > char_entries)
				{
					stretch_sprite(pp_screen, get_ani(pp_game_data.player[i].character.ap[PS_WALK_RIGHT], pp_game_data.frame), cx[i] - pp_game_data.player[i].character.ap[PS_WALK_RIGHT]->w, cy[i] - pp_game_data.player[i].character.ap[PS_WALK_RIGHT]->h + 16, pp_game_data.player[i].character.ap[2]->w * 2, pp_game_data.player[i].character.ap[2]->h * 2);
				}
				else
				{
					#ifdef USE_ALLEGTTF
						aatextout_center(pp_screen, al_font[0], "?", cx[i], cy[i] - 16, 31);
					#else
						textout_centre(pp_screen, al_font[0], "?", cx[i], cy[i] - 16, 31);
					#endif
				}
			}
			if(pp_game_data.player[i].entered < 2)
			{
				if(pp_data_ok(i))
				{
					color1 = 236;
					color2 = 239;
				}
				else
				{
					color1 = 233;
					color2 = 236;
				}
				if(pp_game_data.player[i].data_pick >= 0)
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 - 56, color1, "< %s >", pp_player_data[pp_game_data.player[i].data_pick].name);
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] - 56, color2, "< %s >", pp_player_data[pp_game_data.player[i].data_pick].name);
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 - 56, color1, "< %s >", pp_player_data[pp_game_data.player[i].data_pick].name);
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] - 56, color2, "< %s >", pp_player_data[pp_game_data.player[i].data_pick].name);
					#endif
				}
				else
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 - 56, 236, "< New >", pp_player_data[pp_game_data.player[i].data_pick].name);
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] - 56, 239, "< New >", pp_player_data[pp_game_data.player[i].data_pick].name);
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 - 56, 236, "< New >", pp_game_data.player_data[pp_game_data.player[i].data_pick].name);
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] - 56, 239, "< New >", pp_game_data.player_data[pp_game_data.player[i].data_pick].name);
					#endif
				}
				if(cp == char_entries)
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "Custom");
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "Custom");
					#endif
				}
				else
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "%s", char_entry[cp].name);
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "%s", char_entry[cp].name);
					#endif
				}
			}
			else if(pp_game_data.player[i].entered != 3)
			{
				if(pp_data_ok(i))
				{
					color1 = 236;
					color2 = 239;
				}
				else
				{
					color1 = 233;
					color2 = 236;
				}
				#ifdef USE_ALLEGTTF
					aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] - 56, color2, "%s", pp_player_data[pp_game_data.player[i].data_pick].name);
				#else
					textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] - 56, color2, "%s", pp_player_data[pp_game_data.player[i].data_pick].name);
				#endif
				if(cp == char_entries)
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 + 56 + 16, 229, "< Custom >");
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "< Custom >");
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 + 56 + 16, 229, "< Custom >");
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "< Custom >");
					#endif
				}
				else
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 + 56 + 16, 229, "< %s >", char_entry[cp].name);
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "< %s >", char_entry[cp].name);
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i] + rand() % 5 - 2, cy[i] + rand() % 5 - 2 + 56 + 16, 229, "< %s >", char_entry[cp].name);
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "< %s >", char_entry[cp].name);
					#endif
				}
			}
			else
			{
				#ifdef USE_ALLEGTTF
					aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] - 56, 239, "%s", pp_player_data[pp_game_data.player[i].data_pick].name);
				#else
					textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] - 56, 239, "%s", pp_player_data[pp_game_data.player[i].data_pick].name);
				#endif
				if(pp_game_data.player[i].char_pick == char_entries)
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "Custom");
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "Custom");
					#endif
				}
				else
				{
					#ifdef USE_ALLEGTTF
						aatextprintf_center(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "%s", char_entry[pp_game_data.player[i].char_pick].name);
					#else
						textprintf_centre(pp_screen, al_font[2], cx[i], cy[i] + 56 + 16, 231, "%s", char_entry[pp_game_data.player[i].char_pick].name);
					#endif
				}
			}
		}
	}
	if(pp_start_time <= 300 && (pp_start_time / 15) % 2 == 1)
	{
		#ifdef USE_ALLEGTTF
			aatextprintf_center(pp_screen, al_font[0], 320, 220, 31, "%d", (pp_start_time / 60 + 1) % 10);
		#else
			textprintf_centre(pp_screen, al_font[0], 320, 220, 31, "%d", (pp_start_time / 60 + 1) % 10);
		#endif
	}
//	for(i = 0; i < 4; i++)
//	{
//		textprintf(pp_screen, font, 0, i * 8, 31, "%d", pp_game_data.player[i].entered);
//	}
/*	for(i = 0; i < char_avails; i++)
	{
		textprintf(pp_screen, font, 0, i * 8, 31, "%d", char_avail[i]);
	} */
}

void pp_char_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, 640, 480);
}

int level_is_available(int n)
{
	int i;
	
	if(n < level_entries)
	{
		return 1;
	}
	return 0;
}

void level_next(void)
{
	pp_slev++;
	if(pp_slev >= level_entries)
	{
		pp_slev = 0;
	}
}

void level_prev(void)
{
	pp_slev--;
	if(pp_slev < 0)
	{
		pp_slev = level_entries - 1;
	}
}

void get_first_level(void)
{
	int s = pp_slev;
	
	while(!level_is_available(pp_slev))
	{
		level_next();
		if(pp_slev == s)
		{
			break;
		}
	}
}

void get_next_level(void)
{
	if(level_avails > 0)
	{
		level_next();
		while(!level_is_available(pp_slev))
		{
			level_next();
		}
	}
}

void get_prev_level(void)
{
	if(level_avails > 0)
	{
		level_prev();
		while(!level_is_available(pp_slev))
		{
			level_prev();
		}
	}
}

void pp_level_logic(void)
{
	int i, count;
	int okay;

	ncd_joy_poll();
	read_controller(&pp_game_data.player[0].controller);
	if(pp_game_data.player[0].controller.left == 1 || key[KEY_LEFT])
	{
		pp_slev--;
		if(pp_slev < 0)
		{
			pp_slev = level_entries;
		}
		ncds_play_sample(pp_sound[PP_SOUND_MENU_LEFT], 128, -1, -1);
		key[KEY_LEFT] = 0;
	}
	if(pp_game_data.player[0].controller.right == 1 || key[KEY_RIGHT])
	{
		pp_slev++;
		if(pp_slev > level_entries - 0)
		{
			pp_slev = 0;
		}
		ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
		key[KEY_RIGHT] = 0;
	}
	if(pp_game_data.player[0].controller.up == 1 || key[KEY_UP])
	{
		pp_slev = level_entries;
		ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
		key[KEY_UP] = 0;
	}
	if(pp_game_data.player[0].controller.down == 1 || key[KEY_DOWN])
	{
		pp_slev = rand() % level_entries;
		ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
		key[KEY_DOWN] = 0;
	}
	if(pp_game_data.player[0].controller.fire == 1 || key[KEY_ENTER])
	{
		if(single_player)
		{
			pp_select_menu(PP_MENU_PLAY_1P);
			okay = 1;
		}
		else
		{
			count = 0;
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].entered == 3)
				{
					count++;
				}
			}
			switch(count)
			{
				case 2:
				{
					pp_select_menu(PP_MENU_PLAY_2P);
					break;
				}
				case 3:
				{
					pp_select_menu(PP_MENU_PLAY_3P);
					break;
				}
				case 4:
				{
					pp_select_menu(PP_MENU_PLAY_4P);
					break;
				}
				default:
				{
					pp_select_menu(PP_MENU_MAIN);
					break;
				}
			}
		}
		if(okay)
		{
			ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
			if(pp_slev == level_entries)
			{
				ncds_pause_music();
				pp_title_level_name[0] = '\0';
				if(file_select_ex("Choose Level:", pp_title_level_name, "ppl", 1024, 320, 240))
				{
					ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, -1, -1);
					destroy_level(pp_game_data.level);
					pp_game_data.level = load_level(pp_title_level_name);
					if(!pp_game_data.level)
					{
						ncds_pause_music();
						alert(NULL, "Cannot load level!", NULL, "OK", NULL, 0, 0);
						ncds_resume_music();
						gametime_reset();
						logic_switch = 1;
					}
					else
					{
						pp_state = PP_STATE_TITLE;
					}
				}
				else
				{
					ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
				}
				ncds_resume_music();
				gametime_reset();
				logic_switch = 1;
				key[KEY_ENTER] = 0;
				key[KEY_ESC] = 0;
			}
			else
			{
				destroy_level(pp_game_data.level);
				pp_game_data.level = load_level(level_entry[pp_slev].file);
				if(!pp_game_data.level)
				{
					ncds_pause_music();
					alert(NULL, "Cannot load level!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					gametime_reset();
					logic_switch = 1;
				}
				else
				{
					strcpy(pp_title_level_name, level_entry[pp_slev].file);
					pp_state = PP_STATE_TITLE;
				}
			}
			key[KEY_ENTER] = 0;
		}
	}
	if(pp_game_data.player[0].controller.jump || key[KEY_ESC])
	{
		pp_state = PP_STATE_CHAR;
		ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
		if(single_player)
		{
			pp_game_data.player[0].entered = 2;
//			pp_datad[0] = 0;
//			pp_character[0] = 0;
			pp_game_data.frame = 0;
			pp_game_data.player[0].screen.x = 160;
			pp_game_data.player[0].screen.y = 120;
		}
		else
		{
			pp_game_data.player[0].entered = 0;
			pp_game_data.player[1].entered = 0;
			pp_game_data.player[2].entered = 0;
			pp_game_data.player[3].entered = 0;
			pp_start_time = 301;
			pp_game_data.frame = 0;
			pp_game_data.player[0].screen.x = 0;
			pp_game_data.player[0].screen.y = 0;
			pp_game_data.player[1].screen.x = 320;
			pp_game_data.player[1].screen.y = 240;
			pp_game_data.player[2].screen.x = 320;
			pp_game_data.player[2].screen.y = 0;
			pp_game_data.player[3].screen.x = 0;
			pp_game_data.player[3].screen.y = 240;
		}
		key[KEY_ESC] = 0;
	}
	pp_menu_bg_x--;
	pp_menu_bg_y--;
	pp_game_data.frame++;
}

void pp_level_update(void)
{
	int i;

	pp_menu_bg_draw();
	#ifdef USE_ALLEGTTF
		aatextout_center(pp_screen, al_font[0], "< Level Select >", 320 + rand() % 5 - 2, 42 + rand() % 5 - 2, 236);
		aatextout_center(pp_screen, al_font[0], "< Level Select >", 320, 42, 239);
	#else
		textout_centre(pp_screen, al_font[0], "< Level Select >", 320 + rand() % 5 - 2, 42 + rand() % 5 - 2, 236);
		textout_centre(pp_screen, al_font[0], "< Level Select >", 320, 42, 239);
	#endif
	if(pp_slev < level_entries)
	{
		draw_sprite(pp_screen, level_entry[pp_slev].bp, 320 - level_entry[pp_slev].bp->w / 2, 240 - level_entry[pp_slev].bp->h / 2);
		if(strlen(level_entry[pp_slev].name) <= 16)
		{
			#ifdef USE_ALLEGTTF
				aatextprintf_center(pp_screen, al_font[0], 320, 368, 231, "%s", level_entry[pp_slev].name);
			#else
				textprintf_centre(pp_screen, al_font[0], 320, 368, 231, "%s", level_entry[pp_slev].name);
			#endif
		}
		else if(strlen(level_entry[pp_slev].name) <= 24)
		{
			#ifdef USE_ALLEGTTF
				aatextprintf_center(pp_screen, al_font[1], 320, 384, 231, "%s", level_entry[pp_slev].name);
			#else
				textprintf_centre(pp_screen, al_font[1], 320, 384, 231, "%s", level_entry[pp_slev].name);
			#endif
		}
		else
		{
			#ifdef USE_ALLEGTTF
				aatextprintf_center(pp_screen, al_font[2], 320, 392, 231, "%s", level_entry[pp_slev].name);
			#else
				textprintf_centre(pp_screen, al_font[2], 320, 392, 231, "%s", level_entry[pp_slev].name);
			#endif
		}
	}
	else
	{
		#ifdef USE_ALLEGTTF
			aatextout_center(pp_screen, al_font[0], "?", 320, 208, 31);
			aatextprintf_center(pp_screen, al_font[0], 320, 368, 231, "Custom");
		#else
			textout_centre(pp_screen, al_font[0], "?", 320, 208, 31);
			textprintf_centre(pp_screen, al_font[0], 320, 368, 231, "Custom");
		#endif
	}
}

void pp_level_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, 640, 480);
}
