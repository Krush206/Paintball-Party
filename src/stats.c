#include "includes.h"

void pp_menu_stats_logic(void)
{
	ncd_joy_poll();
	read_joy_keys();
	read_controller(&pp_game_data.player[0].controller);
	if(key[KEY_ESC] || pp_game_data.player[0].controller.jump == 1)
	{
		pp_select_menu(PP_MENU_PLAY);
		pp_state = PP_STATE_TITLE;
		pp_next_reset = 0;
		ncds_play_sample(pp_sound[PP_SOUND_MENU_ESC], 128, -1, -1);
		key[KEY_ESC] = 0;
	}
	if(key[KEY_LEFT] || pp_game_data.player[0].controller.left == 1)
	{
		pp_stat--;
		if(pp_stat < -1)
		{
			pp_stat = pp_players - 1;
		}
		ncds_play_sample(pp_sound[PP_SOUND_MENU_LEFT], 128, -1, -1);
		key[KEY_LEFT] = 0;
	}
	if(key[KEY_RIGHT] || pp_game_data.player[0].controller.right == 1)
	{
		pp_stat++;
		if(pp_stat >= pp_players)
		{
			pp_stat = -1;
		}
		ncds_play_sample(pp_sound[PP_SOUND_MENU_RIGHT], 128, -1, -1);
		key[KEY_RIGHT] = 0;
	}
	if((key[KEY_DEL] || pp_game_data.player[0].controller.select == 1) && pp_stat >= 0)
	{
		ncds_pause_music();
		if(alert(NULL, "Clear player data?", NULL, "Yes", "No", 0, 0))
		{
			pp_player_data[pp_stat].played = 0;
			pp_player_data[pp_stat].won = 0;
			pp_player_data[pp_stat].fired = 0;
			pp_player_data[pp_stat].hit = 0;
		}
		ncds_resume_music();
		gametime_reset();
		logic_switch = 1;
		key[KEY_DEL] = 0;
	}
	pp_menu_bg_x--;
	pp_menu_bg_y--;
	pp_game_data.frame++;
}

void pp_menu_stats_update(void)
{
	int played = 0;
	int i;
	int ibest, best;
	int p, w, c;
	int pbest, abest, addict;
	
	switch(pp_stat)
	{
		case -1:
		{
			/* tally total games played by all players */
			played = pp_config[PP_CONFIG_G_PLAYED];
		
			/* find player with best percentage */
			best = 0;
			ibest = -1;
			for(i = 0; i < pp_players; i++)
			{
				if(pp_player_data[i].played > 10)
				{
					w = pp_player_data[i].won * 100;
					p = pp_player_data[i].played;
					c = w / p;
					if(c > best)
					{
						ibest = i;
						best = c;
					}
				}
			}
			pbest = ibest;
		
			/* find player with best accuracy */
			best = 0;
			ibest = -1;
			for(i = 0; i < pp_players; i++)
			{
				if(pp_player_data[i].hit > 0)
				{
					w = pp_player_data[i].hit * 100;
					p = pp_player_data[i].fired;
					c = w / p;
					if(c > best)
					{
						ibest = i;
						best = c;
					}
				}
			}
			abest = ibest;
	
			/* find player with most plays */
			best = 0;
			ibest = -1;
			for(i = 0; i < pp_players; i++)
			{
				p = pp_player_data[i].played;
				if(p > best)
				{
					ibest = i;
					best = p;
				}
			}
			addict = ibest;
	
			pp_menu_bg_draw();
			#ifdef USE_ALLEGTTF
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2 + rand() % 5 - 2, 8 + rand() % 5 - 2, 220, "< All-Time Stats >");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 8, 223, "< All-Time Stats >");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 72, 239, "Games Played");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 104, 231, "%d", played);
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 168, 239, "Best Player");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 200, 231, "%s", pbest >= 0 ? pp_player_data[pbest].name : "N/A");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 264, 239, "Most Accurate");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 296, 231, "%s", abest >= 0 ? pp_player_data[abest].name : "N/A");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 360, 239, "Most Addicted");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 392, 231, "%s", addict >= 0 ? pp_player_data[addict].name : "N/A");
			#else
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2 + rand() % 5 - 2, 8 + rand() % 5 - 2, 220, "< All-Time Stats >");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 8, 223, "< All-Time Stats >");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 72, 239, "Games Played");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 104, 231, "%d", played);
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 168, 239, "Best Player");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 200, 231, "%s", pbest >= 0 ? pp_player_data[pbest].name : "N/A");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 264, 239, "Most Accurate");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 296, 231, "%s", abest >= 0 ? pp_player_data[abest].name : "N/A");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 360, 239, "Most Addicted");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 392, 231, "%s", addict >= 0 ? pp_player_data[addict].name : "N/A");
			#endif
			break;
		}
		default:
		{
			pp_menu_bg_draw();
			#ifdef USE_ALLEGTTF
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2 + rand() % 5 - 2, 8 + rand() % 5 - 2, 220, "< %s's Stats >", pp_player_data[pp_stat].name);
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 8, 223, "< %s's Stats >", pp_player_data[pp_stat].name);
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 72, 239, "Games Played");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 104, 231, "%d", pp_player_data[pp_stat].played);
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 168, 239, "Games Won");
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 200, 231, "%d", pp_player_data[pp_stat].won);
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 264, 239, "Win Percentage");
			#else
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2 + rand() % 5 - 2, 8 + rand() % 5 - 2, 220, "< %s's Stats >", pp_player_data[pp_stat].name);
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 8, 223, "< %s's Stats >", pp_player_data[pp_stat].name);
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 72, 239, "Games Played");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 104, 231, "%d", pp_player_data[pp_stat].played);
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 168, 239, "Games Won");
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 200, 231, "%d", pp_player_data[pp_stat].won);
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 264, 239, "Win Percentage");
			#endif
			if(pp_player_data[pp_stat].played > 0)
			{
				#ifdef USE_ALLEGTTF
					aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 296, 231, "%d%%", (pp_player_data[pp_stat].won * 100) / pp_player_data[pp_stat].played);
				#else
					textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 296, 231, "%d%%", (pp_player_data[pp_stat].won * 100) / pp_player_data[pp_stat].played);
				#endif
			}
			else
			{
				#ifdef USE_ALLEGTTF
					aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 296, 231, "N/A");
				#else
					textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 296, 231, "N/A");
				#endif
			}
			#ifdef USE_ALLEGTTF
				aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 360, 239, "Accuracy");
			#else
				textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 360, 239, "Accuracy");
			#endif
			if(pp_player_data[pp_stat].fired > 0)
			{
				#ifdef USE_ALLEGTTF
					aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 392, 231, "%d%%", (pp_player_data[pp_stat].hit * 100) / pp_player_data[pp_stat].fired);
				#else
					textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 392, 231, "%d%%", (pp_player_data[pp_stat].hit * 100) / pp_player_data[pp_stat].fired);
				#endif
			}
			else
			{
				#ifdef USE_ALLEGTTF
					aatextprintf_center(pp_screen, al_font[1], pp_screen->w / 2, 392, 231, "N/A");
				#else
					textprintf_centre(pp_screen, al_font[1], pp_screen->w / 2, 392, 231, "N/A");
				#endif
			}
			break;
		}
	}
}

void pp_menu_stats_draw(void)
{
	pp_vsync();
	blit(pp_screen, screen, 0, 0, 0, 0, 640, 480);
}

void pp_stats_logic(void)
{
	int i;
	int mx, my;
	
	if(pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_QUARTER || pp_players_in() > 2)
	{
		mx = 1;
		my = 1;
	}
	else if(pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_VERTICAL)
	{
		mx = 1;
		my = 2;
	}
	else if(pp_config[PP_CONFIG_SPLIT_MODE] == PP_SCREEN_MODE_HORIZONTAL)
	{
		mx = 2;
		my = 1;
	}
	if(pp_game_data.player[pp_game_data.winner].active && pp_game_data.player[pp_game_data.winner].screen.bp)
	{
		if(pp_game_data.player[pp_game_data.winner].screen.w < pp_game_data.player[pp_game_data.winner].screen.bp->w * 2)
		{
			pp_game_data.player[pp_game_data.winner].screen.w += 8 * mx;
			if(pp_game_data.player[pp_game_data.winner].screen.w > SCREEN_W)
			{
				pp_game_data.player[pp_game_data.winner].screen.x -= 4 * mx;
			}
		}
		if(pp_game_data.player[pp_game_data.winner].screen.h < pp_game_data.player[pp_game_data.winner].screen.bp->h * 2)
		{
			pp_game_data.player[pp_game_data.winner].screen.h += 6 * my;
			if(pp_game_data.player[pp_game_data.winner].screen.h > SCREEN_H)
			{
				pp_game_data.player[pp_game_data.winner].screen.y -= 3 * my;
			}
		}
		if(pp_game_data.player[pp_game_data.winner].screen.x > 0)
		{
			pp_game_data.player[pp_game_data.winner].screen.x -= 8 * mx;
		}
		if(pp_game_data.player[pp_game_data.winner].screen.y > 0)
		{
			pp_game_data.player[pp_game_data.winner].screen.y -= 6 * my;
		}
	}
	ncd_joy_poll();
	for(i = 0; i < 4; i++)
	{
		read_controller(&pp_game_data.player[i].controller);
		player_control(&pp_game_data.player[i]);
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

   	if(key[KEY_ESC])
	{
		pp_select_menu(PP_MENU_DONE);
		pp_state = PP_STATE_DONE;
		pp_stats_update_bg();
		if(pp_game_data.player[0].active)
		{
			player_draw_screen(screen, &pp_game_data.player[0]);
		}
		if(pp_game_data.player[1].active)
		{
			player_draw_screen(screen, &pp_game_data.player[1]);
		}
		if(pp_game_data.player[2].active)
		{
			player_draw_screen(screen, &pp_game_data.player[2]);
		}
		if(pp_game_data.player[3].active)
		{
			player_draw_screen(screen, &pp_game_data.player[3]);
		}
		blit(screen, static_screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	}
}

void pp_show_stats(BITMAP * bp)
{
	switch(pp_game_data.mode)
	{
		case PP_MODE_SCAVENGER:
		{
//			ncd_printf_center(bp, pp_font[PP_FONT_MENU_ITEM], 104 + 0, "Gems Collected");
			#ifdef USE_ALLEGTTF
				aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 32 + rand() % 5 - 2, 236, "Gems Collected");
				aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2 - 32, 239, "Gems Collected");
			#else
				textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 32 + rand() % 5 - 2, 236, "Gems Collected");
				textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2 - 32, 239, "Gems Collected");
			#endif
			if(pp_game_data.player[0].gems <= 0)
			{
//				ncd_printf_center(bp, pp_font[PP_FONT_MENU_VARIABLE], 104 + 16, "None");
				#ifdef USE_ALLEGTTF
					aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "None");
					aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "None");
				#else
					textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "None");
					textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "None");
				#endif
			}
			else if(pp_game_data.player[0].gems < 5)
			{
//				ncd_printf_center(bp, pp_font[PP_FONT_MENU_VARIABLE], 104 + 16, "%d of 5", pp_game_data.player[0].gems);
				#ifdef USE_ALLEGTTF
					aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "%d of 5", pp_game_data.player[0].gems);
					aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "%d of 5", pp_game_data.player[0].gems);
				#else
					textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "%d of 5", pp_game_data.player[0].gems);
					textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "%d of 5", pp_game_data.player[0].gems);
				#endif
			}
			else
			{
//				ncd_printf_center(bp, pp_font[PP_FONT_MENU_VARIABLE], 104 + 16, "All");
				#ifdef USE_ALLEGTTF
					aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "All");
					aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "All");
				#else
					textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "All");
					textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "All");
				#endif
			}
			break;
		}
		case PP_MODE_TARGET:
		{
//			ncd_printf_center(bp, pp_font[PP_FONT_MENU_ITEM], 80 + 0, "Targets Hit");
			#ifdef USE_ALLEGTTF
				aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 80 + rand() % 5 - 2, 236, "Targets Hit");
				aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2 - 80, 239, "Targets Hit");
			#else
				textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 80 + rand() % 5 - 2, 236, "Targets Hit");
				textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2 - 80, 239, "Targets Hit");
			#endif
			if(pp_game_data.player[0].score <= 0)
			{
//				ncd_printf_center(bp, pp_font[PP_FONT_MENU_VARIABLE], 80 + 16, "None");
				#ifdef USE_ALLEGTTF
					aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 48 + rand() % 5 - 2, 228, "None");
					aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2 - 48, 231, "None");
				#else
					textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 48 + rand() % 5 - 2, 228, "None");
					textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2 - 48, 231, "None");
				#endif
			}
			else
			{
//				ncd_printf_center(bp, pp_font[PP_FONT_MENU_VARIABLE], 80 + 16, "%d", pp_game_data.player[0].score);
				#ifdef USE_ALLEGTTF
					aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 48 + rand() % 5 - 2, 228, "%d", pp_game_data.player[0].score);
					aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2 - 48, 231, "%d", pp_game_data.player[0].score);
				#else
					textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 48 + rand() % 5 - 2, 228, "%d", pp_game_data.player[0].score);
					textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2 - 48, 231, "%d", pp_game_data.player[0].score);
				#endif
			}
//			ncd_printf_center(bp, pp_font[PP_FONT_MENU_ITEM], 80 + 48, "Accuracy");
			#ifdef USE_ALLEGTTF
				aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 236, "Accuracy");
				aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2, 239, "Accuracy");
			#else
				textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 236, "Accuracy");
				textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2, 239, "Accuracy");
			#endif
			if(pp_game_data.player[0].score <= 0)
			{
//				ncd_printf_center(bp, pp_font[PP_FONT_MENU_VARIABLE], 80 + 64, "0%%");
				#ifdef USE_ALLEGTTF
					aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + 32 + rand() % 5 - 2, 228, "0%%");
					aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2 + 32, 231, "0%%");
				#else
					textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + 32 + rand() % 5 - 2, 228, "0%%");
					textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2 + 32, 231, "0%%");
				#endif
			}
			else
			{
//				ncd_printf_center(bp, pp_font[PP_FONT_MENU_VARIABLE], 80 + 64, "%d%%", (pp_game_data.player[0].score * 100) / pp_game_data.player[0].shots);
				#ifdef USE_ALLEGTTF
					aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + 32 + rand() % 5 - 2, 228, "%d%%", (pp_game_data.player[0].score * 100) / pp_game_data.player[0].shots);
					aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2 + 32, 231, "%d%%", (pp_game_data.player[0].score * 100) / pp_game_data.player[0].shots);
				#else
					textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + 32 + rand() % 5 - 2, 228, "%d%%", (pp_game_data.player[0].score * 100) / pp_game_data.player[0].shots);
					textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2 + 32, 231, "%d%%", (pp_game_data.player[0].score * 100) / pp_game_data.player[0].shots);
				#endif
			}
			break;
		}
		case PP_MODE_SPLAT:
		case PP_MODE_HUNTER:
		case PP_MODE_TAG:
		case PP_MODE_FLAG:
		{
//			ncd_printf_center(bp, pp_font[PP_FONT_MENU_ITEM], 104 + 0, "Winner");
			#ifdef USE_ALLEGTTF
				aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 32 + rand() % 5 - 2, 236, "Winner");
				aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2 - 32, 239, "Winner");
				aatextprintf_center(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "%s", pp_player_data[pp_game_data.player[pp_game_data.winner].data_pick].name);
				aatextprintf_center(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "%s", pp_player_data[pp_game_data.player[pp_game_data.winner].data_pick].name);
			#else
				textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 - 32 + rand() % 5 - 2, 236, "Winner");
				textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2 - 32, 239, "Winner");
				textprintf_centre(bp, al_font[1], bp->w / 2 + rand() % 5 - 2, bp->h / 2 + rand() % 5 - 2, 228, "%s", pp_player_data[pp_game_data.player[pp_game_data.winner].data_pick].name);
				textprintf_centre(bp, al_font[1], bp->w / 2, bp->h / 2, 231, "%s", pp_player_data[pp_game_data.player[pp_game_data.winner].data_pick].name);
			#endif
			break;
		}
	}
}

void pp_stats_update_proc(int who)
{
	int j;
	
	/* draw the background layers */
	draw_tilemap_bg(pp_game_data.player[who].screen.mp, pp_game_data.level->tilemap);

	/* draw active objects */
	for(j = 0; j < PP_MAX_OBJECTS; j++)
	{
		if(pp_game_data.object[j].active)
		{
			object_draw(pp_game_data.player[who].screen.mp, &pp_game_data.object[j], -pp_game_data.level->tilemap->x, -pp_game_data.level->tilemap->y);
		}
	}

	/* draw the active paintballs */
	for(j = 0; j < MAX_PAINTBALLS; j++)
	{
		if(pp_game_data.paintball[j].active)
		{
			rotate_sprite(pp_game_data.player[who].screen.mp, get_ani(pp_game_data.paintball[j].ap, pp_game_data.paintball[j].frame), pp_game_data.paintball[j].x - pp_game_data.level->tilemap->x, pp_game_data.paintball[j].y - pp_game_data.level->tilemap->y, pp_game_data.paintball[j].angle);
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
		    	draw_sprite(pp_game_data.player[who].screen.mp, pp_game_data.player[j].zap_image, pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y);
			}

			else if(pp_game_data.player[j].flash_time && pp_game_data.player[j].flash_time % 2 == 0)
			{
				draw_solid_ani(pp_game_data.player[who].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, 31, pp_game_data.player[j].frame);
			}

			/* otherwise draw as normal */
			else
			{
				if(who == j)
				{
					if(!pp_game_data.player[j].cloak_time)
					{
						draw_ani(pp_game_data.player[who].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, pp_game_data.player[j].frame);
					}
					else
					{
						draw_trans_ani(pp_game_data.player[who].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, pp_game_data.player[j].frame);
					}
				}
				else
				{
					if(!pp_game_data.player[j].cloak_time)
					{
						draw_ani(pp_game_data.player[who].screen.mp, pp_game_data.player[j].character.ap[pp_game_data.player[j].state], pp_game_data.player[j].x - pp_game_data.level->tilemap->x, pp_game_data.player[j].y - pp_game_data.level->tilemap->y, pp_game_data.player[j].frame);
					}
				}
			}
		}
	}

	/* draw the active gun flashes */
	for(j = 0; j < 4; j++)
	{
		if(pp_game_data.player[j].gun_flash.active)
		{
			draw_ani(pp_game_data.player[who].screen.mp, pp_game_data.player[j].gun_flash.ap, pp_game_data.player[j].gun_flash.x - pp_game_data.level->tilemap->x, pp_game_data.player[j].gun_flash.y - pp_game_data.level->tilemap->y, 0);
		}
	}

	/* draw the active particles */
	for(j = 0; j < MAX_PARTICLES; j++)
	{
		if(pp_game_data.particle[j].active)
		{
			draw_ani(pp_game_data.player[who].screen.mp, pp_game_data.particle[j].ap, pp_game_data.particle[j].x - pp_game_data.level->tilemap->x - 1, pp_game_data.particle[j].y - pp_game_data.level->tilemap->y - 1, pp_game_data.frame);
		}
	}

	/* draw the foreground layers */
	draw_tilemap_fg(pp_game_data.player[who].screen.mp, pp_game_data.level->tilemap);
	
}

void pp_stats_update_bg(void)
{
	int i, guy = -1;

	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].active && pp_game_data.player[i].screen.mp)
		{
			pp_stats_update_proc(i);
		}
	}
}

void pp_stats_update(void)
{
	int i, j, guy = -1;

	/* render each active player's screen */
	pp_stats_update_bg();
}

void pp_stats_draw(void)
{
	int i;

	acquire_bitmap(screen);
	pp_vsync();
	if(pp_game_data.player[pp_game_data.winner].active && pp_game_data.player[pp_game_data.winner].screen.mp)
	{
		pp_show_stats(pp_game_data.player[pp_game_data.winner].screen.mp);
		player_draw_screen(screen, &pp_game_data.player[pp_game_data.winner]);
	}
	release_bitmap(screen);
}
