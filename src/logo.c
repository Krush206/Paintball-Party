#include "includes.h"

int logo_step = 0;
int credits_done;
int intro_times = 0;

void pp_create_credit(PP_CREDIT * cp, char * name)
{
	cp->names = 0;
	cp->description = name;
}

void pp_add_credit(char * name)
{
	pp_create_credit(&pp_credit[pp_credits], name);
	pp_credits++;
}

void pp_add_credit_name(char * name)
{
	pp_credit[pp_credits - 1].name[pp_credit[pp_credits - 1].names] = name;
	pp_credit[pp_credits - 1].names++;
}

void pp_make_credits(void)
{
	pp_credits = 0;

	pp_add_credit("Game Design");
	pp_add_credit_name("Todd Cope");
	
	pp_add_credit("Additional Design");
	pp_add_credit_name("Pikawil");
	pp_add_credit_name("Sesignus");
	pp_add_credit_name("RsG87");
	pp_add_credit_name("James Harris");
	pp_add_credit_name("ussbumblebee");
	
	pp_add_credit("Web Design");
	pp_add_credit_name("DNI Web Design");
	
	pp_add_credit("Additional Support");
	pp_add_credit_name("Dawson Irvine");
	
	pp_add_credit("Programming");
	pp_add_credit_name("Todd Cope");

	pp_add_credit("Music");
	pp_add_credit_name("Todd Cope");

	pp_add_credit("Sound Effects");
	pp_add_credit_name("Todd Cope");

	pp_add_credit("Level Design");
	pp_add_credit_name("Todd Cope");
	pp_add_credit_name("Jesse Nelson");
	pp_add_credit_name("Spork");

	pp_add_credit("Character Design");
	pp_add_credit_name("Todd Cope");
	pp_add_credit_name("Dean Mosca");
	pp_add_credit_name("Jesse Nelson");
	pp_add_credit_name("Spork");
	pp_add_credit_name("Alex Cope");

	pp_add_credit("Play Testing");
	pp_add_credit_name("Todd Cope");
	pp_add_credit_name("Alex Cope");
	pp_add_credit_name("Jesse Nelson");
	pp_add_credit_name("Dawson Irvine");
	pp_add_credit_name("Greg Brotzge");
	pp_add_credit_name("Grady O'Connell");
	pp_add_credit_name("mmiinngg");

//	pp_add_credit("Demo Players");
//	pp_add_credit_name("Alex Cope");
//	pp_add_credit_name("Terry Cope");
//	pp_add_credit_name("Todd Cope");
//	pp_add_credit_name("Jesse Nelson");

	pp_add_credit("Base Library");
	pp_add_credit_name("Allegro");

	pp_add_credit("Additional Libraries");
	pp_add_credit_name("AllegTTF");
	pp_add_credit_name("DUMB (dumb.sf.net)");
	pp_add_credit_name("AllegroMP3");
	pp_add_credit_name("AllegroOGG");

	pp_add_credit("Graphics Tools");
	pp_add_credit_name("Paint Shop Pro 7");
	pp_add_credit_name("TTF2PCX");

	pp_add_credit("Audio Tools");
	pp_add_credit_name("MODPlug Tracker");
	pp_add_credit_name("Fasttracker 2");
	pp_add_credit_name("FM Dreams");
	pp_add_credit_name("GoldWave");

	pp_add_credit("Programming Tools");
	pp_add_credit_name("Crimson Editor");
	pp_add_credit_name("DJGPP");
	pp_add_credit_name("MinGW32");

	pp_add_credit("Other Tools");
	pp_add_credit_name("PowerArchiver 2001");
	pp_add_credit_name("Inno Setup");
	pp_add_credit_name("Allegro Grabber");
	pp_add_credit_name("WDOSX");

	pp_add_credit("Thanks");
	pp_add_credit_name("Allegro.cc");
	pp_add_credit_name("Allegro Network");
	pp_add_credit_name("You");
}

void pp_render_credits(void)
{
	int y;
	int i, j;

	y = 480 - pp_game_data.frame;
	for(i = 0; i < pp_credits; i++)
	{
		#ifdef USE_ALLEGTTF
			aatextout_center(pp_screen, al_font[1], pp_credit[i].description, 320, y, 239);
		#else
			textout_centre(pp_screen, al_font[1], pp_credit[i].description, 320, y, 239);
		#endif
		y += 32;
		for(j = 0; j < pp_credit[i].names; j++)
		{
			#ifdef USE_ALLEGTTF
				aatextout_center(pp_screen, al_font[1], pp_credit[i].name[j], 320, y, 31);
			#else
				textout_centre(pp_screen, al_font[1], pp_credit[i].name[j], 320, y, 31);
			#endif
			y += 32;
		}
		y += 96;
	}
	if(y < 32 && i >= pp_credits)
	{
		credits_done = 1;
	}
}

void pp_logo_logic(void)
{
	ncd_joy_poll();
	if(!pp_fading && (cant_wait() || logo_step > 180))
	{
		pp_fade(logo_palette, black_palette, 2);
//		fade_out(2);
		pp_state = PP_STATE_INTRO;
		intro_times++;
		pp_game_data.frame = 0;
		clear_bitmap(screen);
		set_palette(pp_palette);
		pp_update();
		set_palette(black_palette);
		pp_draw();
//		fade_in(pp_palette, 16);
		pp_fade(black_palette, pp_palette, 16);
		ncds_play_sample(pp_sound[PP_SOUND_SPLAT], 128, 1000, -1);
	}
	logo_step++;
}

void pp_logo_update(void)
{
	blit(pp_image[PP_IMAGE_LOGO], pp_screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void pp_logo_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void pp_intro_logic(void)
{
	ncd_joy_poll();
	if(!pp_fading && cant_wait())
	{
		ncds_play_sample(pp_sound[PP_SOUND_MENU_ENTER], 128, 1000, -1);
		pp_fade(pp_palette, black_palette, 2);
		pp_state = PP_STATE_TITLE;
		pp_select_menu(PP_MENU_MAIN);
		clear_bitmap(screen);
		set_palette(pp_palette);
		pp_update();
		set_palette(black_palette);
		pp_draw();
//		fade_in(pp_palette, 2);
//		pp_fade(black_palette, pp_palette, 2);
		pp_start_song("menu.xm", PP_MUSIC_FILE_MENU);
		pp_start_fade(black_palette, pp_palette, 2);
		gametime_reset();
		logic_switch = 1;
		pp_menu_idle_time = 0;
	}
	pp_game_data.frame++;
	if(pp_game_data.frame > 900)
	{
		if(intro_times < 2)
		{
//			fade_from(pp_palette, blue_palette, 2);
			pp_fade(pp_palette, blue_palette, 2);
			pp_state = PP_STATE_CREDITS;
			credits_done = 0;
			pp_game_data.frame = 0;
			gametime_reset();
			logic_switch = 1;
			pp_update();
			pp_draw();
			set_palette(pp_palette);
		}
		else
		{
			if(demo_count > 0)
			{
				pp_fade(pp_palette, black_palette, 2);
				
				/* leave a message here in case the game decides to crash
				   we can tell if it crashed during init */
//				set_palette(pp_palette);
//				textprintf(screen, font, 0, 0, 31, "Starting demo... %d", demo_count);
				if(pp_replay_start_play(demo_list[rand() % demo_count], 0))
				{
					pp_state = PP_STATE_DEMO;
					pp_update();
					pp_draw();
					pp_fade(black_palette, pp_palette, 2);
					gametime_reset();
					logic_switch = 1;
					logo_step = 0;
					demo_playback = 1;
				}
				else
				{
					set_palette(pp_palette);
					ncds_pause_music();
					alert(NULL, "Error in demo!", NULL, "OK", NULL, 0, 0);
					ncds_resume_music();
					gametime_reset();
					logic_switch = 1;
				}
			}
			intro_times = 0;
		}
	}
}

void pp_intro_update(void)
{
	blit(pp_image[PP_IMAGE_TITLE], pp_screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	if((pp_game_data.frame / 15) % 2)
	{
		#ifdef USE_ALLEGTTF
			aatextout_center(pp_screen, al_font[1], "Press Enter", 320, 416, 31);
		#else
			textout_centre(pp_screen, al_font[1], "Press Enter", 320, 416, 31);
		#endif
	}
}

void pp_intro_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void pp_credits_logic(void)
{
	ncd_joy_poll();
	if(!pp_fading && (credits_done || cant_wait()))
	{
//		fade_from(pp_palette, blue_palette, 4);
		pp_fade(pp_palette, blue_palette, 4);
		pp_state = PP_STATE_INTRO;
		intro_times++;
		pp_game_data.frame = 0;
		pp_update();
		pp_draw();
//		fade_from(blue_palette, pp_palette, 16);
		pp_fade(blue_palette, pp_palette, 16);
		ncds_play_sample(pp_sound[PP_SOUND_SPLAT], 128, 1000, -1);
		gametime_reset();
		logic_switch = 1;
	}
	pp_game_data.frame++;
}

void pp_credits_update(void)
{
	clear_to_color(pp_screen, pp_image[PP_IMAGE_TITLE]->line[0][0]);
	pp_render_credits();
}

void pp_credits_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}
