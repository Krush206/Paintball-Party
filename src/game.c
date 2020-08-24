#include "includes.h"

int dpower_list[6] = {PP_OBJECT_POWER_CLOAK, PP_OBJECT_POWER_DEFLECT, PP_OBJECT_POWER_JUMP, PP_OBJECT_POWER_RUN, PP_OBJECT_POWER_FLY, PP_OBJECT_POWER_TURBO};
int power_list[6] = {PP_OBJECT_POWER_CLOAK, PP_OBJECT_POWER_DEFLECT, PP_OBJECT_POWER_JUMP, PP_OBJECT_POWER_RUN, PP_OBJECT_POWER_FLY, PP_OBJECT_POWER_TURBO};
int dammo_list[5] = {PP_OBJECT_AMMO_NORMAL, PP_OBJECT_AMMO_X, PP_OBJECT_AMMO_MINE, PP_OBJECT_AMMO_BOUNCE, PP_OBJECT_AMMO_SEEK};
int ammo_list[5] = {PP_OBJECT_AMMO_NORMAL, PP_OBJECT_AMMO_X, PP_OBJECT_AMMO_MINE, PP_OBJECT_AMMO_BOUNCE, PP_OBJECT_AMMO_SEEK};

int players_left(void)
{
	int i, c;
	
	c = 0;
	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].active)
		{
			c++;
		}
	}
	return c;
}

void power_list_rand(void)
{
	int i, j;
	int list[6] = {PP_OBJECT_POWER_CLOAK, PP_OBJECT_POWER_DEFLECT, PP_OBJECT_POWER_JUMP, PP_OBJECT_POWER_RUN, PP_OBJECT_POWER_FLY, PP_OBJECT_POWER_TURBO};
	int list_length = 6;
	int r;
	
	for(i = 0; i < 6; i++)
	{
		r = rand() % list_length;
		power_list[i] = list[r];
		for(j = r; j < 5; j++)
		{
			list[j] = list[j + 1];
		}
		list_length--;
	}
}

void ammo_list_rand(void)
{
	int i, j;
	int list[5] = {PP_OBJECT_AMMO_NORMAL, PP_OBJECT_AMMO_X, PP_OBJECT_AMMO_MINE, PP_OBJECT_AMMO_BOUNCE, PP_OBJECT_AMMO_SEEK};
	int list_length = 5;
	int r;
	
	for(i = 0; i < 5; i++)
	{
		r = rand() % list_length;
		ammo_list[i] = list[r];
		for(j = r; j < 4; j++)
		{
			list[j] = list[j + 1];
		}
		list_length--;
	}
}

int pp_game_drop_player(int player)
{
	int i, chosen;

	/* create the portal list */
	pp_game_data.portals = 0;
	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		if(pp_game_data.object[i].active && pp_game_data.object[i].type == PP_OBJECT_PORTAL)
		{
			pp_game_data.portal_list[pp_game_data.portals] = i;
			pp_game_data.portals++;
		}
	}

	/* choose one of the available portals */
	if(pp_game_data.portals > 0)
	{
		chosen = pp_game_data.portal_list[rand() % pp_game_data.portals];
	}
	else
	{
		return 0;
	}
	pp_game_data.player[player].fx = pp_game_data.object[chosen].x + character_place->w / 2 - pp_game_data.player[player].character.ap[0]->w / 2;
	if(pp_game_data.player[player].character.cy_table[0] + pp_game_data.player[player].character.ch_table[0] <= 48)
	{
		pp_game_data.player[player].fy = pp_game_data.object[chosen].y + character_place->h / 2 - pp_game_data.player[player].character.ap[0]->h / 2;
	}
	else
	{
		pp_game_data.player[player].fy = pp_game_data.object[chosen].y - itofix((pp_game_data.player[player].character.cy_table[0] + pp_game_data.player[player].character.ch_table[0]) - character_place->h);
	}
	pp_game_data.player[player].x = fixtoi(pp_game_data.player[player].fx);
	pp_game_data.player[player].y = fixtoi(pp_game_data.player[player].fy);
	pp_game_data.object[chosen].active = 0;
	player_update_collision_map(&pp_game_data.player[player]);
	
	return 1;
}

void pp_game_retrieve_objects(void)
{
	int i, j;

	pp_game_data.flags = 0;
	pp_game_data.bases = 0;
	pp_game_data.targets = 0;
	pp_game_data.hunts = 0;
	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		switch(pp_game_data.level->object_type[i])
		{
			case PP_OBJECT_AMMO_NORMAL:
			{
				pp_game_data.object[i].type = ammo_list[0];
				break;
			}
			case PP_OBJECT_AMMO_X:
			{
				pp_game_data.object[i].type = ammo_list[1];
				break;
			}
			case PP_OBJECT_AMMO_MINE:
			{
				pp_game_data.object[i].type = ammo_list[2];
				break;
			}
			case PP_OBJECT_AMMO_BOUNCE:
			{
				pp_game_data.object[i].type = ammo_list[3];
				break;
			}
			case PP_OBJECT_AMMO_SEEK:
			{
				pp_game_data.object[i].type = ammo_list[4];
				break;
			}
			case PP_OBJECT_POWER_CLOAK:
			{
				pp_game_data.object[i].type = power_list[0];
				break;
			}
			case PP_OBJECT_POWER_DEFLECT:
			{
				pp_game_data.object[i].type = power_list[1];
				break;
			}
			case PP_OBJECT_POWER_JUMP:
			{
				pp_game_data.object[i].type = power_list[2];
				break;
			}
			case PP_OBJECT_POWER_RUN:
			{
				pp_game_data.object[i].type = power_list[3];
				break;
			}
			case PP_OBJECT_POWER_FLY:
			{
				pp_game_data.object[i].type = power_list[4];
				break;
			}
			case PP_OBJECT_POWER_TURBO:
			{
				pp_game_data.object[i].type = power_list[5];
				break;
			}
			default:
			{
				pp_game_data.object[i].type = pp_game_data.level->object_type[i];
			}
		}
//		pp_game_data.object[i].type = pp_game_data.level->object_type[i];
		pp_game_data.object[i].x = itofix(pp_game_data.level->object_x[i]);
		pp_game_data.object[i].y = itofix(pp_game_data.level->object_y[i]);
		pp_game_data.object[i].active = pp_game_data.level->object_active[i];
		if(pp_game_data.object[i].type == PP_OBJECT_FLAG_PORTAL && pp_game_data.object[i].active)
		{
			pp_game_data.flag_list[pp_game_data.flags] = i;
			pp_game_data.flags++;
		}
		else if(pp_game_data.object[i].type == PP_OBJECT_BASE_PORTAL && pp_game_data.object[i].active)
		{
			pp_game_data.base_list[pp_game_data.bases] = i;
			pp_game_data.bases++;
		}
		else if(pp_game_data.object[i].type == PP_OBJECT_TARGET_PORTAL && pp_game_data.object[i].active)
		{
			pp_game_data.target_list[pp_game_data.targets] = i;
			pp_game_data.targets++;
		}
		else if(pp_game_data.object[i].type == PP_OBJECT_HUNT_PORTAL && pp_game_data.object[i].active)
		{
			pp_game_data.hunt_list[pp_game_data.hunts] = i;
			pp_game_data.hunts++;
		}
		if(pp_game_data.mode == PP_MODE_SCAVENGER)
		{
			switch(pp_game_data.object[i].type)
			{
				case PP_OBJECT_AMMO_NORMAL:
				case PP_OBJECT_AMMO_X:
				case PP_OBJECT_AMMO_MINE:
				case PP_OBJECT_AMMO_BOUNCE:
				case PP_OBJECT_AMMO_SEEK:
				case PP_OBJECT_POWER_CLOAK:
				case PP_OBJECT_POWER_DEFLECT:
				case PP_OBJECT_POWER_JUMP:
				case PP_OBJECT_POWER_RUN:
				case PP_OBJECT_POWER_FLY:
				case PP_OBJECT_POWER_TURBO:
				{
					pp_game_data.object[i].active = 0;
					break;
				}
			}
		}
		else if(pp_game_data.mode == PP_MODE_TARGET)
		{
			if(pp_game_data.object[i].type == PP_OBJECT_HUNT_BASE)
			{
				pp_game_data.object[i].active = 0;
			}
		}
		else
		{
			switch(pp_game_data.object[i].type)
			{
				case PP_OBJECT_HUNT_BASE:
				{
					pp_game_data.object[i].active = 0;
					break;
				}
				case PP_OBJECT_AMMO_NORMAL:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_AMMO_NORMAL_FLAG];
					break;
				}
				case PP_OBJECT_AMMO_X:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_AMMO_X_FLAG];
					break;
				}
				case PP_OBJECT_AMMO_MINE:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_AMMO_MINE_FLAG];
					break;
				}
				case PP_OBJECT_AMMO_BOUNCE:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_AMMO_BOUNCE_FLAG];
					break;
				}
				case PP_OBJECT_AMMO_SEEK:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_AMMO_SEEK_FLAG];
					break;
				}
				case PP_OBJECT_POWER_CLOAK:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_POWER_CLOAK];
					break;
				}
				case PP_OBJECT_POWER_DEFLECT:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_POWER_DEFLECT];
					break;
				}
				case PP_OBJECT_POWER_RUN:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_POWER_RUN];
					break;
				}
				case PP_OBJECT_POWER_JUMP:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_POWER_JUMP];
					break;
				}
				case PP_OBJECT_POWER_FLY:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_POWER_FLY];
					break;
				}
				case PP_OBJECT_POWER_TURBO:
				{
					pp_game_data.object[i].active *= pp_option[PP_OPTION_POWER_TURBO];
					break;
				}
			}
		}
		for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
		{
			pp_game_data.object[i].flag[j] = pp_game_data.level->object_flag[i][j];
		}
		pp_game_data.object[i].ap = pp_object_ani[pp_game_data.object[i].type];
		object_initialize(&pp_game_data.object[i]);
	}
}

void set_default_options(void)
{
	pp_config[PP_CONFIG_LOGIC] = PP_CONFIG_LOGIC_TIMED;
	pp_config[PP_CONFIG_VSYNC] = 0;
	pp_config[PP_CONFIG_RADAR] = 1;
	pp_config[PP_CONFIG_ICONS] = 1;
	pp_config[PP_CONFIG_TRANS] = 1;
	pp_config[PP_CONFIG_LOWRES] = 0;
	pp_config[PP_CONFIG_MUSIC_VOLUME] = 50;
	pp_config[PP_CONFIG_SOUND_VOLUME] = 100;
	pp_config[PP_CONFIG_JOY] = 1;
	pp_config[PP_CONFIG_RANDOM_MUSIC] = 0;
	pp_config[PP_CONFIG_DMUSIC] = 1;
	pp_config[PP_CONFIG_REPLAY] = 1;
	pp_config[PP_CONFIG_G_PLAYED] = 0;
	pp_config[PP_CONFIG_P_P1] = 0;
	pp_config[PP_CONFIG_P_P2] = 1;
	pp_config[PP_CONFIG_P_P3] = 2;
	pp_config[PP_CONFIG_P_P4] = 3;
	pp_game_data.player[0].data_pick = 0;
	pp_game_data.player[1].data_pick = 1;
	pp_game_data.player[2].data_pick = 2;
	pp_game_data.player[3].data_pick = 3;
	pp_config[PP_CONFIG_SPLIT_MODE] = PP_SCREEN_MODE_VERTICAL;
	pp_config[PP_CONFIG_NET_THROTTLE] = 0;
}

void set_default_game_options(void)
{
	pp_option[PP_OPTION_GAME_TYPE] = PP_OPTION_GAME_NORMAL;
	pp_option[PP_OPTION_GAME_TIME] = 7200;
	pp_option[PP_OPTION_SF_HITS] = 10;
	pp_option[PP_OPTION_T_TIME] = 3600;
	pp_option[PP_OPTION_F_FLAGS] = 10;
	pp_option[PP_OPTION_H_HITS] = 10;
	pp_option[PP_OPTION_AMMO_TIME] = 900;
	pp_option[PP_OPTION_POWER_TIME] = 1800;
	pp_option[PP_OPTION_AMMO_NORMAL_FLAG] = 1;
	pp_option[PP_OPTION_AMMO_X_FLAG] = 1;
	pp_option[PP_OPTION_AMMO_MINE_FLAG] = 1;
	pp_option[PP_OPTION_AMMO_BOUNCE_FLAG] = 1;
	pp_option[PP_OPTION_AMMO_SEEK_FLAG] = 1;
	pp_option[PP_OPTION_AMMO_NORMAL] = 20;
	pp_option[PP_OPTION_AMMO_X] = 0;
	pp_option[PP_OPTION_AMMO_MINE] = 10;
	pp_option[PP_OPTION_AMMO_BOUNCE] = 0;
	pp_option[PP_OPTION_AMMO_SEEK] = 0;
	pp_option[PP_OPTION_POWER_CLOAK] = 1;
	pp_option[PP_OPTION_POWER_DEFLECT] = 1;
	pp_option[PP_OPTION_POWER_RUN] = 1;
	pp_option[PP_OPTION_POWER_JUMP] = 1;
	pp_option[PP_OPTION_POWER_FLY] = 1;
	pp_option[PP_OPTION_POWER_TURBO] = 1;
	pp_option[PP_OPTION_POWER_RAND] = 0;
//	pp_option[PP_OPTION_L_PLAYED] = 0;
//	pp_option[PP_OPTION_L_1P] = 0;
//	pp_option[PP_OPTION_L_2P] = 0;
//	pp_option[PP_OPTION_L_3P] = 0;
//	pp_option[PP_OPTION_L_4P] = 0;
}

void pp_game_pick_music(void)
{
	int i;
//	char * type[7] = {"mid", "mp3", "s3m", "xm", "it", "mod", "ogg"};
	char * fn = NULL;
		
	if(!pp_config[PP_CONFIG_RANDOM_MUSIC])
	{
		if(!pp_config[PP_CONFIG_DMUSIC] && strchr(pp_title_level_name, '#'))
		{
			fn = NULL;
		}
		else
		{
			fn = ncds_get_music(pp_title_level_name);
		}
		if(fn != NULL)
		{
			strcpy(pp_title_music_name, fn);
			free(fn);
		}
		else if(music_index > 0)
		{
			strcpy(pp_title_music_name, music_file[rand() % music_index]);
		}
//		for(i = 0; i < 7; i++)
//		{
//			if(exists(replace_extension(temp_file, pp_title_level_name, type[i], 256)))
//			{
//				strcpy(pp_title_music_name, temp_file);
//				break;
//			}
//		}
//		if(i >= 7)
//		{
//			strcpy(pp_title_music_name, music_file[rand() % music_index]);
//		}
	}
	else if(music_index > 0)
	{
		strcpy(pp_title_music_name, music_file[rand() % music_index]);
	}
	else
	{
		strcpy(pp_title_music_name, "");
	}
}

/* set up everything for a specific game mode */
int pp_game_init(int mode)
{
	int i, j, who;
	int c = 1;

	/* pick the music before randomizing so we don't throw off the random numbers */
	pp_game_pick_music();

	pp_random_seed = time(0);
	srand(pp_random_seed);
	
	pp_center_menu(&pp_menu[PP_MENU_EXIT], pp_screen, 0, 0);
	pp_center_menu(&pp_menu[PP_MENU_DONE], pp_screen, 0, 0);
	ncds_stop_music();
	if(mode == PP_MODE_TARGET)
	{
		pp_copy_options();
		set_default_game_options();
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
	switch(mode)
	{
		case PP_MODE_TARGET:
		{
			/* set up the screen */
			if(pp_config[PP_CONFIG_LOWRES] && (SCREEN_W != 320 || SCREEN_H != 240))
			{
				set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
				pp_center_menu(&pp_menu[PP_MENU_EXIT], pp_screen, 0, 0);
				pp_center_menu(&pp_menu[PP_MENU_DONE], pp_screen, 0, 0);
			}

			pp_game_data.mode = mode;
			pp_game_retrieve_objects();
			if(!player_init(&pp_game_data.player[0], 1))
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
			}
			else
			{
				return 0;
			}
			break;
		}
		case PP_MODE_SCAVENGER:
		{
			/* set up the screen */
			if(pp_config[PP_CONFIG_LOWRES] && (SCREEN_W != 320 || SCREEN_H != 240))
			{
				set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
				pp_center_menu(&pp_menu[PP_MENU_EXIT], pp_screen, 0, 0);
				pp_center_menu(&pp_menu[PP_MENU_DONE], pp_screen, 0, 0);
			}

			pp_game_data.mode = mode;
			pp_game_retrieve_objects();
			if(!player_init(&pp_game_data.player[0], 1))
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
			else
			{
				return 0;
			}
			break;
		}
		case PP_MODE_SPLAT:
		case PP_MODE_TAG:
		case PP_MODE_HUNTER:
		{
			pp_game_data.mode = mode;
			pp_game_retrieve_objects();
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].entered == 3)
				{
					if(!player_init(&pp_game_data.player[i], 1))
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
//					pp_game_data.player[i].active = 0;
					pp_game_data.player[i].just = 0;
				}
			}
			break;
		}
		case PP_MODE_FLAG:
		{
			pp_game_data.mode = mode;
			pp_game_retrieve_objects();
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].entered == 3)
				{
					if(!player_init(&pp_game_data.player[i], 1))
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
	ncds_play_music(pp_title_music_name, 1);
	pp_game_data.player[0].screen.x = 0;
	pp_game_data.player[0].screen.y = 0;
	if(pp_config[PP_CONFIG_LOWRES])
	{
		pp_game_data.player[0].screen.w = 320;
		pp_game_data.player[0].screen.h = 240;
	}
	else
	{
		if(pp_game_data.mode == PP_MODE_SCAVENGER || pp_game_data.mode == PP_MODE_TARGET)
		{
			pp_game_data.player[0].screen.w = 640;
			pp_game_data.player[0].screen.h = 480;
		}
		else
		{
			pp_game_data.player[0].screen.w = 320;
			pp_game_data.player[0].screen.h = 240;
		}
	}
	pp_game_data.player[0].screen.active = 1;
	pp_game_data.player[1].screen.x = 320;
	pp_game_data.player[1].screen.y = 240;
	pp_game_data.player[1].screen.w = 320;
	pp_game_data.player[1].screen.h = 240;
	pp_game_data.player[1].screen.active = 1;
	pp_game_data.player[2].screen.x = 320;
	pp_game_data.player[2].screen.y = 0;
	pp_game_data.player[2].screen.w = 320;
	pp_game_data.player[2].screen.h = 240;
	pp_game_data.player[2].screen.active = 1;
	pp_game_data.player[3].screen.x = 0;
	pp_game_data.player[3].screen.y = 240;
	pp_game_data.player[3].screen.w = 320;
	pp_game_data.player[3].screen.h = 240;
	pp_game_data.player[3].screen.active = 1;
	pp_game_data.frame = 0;
	pp_game_data.winner = -1;
	pp_replay_start_record("last.ppr");
	return 1;
}

/*void pp_game_exit(void)
{
	switch(pp_game_data.mode)
	{
		case PP_MODE_TARGET:
		case PP_MODE_SCAVENGER:
		case PP_MODE_1P_TEST:
		{
			if(pp_config[PP_CONFIG_LOWRES])
			{
				set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
			}
			pp_restore_options();
		}
	}
} */

/* find out how many players are it (0 means tag everybody again) */
int pp_tag_all(void)
{
	int count = 0;
	int i;

	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].active && pp_game_data.player[i].it && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out)
		{
			count++;
		}
	}
	return count;
}

void pp_game_rules(void)
{
	int i, j, c, ci;
	int highest, phighest;

	/* handle some game rules for each mode */
	switch(pp_game_data.mode)
	{
		case PP_MODE_TARGET:
		{
			pp_game_data.time--;
			if(pp_game_data.time <= 300 && pp_game_data.time > 0 && pp_game_data.time % 60 == 0)
			{
				ncds_play_sample(pp_sound[PP_SOUND_TIME], 128, -1, -1);
			}
			else if(pp_game_data.time <= 0)
			{
				if(pp_game_data.target_sprite >= 0)
				{
					pp_game_data.object[pp_game_data.target_sprite].active = 0;
				}
				pp_game_data.winner = 0;
			}
			break;
		}
		case PP_MODE_SCAVENGER:
		{
			pp_game_data.time--;
			if(pp_game_data.time <= 300 && pp_game_data.time > 0 && pp_game_data.time % 60 == 0)
			{
				ncds_play_sample(pp_sound[PP_SOUND_TIME], 128, -1, -1);
			}
			else if(pp_game_data.time <= 0)
			{
				if(pp_game_data.player[0].gems < 5)
				{
					pp_game_data.object[pp_game_data.gem_sprite].active = 0;
					pp_game_data.player[0].item = 0;
				}
				pp_game_data.winner = 0;
			}
			if(pp_game_data.player[0].gems >= 5)
			{
				pp_game_data.winner = 0;
			}
			break;
		}
		case PP_MODE_SPLAT:
		{
			if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
			{
				for(i = 0; i < 4; i++)
				{
					if(pp_game_data.player[i].score >= pp_option[PP_OPTION_SF_HITS] && pp_players_left() > 1)
					{
						for(j = 0; j < 4; j++)
						{
							if(j != i && pp_game_data.player[j].active && !pp_game_data.player[j].tele_out)
							{
								pp_game_data.player[j].tele_out = 1;
								pp_game_data.player[j].tele_state = pp_game_data.player[j].state;
								pp_game_data.player[j].tele_frame = pp_game_data.player[j].frame;
								pp_game_data.player[j].frame = 0;
							    paintball_remove(pp_game_data.player[j].color);
							}
						}
						pp_game_data.winner = i;
						break;
					}
				}
			}
			else if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_TIMED)
			{
				pp_game_data.time--;
				if(pp_game_data.time <= 0)
				{
					phighest = pp_game_data.player[0].score;
					highest = 0;
					for(i = 0; i < 4; i++)
					{
						if(pp_game_data.player[i].active)
						{
							if(pp_game_data.player[i].score >= phighest)
							{
								highest = i;
								phighest = pp_game_data.player[i].score;
							}
						}
					}
					pp_game_data.winner = highest;
					for(i = 0; i < 4; i++)
					{
						if(!pp_game_data.player[i].tele_out && i != pp_game_data.winner)
						{
							pp_game_data.player[i].tele_out = 1;
							pp_game_data.player[i].tele_state = pp_game_data.player[i].state;
							pp_game_data.player[i].tele_frame = pp_game_data.player[i].frame;
							pp_game_data.player[i].frame = 0;
						    paintball_remove(pp_game_data.player[i].color);
						}
					}
				}
				if(pp_game_data.time <= 300 && pp_game_data.time > 0 && pp_game_data.time % 60 == 0)
				{
					ncds_play_sample(pp_sound[PP_SOUND_TIME], 128, -1, -1);
				}
			}
			break;
		}
		case PP_MODE_FLAG:
		{
			if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
			{
				for(i = 0; i < 4; i++)
				{
					if(pp_game_data.player[i].score >= pp_option[PP_OPTION_F_FLAGS])
					{
						for(j = 0; j < 4; j++)
						{
							if(j != i && pp_game_data.player[j].active && !pp_game_data.player[j].tele_out)
							{
								pp_game_data.player[j].tele_out = 1;
								pp_game_data.player[j].tele_state = pp_game_data.player[j].state;
								pp_game_data.player[j].tele_frame = pp_game_data.player[j].frame;
								pp_game_data.player[j].frame = 0;
							    paintball_remove(pp_game_data.player[j].color);
							}
						}
						pp_game_data.winner = i;
						break;
					}
				}
			}
			else if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_TIMED)
			{
				pp_game_data.time--;
				if(pp_game_data.time <= 0)
				{
					phighest = pp_game_data.player[0].score;
					highest = 0;
					for(i = 0; i < 4; i++)
					{
						if(pp_game_data.player[i].active)
						{
							if(pp_game_data.player[i].score >= phighest)
							{
								highest = i;
								phighest = pp_game_data.player[i].score;
							}
						}
					}
					pp_game_data.winner = highest;
					for(i = 0; i < 4; i++)
					{
						if(!pp_game_data.player[i].tele_out && i != pp_game_data.winner)
						{
							pp_game_data.player[i].tele_out = 1;
							pp_game_data.player[i].tele_state = pp_game_data.player[i].state;
							pp_game_data.player[i].tele_frame = pp_game_data.player[i].frame;
							pp_game_data.player[i].frame = 0;
						    paintball_remove(pp_game_data.player[i].color);
						}
						pp_game_data.player[i].flag = 0;
						if(pp_game_data.flag_sprite >= 0)
						{
							if(pp_game_data.object[pp_game_data.flag_sprite].type == PP_OBJECT_FLAG)
							{
								pp_game_data.object[pp_game_data.flag_sprite].active = 0;
							}
						}
						if(pp_game_data.base_sprite >= 0)
						{
							if(pp_game_data.object[pp_game_data.base_sprite].type == PP_OBJECT_BASE)
							{
								pp_game_data.object[pp_game_data.base_sprite].active = 0;
							}
						}
					}
				}
				if(pp_game_data.time <= 300 && pp_game_data.time > 0 && pp_game_data.time % 60 == 0)
				{
					ncds_play_sample(pp_sound[PP_SOUND_TIME], 128, -1, -1);
				}
			}
			break;
		}
		case PP_MODE_TAG:
		{
			c = 0;
			if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_NORMAL)
			{
				for(i = 0; i < 4; i++)
				{
					if(pp_game_data.player[i].active)
					{
	        			if(pp_game_data.player[i].it && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out)
	        			{
		        			pp_game_data.player[i].time++;
		        			if(pp_game_data.player[i].time >= pp_option[PP_OPTION_T_TIME] - 300 && pp_game_data.player[i].time < pp_option[PP_OPTION_T_TIME] && pp_game_data.player[i].time % 60 == 0)
		        			{
								ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_TIME], 128, -1, -1);
	        				}
	        			}
						if(pp_game_data.player[i].time >= pp_option[PP_OPTION_T_TIME] && !pp_game_data.player[i].tele_out)
						{
							pp_game_data.player[i].tele_out = 1;
							pp_game_data.player[i].tele_state = pp_game_data.player[i].state;
							pp_game_data.player[i].tele_frame = pp_game_data.player[i].frame;
							pp_game_data.player[i].frame = 0;
						    paintball_remove(pp_game_data.player[i].color);
//							for(j = 0; j < 4; j++)
//							{
//								if(pp_game_data.player[j].active)
//								{
//									pp_game_data.player[j].it = 1;
//								}
//							}
						}
						if(pp_game_data.player[i].weapon == PP_AMMO_TYPE_SEEK)
						{
							if(pp_game_data.player[i].target >= 0)
							{
								if(!pp_game_data.player[pp_game_data.player[i].target].active)
								{
									pp_cycle_target(&pp_game_data.player[i]);
								}
							}
						}
					}
				}
				if(pp_tag_all() <= 0)
				{
					for(i = 0; i < 4; i++)
					{
						if(pp_game_data.player[i].active && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out)
						{
							pp_game_data.player[i].it = 1;
						}
					}
				}
				for(i = 0; i < 4; i++)
				{
					if(pp_game_data.player[i].active && !pp_game_data.player[i].tele_out)
					{
						c++;
						ci = i;
					}
				}
				if(c == 1)
				{
					pp_game_data.winner = ci;
				}
			}
			else if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_TIMED)
			{
				for(i = 0; i < 4; i++)
				{
					if(pp_game_data.player[i].active && !pp_game_data.player[i].tele_out)
					{
	        			if(pp_game_data.player[i].it)
			        	{
				        	pp_game_data.player[i].time++;
	        			}
					}
				}
				pp_game_data.time--;
				if(pp_game_data.time <= 0)
				{
					pp_game_data.time = 0;
					phighest = pp_game_data.player[0].time;
					highest = 0;
					for(i = 0; i < 4; i++)
					{
						if(pp_game_data.player[i].active)
						{
							if(pp_game_data.player[i].time <= phighest)
							{
								highest = i;
								phighest = pp_game_data.player[i].time;
							}
						}
					}
					pp_game_data.winner = highest;
					for(i = 0; i < 4; i++)
					{
						if(!pp_game_data.player[i].tele_out && i != pp_game_data.winner)
						{
							pp_game_data.player[i].tele_out = 1;
							pp_game_data.player[i].tele_state = pp_game_data.player[i].state;
							pp_game_data.player[i].tele_frame = pp_game_data.player[i].frame;
							pp_game_data.player[i].frame = 0;
						    paintball_remove(pp_game_data.player[i].color);
						}
					}
				}
				if(pp_game_data.time <= 300 && pp_game_data.time > 0 && pp_game_data.time % 60 == 0)
				{
					ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_TIME], 128, -1, -1);
				}
			}
			break;
		}
		case PP_MODE_HUNTER:
		{
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].active && !pp_game_data.player[i].tele_out)
				{
					if(pp_game_data.player[i].hits >= pp_option[PP_OPTION_H_HITS] && pp_players_left() > 1)
					{
						pp_game_data.player[i].tele_out = 1;
						pp_game_data.player[i].tele_state = pp_game_data.player[i].state;
						pp_game_data.player[i].tele_frame = pp_game_data.player[i].frame;
						pp_game_data.player[i].frame = 0;
					    paintball_remove(pp_game_data.player[i].color);
					}
					if(pp_game_data.player[i].weapon == PP_AMMO_TYPE_SEEK)
					{
						if(pp_game_data.player[i].target >= 0)
						{
							if(!pp_game_data.player[pp_game_data.player[i].target].active || pp_game_data.player[pp_game_data.player[i].target].tele_out)
							{
								pp_cycle_target(&pp_game_data.player[i]);
							}
						}
					}
				}
			}
			c = 0;
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].active && !pp_game_data.player[i].tele_out)
				{
					c++;
					ci = i;
				}
			}
			if(c == 1)
			{
				pp_game_data.winner = ci;
			}
			break;
		}
	}
}

int pp_players_left(void)
{
	int i;
	int count = 0;

	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].active && !pp_game_data.player[i].tele_out)
		{
			count++;
		}
	}
	return count;
}

int pp_players_in(void)
{
	int i;
	int count = 0;

	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].entered == 3)
		{
			count++;
		}
	}
	return count;
}

void pp_game_logic(void)
{
	int i, j, k;
	int f[4] = {0, 0, 0, 0};
	int okay;
	int fr = 0;
	char * fn;
	unsigned char bit = 0;

	ncd_joy_poll();
	read_joy_keys();
	
	frame_okay = 1;
	for(j = 0; j < frame_okay; j++)
	{
		for(i = 0; i < 4; i++)
		{
			if(pp_game_data.player[i].active)
			{
				if(pp_replay_file && !demo_playback)
				{
					get_key_state(&pp_game_data.player[i].controller);
					compress_controller(&pp_game_data.player[i].controller, &bit);
					pack_putc(bit, pp_replay_file);
					read_controller_net(&pp_game_data.player[i].controller);
				}
				else if(demo_playback && pp_replay_file)
				{
					bit = pack_getc(pp_replay_file);
					uncompress_controller(&pp_game_data.player[i].controller, &bit);
					read_controller_net(&pp_game_data.player[i].controller);
				}
				player_control(&pp_game_data.player[i]);
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
	frame_okay = 0;
	if(pp_game_data.winner >= 0 && players_left() <= 1)
	{
		pp_replay_stop_record();
		if(pp_game_data.player[pp_game_data.winner].screen.mp)
		{
			center_tilemap(pp_game_data.player[pp_game_data.winner].screen.mp, pp_game_data.level->tilemap, pp_game_data.player[pp_game_data.winner].x + pp_game_data.player[pp_game_data.winner].character.ap[0]->w / 2, pp_game_data.player[pp_game_data.winner].y + pp_game_data.player[pp_game_data.winner].character.ap[0]->h / 2);
		}
		acquire_bitmap(screen);
		pp_vsync();
		for(i = 0; i < 4; i++)
		{
			if(i != pp_game_data.winner && pp_game_data.player[i].entered == 3)
			{
				stretch_blit(pp_image[PP_IMAGE_ELIMINATED], screen, 0, 0, pp_image[PP_IMAGE_ELIMINATED]->w, pp_image[PP_IMAGE_ELIMINATED]->h, pp_game_data.player[i].screen.x, pp_game_data.player[i].screen.y, pp_game_data.player[i].screen.w, pp_game_data.player[i].screen.h);
			}
		}
		release_bitmap(screen);
		pp_config[PP_CONFIG_G_PLAYED]++;
		
		if(pp_game_data.mode != PP_MODE_SCAVENGER && pp_game_data.mode != PP_MODE_TARGET)
		{
			for(i = 0; i < 4; i++)
			{	
				if(pp_game_data.player[i].entered == 3)
				{
					pp_player_data[pp_game_data.player[i].data_pick].played++;
				}
			}
			pp_player_data[pp_game_data.player[pp_game_data.winner].data_pick].won++;
		}

		/* attempt to play character theme */
		if(pp_config[PP_CONFIG_CHAR_THEMES])
		{
			fn = ncds_get_music(pp_title_char_name[pp_game_data.winner]);
			if(fn)
			{
	   			ncds_play_music(fn, 0);
			}
		}
		ncds_queue_sample(pp_game_data.player[pp_game_data.winner].sound[PP_SOUND_WIN]);
		pp_state = PP_STATE_STATS;
	}
	
   	if(key[KEY_ESC])
	{
		pp_select_menu(PP_MENU_EXIT);
		pp_state = PP_STATE_QUIT;
		blit(screen, static_screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		key[KEY_ESC] = 0;
	}
}

void pp_game_in_logic(void)
{
//	if(pp_game_data.mode == PP_MODE_1P_SCAVENGER || pp_game_data.mode == PP_MODE_1P_TARGET || pp_game_data.mode == PP_MODE_1P_TEST)
	int p[2], pc;
	int i;
	
	if(pp_game_data.mode == PP_MODE_SCAVENGER || pp_game_data.mode == PP_MODE_TARGET)
	{
		pp_state = PP_STATE_GAME;
	}
	else
	{
		if(pp_players_in() > 2 || pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_QUARTER)
		{
			pp_game_data.player[0].screen.x += 8;
			pp_game_data.player[0].screen.y += 6;
			if(pp_game_data.player[0].screen.x >= 0 && pp_game_data.player[0].screen.y >= 0)
			{
				if(pp_state != PP_STATE_GAME)
				{
					ncds_play_sample(pp_game_data.player[0].sound[PP_SOUND_CRUSH], 128, -1, -1);
				}
				pp_state = PP_STATE_GAME;
			}
			pp_game_data.player[0].just = 1;
			pp_game_data.player[1].screen.x -= 8;
			pp_game_data.player[1].screen.y -= 6;
			if(pp_game_data.player[1].screen.x <= 320 && pp_game_data.player[1].screen.y <= 240)
			{
				if(pp_state != PP_STATE_GAME)
				{
					ncds_play_sample(pp_game_data.player[1].sound[PP_SOUND_CRUSH], 128, -1, -1);
				}
				pp_state = PP_STATE_GAME;
			}
			pp_game_data.player[1].just = 1;
			pp_game_data.player[2].screen.x -= 8;
			pp_game_data.player[2].screen.y += 6;
			if(pp_game_data.player[2].screen.x <= 320 && pp_game_data.player[2].screen.y >= 0)
			{
				if(pp_state != PP_STATE_GAME)
				{
					ncds_play_sample(pp_game_data.player[2].sound[PP_SOUND_CRUSH], 128, -1, -1);
				}
				pp_state = PP_STATE_GAME;
			}
			pp_game_data.player[2].just = 1;
			pp_game_data.player[3].screen.x += 8;
			pp_game_data.player[3].screen.y -= 6;
			if(pp_game_data.player[3].screen.x >= 0 && pp_game_data.player[3].screen.y <= 240)
			{
				if(pp_state != PP_STATE_GAME)
				{
					ncds_play_sample(pp_game_data.player[3].sound[PP_SOUND_CRUSH], 128, -1, -1);
				}
				pp_state = PP_STATE_GAME;
			}
			pp_game_data.player[3].just = 1;
		}
		else
		{
			pc = 0;
			for(i = 0; i < 4; i++)
			{
				if(pp_game_data.player[i].active)
				{
					p[pc] = i;
					pc++;
					pp_game_data.player[i].just = 0;
				}
			}
			if(pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_VERTICAL)
			{
				pp_game_data.player[p[0]].screen.x += 8;
				if(pp_game_data.player[p[0]].screen.x >= 0)
				{
					if(pp_state != PP_STATE_GAME)
					{
						ncds_play_sample(pp_game_data.player[p[0]].sound[PP_SOUND_CRUSH], 128, -1, -1);
					}
					pp_state = PP_STATE_GAME;
				}
				pp_game_data.player[p[0]].just = 1;
				pp_game_data.player[p[1]].screen.x -= 8;
				if(pp_game_data.player[p[1]].screen.x <= 320)
				{
					if(pp_state != PP_STATE_GAME)
					{
						ncds_play_sample(pp_game_data.player[p[1]].sound[PP_SOUND_CRUSH], 128, -1, -1);
					}
					pp_state = PP_STATE_GAME;
				}
				pp_game_data.player[p[1]].just = 1;
			}
			else
			{
				pp_game_data.player[p[0]].screen.y += 6;
				if(pp_game_data.player[p[0]].screen.y >= 0)
				{
					if(pp_state != PP_STATE_GAME)
					{
						ncds_play_sample(pp_game_data.player[p[0]].sound[PP_SOUND_CRUSH], 128, -1, -1);
					}
					pp_state = PP_STATE_GAME;
				}
				pp_game_data.player[p[0]].just = 1;
				pp_game_data.player[p[1]].screen.y -= 6;
				if(pp_game_data.player[p[1]].screen.y <= 240)
				{
					if(pp_state != PP_STATE_GAME)
					{
						ncds_play_sample(pp_game_data.player[p[1]].sound[PP_SOUND_CRUSH], 128, -1, -1);
					}
					pp_state = PP_STATE_GAME;
				}
				pp_game_data.player[p[1]].just = 1;
			}
		}
	}
}

void pp_quit_yes_proc(void)
{
	int i;

	pp_replay_stop_record();
	for(i = 0; i < 4; i++)
	{
		destroy_char(&pp_game_data.player[i]);
		pp_game_data.player[i].entered = 0;
	}
	destroy_level(pp_game_data.level);
	pp_game_data.level = NULL;
	if(pp_game_data.mode == PP_MODE_TARGET)
	{
		pp_restore_options();
	}
	pp_fade(pp_palette, black_palette, 2);
	pp_state = PP_STATE_TITLE;
	pp_select_menu(PP_MENU_MAIN);
	ncds_stop_music();
	if(SCREEN_W != 640 || SCREEN_H != 480)
	{
		set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
	}
	pp_start_song("menu.xm", PP_MUSIC_FILE_MENU);
	set_palette(black_palette);
	pp_update();
	pp_draw();
	pp_start_fade(black_palette, pp_palette, 2);
	pp_write_config("pp.cfg");
	pp_player_data_save("pp.pdt");
	gametime_reset();
	logic_switch = 1;
}

void pp_quit_yes(void)
{
	pp_select_menu(PP_MENU_BLANK);
	pp_next_proc = pp_quit_yes_proc;
}

void pp_quit_no_proc(void)
{
	pp_state = PP_STATE_GAME;
}

void pp_quit_no(void)
{
	pp_select_menu(PP_MENU_BLANK);
	pp_next_proc = pp_quit_no_proc;
}

void pp_done_no_proc(void)
{
	pp_replay_stop_record();
	if(pp_game_data.mode == PP_MODE_TARGET)
	{
		pp_restore_options();
	}
	if(!pp_game_init(pp_game_data.mode))
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
	gametime_reset();
	logic_switch = 1;
}

void pp_done_no(void)
{
	pp_select_menu(PP_MENU_BLANK);
	pp_next_proc = pp_done_no_proc;
}

void pp_instant_replay_proc(void)
{
	char temp_name[1024] = {0};
	int saved = 0;

	ncds_pause_music();
	while(!saved)
	{
		if(file_select_ex("Save Replay:", temp_name, "ppr", 1024, 320, 240))
		{
			if(exists(replace_extension(temp_name, temp_name, "ppr", 1000)))
			{
				if(alert(NULL, "Overwrite?", NULL, "Yes", "No", 0, 0) == 1)
				{
					pp_copy_file("last.ppr", replace_extension(temp_name, temp_name, "ppr", 1000));
					saved = 1;
				}
			}
			else
			{
				pp_copy_file("last.ppr", replace_extension(temp_name, temp_name, "ppr", 1000));
				saved = 1;
			}
		}
		else
		{
			saved = 1;
		}
	}
	ncds_resume_music();
	gametime_reset();
	logic_switch = 1;
	pp_select_menu(PP_MENU_DONE);
	pp_next_proc = pp_nop;
}

void pp_instant_replay(void)
{
	pp_select_menu(PP_MENU_BLANK);
	pp_next_proc = pp_instant_replay_proc;
}

void pp_game_quit_logic(void)
{
	pp_title_logic();
}

void pp_game_quit_update(void)
{
	blit(static_screen, pp_screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	#ifdef USE_ALLEGTTF
		aatextout_center(pp_screen, al_font[0], pp_menu[pp_current_menu].title, SCREEN_W / 2 + rand() % 5 - 2, pp_menu[pp_current_menu].y - 64 + rand() % 5 - 2, 220);
		aatextout_center(pp_screen, al_font[0], pp_menu[pp_current_menu].title, SCREEN_W / 2, pp_menu[pp_current_menu].y - 64, 223);
	#else
		textout_centre(pp_screen, al_font[0], pp_menu[pp_current_menu].title, SCREEN_W / 2 + rand() % 5 - 2, pp_menu[pp_current_menu].y - 64 + rand() % 5 - 2, 220);
		textout_centre(pp_screen, al_font[0], pp_menu[pp_current_menu].title, SCREEN_W / 2, pp_menu[pp_current_menu].y - 64, 223);
	#endif
	pp_draw_menu(pp_screen, &pp_menu[pp_current_menu]);
}

void pp_game_quit_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void pp_game_update_radar(PLAYER * pp)
{
	int i, j, k;
	int rx, ry;
	int px, py;
	int guy = -1;
	int st, sb, sl, sr;
	int tx, ty;

	if(pp_game_data.level->tilemap->cr - pp_game_data.level->tilemap->cx < pp->radar_image->w)
	{
		rx = (pp_game_data.level->tilemap->cr - pp_game_data.level->tilemap->cx - pp->radar_image->w) / 2 + pp_game_data.level->tilemap->cx;
	}
	else
	{
		rx = pp_game_data.level->tilemap->x / pp_game_data.level->tileset->w + (pp->screen.bp->w / pp_game_data.level->tileset->w) / 2 - pp->radar_image->w / 2;
		if(rx < pp_game_data.level->tilemap->cx)
		{
			rx = pp_game_data.level->tilemap->cx;
		}
		if(rx + pp->radar_image->w > pp_game_data.level->tilemap->cr)
		{
			rx = pp_game_data.level->tilemap->cr - pp->radar_image->w + 1;
		}
	}
	if(pp_game_data.level->tilemap->cb - pp_game_data.level->tilemap->cy < pp->radar_image->h)
	{
		ry = (pp_game_data.level->tilemap->cb - pp_game_data.level->tilemap->cy - pp->radar_image->h) / 2 + pp_game_data.level->tilemap->cy;
	}
	else
	{
		ry = pp_game_data.level->tilemap->y / pp_game_data.level->tileset->h + (pp->screen.bp->h / pp_game_data.level->tileset->h) / 2 - pp->radar_image->h / 2;
		if(ry < pp_game_data.level->tilemap->cy)
		{
			ry = pp_game_data.level->tilemap->cy;
		}
		if(ry + pp->radar_image->h > pp_game_data.level->tilemap->cb)
		{
			ry = pp_game_data.level->tilemap->cb - pp->radar_image->h + 1;
		}
	}

	clear_to_color(pp->radar_image, 53);
	for(i = 0; i < pp->radar_image->h; i++)
	{
		for(j = 0; j < pp->radar_image->w; j++)
		{
			st = tilemap_flag_tile(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, (rx + j), (ry + i));
			sb = tilemap_flag_tile(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDB, (rx + j), (ry + i));
			sl = tilemap_flag_tile(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDL, (rx + j), (ry + i));
			sr = tilemap_flag_tile(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDR, (rx + j), (ry + i));
			if(st && sb && sl && sr)
			{
				if(rx + j >= pp_game_data.level->tilemap->cx && rx + j <= pp_game_data.level->tilemap->cr && ry + i >= pp_game_data.level->tilemap->cy && ry + i <= pp_game_data.level->tilemap->cb)
				{
					putpixel(pp->radar_image, j, i, 50);
				}
			}
			else if(st || sb || sl || sr)
			{
				if(rx + j >= pp_game_data.level->tilemap->cx && rx + j <= pp_game_data.level->tilemap->cr && ry + i >= pp_game_data.level->tilemap->cy && ry + i <= pp_game_data.level->tilemap->cb)
				{
					putpixel(pp->radar_image, j, i, 52);
				}
			}
		}
	}
	px = (pp->x + pp->character.mix) / pp_game_data.level->tileset->w - rx;
	py = (pp->y + pp->character.miy) / pp_game_data.level->tileset->h - ry;
	putpixel(pp->radar_image, px, py, 31);
	putpixel(pp->radar_image, px - 1, py, 31);
	putpixel(pp->radar_image, px, py - 1, 31);
	putpixel(pp->radar_image, px + 1, py, 31);
	putpixel(pp->radar_image, px, py + 1, 31);
	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].active && i != pp->color)
		{
			if(pp_game_data.player[i].flag)
			{
				guy = i;
			}
			if(!pp_game_data.player[i].cloak_time)
			{
				px = (pp_game_data.player[i].x + pp_game_data.player[i].character.mix) / pp_game_data.level->tileset->w - rx;
				py = (pp_game_data.player[i].y + pp_game_data.player[i].character.miy) / pp_game_data.level->tileset->h - ry;
				if(pp_game_data.player[i].flag)
				{
					putpixel(pp->radar_image, px, py, 210);
					putpixel(pp->radar_image, px - 1, py, 210);
					putpixel(pp->radar_image, px, py - 1, 210);
					putpixel(pp->radar_image, px + 1, py, 210);
					putpixel(pp->radar_image, px, py + 1, 210);
				}
				else
				{
					putpixel(pp->radar_image, px, py, 80);
					putpixel(pp->radar_image, px - 1, py, 80);
					putpixel(pp->radar_image, px, py - 1, 80);
					putpixel(pp->radar_image, px + 1, py, 80);
					putpixel(pp->radar_image, px, py + 1, 80);
				}
			}
		}
	}
	if(pp_game_data.mode == PP_MODE_FLAG)
	{
		if(guy < 0 && !pp->flag && pp_game_data.flag_sprite >= 0)
		{
			px = fixtoi(pp_game_data.object[pp_game_data.flag_sprite].x + pp_game_data.object[pp_game_data.flag_sprite].ap->w / 2) / pp_game_data.level->tileset->w - rx;
			py = fixtoi(pp_game_data.object[pp_game_data.flag_sprite].y + pp_game_data.object[pp_game_data.flag_sprite].ap->h / 2) / pp_game_data.level->tileset->h - ry;
			putpixel(pp->radar_image, px, py, 210);
			putpixel(pp->radar_image, px - 1, py, 210);
			putpixel(pp->radar_image, px, py - 1, 210);
			putpixel(pp->radar_image, px + 1, py, 210);
			putpixel(pp->radar_image, px, py + 1, 210);
		}
		if(pp_game_data.base_sprite >= 0)
		{
			px = fixtoi(pp_game_data.object[pp_game_data.base_sprite].x + pp_game_data.object[pp_game_data.base_sprite].ap->w / 2) / pp_game_data.level->tileset->w - rx;
			py = fixtoi(pp_game_data.object[pp_game_data.base_sprite].y + pp_game_data.object[pp_game_data.base_sprite].ap->h / 2) / pp_game_data.level->tileset->h - ry;
			putpixel(pp->radar_image, px, py, 151);
			putpixel(pp->radar_image, px - 1, py, 151);
			putpixel(pp->radar_image, px, py - 1, 151);
			putpixel(pp->radar_image, px + 1, py, 151);
			putpixel(pp->radar_image, px, py + 1, 151);
		}
	}
	if(pp_game_data.mode == PP_MODE_TARGET)
	{
		if(pp_game_data.target_sprite >= 0)
		{
			if(pp_game_data.object[pp_game_data.target_sprite].active)
			{
				px = fixtoi(pp_game_data.object[pp_game_data.target_sprite].x + pp_game_data.object[pp_game_data.target_sprite].ap->w / 2) / pp_game_data.level->tileset->w - rx;
				py = fixtoi(pp_game_data.object[pp_game_data.target_sprite].y + pp_game_data.object[pp_game_data.target_sprite].ap->h / 2) / pp_game_data.level->tileset->h - ry;
				putpixel(pp->radar_image, px, py, 95);
				putpixel(pp->radar_image, px - 1, py, 31);
				putpixel(pp->radar_image, px, py - 1, 31);
				putpixel(pp->radar_image, px + 1, py, 31);
				putpixel(pp->radar_image, px, py + 1, 31);
			}
		}
	}
	for(i = 0; i < MAX_PAINTBALLS; i++)
	{
		if(pp_game_data.paintball[i].active)
		{
			px = (pp_game_data.paintball[i].x + pp_game_data.player[pp_game_data.paintball[i].who].character.pap->w / 2) / pp_game_data.level->tileset->w - rx;
			py = (pp_game_data.paintball[i].y + pp_game_data.player[pp_game_data.paintball[i].who].character.pap->h / 2) / pp_game_data.level->tileset->h - ry;
			putpixel(pp->radar_image, px, py, 112);
		}
	}
	if(pp_config[PP_CONFIG_TRANS])
	{
		color_map = &pp_color_map;
		draw_trans_sprite(pp->screen.mp, pp->radar_image, pp->screen.mp->w - pp->radar_image->w - 2, 2);
	}
	else
	{
		draw_sprite(pp->screen.mp, pp->radar_image, pp->screen.mp->w - pp->radar_image->w - 2, 2);
	}
}

void pp_game_draw_status(int i)
{
	player_message_draw(pp_game_data.player[i].screen.mp, &pp_game_data.player[i].message, pp_game_data.player[i].x + pp_game_data.player[i].character.ap[0]->w / 2, pp_game_data.player[i].y);
	switch(pp_game_data.mode)
	{
		case PP_MODE_SCAVENGER:
		{
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_GEMS_0 + pp_game_data.player[i].gems], 8, 1);
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_CLOCK], 1, 31);
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15, 31 + 7, (pp_game_data.time + 59) / 3600, 2);
			ncd_textout(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 16, 31 + 7, ":");
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 24, 31 + 7, ((pp_game_data.time + 59) / 60) % 60, 2);
			break;
		}
		case PP_MODE_1P_TEST:
		case PP_MODE_TARGET:
		{
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_TARGETS], 1, 1);
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 27, 1 + 14, pp_game_data.player[i].score, 3);
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_CLOCK], 1, 31);
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15, 31 + 7, (pp_game_data.time + 59) / 3600, 2);
			ncd_textout(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 16, 31 + 7, ":");
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 24, 31 + 7, ((pp_game_data.time + 59) / 60) % 60, 2);
			if(pp_game_data.player[i].ammo[pp_game_data.player[i].weapon])
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_AMMO], 56, 8);
				ncd_printf(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 56 + 7, 8 + 7, "%s", weapon_table[pp_game_data.player[i].weapon]);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 56 + 22, 8 + 7, pp_game_data.player[i].ammo[pp_game_data.player[i].weapon], 2);
			}
			if(pp_game_data.player[i].cloak_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 60, 31);
			}
			if(pp_game_data.player[i].deflect_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 73, 31);
			}
			if(pp_game_data.player[i].fly_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 86, 31);
			}
			if(pp_game_data.player[i].run_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 60, 44);
			}
			if(pp_game_data.player[i].moon_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 73, 44);
			}
			if(pp_game_data.player[i].turbo_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 86, 44);
			}
			break;
		}
		case PP_MODE_SPLAT:
		{
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_SCORE], 1, 1);
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 20, 1 + 14, pp_game_data.player[i].score, 4);
			if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_TIMED)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_CLOCK], 1, 31);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15, 31 + 7, (pp_game_data.time + 59) / 3600, 2);
				ncd_textout(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 16, 31 + 7, ":");
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 24, 31 + 7, ((pp_game_data.time + 59) / 60) % 60, 2);
				if(pp_game_data.player[i].cloak_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 60, 31);
				}
				if(pp_game_data.player[i].deflect_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 73, 31);
				}
				if(pp_game_data.player[i].fly_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 86, 31);
				}
				if(pp_game_data.player[i].run_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 60, 44);
				}
				if(pp_game_data.player[i].moon_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 73, 44);
				}
				if(pp_game_data.player[i].turbo_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 86, 44);
				}
			}
			else
			{
				if(pp_game_data.player[i].cloak_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 42, 31);
				}
				if(pp_game_data.player[i].deflect_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 55, 31);
				}
				if(pp_game_data.player[i].fly_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 68, 31);
				}
				if(pp_game_data.player[i].run_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 42, 44);
				}
				if(pp_game_data.player[i].moon_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 55, 44);
				}
				if(pp_game_data.player[i].turbo_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 68, 44);
				}
			}
			if(pp_game_data.player[i].ammo[pp_game_data.player[i].weapon])
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_AMMO], 57, 8);
				ncd_printf(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 57 + 7, 8 + 7, "%s", weapon_table[pp_game_data.player[i].weapon]);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 57 + 22, 8 + 7, pp_game_data.player[i].ammo[pp_game_data.player[i].weapon], 2);
			}
			break;
		}
		case PP_MODE_HUNTER:
		{
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_HITS], 1, 1);
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 20, 1 + 14, pp_game_data.player[i].hits, 4);
			if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_TIMED)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_CLOCK], 1, 31);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15, 31 + 7, (pp_game_data.time + 59) / 3600, 2);
				ncd_textout(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 16, 31 + 7, ":");
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 24, 31 + 7, ((pp_game_data.time + 59) / 60) % 60, 2);
			}
			if(pp_game_data.player[i].ammo[pp_game_data.player[i].weapon])
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_AMMO], 57, 8);
				ncd_printf(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 57 + 7, 8 + 7, "%s", weapon_table[pp_game_data.player[i].weapon]);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 57 + 22, 8 + 7, pp_game_data.player[i].ammo[pp_game_data.player[i].weapon], 2);
			}
			if(pp_game_data.player[i].cloak_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 42, 31);
			}
			if(pp_game_data.player[i].deflect_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 55, 31);
			}
			if(pp_game_data.player[i].fly_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 68, 31);
			}
			if(pp_game_data.player[i].run_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 42, 44);
			}
			if(pp_game_data.player[i].moon_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 55, 44);
			}
			if(pp_game_data.player[i].turbo_time)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 68, 44);
			}
			break;
		}
		case PP_MODE_TAG:
		{
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_TIME], 1, 1);
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 22, 1 + 13, (pp_game_data.player[i].time) / 3600, 2);
			ncd_textout(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 22 + 16, 1 + 13, ":");
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 22 + 24, 1 + 13, ((pp_game_data.player[i].time) / 60) % 60, 2);
			if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_TIMED)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_CLOCK], 1, 31);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15, 31 + 7, (pp_game_data.time + 59) / 3600, 2);
				ncd_textout(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 16, 31 + 7, ":");
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 24, 31 + 7, ((pp_game_data.time + 59) / 60) % 60, 2);
				if(pp_game_data.player[i].cloak_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 60, 30);
				}
				if(pp_game_data.player[i].deflect_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 73, 30);
				}
				if(pp_game_data.player[i].fly_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 86, 30);
				}
				if(pp_game_data.player[i].run_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 60, 43);
				}
				if(pp_game_data.player[i].moon_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 73, 43);
				}
				if(pp_game_data.player[i].turbo_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 86, 43);
				}
			}
			else
			{
				if(pp_game_data.player[i].cloak_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 52, 31);
				}
				if(pp_game_data.player[i].deflect_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 65, 31);
				}
				if(pp_game_data.player[i].fly_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 88, 31);
				}
				if(pp_game_data.player[i].run_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 52, 44);
				}
				if(pp_game_data.player[i].moon_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 65, 44);
				}
				if(pp_game_data.player[i].turbo_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 88, 44);
				}
			}
			if(pp_game_data.player[i].ammo[pp_game_data.player[i].weapon])
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_AMMO], 67, 7);
				ncd_printf(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 67 + 7, 7 + 7, "%s", weapon_table[pp_game_data.player[i].weapon]);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 67 + 22, 7 + 7, pp_game_data.player[i].ammo[pp_game_data.player[i].weapon], 2);
			}
			break;
		}
		case PP_MODE_FLAG:
		{
			draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_FLAGS], 1, 1);
			ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 18, 1 + 14, pp_game_data.player[i].score, 3);
			if(pp_option[PP_OPTION_GAME_TYPE] == PP_OPTION_GAME_TIMED)
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_CLOCK], 1, 31);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15, 31 + 7, (pp_game_data.time + 59) / 3600, 2);
				ncd_textout(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 16, 31 + 7, ":");
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 1 + 15 + 24, 31 + 7, ((pp_game_data.time + 59) / 60) % 60, 2);
				if(pp_game_data.player[i].cloak_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 60, 31);
				}
				if(pp_game_data.player[i].deflect_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 73, 31);
				}
				if(pp_game_data.player[i].fly_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 86, 31);
				}
				if(pp_game_data.player[i].run_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 60, 44);
				}
				if(pp_game_data.player[i].moon_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 73, 44);
				}
				if(pp_game_data.player[i].turbo_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 86, 44);
				}
			}
			else
			{
				if(pp_game_data.player[i].cloak_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_C], 32, 31);
				}
				if(pp_game_data.player[i].deflect_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_D], 45, 31);
				}
				if(pp_game_data.player[i].fly_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_F], 58, 31);
				}
				if(pp_game_data.player[i].run_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_R], 32, 44);
				}
				if(pp_game_data.player[i].moon_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_J], 45, 44);
				}
				if(pp_game_data.player[i].turbo_time)
				{
					draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_POWER_T], 58, 44);
				}
			}
			if(pp_game_data.player[i].ammo[pp_game_data.player[i].weapon])
			{
				draw_sprite(pp_game_data.player[i].screen.mp, pp_image[PP_IMAGE_AMMO], 47, 8);
				ncd_printf(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 47 + 7, 8 + 7, "%s", weapon_table[pp_game_data.player[i].weapon]);
				ncd_draw_number(pp_game_data.player[i].screen.mp, pp_font[PP_FONT_GAME], 47 + 22, 8 + 7, pp_game_data.player[i].ammo[pp_game_data.player[i].weapon], 2);
			}
			break;
		}
	}
}

void pp_game_update(void)
{
	int i, j, guy = -1;
	int p;

	/* render each active player's screen */
	for(i = 0; i < 4; i++)
	{
//		if(pp_game_data.player[i].active && !pp_game_data.player[i].ai_flag)
		if(pp_game_data.player[i].active && pp_game_data.player[i].screen.mp)
		{
			/* center tilemap around player i */
			center_tilemap(pp_game_data.player[i].screen.mp, pp_game_data.level->tilemap, pp_game_data.player[i].x + pp_game_data.player[i].character.ap[0]->w / 2, pp_game_data.player[i].y + pp_game_data.player[i].character.ap[0]->h / 2);

			/* draw the background layers */
			draw_tilemap_bg(pp_game_data.player[i].screen.mp, pp_game_data.level->tilemap);

			/* draw active objects */
			for(j = 0; j < PP_MAX_OBJECTS; j++)
			{
				if(pp_game_data.object[j].active)
				{
					object_draw(pp_game_data.player[i].screen.mp, &pp_game_data.object[j], -pp_game_data.level->tilemap->x, -pp_game_data.level->tilemap->y);
				}
			}

			/* draw the active paintballs */
			for(j = 0; j < MAX_PAINTBALLS; j++)
			{
				if(pp_game_data.paintball[j].active)
				{
//					draw_ani(pp_game_data.player[i].screen.mp, pp_game_data.paintball[j].ap, pp_game_data.paintball[j].x - pp_game_data.level->tilemap->x, pp_game_data.paintball[j].y - pp_game_data.level->tilemap->y, pp_game_data.paintball[j].frame);
					rotate_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_game_data.paintball[j].ap, pp_game_data.paintball[j].frame), pp_game_data.paintball[j].x - pp_game_data.level->tilemap->x, pp_game_data.paintball[j].y - pp_game_data.level->tilemap->y, pp_game_data.paintball[j].angle);
				}
			}

			/* draw the active players */
			for(j = 0; j < 4; j++)
			{
				if(pp_game_data.player[j].active)
				{
					/* if player is teleporting */
					if(pp_game_data.player[j].tele_in || pp_game_data.player[j].tele_out)
					{
				    	draw_sprite(pp_game_data.player[i].screen.mp, pp_game_data.player[j].zap_image, pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y);
					}

					else if(pp_game_data.player[j].flash_time && pp_game_data.player[j].flash_time % 2 == 0)
					{
						draw_solid_ani(pp_game_data.player[i].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, 31, pp_game_data.player[j].frame);
					}

					/* otherwise draw as normal */
					else
					{
						if(i == j)
						{
							if(!pp_game_data.player[j].cloak_time)
							{
								draw_ani(pp_game_data.player[i].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, pp_game_data.player[j].frame);
							}
							else
							{
								color_map = &pp_color_map;
								draw_trans_ani(pp_game_data.player[i].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, pp_game_data.player[j].frame);
							}
						}
						else
						{
							if(!pp_game_data.player[j].cloak_time)
							{
								draw_ani(pp_game_data.player[i].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, pp_game_data.player[j].frame);
							}
						}
					}
					if(pp_config[PP_CONFIG_ICONS] && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out && pp_game_data.winner < 0)
					{
						if((pp_game_data.player[j].it || pp_game_data.player[j].flag) && !pp_game_data.player[j].tele_in && !pp_game_data.player[j].tele_out)
						{
							if(i == j)
							{
								if(pp_game_data.player[j].cloak_time)
								{
									color_map = &pp_color_map;
									draw_trans_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_IT_POINTER], pp_game_data.frame), pp_game_data.player[j].x + pp_game_data.player[j].character.ix - pp_game_data.level->tilemap->x, pp_game_data.player[j].y + pp_game_data.player[j].character.iy - pp_game_data.level->tilemap->y);
								}
								else
								{
									draw_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_IT_POINTER], pp_game_data.frame), pp_game_data.player[j].x + pp_game_data.player[j].character.ix - pp_game_data.level->tilemap->x, pp_game_data.player[j].y + pp_game_data.player[j].character.iy - pp_game_data.level->tilemap->y);
								}
							}
							else
							{
								if(!pp_game_data.player[j].cloak_time)
								{
									draw_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_IT_POINTER], pp_game_data.frame), pp_game_data.player[j].x + pp_game_data.player[j].character.ix - pp_game_data.level->tilemap->x, pp_game_data.player[j].y + pp_game_data.player[j].character.iy - pp_game_data.level->tilemap->y);
								}
							}
						}
					}
				}
			}

			/* draw the active gun flashes */
			for(j = 0; j < 4; j++)
			{
				if(pp_game_data.player[j].active && pp_game_data.player[j].gun_flash.active)
				{
					draw_ani(pp_game_data.player[i].screen.mp, pp_game_data.player[j].gun_flash.ap, pp_game_data.player[j].gun_flash.x - pp_game_data.level->tilemap->x, pp_game_data.player[j].gun_flash.y - pp_game_data.level->tilemap->y, pp_game_data.player[j].gun_flash.frame);
				}
			}

			/* draw the active particles */
			for(j = 0; j < MAX_PARTICLES; j++)
			{
				if(pp_game_data.particle[j].active)
				{
					draw_ani(pp_game_data.player[i].screen.mp, pp_game_data.particle[j].ap, pp_game_data.particle[j].x - pp_game_data.level->tilemap->x - 1, pp_game_data.particle[j].y - pp_game_data.level->tilemap->y - 1, pp_game_data.frame);
				}
			}

			/* draw the foreground layers */
			draw_tilemap_fg(pp_game_data.player[i].screen.mp, pp_game_data.level->tilemap);
			
			if(pp_config[PP_CONFIG_ICONS] && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out && pp_game_data.winner < 0)
			{
				if(pp_game_data.mode == PP_MODE_FLAG)
				{
					for(j = 0; j < 4; j++)
					{
						if(pp_game_data.player[j].flag && pp_game_data.player[j].active)
						{
							guy = j;
						}
					}
					if(pp_game_data.player[i].flag && pp_game_data.base_sprite >= 0)
					{
						pivot_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_POINTER], pp_game_data.frame), (pp_game_data.player[i].x + pp_game_data.player[i].character.mix) - pp_game_data.level->tilemap->x, (pp_game_data.player[i].y + pp_game_data.player[i].character.miy) - pp_game_data.level->tilemap->y, pp_game_data.player[i].character.mir, pp_ani[PP_ANI_POINTER]->h / 2, fatan2(fsub(pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.miy), pp_game_data.object[pp_game_data.base_sprite].y + itofix(pp_game_data.object[pp_game_data.base_sprite].ap->w / 2)), fsub(pp_game_data.player[i].fx + itofix(pp_game_data.player[i].character.mix), pp_game_data.object[pp_game_data.base_sprite].x + itofix(pp_game_data.object[pp_game_data.base_sprite].ap->w / 2))));
					}
					else if(pp_game_data.flag_sprite >= 0)
					{
						if(guy < 0 && pp_game_data.object[pp_game_data.flag_sprite].active)
						{
							pivot_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_POINTER], pp_game_data.frame), (pp_game_data.player[i].x + pp_game_data.player[i].character.mix) - pp_game_data.level->tilemap->x, (pp_game_data.player[i].y + pp_game_data.player[i].character.miy) - pp_game_data.level->tilemap->y, pp_game_data.player[i].character.mir, pp_ani[PP_ANI_POINTER]->h / 2, fatan2(fsub(pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.miy), pp_game_data.object[pp_game_data.flag_sprite].y + itofix(pp_game_data.object[pp_game_data.flag_sprite].ap->h / 2)), fsub(pp_game_data.player[i].fx + itofix(pp_game_data.player[i].character.mix), pp_game_data.object[pp_game_data.flag_sprite].x + itofix(pp_game_data.object[pp_game_data.flag_sprite].ap->w / 2))));
						}
						else if(!pp_game_data.player[guy].cloak_time && !pp_game_data.player[guy].tele_in && !pp_game_data.player[guy].tele_out)
						{
							pivot_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_POINTER], pp_game_data.frame), (pp_game_data.player[i].x + pp_game_data.player[i].character.mix) - pp_game_data.level->tilemap->x, (pp_game_data.player[i].y + pp_game_data.player[i].character.miy) - pp_game_data.level->tilemap->y, pp_game_data.player[i].character.mir, pp_ani[PP_ANI_POINTER]->h / 2, fatan2(fsub(pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.miy), itofix(pp_game_data.player[guy].y + pp_game_data.player[guy].character.miy)), fsub(pp_game_data.player[i].fx + itofix(pp_game_data.player[i].character.mix), itofix(pp_game_data.player[guy].x + pp_game_data.player[guy].character.mix))));
						}
					}
				}
				if(pp_game_data.mode == PP_MODE_TARGET)
				{
					if(pp_game_data.target_sprite >= 0)
					{
						if(pp_game_data.player[i].weapon == PP_AMMO_TYPE_SEEK)
						{
							pivot_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_TARGET], pp_game_data.frame), (pp_game_data.player[i].x + pp_game_data.player[i].character.mix) - pp_game_data.level->tilemap->x, (pp_game_data.player[i].y + pp_game_data.player[i].character.miy) - pp_game_data.level->tilemap->y, pp_game_data.player[i].character.mir, pp_ani[PP_ANI_TARGET]->h / 2, fatan2(fsub(pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.miy), pp_game_data.object[pp_game_data.target_sprite].y + itofix(pp_game_data.object[pp_game_data.target_sprite].ap->h / 2)), fsub(pp_game_data.player[i].fx + itofix(pp_game_data.player[i].character.mix), pp_game_data.object[pp_game_data.target_sprite].x + itofix(pp_game_data.object[pp_game_data.target_sprite].ap->w / 2))));
						}
						else
						{
							pivot_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_POINTER], pp_game_data.frame), (pp_game_data.player[i].x + pp_game_data.player[i].character.mix) - pp_game_data.level->tilemap->x, (pp_game_data.player[i].y + pp_game_data.player[i].character.miy) - pp_game_data.level->tilemap->y, pp_game_data.player[i].character.mir, pp_ani[PP_ANI_POINTER]->h / 2, fatan2(fsub(pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.miy), pp_game_data.object[pp_game_data.target_sprite].y + itofix(pp_game_data.object[pp_game_data.target_sprite].ap->h / 2)), fsub(pp_game_data.player[i].fx + itofix(pp_game_data.player[i].character.mix), pp_game_data.object[pp_game_data.target_sprite].x + itofix(pp_game_data.object[pp_game_data.target_sprite].ap->w / 2))));
						}
					}
				}
				else
				{
					if(pp_game_data.player[i].target >= 0)
					{
						if(pp_game_data.player[i].weapon == PP_AMMO_TYPE_SEEK && pp_game_data.player[pp_game_data.player[i].target].active && pp_game_data.player[i].target != i && !pp_game_data.player[pp_game_data.player[i].target].tele_in && !pp_game_data.player[pp_game_data.player[i].target].tele_out)
						{
							pivot_sprite(pp_game_data.player[i].screen.mp, get_ani(pp_ani[PP_ANI_TARGET], pp_game_data.frame), (pp_game_data.player[i].x + pp_game_data.player[i].character.mix) - pp_game_data.level->tilemap->x, (pp_game_data.player[i].y + pp_game_data.player[i].character.miy) - pp_game_data.level->tilemap->y, pp_game_data.player[i].character.mir, pp_ani[PP_ANI_TARGET]->h / 2, fatan2(fsub(pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.miy), itofix(pp_game_data.player[pp_game_data.player[i].target].y + pp_game_data.player[pp_game_data.player[i].target].character.miy)), fsub(pp_game_data.player[i].fx + itofix(pp_game_data.player[i].character.mix), itofix(pp_game_data.player[pp_game_data.player[i].target].x + pp_game_data.player[pp_game_data.player[i].target].character.mix))));
							rect(pp_game_data.player[i].screen.mp, pp_game_data.player[pp_game_data.player[i].target].x + pp_game_data.player[pp_game_data.player[i].target].character.cx_table[pp_game_data.player[pp_game_data.player[i].target].state] - pp_game_data.level->tilemap->x - 5, pp_game_data.player[pp_game_data.player[i].target].y + pp_game_data.player[pp_game_data.player[i].target].character.cy_table[pp_game_data.player[pp_game_data.player[i].target].state] - pp_game_data.level->tilemap->y - 5, pp_game_data.player[pp_game_data.player[i].target].x + pp_game_data.player[pp_game_data.player[i].target].character.cx_table[pp_game_data.player[pp_game_data.player[i].target].state] + pp_game_data.player[pp_game_data.player[i].target].character.cw_table[pp_game_data.player[pp_game_data.player[i].target].state] - 1 - pp_game_data.level->tilemap->x + 5, pp_game_data.player[pp_game_data.player[i].target].y + pp_game_data.player[pp_game_data.player[i].target].character.cy_table[pp_game_data.player[pp_game_data.player[i].target].state] + pp_game_data.player[pp_game_data.player[i].target].character.ch_table[pp_game_data.player[pp_game_data.player[i].target].state] - 1 - pp_game_data.level->tilemap->y + 5, makecol(255, 0, 0));
						}
					}
				}
			}
			if(pp_config[PP_CONFIG_RADAR] && pp_game_data.mode != PP_MODE_SCAVENGER)
			{
				pp_game_update_radar(&pp_game_data.player[i]);
			}

			/* cheap text status bar until I make a real one */
			pp_game_draw_status(i);
		}
		else if(pp_game_data.frame <= 1)
		{
			if(pp_players_in() > 2 || pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_QUARTER && pp_game_data.player[i].screen.bp)
			{
				blit(pp_image[PP_IMAGE_ELIMINATED], pp_game_data.player[i].screen.bp, 0, 0, 0, 0, 320, 240);
			}
		}
	}
}

void pp_game_draw(void)
{
	int i, j;

	acquire_bitmap(screen);
	pp_vsync();
	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].active && pp_game_data.player[i].screen.bp)
		{
			player_draw_screen(screen, &pp_game_data.player[i]);
		}
		else if((pp_game_data.mode != PP_MODE_1P_TEST && pp_game_data.mode != PP_MODE_SCAVENGER && pp_game_data.mode != PP_MODE_TARGET) && (pp_game_data.frame <= 1 || pp_game_data.player[i].just) && pp_game_data.player[i].screen.bp)
		{
			player_draw_screen(screen, &pp_game_data.player[i]);
			pp_game_data.player[i].just = 0;
		}
	}
	release_bitmap(screen);
}

void pp_pause_update(void)
{
	int i;
	
	pp_game_update();
//	aatextprintf_center(pp_game_data.player[pp_network.client[0]->client_num].screen.bp, al_font[1], 160 + rand() % 5 - 2, 104 + rand() % 5 - 2, 236, "Paused");
//	aatextprintf_center(pp_game_data.player[pp_network.client[0]->client_num].screen.bp, al_font[1], 160, 104, 239, "Paused");
}

void pp_pause_draw(void)
{
	pp_game_draw();
}
