#include "includes.h"

PACKFILE * pp_replay_file = NULL;
int demo_playback = 0;
int pp_old_option[PP_OPTIONS];
int pp_old_character[4];
int pp_old_name[4];
int pp_old_slev;
char pp_replay_character[4][256];
char pp_replay_level[256];
char pp_replay_music[1024];
int replay_pause = 0;
int replay_step = 0;
int replay_camera = 4;
int replay_zoom = 0;
extern int dpower_list[5], power_list[5], dammo_list[5], ammo_list[5];

void replay_fix_filename(char * fn)
{
	char * fnp;
	int i;

	fnp = get_filename(fn);
	for(i = 0; i < strlen(fnp); i++)
	{
		switch(fnp[i])
		{
			case '?':
			case ':':
			case ';':
			case '/':
			case '\\':
			case '+':
			case '*':
			case '<':
			case '>':
			case '"':
			case '|':
			{
				fnp[i] = '_';
			}
			default:
			{
				break;
			}
		}
	}
}

void pp_fix_camera(void)
{
	if(replay_camera < 4)
	{
		if(!pp_game_data.player[replay_camera].active)
		{
			pp_replay_set_camera(4);
			replay_zoom = 0;
		}
	}
}

void pp_replay_set_camera(int num)
{
	int p[2], pc;
	int i;
	
	switch(num)
	{
		case 0:
		{
			if(pp_game_data.player[0].active)
			{
				pp_game_data.player[0].screen.w = pp_game_data.player[0].screen.bp->w * 2;
				pp_game_data.player[0].screen.h = pp_game_data.player[0].screen.bp->h * 2;
				pp_game_data.player[0].screen.x = (pp_game_data.player[0].screen.w - SCREEN_W) / 2;
				pp_game_data.player[0].screen.y = (pp_game_data.player[0].screen.h - SCREEN_H) / 2;
				pp_game_data.player[0].screen.active = 1;
				pp_game_data.player[1].screen.active = 0;
				pp_game_data.player[2].screen.active = 0;
				pp_game_data.player[3].screen.active = 0;
				replay_camera = num;
			}
			break;
		}
		case 1:
		{
			if(pp_game_data.player[1].active)
			{
				pp_game_data.player[1].screen.x = 0;
				pp_game_data.player[1].screen.y = 0;
				pp_game_data.player[1].screen.w = pp_game_data.player[1].screen.bp->w * 2;
				pp_game_data.player[1].screen.h = pp_game_data.player[1].screen.bp->h * 2;
				pp_game_data.player[1].screen.active = 1;
				pp_game_data.player[0].screen.active = 0;
				pp_game_data.player[2].screen.active = 0;
				pp_game_data.player[3].screen.active = 0;
				replay_camera = num;
			}
			break;
		}
		case 2:
		{
			if(pp_game_data.player[2].active)
			{
				pp_game_data.player[2].screen.x = 0;
				pp_game_data.player[2].screen.y = 0;
				pp_game_data.player[2].screen.w = pp_game_data.player[2].screen.bp->w * 2;
				pp_game_data.player[2].screen.h = pp_game_data.player[2].screen.bp->h * 2;
				pp_game_data.player[2].screen.active = 1;
				pp_game_data.player[0].screen.active = 0;
				pp_game_data.player[1].screen.active = 0;
				pp_game_data.player[3].screen.active = 0;
				replay_camera = num;
			}
			break;
		}
		case 3:
		{
			if(pp_game_data.player[3].active)
			{
				pp_game_data.player[3].screen.x = 0;
				pp_game_data.player[3].screen.y = 0;
				pp_game_data.player[3].screen.w = pp_game_data.player[3].screen.bp->w * 2;
				pp_game_data.player[3].screen.h = pp_game_data.player[3].screen.bp->h * 2;
				pp_game_data.player[3].screen.active = 1;
				pp_game_data.player[0].screen.active = 0;
				pp_game_data.player[1].screen.active = 0;
				pp_game_data.player[2].screen.active = 0;
				replay_camera = num;
			}
			break;
		}
		default:
		{
			if(pp_players_in() > 2 || pp_players_in() == 2 && pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_QUARTER)
			{
				pp_game_data.player[0].screen.x = 0;
				pp_game_data.player[0].screen.y = 0;
				pp_game_data.player[0].screen.w = 320;
				pp_game_data.player[0].screen.h = 240;
				pp_game_data.player[0].screen.active = 1;
				pp_game_data.player[0].just = 1;
				pp_game_data.player[1].screen.x = 320;
				pp_game_data.player[1].screen.y = 240;
				pp_game_data.player[1].screen.w = 320;
				pp_game_data.player[1].screen.h = 240;
				pp_game_data.player[1].screen.active = 1;
				pp_game_data.player[1].just = 1;
				pp_game_data.player[2].screen.x = 320;
				pp_game_data.player[2].screen.y = 0;
				pp_game_data.player[2].screen.w = 320;
				pp_game_data.player[2].screen.h = 240;
				pp_game_data.player[2].screen.active = 1;
				pp_game_data.player[2].just = 1;
				pp_game_data.player[3].screen.x = 0;
				pp_game_data.player[3].screen.y = 240;
				pp_game_data.player[3].screen.w = 320;
				pp_game_data.player[3].screen.h = 240;
				pp_game_data.player[3].screen.active = 1;
				pp_game_data.player[3].just = 1;
				replay_camera = num;
			}
			else
			{
				if(pp_game_data.mode == PP_MODE_SCAVENGER || pp_game_data.mode == PP_MODE_TARGET)
				{
					pp_game_data.player[0].screen.x = 0;
					pp_game_data.player[0].screen.y = 0;
					pp_game_data.player[0].screen.w = 640;
					pp_game_data.player[0].screen.h = 480;
					pp_game_data.player[0].screen.active = 1;
					pp_game_data.player[0].just = 1;
					replay_camera = num;
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
							pp_game_data.player[i].screen.active = 0;
						}
					}
					if(pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_VERTICAL)
					{
						pp_game_data.player[p[0]].screen.x = 0;
						pp_game_data.player[p[0]].screen.y = 0;
						pp_game_data.player[p[0]].screen.w = 320;
						pp_game_data.player[p[0]].screen.h = 480;
						pp_game_data.player[p[1]].screen.x = 320;
						pp_game_data.player[p[1]].screen.y = 0;
						pp_game_data.player[p[1]].screen.w = 320;
						pp_game_data.player[p[1]].screen.h = 480;
					}
					else
					{
						pp_game_data.player[p[0]].screen.x = 0;
						pp_game_data.player[p[0]].screen.y = 0;
						pp_game_data.player[p[0]].screen.w = 640;
						pp_game_data.player[p[0]].screen.h = 240;
						pp_game_data.player[p[1]].screen.x = 0;
						pp_game_data.player[p[1]].screen.y = 240;
						pp_game_data.player[p[1]].screen.w = 640;
						pp_game_data.player[p[1]].screen.h = 240;
					}
					pp_game_data.player[p[0]].screen.active = 1;
					pp_game_data.player[p[0]].just = 1;
					pp_game_data.player[p[1]].screen.active = 1;
					pp_game_data.player[p[1]].just = 1;
					replay_camera = num;
				}
			}
			break;
		}
	}
}

void pp_replay_pan(void)
{
	int ox, oy;

//	if(replay_zoom)
	if(replay_camera < 4)
	{
		get_mouse_mickeys(&ox, &oy);
		pp_game_data.player[replay_camera].screen.x -= ox;
		if(pp_game_data.player[replay_camera].screen.x < -pp_game_data.player[replay_camera].screen.w + SCREEN_W)
		{
			pp_game_data.player[replay_camera].screen.x = -pp_game_data.player[replay_camera].screen.w + SCREEN_W;
		}
		else if(pp_game_data.player[replay_camera].screen.x >= 0)
		{
			pp_game_data.player[replay_camera].screen.x = 0;
		}
		pp_game_data.player[replay_camera].screen.y -= oy;
		if(pp_game_data.player[replay_camera].screen.y < -pp_game_data.player[replay_camera].screen.h + SCREEN_H)
		{
			pp_game_data.player[replay_camera].screen.y = -pp_game_data.player[replay_camera].screen.h + SCREEN_H;
		}
		else if(pp_game_data.player[replay_camera].screen.y >= 0)
		{
			pp_game_data.player[replay_camera].screen.y = 0;
		}
	}
}

void pp_replay_toggle_zoom(void)
{
	replay_zoom = 1 - replay_zoom;
	if(replay_camera >= 4)
	{
		replay_camera = 0;
	}
	if(replay_zoom)
	{
		pp_game_data.player[replay_camera].screen.w = pp_game_data.player[replay_camera].screen.bp->w * 4;
		pp_game_data.player[replay_camera].screen.h = pp_game_data.player[replay_camera].screen.bp->h * 4;
		pp_game_data.player[replay_camera].screen.x = -pp_game_data.player[replay_camera].screen.w / 4;
		pp_game_data.player[replay_camera].screen.y = -pp_game_data.player[replay_camera].screen.h / 4;
		pp_game_data.player[replay_camera].screen.active = 1;
		pp_game_data.player[replay_camera].just = 1;
	}
	else
	{
		pp_game_data.player[replay_camera].screen.x = 0;
		pp_game_data.player[replay_camera].screen.y = 0;
		pp_game_data.player[replay_camera].screen.w = pp_game_data.player[replay_camera].screen.bp->w * 2;
		pp_game_data.player[replay_camera].screen.h = pp_game_data.player[replay_camera].screen.bp->h * 2;
		pp_game_data.player[replay_camera].screen.active = 1;
		pp_game_data.player[replay_camera].just = 1;
	}
}

void pp_copy_options(void)
{
	int i;

	for(i = 0; i < PP_OPTIONS; i++)
	{
		pp_old_option[i] = pp_option[i];
	}
	for(i = 0; i < 4; i++)
	{
		pp_old_character[i] = pp_game_data.player[i].char_pick;
		pp_old_name[i] = pp_game_data.player[i].data_pick;
	}
	pp_old_slev = pp_slev;
}

void pp_restore_options(void)
{
	int i;

	for(i = 0; i < PP_OPTIONS; i++)
	{
		pp_option[i] = pp_old_option[i];
	}
	for(i = 0; i < 4; i++)
	{
		pp_game_data.player[i].char_pick = pp_old_character[i];
		pp_game_data.player[i].data_pick = pp_old_name[i];
		pp_config[PP_CONFIG_P_P1 + i] = pp_game_data.player[i].data_pick;
	}
	pp_slev = pp_old_slev;
}

void pp_copy_file(char * fn, char * dfn)
{
	PACKFILE * source, * destination;

	source = pack_fopen(fn, "r");
	if(!source)
	{
		return;
	}
	replay_fix_filename(dfn);
	destination = pack_fopen(dfn, "w");
	if(!destination)
	{
		pack_fclose(source);
		return;
	}
	while(!pack_feof(source))
	{
		pack_putc(pack_getc(source), destination);
	}
	pack_fclose(source);
	pack_fclose(destination);
}

int pp_replay_read_options(void)
{
	int i;

	if(pp_replay_file)
	{
		pp_copy_options();
		for(i = 0; i < 4; i++)
		{
			pp_game_data.player[i].entered = pack_igetl(pp_replay_file);
			if(pp_game_data.player[i].entered == 3)
			{
				pack_fread(pp_replay_character[i], 256, pp_replay_file);
				if(!exists(pp_replay_character[i]) && !exists(get_filename(pp_replay_character[i])))
				{
					return 0;
				}
				if(!load_char(&pp_game_data.player[i], pp_replay_character[i]))
				{
					return 0;
				}
//				memcpy(pp_game_data.player[i].name, get_filename(pp_replay_character[i]), 224);
				strcpy(pp_game_data.player[i].name, get_filename(pp_replay_character[i]));
				delete_extension(pp_game_data.player[i].name);
			}
		}
		pack_fread(pp_replay_level, 256, pp_replay_file);
		destroy_level(pp_game_data.level);
		pp_game_data.level = load_level(pp_replay_level);
		if(!pp_game_data.level)
		{
			return 0;
		}
		pack_fread(pp_replay_music, 256, pp_replay_file);
		if(exists(pp_replay_music))
		{
			strcpy(pp_title_music_name, pp_replay_music);
		}
		else
		{
			strcpy(pp_title_music_name, music_file[rand() % music_index]);
		}
		for(i = 0; i < PP_OPTIONS; i++)
		{
			pp_option[i] = pack_igetl(pp_replay_file);
		}
	}
	return 1;
}

void pp_replay_write_options(void)
{
	int i;

	if(pp_replay_file)
	{
		for(i = 0; i < 4; i++)
		{
			pack_iputl(pp_game_data.player[i].entered, pp_replay_file);
			if(pp_game_data.player[i].entered == 3)
			{
				pack_fwrite(pp_title_char_name[i], 256, pp_replay_file);
			}
		}
		pack_fwrite(pp_title_level_name, 256, pp_replay_file);
		pack_fwrite(pp_title_music_name, 256, pp_replay_file);
		for(i = 0; i < PP_OPTIONS; i++)
		{
			pack_iputl(pp_option[i], pp_replay_file);
		}
	}
}

int pp_replay_start_play(char * fn, int rew)
{
	int i, j, who;
	int p[2], pc;

	pp_replay_stop();
	pp_replay_file = pack_fopen(fn, "r");
	if(!pp_replay_file)
	{
		return 0;
	}
	pp_random_seed = pack_igetl(pp_replay_file);
	pp_game_data.mode = pack_igetl(pp_replay_file);
	if(!pp_replay_read_options())
	{
		return 0;
	}

	srand(pp_random_seed);
	
	/* if level or character not installed, abort replay */
	if(!pp_game_data.level)
	{
		return 0;
	}
	if(!rew)
	{
		if(pp_players_in() > 2 || pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_QUARTER)
		{
			for(i = 0; i < 4; i++)
			{
				pp_game_data.player[i].screen.bp = pp_game_data.player[i].screen.b[PP_SCREEN_MODE_QUARTER];
				if(!player_generate_screen(&pp_game_data.player[i], pp_game_data.level))
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
			}
		}
		else
		{
			if(pp_game_data.mode == PP_MODE_TARGET || pp_game_data.mode == PP_MODE_SCAVENGER)
			{
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
				}
				if(pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_VERTICAL)
				{
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
				}
				else
				{
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
				}
			}
		}
		ncds_stop_music();
	}
	if(pp_option[PP_OPTION_POWER_RAND])
	{
		power_list_rand();
		ammo_list_rand();
	}
	else
	{
		for(i = 0; i < 5; i++)
		{
			power_list[i] = dpower_list[i];
			ammo_list[i] = dammo_list[i];
		}
	}
	switch(pp_game_data.mode)
	{
		case PP_MODE_TARGET:
		{
			pp_game_retrieve_objects();
			if(!player_init(&pp_game_data.player[0], 0))
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
			pp_game_data.player[1].active = 0;
			pp_game_data.player[1].just = 0;
			pp_game_data.player[2].active = 0;
			pp_game_data.player[2].just = 0;
			pp_game_data.player[3].active = 0;
			pp_game_data.player[3].just = 0;
			if(pp_game_data.targets > 0)
			{
				who = pp_game_data.target_list[rand() % pp_game_data.targets];
				pp_game_data.target_sprite = object_drop(&pp_game_data.object[who], PP_OBJECT_TARGET, 0, 0);
				if(pp_game_data.target_sprite >= 0)
				{
					pp_game_data.object[pp_game_data.target_sprite].flag[PP_TARGET_FLAG_WHO] = who;
				}
				else
				{
					return 0;
				}
			}
			pp_option[PP_OPTION_AMMO_TIME] = 900;
			pp_option[PP_OPTION_POWER_TIME] = 1800;
			break;
		}
		case PP_MODE_SCAVENGER:
		{
			pp_game_retrieve_objects();
			if(!player_init(&pp_game_data.player[0], 0))
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
			pp_game_data.player[1].active = 0;
			pp_game_data.player[1].just = 0;
			pp_game_data.player[2].active = 0;
			pp_game_data.player[2].just = 0;
			pp_game_data.player[3].active = 0;
			pp_game_data.player[3].just = 0;
			if(pp_game_data.hunts > 0)
			{
				who = pp_game_data.hunt_list[rand() % pp_game_data.hunts];
				pp_game_data.gem_sprite = object_drop(&pp_game_data.object[who], PP_OBJECT_GEM_1, 0, 0);
				pp_game_data.object[who].active = 0;
			}
			break;
		}
		case PP_MODE_SPLAT:
		case PP_MODE_TAG:
		{
			pp_game_retrieve_objects();
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].entered == 3)
				{
					if(!player_init(&pp_game_data.player[i], 0))
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
				}
				else
				{
					pp_game_data.player[i].active = 0;
					pp_game_data.player[i].just = 0;
				}
			}
			break;
		}
		case PP_MODE_FLAG:
		{
			pp_game_retrieve_objects();
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].entered == 3)
				{
					if(!player_init(&pp_game_data.player[i], 0))
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
				}
				else
				{
					pp_game_data.player[i].active = 0;
					pp_game_data.player[i].just = 0;
				}
			}
			if(pp_game_data.flags > 0 && pp_game_data.bases > 0)
			{
				pp_game_data.flag_sprite = object_drop(&pp_game_data.object[pp_game_data.flag_list[rand() % pp_game_data.flags]], PP_OBJECT_FLAG, 0, 0);
				pp_game_data.base_sprite = object_drop(&pp_game_data.object[pp_game_data.base_list[rand() % pp_game_data.bases]], PP_OBJECT_BASE, 0, 0);
			}
			break;
		}
		case PP_MODE_HUNTER:
		{
			pp_option[PP_OPTION_GAME_TYPE] = PP_OPTION_GAME_NORMAL;
			pp_game_retrieve_objects();
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].entered == 3)
				{
					if(!player_init(&pp_game_data.player[i], 0))
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
				}
				else
				{
					pp_game_data.player[i].active = 0;
					pp_game_data.player[i].just = 0;
				}
			}
			break;
		}
	}
	if(pp_game_data.mode == PP_MODE_SCAVENGER || pp_game_data.mode == PP_MODE_TARGET)
	{
		pp_game_data.time = 3600;
	}
	else
	{
		pp_game_data.time = pp_option[PP_OPTION_GAME_TIME];
	}
	tileset_set_trans(pp_game_data.level->tileset, pp_config[PP_CONFIG_TRANS]);
	reset_tileset(pp_game_data.level->tileset);
	reset_paintballs();
	reset_particles();
	if(!rew)
	{
		ncds_play_music(pp_title_music_name, 1);
	}
	pp_replay_set_camera(4);
	pp_game_data.frame = 0;
	replay_pause = 0;
	pp_game_data.winner = -1;

	return 1;
}

void pp_replay_start_record(char * fn)
{
	pp_replay_stop_record();
	if(pp_config[PP_CONFIG_REPLAY])
	{
		pp_replay_file = pack_fopen(fn, "w");
		pack_iputl(pp_random_seed, pp_replay_file);
		pack_iputl(pp_game_data.mode, pp_replay_file);
		pp_replay_write_options();
	}
}

void pp_replay_stop(void)
{
	int i;

	if(pp_replay_file)
	{
		pack_fclose(pp_replay_file);
		pp_replay_file = NULL;
		pp_restore_options();
	}
}

void pp_replay_stop_record(void)
{
	int i;

	if(pp_replay_file)
	{
		pack_fclose(pp_replay_file);
		pp_replay_file = NULL;
	}
}

void pp_replay_exit(void)
{
	pp_replay_stop();
	pp_state = PP_STATE_TITLE;
	pp_update();
	pp_draw();
	ncds_play_music(PP_MUSIC_FILE_MENU, 1);
	pp_select_menu(PP_MENU_PLAY);
	pp_next_reset = 0;
	pp_start_fade(black_palette, pp_palette, 2);
	gametime_reset();
	logic_switch = 1;
//	pp_menu[PP_MENU_MAIN].selected = 0;
}

void pp_replay_logic(void)
{
	int i;
	unsigned char bit;

	pp_fix_camera();
	if(!(replay_pause && !replay_step) && !pack_feof(pp_replay_file))
	{
//		pp_replay_read_controls();
		for(i = 0; i < 4; i++)
		{
			if(pp_game_data.player[i].active)
			{
				bit = pack_getc(pp_replay_file);
				uncompress_controller(&pp_game_data.player[i].controller, &bit);
				read_controller_net(&pp_game_data.player[i].controller);
				if(!pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out)
				{
					player_control(&pp_game_data.player[i]);
				}
    		}
		}
		for(i = 0; i < MAX_PARTICLES; i++)
		{
			particle_logic(&pp_game_data.particle[i]);
		}
		for(i = 0; i < PP_MAX_OBJECTS; i++)
		{
			object_logic(&pp_game_data.object[i]);
		}
		for(i = 0; i < 4; i++)
		{
			player_logic(i, pp_game_data.level->tilemap);
		}
		for(i = 0; i < MAX_PAINTBALLS; i++)
		{
			paintball_logic(&pp_game_data.paintball[i]);
		}

		/* update tile animations */
	    animate_tileset(pp_game_data.level->tileset);
	    pp_game_data.frame++;

	    pp_game_rules();
	}
	if(replay_step)
	{
		replay_step--;
	}
    if(key[KEY_ESC] && !pp_fading)
	{
//		pp_select_menu(PP_MENU_MAIN);
		pp_next_proc = pp_replay_exit;
		pp_start_fade(pp_palette, black_palette, 2);
		key[KEY_ESC] = 0;
	}
	if(key[KEY_SPACE])
	{
		replay_pause = 1 - replay_pause;
		key[KEY_SPACE] = 0;
	}
	if(key[KEY_ENTER])
	{
		replay_pause = 1;
		replay_step = 1;
		key[KEY_ENTER] = 0;
	}
	if(key[KEY_1])
	{
		pp_replay_set_camera(0);
		replay_zoom = 0;
	}
	if(key[KEY_2])
	{
		pp_replay_set_camera(1);
		replay_zoom = 0;
	}
	if(key[KEY_3])
	{
		pp_replay_set_camera(2);
		replay_zoom = 0;
	}
	if(key[KEY_4])
	{
		pp_replay_set_camera(3);
		replay_zoom = 0;
	}
	if(key[KEY_5])
	{
		pp_replay_set_camera(4);
		replay_zoom = 0;
	}
	if(key[KEY_Z])
	{
		if(replay_camera < 4 || pp_game_data.mode == PP_MODE_SCAVENGER || pp_game_data.mode == PP_MODE_TARGET)
		{
			pp_replay_toggle_zoom();
		}
		key[KEY_Z] = 0;
	}
	if(key[KEY_L])
	{
//		if(exists(pp_replay_music))
//		{
			ncds_play_music(pp_title_music_name, 1);
//		}
		key[KEY_L] = 0;
	}
	pp_replay_pan();
	if(pp_next_proc)
	{
		if(!pp_fading)
		{
			pp_next_proc();
			pp_next_proc = NULL;
		}
	}
}
