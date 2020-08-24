#include "includes.h"
#include "mygui.h"

int step_mode = 0;
int pp_text_x = 0;
int pp_text_y = 0;
int pp_text_color;
char pp_text_buffer[1024] = {0};
int pp_windowed = 0;
int pp_quiet = 0;
int gdriver = GFX_AUTODETECT;

void pp_nop(void)
{
}

void pp_text2screen(char * text)
{
	int i;

	for(i = 0; i < strlen(text); i++)
	{
		if(text[i] == '\n')
		{
			pp_text_x = 0;
			pp_text_y += 8;
		}
		else
		{
			textprintf_ex(screen, font, pp_text_x, pp_text_y, pp_text_color, -1, "%c", text[i]);
			pp_text_x += 8;
		}
	}
}

void pp_load_color_map(COLOR_MAP * mp, char * fn)
{
	int i, j;
	PACKFILE * fp;

	fp = pack_fopen(fn, "r");
	for(i = 0; i < PAL_SIZE; i++)
	{
		for(j = 0; j < PAL_SIZE; j++)
		{
			mp->data[i][j] = pack_getc(fp);
		}
	}
	pack_fclose(fp);
}

void pp_save_color_map(COLOR_MAP * mp, char * fn)
{
	int i, j;
	PACKFILE * fp;

	fp = pack_fopen(fn, "w");
	for(i = 0; i < PAL_SIZE; i++)
	{
		for(j = 0; j < PAL_SIZE; j++)
		{
			pack_putc(mp->data[i][j], fp);
		}
	}
	pack_fclose(fp);
}

void pp_init(void)
{
	int i;

	if(allegro_init() != 0)
	{
		allegro_message("Allegro initialization failed!");
		exit(0);
	}
	mygui_init();
	
	/* set some window parameters */
	set_window_title("Paintball Party");
//	set_window_close_button(0);
	
	/* set the graphics mode */
	if(net_debug)
	{
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
	}
	else
	{
		if(pp_windowed == 1)
		{
			if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) < 0)
			{
				if(set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) < 0)
				{
					allegro_message("Unable to set graphics mode!");
					exit(0);
				}
			}
		}
		else
		{
			if(set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) < 0)
			{
				allegro_message("Unable to set graphics mode!");
				exit(0);
			}
		}
	}
	set_display_switch_mode(SWITCH_BACKGROUND);
	
	if(install_timer() < 0)
	{
		allegro_message("Unable to initialize timer!");
		exit(0);
	}
	if(install_mouse() == -1)
	{
		allegro_message("Unable to initialize mouse!");
		exit(0);
	
	}
	if(install_keyboard() < 0)
	{
		allegro_message("Unable to initialize keyboard!");
		exit(0);
	}
	pp_text_color = makecol(255, 255, 255);
	if(!pp_quiet)
	{
		set_volume_per_voice(0);
		ncds_install();
	}
	pp_text2screen("Paintball Party 1.51 (c) 2005 NewCreature Design\n");
	pp_text2screen("------------------------------------------------\n");
	if(ncds_initialized)
	{
		pp_text2screen("Soundcard found (supports ");
		if(ncds_music_types)
		{
			for(i = 0; i < ncds_music_types; i++)
			{
				pp_text2screen(ncds_music_type[i]);
				pp_text2screen(" ");
			}
		}
		else
		{
			pp_text2screen("no ");
		}
		pp_text2screen("music formats)\n");
		ncds_set_compressed_buffer_size(20000);
	}
	else
	{
//		pp_text2screen(allegro_error);
//		pp_text2screen("\n");
	}
	pp_text2screen("Graphics mode initialized\n");
	pp_text2screen("Creating drawing surfaces... ");
	pp_screen = create_bitmap(640, 480);
	if(!pp_screen)
	{
		allegro_message("Out of memory!");
		exit(0);
	}
	static_screen = create_bitmap(640, 480);
	if(!static_screen)
	{
		allegro_message("Out of memory!");
		exit(0);
	}
	for(i = 0; i < 4; i++)
	{
		pp_game_data.player[i].screen.b[0] = create_bitmap(320, 240);
		if(!pp_game_data.player[i].screen.b[0])
		{
			allegro_message("Out of memory!");
			exit(0);
		}
		clear_bitmap(pp_game_data.player[i].screen.b[0]);
		pp_game_data.player[i].screen.b[1] = create_bitmap(320, 480);
		if(!pp_game_data.player[i].screen.b[1])
		{
			allegro_message("Out of memory!");
			exit(0);
		}
		clear_bitmap(pp_game_data.player[i].screen.b[1]);
		pp_game_data.player[i].screen.b[2] = create_bitmap(640, 240);
		if(!pp_game_data.player[i].screen.b[2])
		{
			allegro_message("Out of memory!");
			exit(0);
		}
		clear_bitmap(pp_game_data.player[i].screen.b[2]);
		pp_game_data.player[i].screen.bp = pp_game_data.player[i].screen.b[PP_SCREEN_MODE_QUARTER];
		pp_game_data.player[i].screen.mp = NULL;
	}
	pp_text2screen("done\n");
	ncd_joy_install();
	if(ncd_joy_enabled)
	{
		pp_text2screen("Joystick found!\n");
	}
	pp_text2screen("Loading game data...\n");
	pp_load_data();
//	pp_text2screen("done\n");

	pp_text2screen("Creating menu system... ");
	pp_title_init();
	pp_text2screen("done\n");
	pp_text2screen("Building credits list... ");
	pp_make_credits();
	pp_text2screen("done\n");
	srand(time(0));
	ncds_set_music_volume(pp_config[PP_CONFIG_MUSIC_VOLUME]);
	ncds_set_sound_volume(pp_config[PP_CONFIG_SOUND_VOLUME]);
	if(!pp_config[PP_CONFIG_JOY])
	{
		ncd_joy_remove();
	}
	pp_text2screen("Initialization completed successfully!\n");
	rest(500);
	srand(time(0));
	_textmode = -1;
	font = al_font[4];
}

void pp_exit(void)
{
	int i, j;

	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].screen.mp)
		{
			destroy_bitmap(pp_game_data.player[i].screen.mp);
		}
		for(j = 0; j < PP_SCREEN_MODES; j++)
		{
			if(pp_game_data.player[i].screen.b[j])
			{
				destroy_bitmap(pp_game_data.player[i].screen.b[j]);
			}
		}
	}
	if(pp_screen)
	{
		destroy_bitmap(pp_screen);
	}
	if(static_screen)
	{
		destroy_bitmap(static_screen);
	}
	pp_free_data();
	ncd_joy_remove();
	ncds_remove();
	pp_write_config("pp.cfg");
	pp_player_data_save("pp.pdt");
}

void pp_global_control(void)
{
	if(key[KEY_F1])
	{
		pp_config[PP_CONFIG_MUSIC_VOLUME] -= 5;
		if(pp_config[PP_CONFIG_MUSIC_VOLUME] < 0)
		{
			pp_config[PP_CONFIG_MUSIC_VOLUME] = 0;
		}
		ncds_set_music_volume(pp_config[PP_CONFIG_MUSIC_VOLUME]);
		key[KEY_F1] = 0;
	}
	if(key[KEY_F2])
	{
		pp_config[PP_CONFIG_MUSIC_VOLUME] += 5;
		if(pp_config[PP_CONFIG_MUSIC_VOLUME] > 100)
		{
			pp_config[PP_CONFIG_MUSIC_VOLUME] = 100;
		}
		ncds_set_music_volume(pp_config[PP_CONFIG_MUSIC_VOLUME]);
		key[KEY_F2] = 0;
	}
	if(key[KEY_F3])
	{
		pp_config[PP_CONFIG_SOUND_VOLUME] -= 5;
		if(pp_config[PP_CONFIG_SOUND_VOLUME] < 0)
		{
			pp_config[PP_CONFIG_SOUND_VOLUME] = 0;
		}
		ncds_set_sound_volume(pp_config[PP_CONFIG_SOUND_VOLUME]);
		key[KEY_F3] = 0;
	}
	if(key[KEY_F4])
	{
		pp_config[PP_CONFIG_SOUND_VOLUME] += 5;
		if(pp_config[PP_CONFIG_SOUND_VOLUME] > 100)
		{
			pp_config[PP_CONFIG_SOUND_VOLUME] = 100;
		}
		ncds_set_sound_volume(pp_config[PP_CONFIG_SOUND_VOLUME]);
		key[KEY_F4] = 0;
	}
	if(key[KEY_F5])
	{
		pp_logic_frames += 5;
		key[KEY_F5] = 0;
	}
	if(key[KEY_F7])
	{
		save_pcx("screen.pcx", screen, pp_palette);
		key[KEY_F7] = 0;
	}
	if(key[KEY_F8])
	{
		if(pp_config[PP_CONFIG_ICONS])
		{
			pp_config[PP_CONFIG_ICONS] = 0;
		}
		else
		{
			pp_config[PP_CONFIG_ICONS] = 1;
		}
		key[KEY_F8] = 0;
	}
	if(key[KEY_F9])
	{
		if(pp_config[PP_CONFIG_RADAR])
		{
			pp_config[PP_CONFIG_RADAR] = 0;
		}
		else
		{
			pp_config[PP_CONFIG_RADAR] = 1;
		}
		key[KEY_F9] = 0;
	}
	if(key[KEY_F10])
	{
		if(pp_config[PP_CONFIG_TRANS])
		{
			pp_config[PP_CONFIG_TRANS] = 0;
			tileset_set_trans(pp_game_data.level->tileset, 0);
		}
		else
		{
			pp_config[PP_CONFIG_TRANS] = 1;
			tileset_set_trans(pp_game_data.level->tileset, 1);
		}
		key[KEY_F10] = 0;
	}
	if(key[KEY_F11])
	{
		if(pp_config[PP_CONFIG_VSYNC])
		{
			pp_config[PP_CONFIG_VSYNC] = 0;
		}
		else
		{
			pp_config[PP_CONFIG_VSYNC] = 1;
		}
		key[KEY_F11] = 0;
	}
	if(key[KEY_F12])
	{
		if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_SMOOTH)
		{
			pp_config[PP_CONFIG_LOGIC] = PP_CONFIG_LOGIC_TIMED;
			gametime_init(60);
			logic_switch = 1;
		}
		else if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_TIMED)
		{
			pp_config[PP_CONFIG_LOGIC] = PP_CONFIG_LOGIC_FAST;
			gametime_init(75);
			logic_switch = 1;
		}
		else
		{
			pp_config[PP_CONFIG_LOGIC] = PP_CONFIG_LOGIC_SMOOTH;
		}
		key[KEY_F12] = 0;
	}
}

void pp_vsync(void)
{
	if(pp_config[PP_CONFIG_VSYNC] && !pp_fading)
	{
		vsync();
	}
	if(step_mode)
	{
		while(!key[KEY_MINUS] && !key[KEY_F8]);
		key[KEY_MINUS] = 0;
	}
}

void pp_start_fade(PALETTE pal1, PALETTE pal2, int dir)
{
	int i;

	if(dir < 0)
	{
		pp_fade_step = 64;
	}
	else
	{
		pp_fade_step = 0;
	}
	pp_fade_dir = dir;
	pp_fading = 1;
	for(i = 0; i < 256; i++)
	{
		pp_fade_start_pal[i].r = pal1[i].r;
		pp_fade_end_pal[i].r = pal2[i].r;
		pp_fade_start_pal[i].g = pal1[i].g;
		pp_fade_end_pal[i].g = pal2[i].g;
		pp_fade_start_pal[i].b = pal1[i].b;
		pp_fade_end_pal[i].b = pal2[i].b;
	}
	pp_fade_proc = NULL;
}

void pp_handle_fade(void)
{
	PALETTE opal;

	if(pp_fading)
	{
		pp_fade_step += pp_fade_dir;
		if(pp_fade_step < 0)
		{
			pp_fading = 0;
			pp_fade_step = 0;
		}
		else if(pp_fade_step > 64)
		{
			pp_fading = 0;
			pp_fade_step = 64;
		}
		fade_interpolate(pp_fade_start_pal, pp_fade_end_pal, opal, pp_fade_step, 0, 255);
		set_palette(opal);
		if(!pp_fading && pp_fade_proc)
		{
			pp_fade_proc();
		}
	}
}

void pp_fade(PALETTE pal1, PALETTE pal2, int dir)
{
	PALETTE opal;
	int step;
	int i;

	if(dir < 0)
	{
		step = 64;
	}
	else
	{
		step = 0;
	}
	for(i = 0; i < 256; i++)
	{
		pp_fade_start_pal[i].r = pal1[i].r;
		pp_fade_end_pal[i].r = pal2[i].r;
		pp_fade_start_pal[i].g = pal1[i].g;
		pp_fade_end_pal[i].g = pal2[i].g;
		pp_fade_start_pal[i].b = pal1[i].b;
		pp_fade_end_pal[i].b = pal2[i].b;
	}
	while(1)
	{
		step += dir;
		if(step < 0)
		{
			step = 0;
		}
		else if(step > 64)
		{
			step = 64;
		}
		fade_interpolate(pp_fade_start_pal, pp_fade_end_pal, opal, step, 0, 255);
		set_palette(opal);
		ncds_update_music();
		if(step <= 0 || step >= 64)
		{
			break;
		}
	}
	gametime_reset();
	logic_switch = 1;
}

void pp_logic(void)
{
	int i, new_step, was;
	int ok;

	if(key[KEY_T] && !pp_typing && pp_state != PP_STATE_NEW_PLAYER)
	{
		pp_typing = 1;
		clear_keybuf();
		strcpy(pp_text, "");
		pp_text_pos = 0;
		key[KEY_T] = 0;
	}

	if(pp_typing)
	{
		text_enter();
	}
	
	pp_global_control();
	switch(pp_state)
	{
		case PP_STATE_LOGO:
		{
			pp_logo_logic();
			break;
		}
		case PP_STATE_INTRO:
		{
			pp_intro_logic();
			break;
		}
		case PP_STATE_CREDITS:
		{
			pp_credits_logic();
			break;
		}
		case PP_STATE_TITLE:
		{
			pp_title_logic();
			break;
		}
		case PP_STATE_GAME:
		{
			pp_game_logic();
			break;
		}
		case PP_STATE_PAUSE:
		{
			break;
		}
		case PP_STATE_STATS:
		{
			pp_stats_logic();
			break;
		}
		case PP_STATE_GAME_IN:
		{
			pp_game_in_logic();
			break;
		}
		case PP_STATE_CHAR:
		{
			pp_char_logic();
			break;
		}
		case PP_STATE_LEVEL:
		{
			pp_level_logic();
			break;
		}
		case PP_STATE_QUIT:
		case PP_STATE_DONE:
		{
			pp_game_quit_logic();
			break;
		}
		case PP_STATE_REPLAY:
		{
			if(pp_replay_file)
			{
				if(key[KEY_RIGHT])
				{
					for(i = 0; i < 4; i++)
					{
						pp_replay_logic();
					}
				}
				else if(key[KEY_LEFT])
				{
					was = replay_pause;
					replay_pause = 0;
					new_step = pp_game_data.frame - 120;
					if(new_step < 0)
					{
						new_step = 0;
					}
					ncds_pause_music();
					if(!pp_replay_start_play(pp_replay_name, 1))
					{
						ncds_pause_music();
						alert(NULL, "Error in replay!", NULL, "OK", NULL, 0, 0);
						ncds_resume_music();
						pp_select_menu(PP_MENU_PLAY);
						logic_switch = 1;
						pp_state = PP_STATE_TITLE;
						return;
					}
					gametime_reset();
					logic_switch = 1;
					ncds_disable_effects();
					while(pp_game_data.frame < new_step)
					{
						pp_replay_logic();
					}
					ncds_enable_effects();
					gametime_reset();
					logic_switch = 1;
					ncds_resume_music();
					replay_pause = was;
				}
				else if(key[KEY_UP])
				{
					ncds_pause_music();
					if(!pp_replay_start_play(pp_replay_name, 1))
					{
						ncds_pause_music();
						alert(NULL, "Error in replay!", NULL, "OK", NULL, 0, 0);
						ncds_resume_music();
						pp_select_menu(PP_MENU_PLAY);
						pp_next_reset = 0;
						pp_state = PP_STATE_TITLE;
						return;
					}
					gametime_reset();
					logic_switch = 1;
					ncds_resume_music();
					key[KEY_UP] = 0;
				}
				else if(key[KEY_S])
				{
					if(pp_global_frame % 3 == 0)
					{
						pp_replay_logic();
					}
				}
				else
				{
					pp_replay_logic();
				}
			}
			break;
		}
		case PP_STATE_DEMO:
		{
			pp_demo_logic();
			break;
		}
		case PP_STATE_MENU_STATS:
		{
			pp_menu_stats_logic();
			break;
		}
		case PP_STATE_NEW_PLAYER:
		{
			pp_new_player_logic();
			break;
		}
	}
	pp_global_frame++;
	ncdmsg_logic(&pp_messages);
	rest(0);
}

void pp_update(void)
{
	int i, j;
	
	switch(pp_state)
	{
		case PP_STATE_LOGO:
		{
			pp_logo_update();
			break;
		}
		case PP_STATE_INTRO:
		{
			pp_intro_update();
			break;
		}
		case PP_STATE_CREDITS:
		{
			pp_credits_update();
			break;
		}
		case PP_STATE_TITLE:
		{
			pp_title_update();
			break;
		}
		case PP_STATE_GAME:
		{
			pp_game_update();
			break;
		}
		case PP_STATE_PAUSE:
		{
			pp_pause_update();
			break;
		}
		case PP_STATE_STATS:
		{
			pp_stats_update();
			break;
		}
		case PP_STATE_GAME_IN:
		{
			pp_game_update();
			break;
		}
		case PP_STATE_CHAR:
		{
			pp_char_update();
			break;
		}
		case PP_STATE_LEVEL:
		{
			pp_level_update();
			break;
		}
		case PP_STATE_QUIT:
		case PP_STATE_DONE:
		{
			pp_game_quit_update();
			break;
		}
		case PP_STATE_REPLAY:
		{
			pp_game_update();
			break;
		}
		case PP_STATE_DEMO:
		{
			pp_game_update();
			break;
		}
		case PP_STATE_MENU_STATS:
		{
			pp_menu_stats_update();
			break;
		}
		case PP_STATE_NEW_PLAYER:
		{
			pp_new_player_update();
			break;
		}
	}
	for(i = 0; i < pp_messages.messages; i++)
	{
		aatextprintf(pp_screen, al_font[2], 0 + 2, (i + 1) * 16 + 2, makecol(0, 0, 0), "%s", pp_messages.message[i].text);
		aatextprintf(pp_screen, al_font[2], 0, (i + 1) * 16, makecol(255, 255, 255), "%s", pp_messages.message[i].text);
	}
}

void pp_draw(void)
{
	int i;
	
	switch(pp_state)
	{
		case PP_STATE_LOGO:
		{
			pp_logo_draw();
			break;
		}
		case PP_STATE_INTRO:
		{
			pp_intro_draw();
			break;
		}
		case PP_STATE_CREDITS:
		{
			pp_credits_draw();
			break;
		}
		case PP_STATE_TITLE:
		{
			pp_title_draw();
			break;
		}
		case PP_STATE_GAME:
		{
			pp_game_draw();
			break;
		}
		case PP_STATE_PAUSE:
		{
			pp_pause_draw();
			break;
		}
		case PP_STATE_STATS:
		{
			pp_stats_draw();
			break;
		}
		case PP_STATE_GAME_IN:
		{
			pp_game_draw();
			break;
		}
		case PP_STATE_CHAR:
		{
			pp_char_draw();
			break;
		}
		case PP_STATE_LEVEL:
		{
			pp_level_draw();
			break;
		}
		case PP_STATE_QUIT:
		case PP_STATE_DONE:
		{
			pp_game_quit_draw();
			break;
		}
		case PP_STATE_REPLAY:
		{
			pp_game_draw();
			break;
		}
		case PP_STATE_DEMO:
		{
			pp_game_draw();
			break;
		}
		case PP_STATE_MENU_STATS:
		{
			pp_menu_stats_draw();
			break;
		}
		case PP_STATE_NEW_PLAYER:
		{
			pp_new_player_draw();
			break;
		}
	}
//	for(i = 0; i < 4; i++)
//	{
//		textprintf(screen, font, 0, i * 8, 31, "%d %d %d", pp_game_data.player[i].entered, pp_game_data.player[i].char_pick, pp_game_data.player[i].active);
//	}
}

void pp_start_song(char * fn, char * dfn)
{
	char * f;
	
	f = ncds_get_music(fn);
	if(f)
	{
		ncds_play_music(f, 1);
		free(f);
	}
	else
	{
		ncds_play_music(dfn, 1);
	}
}

int main(int argc, char * argv[])
{
	int i;
	char server_ip[64] = {0};
	char new_name[128] = {0};
	int updated = 0, processed = 0;

	/* check arguments */
	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if(!stricmp(argv[i], "-w"))
			{
				if(!pp_windowed)
				{
					pp_windowed = 1;
				}
			}
			if(!stricmp(argv[i], "-q"))
			{
				pp_quiet = 1;
			}
		}
	}
	
	/* initialize the game */
//	strcpy(pp_level_name, "level.dat#box");
	pp_init();
	
	/* set up initial logic mode */
	if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_SMOOTH)
	{
	}
	else if(pp_config[PP_CONFIG_LOGIC] == PP_CONFIG_LOGIC_TIMED)
	{
		gametime_init(60);
	}
	else
	{
		gametime_init(75);
	}
	
	/* start at the logo screen */
	pp_start_song("title.xm", PP_MUSIC_FILE_INTRO);
	pp_state = PP_STATE_LOGO;
	set_palette(pp_palette);
	set_mouse_sprite(NULL);
	pp_update();
	set_palette(black_palette);
	pp_draw();
	pp_fade(black_palette, logo_palette, 8);
	
	/* the game loop */
	while(!(pp_quit && !pp_fading))
	{
		int updated = 0;
		
		/* frame skip mode */
		if(pp_config[PP_CONFIG_LOGIC] != PP_CONFIG_LOGIC_SMOOTH)
		{
			while(gametime_get_frames() - gametime_tick > 0)
			{
				pp_logic();
				updated = 0;
				++gametime_tick;
				if(logic_switch)
				{
					logic_switch = 0;
					break;
				}
			}
		}
		/* smooth mode */
		else
		{
			pp_logic();
			updated = 0;
		}
		
		logic_switch = 0;
		
		/* update and draw the screen */
		pp_update();
		pp_draw();
		updated = 1;
		
		/* handle any fading */
		pp_handle_fade();
		
		/* update the music */
		ncds_update_music();
	}
	
	/* stop music */
	ncds_stop_music();
	
	/* shut down program */
	pp_exit();

	/* success */
	return 0;
}
END_OF_MAIN()
