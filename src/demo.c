#include "includes.h"

void pp_demo_exit(void)
{
	pp_replay_stop();
	pp_state = PP_STATE_LOGO;
	pp_update();
	pp_draw();
	ncds_play_music(PP_MUSIC_FILE_INTRO, 1);
	pp_start_fade(black_palette, logo_palette, 2);
	pp_logic_frames = 0;
}

void pp_demo_logic(void)
{
	int i;
	unsigned char bit;

	if(!pack_feof(pp_replay_file))
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
	ncd_joy_poll();
    if((cant_wait() || pack_feof(pp_replay_file)) && !pp_next_proc)
	{
		pp_next_proc = pp_demo_exit;
		pp_start_fade(pp_palette, black_palette, 2);
	}
	if(pp_next_proc)
	{
		if(!pp_fading)
		{
			pp_next_proc();
			pp_next_proc = NULL;
		}
	}
}
