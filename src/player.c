#include "includes.h"

int player_can_reach(int p, int x, int y)
{
	int i, max;
	
	if(pp_game_data.player[p].moon_time)
	{
		max = 4;
	}
	else
	{
		max = 3;
	}
	if((pp_game_data.player[p].cmap[pp_game_data.player[p].state].y + pp_game_data.player[p].cmap[pp_game_data.player[p].state].ry + pp_game_data.player[p].cmap[pp_game_data.player[p].state].h) / 16 - y <= max && tilemap_flag_tile(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, x, y))
	{
		return 1;
	}
	return 0;
}

int player_generate_screen(PLAYER * dp, LEVEL * lp)
{
	int mw, mh;
	int cx, cy, cw, ch;
	
	mw = (lp->tilemap->cr - lp->tilemap->cx + 1) * lp->tileset->w;
	mh = (lp->tilemap->cb - lp->tilemap->cy + 1) * lp->tileset->h;
	if(mw < dp->screen.bp->w)
	{
		cw = mw - 2;
		cx = ((dp->screen.bp->w - 2) - mw) / 2;
	}
	else
	{
		cw = dp->screen.bp->w - 2;
		cx = 1;
	}
	if(mh < dp->screen.bp->h)
	{
		ch = mh - 2;
		cy = ((dp->screen.bp->h - 2) - mh) / 2;
	}
	else
	{
		ch = dp->screen.bp->h - 2;
		cy = 1;
	}
	if(dp->screen.mp)
	{
		destroy_bitmap(dp->screen.mp);
		dp->screen.mp = NULL;
	}
	dp->screen.mp = create_sub_bitmap(dp->screen.bp, cx, cy, cw, ch);
	if(!dp->screen.mp)
	{
		return 0;
	}
	clear_bitmap(dp->screen.bp);
	return 1;
}

void player_message_generate(PLAYER_MESSAGE * mp, char * text, int ox, int oy, int vx, int vy, int life)
{
//	mp-
//	mp->text = text;
	mp->ox = ox;
	mp->oy = oy;
	mp->vx = vx;
	mp->vy = vy;
	mp->life = life;
	mp->active = 1;
}

void player_message_logic(PLAYER_MESSAGE * mp)
{
	if(mp->active)
	{
		mp->ox += mp->vx;
		mp->oy += mp->vy;
		mp->life--;
		if(mp->life <= 0)
		{
			mp->active = 0;
		}
	}
}

void player_message_draw(BITMAP * bp, PLAYER_MESSAGE * mp, int x, int y)
{
	if(mp->active)
	{
		ncd_printf_center_pivot(bp, pp_font[PP_FONT_TINY], x + mp->ox - pp_game_data.level->tilemap->x, y + mp->oy - pp_game_data.level->tilemap->y, "%s", mp->buffer);
	}
}

int load_char_fp(PLAYER * dp, PACKFILE * fp)
{
	int i;
	int temp;

	temp = pack_igetl(fp);
	if(temp)
	{
		dp->character.pap = load_ani_fp(fp, NULL);
		if(!dp->character.pap)
		{
			destroy_char(dp);
			return 0;
		}
	}
	else
	{
		dp->character.pap = NULL;
		destroy_char(dp);
		return 0;
	}
	temp = pack_igetl(fp);
	if(temp)
	{
		dp->character.ppap = load_ani_fp(fp, NULL);
		if(!dp->character.ppap)
		{
			destroy_char(dp);
			return 0;
		}
	}
	else
	{
		dp->character.ppap = NULL;
		destroy_char(dp);
		return 0;
	}
	for(i = 0; i < GUN_FLASH_TYPES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->character.gap[i] = load_ani_fp(fp, NULL);
			if(!dp->character.gap[i])
			{
				destroy_char(dp);
				return 0;
			}
		}
		else
		{
			dp->character.gap[i] = NULL;
			destroy_char(dp);
			return 0;
		}
	}
	for(i = 0; i < PLAYER_STATES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->character.ap[i] = load_ani_fp(fp, NULL);
			if(!dp->character.ap[i])
			{
				destroy_char(dp);
				return 0;
			}
		}
		else
		{
			dp->character.ap[i] = NULL;
			destroy_char(dp);
			return 0;
		}
		dp->character.cx_table[i] = pack_igetl(fp);
		dp->character.cy_table[i] = pack_igetl(fp);
		dp->character.cw_table[i] = pack_igetl(fp);
		dp->character.ch_table[i] = pack_igetl(fp);
		dp->character.px_table[i] = pack_igetl(fp);
		dp->character.py_table[i] = pack_igetl(fp);
		dp->character.gx_table[i] = pack_igetl(fp);
		dp->character.gy_table[i] = pack_igetl(fp);
		if(dp->character.ap[i])
		{
			generate_collision_map(&dp->cmap[i], dp->character.cx_table[i], dp->character.cy_table[i], dp->character.cw_table[i], dp->character.ch_table[i], 16, 16);
			cmap_fillet_top(&dp->cmap[i], 6);
			dp->cmap[i].lnudge = 1;
			dp->cmap[i].rnudge = 1;
		}
	}
	dp->character.mix = pack_igetl(fp);
	dp->character.miy = pack_igetl(fp);
	dp->character.mir = pack_igetl(fp);
	dp->character.ix = pack_igetl(fp);
	dp->character.iy = pack_igetl(fp);
	dp->character.pf = pack_igetl(fp);
	dp->radar_image = create_bitmap(48, 48);
	if(!dp->radar_image)
	{
		destroy_char(dp);
		return 0;
	}
	dp->zap_image = create_bitmap(dp->character.ap[0]->w, dp->character.ap[0]->h);
	if(!dp->zap_image)
	{
		destroy_char(dp);
		return 0;
	}
	for(i = 0; i < PP_MAX_SOUNDS; i++)
	{
		dp->sound[i] = pp_sound[i];
	}
	return 1;
}

int load_char(PLAYER * dp, const char * fn)
{
	PACKFILE * fp;
	char * nfn;
	char ffn[256];
	int load_ok;

	if(!exists(fn))
	{
		nfn = get_filename(fn);
	}
	else
	{
		nfn = (char *)fn;
	}
	fp = pack_fopen(fn, "r");
	if(!fp)
	{
		return 0;
	}
	destroy_char(dp);
	pack_igetl(fp);
	load_ok = load_char_fp(dp, fp);
	pack_fclose(fp);
	if(!load_ok)
	{
		return 0;
	}
	replace_extension(ffn, fn, "pps", 255);
	if(exists(ffn))
	{
		if(!load_sounds(dp, ffn))
		{
			destroy_char(dp);
			return 0;
		}
	}
	return 1;
}

int load_char_ns(PLAYER * dp, const char * fn)
{
	PACKFILE * fp;
	char * nfn;
	char ffn[256];
	int load_ok;

	if(!exists(fn))
	{
		nfn = get_filename(fn);
	}
	else
	{
		nfn = (char *)fn;
	}
	fp = pack_fopen(fn, "r");
	if(!fp)
	{
		return 0;
	}
	pack_igetl(fp);
	load_ok = load_char_fp(dp, fp);
	pack_fclose(fp);
	if(!load_ok)
	{
		return 0;
	}
	return 1;
}

void destroy_char(PLAYER * pp)
{
	int i;

	if(pp->character.pap)
	{
		destroy_ani(pp->character.pap);
		pp->character.pap = NULL;
	}
	if(pp->character.ppap)
	{
		destroy_ani(pp->character.ppap);
		pp->character.ppap = NULL;
	}
	for(i = 0; i < 4; i++)
	{
		if(pp->character.gap[i])
		{
			destroy_ani(pp->character.gap[i]);
			pp->character.gap[i] = NULL;
		}
	}
	for(i = 0; i < PLAYER_STATES; i++)
	{
		if(pp->character.ap[i])
		{
			destroy_ani(pp->character.ap[i]);
			pp->character.ap[i] = NULL;
		}
	}
	if(pp->radar_image)
	{
		destroy_bitmap(pp->radar_image);
		pp->radar_image = NULL;
	}
	if(pp->zap_image)
	{
		destroy_bitmap(pp->zap_image);
		pp->zap_image = NULL;
	}
	for(i = 0; i < PP_MAX_SOUNDS; i++)
	{
		if(pp->sound[i] && pp->sound[i] != pp_sound[i] && pp->sound[i] != ncds_internal_sample)
		{
			destroy_sample(pp->sound[i]);
		}
		pp->sound[i] = NULL;
	}
}

void erase_char(PLAYER * pp)
{
	int i;

	pp->character.pap = NULL;
	pp->character.ppap = NULL;
	for(i = 0; i < 4; i++)
	{
		pp->character.gap[i] = NULL;
	}
	for(i = 0; i < PLAYER_STATES; i++)
	{
		pp->character.ap[i] = NULL;
	}
	pp->radar_image = NULL;
	pp->zap_image = NULL;
	pp->screen.mp = NULL;
}

int load_sounds(PLAYER * dp, const char * fn)
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
			if(dp->sound[i])
			{
				if(dp->sound[i]->len <= 1)
				{
					destroy_sample(dp->sound[i]);
					dp->sound[i] = ncds_internal_sample;
				}
			}
			else
			{
				return 0;
			}
		}
	}
	pack_fclose(fp);
	return 1;
}

int save_sounds(PLAYER * dp, const char * fn)
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
		if(dp->sound[i] && dp->sound[i] != pp_sound[i])
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

int player_init(PLAYER * pp, int reload)
{
	if(reload)
	{
		if(!load_char(pp, char_entry[pp->char_pick].file))
		{
			return 0;
		}
	}
	pp->state = 0;
	pp->frame = 0;
	pp->ground = 1;
	pp->active = 1;
	pp->tele_in = 1;
	pp->tele_out = 0;
	pp->tele_state = 0;
	pp->tele_frame = 0;
	if(!pp_game_drop_player(pp->color))
	{
		return 0;
	}
	pp->target = -1;
	pp->gy = ftofix(0.5);
	pp->mx = itofix(2);
	pp->vx = itofix(0);
	pp->vy = itofix(0);
	pp->ammo[PP_AMMO_TYPE_NORMAL] = pp_option[PP_OPTION_AMMO_NORMAL];
	pp->ammo[PP_AMMO_TYPE_X] = pp_option[PP_OPTION_AMMO_X];
	pp->ammo[PP_AMMO_TYPE_MINE] = pp_option[PP_OPTION_AMMO_MINE];
	pp->ammo[PP_AMMO_TYPE_BOUNCE] = pp_option[PP_OPTION_AMMO_BOUNCE];
	pp->ammo[PP_AMMO_TYPE_SEEK] = pp_option[PP_OPTION_AMMO_SEEK];
	pp->score = 0;
	pp->shots = 0;
	pp->cloak_time = 0;
	pp->moon_time = 0;
	pp->turbo_time = 0;
	pp->deflect_time = 0;
	pp->run_time = 0;
	pp->fly_time = 0;
	pp->flash_time = 0;
	pp->gun_flash.active = 0;
	pp->reload_time = 0;
	pp->reload_fast = 0;
	pp->weapon = PP_AMMO_TYPE_NORMAL;
	if(pp->ammo[pp->weapon] <= 0)
	{
	    pp_cycle_ammo(pp);
    }
	pp->target = -1;
	if(pp_game_data.mode == PP_MODE_TAG)
	{
		pp->it = 1;
		pp->time = 0;
	}
	else
	{
		pp->it = 0;
	}
	pp->flag = 0;
	pp->hits = 0;
	pp->shock_time = 0;
	if(pp_game_data.mode == PP_MODE_TARGET)
	{
		pp->time = 3599;
	}
	if(pp_game_data.mode == PP_MODE_SCAVENGER)
	{
		pp->item = 0;
		pp->time = 3599;
		pp->gem[0] = 0;
		pp->gem[1] = 0;
		pp->gem[2] = 0;
		pp->gem[3] = 0;
		pp->gem[4] = 0;
		pp->gems = 0;
	}
//	if(pp_game_data.mode == PP_MODE_1P_TEST && pp->color != 0)
    clear_bitmap(pp->zap_image);
    return 1;
}

void player_finish(PLAYER * pp)
{
}

void player_update_collision_map(PLAYER * pp)
{
	int i;

	for(i = 0; i < PLAYER_STATES; i++)
	{
		fupdate_collision_map(&pp->cmap[i], pp->fx, pp->fy);
	}
}

void player_move_collision_map(PLAYER * pp)
{
	int i;

	for(i = 0; i < PLAYER_STATES; i++)
	{
		fmove_collision_map(&pp->cmap[i], pp->fx, pp->fy);
	}
}

int player_grounded(PLAYER * pp, TILEMAP * mp)
{
	if(pp->vy >= itofix(0))
	{
		return tilemap_rubb_check(mp, mp->il, TILEMAP_FLAG_SOLIDT, &pp->cmap[pp->state]);
	}
	return 0;
}

/* find the next weapon with ammo */
void pp_cycle_ammo(PLAYER * pp)
{
	int start = pp->weapon;
	int i, closest = 50000, closesti = -1;

	pp->weapon++;
	if(pp->weapon >= 5)
	{
		pp->weapon = 0;
	}
	while(pp->ammo[pp->weapon] <= 0)
	{
		pp->weapon++;
		if(pp->weapon >= 5)
		{
			pp->weapon = 0;
		}
		if(pp->weapon == start)
		{
			break;
		}
	}
	if(pp->ammo[pp->weapon])
	{
		player_message_generate(&pp->message, weapon_info[pp->weapon], 0, 0, 0, -1, 30);
		sprintf(pp->message.buffer, "%s (%i)", weapon_info[pp->weapon], pp->ammo[pp->weapon]);
	}
	if(pp->weapon == PP_AMMO_TYPE_SEEK)
	{
		if(pp_game_data.mode == PP_MODE_TARGET)
		{
			if(pp_game_data.winner < 0 && pp->ammo[pp->weapon] > 0)
			{
				pp->target = pp_game_data.target_sprite;
				if(pp->target >= 0)
				{
					if(pp_game_data.object[pp->target].type != PP_OBJECT_TARGET || !pp_game_data.object[pp->target].active)
					{
						pp->target = -1;
						pp_game_data.target_sprite = -1;
					}
				}
				else
				{
					pp->target = -1;
					pp_game_data.target_sprite = -1;
				}
				ncds_play_sample(pp->sound[PP_SOUND_TARGET], 128, -1, -1);
			}
			else
			{
				pp->target = -1;
			}
		}
		else
		{
			if(pp->ammo[pp->weapon] > 0)
			{
				if(!pp_option[PP_OPTION_AUTO_TARGET])
				{
					if(pp->target >= 0)
					{
						pp_select_target(pp);
					}
					else
					{
						pp_cycle_target(pp);
					
					}
				}
				else
				{
					for(i = 0; i < 4; i++)
					{
						if(i != pp->color)
						{
							if(fixtoi(fixhypot(pp->fx - pp_game_data.player[i].fx, pp->fy - pp_game_data.player[i].fy)) < closest)
							{
								closest = fixhypot(pp->fx - pp_game_data.player[i].fx, pp->fy - pp_game_data.player[i].fy);
								closesti = i;
							}
						}
					}
					if(closesti >= 0)
					{
						pp->target = closesti;
					}
				}
			}
			else
			{
				pp->target = -1;
			}
		}
	}
}

void pp_find_target(PLAYER * pp)
{
	int i, closest = 50000, closesti = -1;

	if(pp_game_data.mode == PP_MODE_FLAG)
	{
		for(i = 0; i < 4; i++)
		{
			if(pp_game_data.player[i].flag && i != pp->color && pp_game_data.player[i].active && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out && !pp_game_data.player[i].cloak_time)
			{
				closesti = i;
			}
		}
		if(closesti >= 0)
		{
			if(pp->target != closesti)
			{
				ncds_play_sample(pp->sound[PP_SOUND_TARGET], 128, -1, -1);
				player_message_generate(&pp->message, NULL, 0, 0, 0, -1, 30);
				sprintf(pp->message.buffer, "Seek %s (%d)", pp_game_data.player[pp->target].name, pp->ammo[pp->weapon]);
			}
		}
		pp->target = closesti;
	}
	else
	{
		for(i = 0; i < 4; i++)
		{
			if(i != pp->color && pp_game_data.player[i].active && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out && !pp_game_data.player[i].cloak_time)
			{
				if(fixtoi(fixhypot(pp->fx - pp_game_data.player[i].fx, pp->fy - pp_game_data.player[i].fy)) < closest)
				{
					closest = fixtoi(fixhypot(pp->fx - pp_game_data.player[i].fx, pp->fy - pp_game_data.player[i].fy));
					closesti = i;
				}
			}
		}
		if(closesti >= 0)
		{
			if(pp->target != closesti)
			{
				ncds_play_sample(pp->sound[PP_SOUND_TARGET], 128, -1, -1);
				player_message_generate(&pp->message, NULL, 0, 0, 0, -1, 30);
				sprintf(pp->message.buffer, "Seek %s (%d)", pp_game_data.player[pp->target].name, pp->ammo[pp->weapon]);
			}
			pp->target = closesti;
		}
	}
}

/* find the next target */
void pp_cycle_target(PLAYER * pp)
{
	int start = pp->target;

	pp->target++;
	if(pp->target >= 4)
	{
		pp->target = 0;
	}
	while(!pp_game_data.player[pp->target].active || pp_game_data.player[pp->target].tele_in || pp_game_data.player[pp->target].tele_out || pp->target == pp->color || pp_game_data.player[pp->target].cloak_time)
	{
		pp->target++;
		if(pp->target >= 4)
		{
			if(start < 0)
			{
				pp->target = start;
			}
			else
			{
				pp->target = 0;
			}
		}
		if(pp->target == start)
		{
			break;
		}
	}
	if(pp->target >= 0)
	{
		if(!pp_game_data.player[pp->target].active || pp_game_data.player[pp->target].tele_in || pp_game_data.player[pp->target].tele_out || pp->target == pp->color || pp_game_data.player[pp->target].cloak_time)
		{
			pp->target = -1;
		}
		else if(pp->target != start)
		{
			ncds_play_sample(pp->sound[PP_SOUND_TARGET], 128, -1, -1);
			player_message_generate(&pp->message, NULL, 0, 0, 0, -1, 30);
			sprintf(pp->message.buffer, "Seek %s (%d)", pp_game_data.player[pp->target].name, pp->ammo[pp->weapon]);
		}
	}
}

void pp_select_target(PLAYER * pp)
{
	int start = pp->target;

	if(pp->target >= 0)
	{
		while(!pp_game_data.player[pp->target].active || pp_game_data.player[pp->target].tele_in || pp_game_data.player[pp->target].tele_out || pp->target == pp->color || pp_game_data.player[pp->target].cloak_time)
		{
			pp->target++;
			if(pp->target >= 4)
			{
				pp->target = 0;
			}
			if(pp->target == start)
			{
				break;
			}
		}
		if(pp->target >= 0)
		{
			if(!pp_game_data.player[pp->target].active || pp_game_data.player[pp->target].tele_in || pp_game_data.player[pp->target].tele_out || pp->target == pp->color || pp_game_data.player[pp->target].cloak_time)
			{
				pp->target = -1;
			}
			else
			{
				ncds_play_sample(pp->sound[PP_SOUND_TARGET], 128, -1, -1);
				player_message_generate(&pp->message, NULL, 0, 0, 0, -1, 30);
				sprintf(pp->message.buffer, "Seek %s (%d)", pp_game_data.player[pp->target].name, pp->ammo[pp->weapon]);
			}
		}
	}
}

/* determine if player is allowed to fire */
int player_can_fire(PLAYER * pp)
{
	switch(pp_game_data.mode)
	{
		case PP_MODE_TARGET:
		case PP_MODE_1P_TEST:
//		case PP_MODE_1P_SCAVENGER:
		case PP_MODE_SPLAT:
		case PP_MODE_HUNTER:
		case PP_MODE_FLAG:
		{
			if(pp->ammo[pp->weapon] > 0 && pp->reload_time <= 0)
			{
				if(pp->vx > itofix(-4) && pp->vx < itofix(4))
				{
					return 1;
				}
				else if(pp->state == PS_STAND_UP_RIGHT || pp->state == PS_STAND_UP_LEFT)
				{
					return 1;
				}
				return 0;
			}
		}
		case PP_MODE_TAG:
		{
			if(pp->ammo[pp->weapon] > 0 && pp->reload_time <= 0 && pp->it)
			{
				if(pp->vx > itofix(-6) && pp->vx < itofix(6))
				{
					return 1;
				}
				else if(pp->state == PS_STAND_UP_RIGHT || pp->state == PS_STAND_UP_LEFT)
				{
					return 1;
				}
				return 0;
			}
		}
		default:
		{
			return 0;
		}
	}
}

void player_update_state(PLAYER * pp)
{
	if(pp_game_data.winner >= 0)
	{
		pp->target = -1;
	}
	if(pp->state >= OLD_PLAYER_STATES)
	{
		pp->state -= OLD_PLAYER_STATES;
	}
	switch(pp->state)
	{
		case PS_STAND_RIGHT:
		case PS_FSTAND_RIGHT:
		{
	    	if(pp->controller.left && !pp->controller.right)
    		{
                pp->state = PS_WALK_LEFT;
               	pp->frame = 0;
    		}
	    	else if(pp->controller.right && !pp->controller.left)
    		{
                pp->state = PS_WALK_RIGHT;
               	pp->frame = 0;
    		}
    		else if(pp->controller.down)
    		{
	    		pp->state = PS_DUCK_RIGHT;
	    		pp->frame = 0;
    		}
    		else if(pp->controller.up)
    		{
	    		pp->state = PS_STAND_UP_RIGHT;
	    		pp->frame = 0;
    		}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_RIGHT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_RIGHT;
    		}
	    	break;
		}
		case PS_STAND_LEFT:
		case PS_FSTAND_LEFT:
		{
	    	if(pp->controller.left && !pp->controller.right)
    		{
                pp->state = PS_WALK_LEFT;
               	pp->frame = 0;
    		}
	    	else if(pp->controller.right && !pp->controller.left)
    		{
                pp->state = PS_WALK_RIGHT;
               	pp->frame = 0;
    		}
    		else if(pp->controller.down)
    		{
	    		pp->state = PS_DUCK_LEFT;
	    		pp->frame = 0;
    		}
    		else if(pp->controller.up)
    		{
	    		pp->state = PS_STAND_UP_LEFT;
	    		pp->frame = 0;
    		}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_LEFT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_LEFT;
    		}
	    	break;
		}
		case PS_STAND_UP_RIGHT:
		case PS_FSTAND_UP_RIGHT:
		{
			if(!pp->controller.up)
			{
				pp->state = PS_STAND_RIGHT;
				pp->frame = 0;
			}
			else if(pp->controller.left && !pp->controller.right && pp->vx < itofix(4))
			{
				pp->state = PS_WALK_LEFT;
				pp->frame = 0;
			}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_RIGHT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_RIGHT;
    		}
			break;
		}
		case PS_STAND_UP_LEFT:
		case PS_FSTAND_UP_LEFT:
		{
			if(!pp->controller.up)
			{
				pp->state = PS_STAND_LEFT;
				pp->frame = 0;
			}
			else if(pp->controller.right && !pp->controller.left && pp->vx > itofix(-4))
			{
				pp->state = PS_WALK_RIGHT;
				pp->frame = 0;
			}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_LEFT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_LEFT;
    		}
			break;
		}
		case PS_WALK_RIGHT:
		case PS_FWALK_RIGHT:
		{
			if(pp->controller.up)
			{
				pp->state = PS_STAND_UP_RIGHT;
				pp->frame = 0;
			}
    		else if(pp->controller.down)
    		{
	    		pp->state = PS_DUCK_RIGHT;
	    		pp->frame = 0;
    		}
    		else if(pp->controller.left && !pp->controller.right && pp->vx < itofix(4))
    		{
	    		pp->state = PS_WALK_LEFT;
	    		pp->frame = 0;
    		}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_RIGHT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_RIGHT;
    		}
	    	break;
		}
		case PS_WALK_LEFT:
		case PS_FWALK_LEFT:
		{
			if(pp->controller.up)
			{
				pp->state = PS_STAND_UP_LEFT;
				pp->frame = 0;
			}
    		else if(pp->controller.down)
    		{
	    		pp->state = PS_DUCK_LEFT;
	    		pp->frame = 0;
    		}
    		else if(pp->controller.right && !pp->controller.left && pp->vx > itofix(-4))
    		{
	    		pp->state = PS_WALK_RIGHT;
	    		pp->frame = 0;
    		}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_LEFT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_LEFT;
    		}
	    	break;
		}
		case PS_DUCK_RIGHT:
		case PS_FDUCK_RIGHT:
		{
			if(!pp->controller.down)
			{
				pp->state = PS_WALK_RIGHT;
				pp->frame = 0;
			}
			else if(pp->controller.left && !pp->controller.right && pp->vx < itofix(4))
			{
				pp->state = PS_DUCK_LEFT;
				pp->frame = 0;
			}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_RIGHT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_RIGHT;
    		}
			break;
		}
		case PS_DUCK_LEFT:
		case PS_FDUCK_LEFT:
		{
			if(!pp->controller.down)
			{
				pp->state = PS_WALK_LEFT;
				pp->frame = 0;
			}
			else if(pp->controller.right && !pp->controller.left && pp->vx > itofix(-4))
			{
				pp->state = PS_DUCK_RIGHT;
				pp->frame = 0;
			}
    		if(pp->controller.jump)
    		{
		    	ncds_play_sample(pp->sound[PP_SOUND_JUMP], 128, -1, -1);
	        	pp->vy = itofix(-7);
	            pp->state = PS_JUMP_LEFT;
    		}
    		else if(pp->fly_time && pp->controller.wjump)
    		{
	        	pp->vy = itofix(-1);
	    		pp->state = PS_JUMP_LEFT;
    		}
			break;
		}
		case PS_JUMP_RIGHT:
		case PS_FJUMP_RIGHT:
		{
			if(pp->controller.left && !pp->controller.right && pp->vx < itofix(4))
			{
				pp->state = PS_JUMP_LEFT;
				pp->frame = 0;
			}
			break;
		}
		case PS_JUMP_LEFT:
		case PS_FJUMP_LEFT:
		{
			if(pp->controller.right && !pp->controller.left && pp->vx > itofix(-4))
			{
				pp->state = PS_JUMP_RIGHT;
				pp->frame = 0;
			}
			break;
		}
		case PS_FALL_RIGHT:
		case PS_FFALL_RIGHT:
		{
			if(pp->controller.left && !pp->controller.right && pp->vx < itofix(4))
			{
				pp->state = PS_FALL_LEFT;
				pp->frame = 0;
			}
			break;
		}
		case PS_FALL_LEFT:
		case PS_FFALL_LEFT:
		{
			if(pp->controller.right && !pp->controller.left && pp->vx > itofix(-4))
			{
				pp->state = PS_FALL_RIGHT;
				pp->frame = 0;
			}
			break;
		}
	}

	if(pp->controller.fire && player_can_fire(pp) && pp->state < OLD_PLAYER_STATES)
	{
		pp->state += OLD_PLAYER_STATES;
       	generate_paintball(pp);
        pp->ammo[pp->weapon]--;
        if(pp_game_data.winner < 0)
        {
       		pp->shots++;
   		}
       	ncds_play_sample(pp->sound[PP_SOUND_FIRE], 128, -1, -1);
       	if(pp->turbo_time)
       	{
       		pp->reload_time = 15;
       		pp->reload_fast = 1;
       	}
       	else
       	{
       		pp->reload_time = 35;
       		pp->reload_fast = 0;
       	}
       	pp_player_data[pp->data_pick].fired++;
   	}
   	if(pp->controller.select)
   	{
	    pp_cycle_ammo(pp);
    	if(pp->ammo[pp->weapon] > 0)
    	{
	    	if(pp->turbo_time)
	    	{
	    		pp->reload_time = 15;
	       		pp->reload_fast = 1;
	    	}
	    	else
	    	{
	    		pp->reload_time = 35;
	       		pp->reload_fast = 0;
    		}
   		}
   	}
   	if(pp->controller.option && pp->weapon == PP_AMMO_TYPE_SEEK && pp->ammo[pp->weapon] > 0 && !pp_option[PP_OPTION_AUTO_TARGET])
   	{
	    pp_cycle_target(pp);
   	}
}

void player_add_friction(PLAYER * pp)
{
	if(pp->vx < itofix(0))
	{
		pp->vx = fixadd(pp->vx, ftofix(0.5));
		if(pp->vx >= itofix(0))
		{
			pp->vx = itofix(0);
		}
	}
	else if(pp->vx > itofix(0))
	{
		pp->vx = fixsub(pp->vx, ftofix(0.5));
		if(pp->vx <= itofix(0))
		{
			pp->vx = itofix(0);
		}
	}
}

/* handle friction */
int player_friction(PLAYER * pp)
{
	int r = 0;
	
	if(!tilemap_scrapeb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap[pp->state]))
	{
		if(!tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &pp->cmap[pp->state]))
		{
			player_add_friction(pp);
			r = 1;
		}
		else if(!tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &pp->cmap[pp->state]) && tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap[pp->state]))
		{
			player_add_friction(pp);
			r = 1;
		}
	}
	else
	{
		if(!tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &pp->cmap[pp->state]))
		{
			player_add_friction(pp);
			r = 1;
		}
	}
	pp->slip = !r;
	return r;
}

void player_adjust(PLAYER * pp)
{
	switch(pp->state)
	{
		case PS_STAND_RIGHT:
		{
			if(pp->vx != itofix(0) && !pp->slip)
			{
				pp->state = PS_WALK_RIGHT;
				pp->frame = 0;
			}
			player_friction(pp);
	    	break;
		}
		case PS_STAND_LEFT:
		{
			if(pp->vx != itofix(0) && !pp->slip)
			{
				pp->state = PS_WALK_LEFT;
				pp->frame = 0;
			}
			player_friction(pp);
	    	break;
		}
		case PS_STAND_UP_RIGHT:
		{
			player_friction(pp);
			break;
		}
		case PS_STAND_UP_LEFT:
		{
			player_friction(pp);
			break;
		}
		case PS_WALK_RIGHT:
		{
			if(pp->controller.right)
			{
				if(pp->vx < pp->mx)
				{
	            	pp->vx = fixadd(pp->vx, ftofix(0.5));
            		if(pp->vx > pp->mx)
            		{
		                pp->vx = pp->mx;
            		}
            	}
            	else
            	{
					player_add_friction(pp);
            	}
			}
			else
			{
				if(player_friction(pp))
				{
					if(pp->vx == itofix(0))
					{
						pp->state = PS_STAND_RIGHT;
						pp->frame = 0;
					}
				}
				else
				{
					pp->state = PS_STAND_RIGHT;
					pp->frame = 0;
				}
			}
	    	break;
		}
		case PS_WALK_LEFT:
		{
			if(pp->controller.left)
			{
				if(pp->vx > -pp->mx)
				{
	            	pp->vx = fixsub(pp->vx, ftofix(0.5));
            		if(pp->vx < -pp->mx)
            		{
		                pp->vx = -pp->mx;
            		}
            	}
            	else
            	{
					player_add_friction(pp);
            	}
			}
			else
			{
				if(player_friction(pp))
				{
					if(pp->vx == itofix(0))
					{
						pp->state = PS_STAND_LEFT;
						pp->frame = 0;
					}
				}
				else
				{
					pp->state = PS_STAND_LEFT;
					pp->frame = 0;
				}
			}
	    	break;
		}
		case PS_DUCK_RIGHT:
		{
			player_friction(pp);
			break;
		}
		case PS_DUCK_LEFT:
		{
			player_friction(pp);
			break;
		}
		case PS_JUMP_RIGHT:
		{
			if(pp->controller.right)
			{
				if(pp->vx < pp->mx)
				{
	            	pp->vx = fixadd(pp->vx, ftofix(0.5));
            		if(pp->vx > pp->mx)
            		{
		                pp->vx = pp->mx;
            		}
            	}
			}
			else
			{
				player_add_friction(pp);
			}
			break;
		}
		case PS_JUMP_LEFT:
		{
			if(pp->controller.left)
			{
				if(pp->vx > -pp->mx)
				{
	            	pp->vx = fixsub(pp->vx, ftofix(0.5));
            		if(pp->vx < -pp->mx)
            		{
		                pp->vx = -pp->mx;
            		}
            	}
			}
			else
			{
				player_add_friction(pp);
			}
			break;
		}
		case PS_FALL_RIGHT:
		{
			if(pp->controller.right)
			{
				if(pp->vx < pp->mx)
				{
	            	pp->vx = fixadd(pp->vx, ftofix(0.5));
            		if(pp->vx > pp->mx)
            		{
		                pp->vx = pp->mx;
            		}
            	}
			}
			else
			{
				player_add_friction(pp);
			}
			break;
		}
		case PS_FALL_LEFT:
		{
			if(pp->controller.left)
			{
				if(pp->vx > -pp->mx)
				{
	            	pp->vx = fixsub(pp->vx, ftofix(0.5));
            		if(pp->vx < -pp->mx)
            		{
		                pp->vx = -pp->mx;
            		}
            	}
			}
			else
			{
				player_add_friction(pp);
			}
			break;
		}
	}
}

/* control player using the current state of it's controller */
void player_control(PLAYER * pp)
{
	if(pp->active && !pp->tele_in && !pp->tele_out)
	{
		player_update_state(pp);
   		player_adjust(pp);
    }
}

void player_grab_object(PLAYER * pp, OBJECT * op)
{
	int i;

	if(op->active)
	{
		if(collide(&pp->cmap[pp->state], &op->cmap))
		{
			switch(op->type)
			{
				case PP_OBJECT_AMMO_NORMAL:
				{
					if(pp->ammo[PP_AMMO_TYPE_NORMAL] < 99)
					{
						pp->ammo[PP_AMMO_TYPE_NORMAL] += 20;
						if(pp->ammo[PP_AMMO_TYPE_NORMAL] > 99)
						{
							pp->ammo[PP_AMMO_TYPE_NORMAL] = 99;
						}
						ncds_play_sample(pp->sound[PP_SOUND_AMMO], 128, -1, -1);
						object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
						object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
						op->active = 0;
						if(pp->ammo[pp->weapon] == 0)
						{
			    			pp_cycle_ammo(pp);
			    			if(pp->ammo[pp->weapon] > 0)
		    				{
			    				if(pp->turbo_time)
			    				{
			    					pp->reload_time = 15;
						       		pp->reload_fast = 1;
			    				}
			    				else
			    				{
			    					pp->reload_time = 35;
						       		pp->reload_fast = 0;
		    					}
		    				}
						}
					}
					return;
				}
				case PP_OBJECT_AMMO_X:
				{
					if(pp->ammo[PP_AMMO_TYPE_X] < 99)
					{
						pp->ammo[PP_AMMO_TYPE_X] += 20;
						if(pp->ammo[PP_AMMO_TYPE_X] > 99)
						{
							pp->ammo[PP_AMMO_TYPE_X] = 99;
						}
						ncds_play_sample(pp->sound[PP_SOUND_AMMO], 128, -1, -1);
						object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
						object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
						op->active = 0;
						if(pp->ammo[pp->weapon] == 0)
						{
			    			pp_cycle_ammo(pp);
			    			if(pp->ammo[pp->weapon] > 0)
		    				{
			    				if(pp->turbo_time)
			    				{
			    					pp->reload_time = 15;
						       		pp->reload_fast = 1;
			    				}
			    				else
			    				{
			    					pp->reload_time = 35;
						       		pp->reload_fast = 0;
		    					}
		    				}
						}
					}
					return;
				}
				case PP_OBJECT_AMMO_MINE:
				{
					if(pp->ammo[PP_AMMO_TYPE_MINE] < 99)
					{
						pp->ammo[PP_AMMO_TYPE_MINE] += 20;
						if(pp->ammo[PP_AMMO_TYPE_MINE] > 99)
						{
							pp->ammo[PP_AMMO_TYPE_MINE] = 99;
						}
						ncds_play_sample(pp->sound[PP_SOUND_AMMO], 128, -1, -1);
						object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
						object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
						op->active = 0;
						if(pp->ammo[pp->weapon] == 0)
						{
			    			pp_cycle_ammo(pp);
			    			if(pp->ammo[pp->weapon] > 0)
		    				{
			    				if(pp->turbo_time)
			    				{
			    					pp->reload_time = 15;
						       		pp->reload_fast = 1;
		    					}
		    					else
		    					{
			    					pp->reload_time = 35;
						       		pp->reload_fast = 0;
		    					}
		    				}
						}
					}
					return;
				}
				case PP_OBJECT_AMMO_BOUNCE:
				{
					if(pp->ammo[PP_AMMO_TYPE_BOUNCE] < 99)
					{
						pp->ammo[PP_AMMO_TYPE_BOUNCE] += 20;
						if(pp->ammo[PP_AMMO_TYPE_BOUNCE] > 99)
						{
							pp->ammo[PP_AMMO_TYPE_BOUNCE] = 99;
						}
						ncds_play_sample(pp->sound[PP_SOUND_AMMO], 128, -1, -1);
						object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
						object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
						op->active = 0;
						if(pp->ammo[pp->weapon] == 0)
						{
			    			pp_cycle_ammo(pp);
			    			if(pp->ammo[pp->weapon] > 0)
		    				{
			    				if(pp->turbo_time)
			    				{
			    					pp->reload_time = 15;
						       		pp->reload_fast = 1;
		    					}
		    					else
		    					{
			    					pp->reload_time = 35;
						       		pp->reload_fast = 0;
		    					}
		    				}
						}
					}
					return;
				}
				case PP_OBJECT_AMMO_SEEK:
				{
					if(pp->ammo[PP_AMMO_TYPE_SEEK] < 99)
					{
						pp->ammo[PP_AMMO_TYPE_SEEK] += 20;
						if(pp->ammo[PP_AMMO_TYPE_SEEK] > 99)
						{
							pp->ammo[PP_AMMO_TYPE_SEEK] = 99;
						}
						ncds_play_sample(pp->sound[PP_SOUND_AMMO], 128, -1, -1);
						object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
						object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
						op->active = 0;
						if(pp->ammo[pp->weapon] == 0)
						{
			    			pp_cycle_ammo(pp);
			    			if(pp->ammo[pp->weapon] > 0)
		    				{
			    				if(pp->turbo_time)
			    				{
			    					pp->reload_time = 15;
						       		pp->reload_fast = 1;
		    					}
		    					else
		    					{
			    					pp->reload_time = 35;
						       		pp->reload_fast = 0;
		    					}
		    				}
						}
					}
					return;
				}
				case PP_OBJECT_POWER_CLOAK:
				{
					pp->cloak_time = 600;
					ncds_play_sample(pp->sound[PP_SOUND_CLOAK], 128, -1, -1);
					object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
					object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
					op->active = 0;
					if(pp_game_data.mode != PP_MODE_TARGET)
					{
						for(i = 0; i < 4; i++)
						{
							if(pp_game_data.player[i].target == pp->color)
							{
								pp_game_data.player[i].target = -1;
							}
						}
					}
					return;
				}
				case PP_OBJECT_POWER_DEFLECT:
				{
					pp->deflect_time = 600;
					ncds_play_sample(pp->sound[PP_SOUND_DEFLECT], 128, -1, -1);
					object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
					object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
					op->active = 0;
					return;
				}
				case PP_OBJECT_POWER_JUMP:
				{
					pp->gy = ftofix(0.4);
					pp->moon_time = 600;
					ncds_play_sample(pp->sound[PP_SOUND_PJUMP], 128, -1, -1);
					object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
					object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
					op->active = 0;
					return;
				}
				case PP_OBJECT_POWER_RUN:
				{
			        pp->mx = ftofix(2.5);
			        pp->run_time = 600;
					ncds_play_sample(pp->sound[PP_SOUND_RUN], 128, -1, -1);
					object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
					object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
					op->active = 0;
					return;
				}
				case PP_OBJECT_POWER_FLY:
				{
			        pp->fly_time = 600;
					ncds_play_sample(pp->sound[PP_SOUND_PFLY], 128, -1, -1);
					object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
					object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
					op->active = 0;
					return;
				}
				case PP_OBJECT_POWER_TURBO:
				{
			        pp->turbo_time = 600;
					ncds_play_sample(pp->sound[PP_SOUND_PTURBO], 128, -1, -1);
					object_drop(op, PP_OBJECT_GENERATOR, itofix(0), itofix(0));
					object_drop(op, PP_OBJECT_POWER_POOF, itofix(0), itofix(0));
					op->active = 0;
					return;
				}
				case PP_OBJECT_FLAG:
				{
					if(!pp->shock_time)
					{
						ncds_play_sample(pp->sound[PP_SOUND_FLAG], 128, -1, -1);
						object_drop(op, PP_OBJECT_POOF, itofix(0), itofix(0));
						pp_game_data.flag_sprite = -1;
						pp->flag = 1;
						op->active = 0;
					}
					return;
				}
				case PP_OBJECT_BASE:
				{
					if(pp->flag)
					{
						ncds_play_sample(pp->sound[PP_SOUND_BASE], 128, -1, -1);
						pp->flag = 0;
						pp->score++;
						if(pp_game_data.winner < 0)
						{
							if(pp_game_data.flags > 0)
							{
								pp_game_data.flag_sprite = object_drop(&pp_game_data.object[pp_game_data.flag_list[rand() % pp_game_data.flags]], PP_OBJECT_FLAG, 0, 0);
								if(pp_game_data.flag_sprite >= 0)
								{
									pp_game_data.object[pp_game_data.flag_sprite].flag[PP_FLAG_FLAG_DROP] = 0;
								}
							}
							else
							{
								pp_game_data.flag_sprite = -1;
							}
							if(pp_game_data.bases > 0)
							{
								pp_game_data.base_sprite = object_drop(&pp_game_data.object[pp_game_data.base_list[rand() % pp_game_data.bases]], PP_OBJECT_BASE, 0, 0);
							}
							else
							{
								pp_game_data.base_sprite = -1;
							}
						}
						else
						{
							pp_game_data.flag_sprite = -1;
							pp_game_data.base_sprite = -1;
						}
						op->active = 0;
					}
					return;
				}
				case PP_OBJECT_BANK:
				{
					if(pp->loose)
					{
						ncds_play_sample(pp->sound[PP_SOUND_AMMO], 128, -1, -1);
						pp->score += pp->loose;
						pp->time += 60 * pp->loose;
						pp->loose = 0;
					}
					return;
				}
				case PP_OBJECT_GEM_1:
				case PP_OBJECT_GEM_2:
				case PP_OBJECT_GEM_3:
				case PP_OBJECT_GEM_4:
				case PP_OBJECT_GEM_5:
				{
					ncds_play_sample(pp->sound[PP_SOUND_GEM], 128, -1, -1);
					object_drop(op, PP_OBJECT_GEM_POOF, itofix(0), itofix(0));
					pp->item = 1;
					op->active = 0;
					return;
				}
				case PP_OBJECT_HUNT_BASE:
				{
					if(pp->item)
					{
						ncds_play_sample(pp->sound[PP_SOUND_BANK], 128, -1, -1);
						pp->gem[pp->gems] = 1;
						pp->gems++;
						pp->item = 0;
						pp_game_data.hunts = 0;
						for(i = 0; i < PP_MAX_OBJECTS; i++)
						{
							if(pp_game_data.object[i].active && pp_game_data.object[i].type == PP_OBJECT_HUNT_PORTAL)
							{
								pp_game_data.hunt_list[pp_game_data.hunts] = i;
								pp_game_data.hunts++;
							}
						}
						if(pp_game_data.hunts > 0 && pp->gems < 5)
						{
							i = pp_game_data.hunt_list[rand() % pp_game_data.hunts];
							pp_game_data.gem_sprite = object_drop(&pp_game_data.object[i], PP_OBJECT_GEM_1 + pp->gems, 0, 0);
							pp_game_data.object[i].active = 0;
						}
					}
					return;
				}
				case PP_OBJECT_SPRING_UP:
				{
					if(collide_bottom(&pp->cmap[pp->state], &op->cmap))
					{
						pp->vy = itofix(-14);
						pp->state = PS_JUMP_RIGHT + pp->state % 2;
						pp->y = fixtoi(op->y) - pp->character.ap[pp->state]->h + 1;
						pp->fy = itofix(pp->y);
		        		player_update_collision_map(pp);
		        		op->flag[PP_POOF_FRAME] = -1;
						ncds_play_sample(pp->sound[PP_SOUND_SPRING], 128, -1, -1);
					}
					return;
				}
				case PP_OBJECT_SPRING_DOWN:
				{
					if(collide_top(&pp->cmap[pp->state], &op->cmap))
					{
						pp->vy = itofix(7);
						pp->state = PS_FALL_RIGHT + pp->state % 2;
						pp->y = fixtoi(op->y) + pp_object_ani[op->type]->h - pp->cmap[pp->state].ry;
						pp->fy = itofix(pp->y);
		        		player_update_collision_map(pp);
		        		op->flag[PP_POOF_FRAME] = -1;
						ncds_play_sample(pp->sound[PP_SOUND_SPRING], 128, -1, -1);
					}
					return;
				}
				case PP_OBJECT_SPRING_LEFT:
				{
					if(collide_right(&pp->cmap[pp->state], &op->cmap))
					{
						pp->vx = itofix(-14);
						pp->state = pp->state % 2 == 0 ? pp->state + 1 : pp->state;
						pp->x = fixtoi(op->x) - pp->character.ap[pp->state]->w + 1;
						pp->fx = itofix(pp->x);
		        		player_update_collision_map(pp);
		        		op->flag[PP_POOF_FRAME] = -1;
						ncds_play_sample(pp->sound[PP_SOUND_SPRING], 128, -1, -1);
					}
					return;
				}
				case PP_OBJECT_SPRING_RIGHT:
				{
					if(collide_left(&pp->cmap[pp->state], &op->cmap))
					{
						pp->vx = itofix(14);
						pp->state = pp->state % 2 == 0 ? pp->state : pp->state - 1;
						pp->x = fixtoi(op->x) + pp_object_ani[op->type]->w - pp->cmap[pp->state].rx;
						pp->fx = itofix(pp->x);
		        		player_update_collision_map(pp);
		        		op->flag[PP_POOF_FRAME] = -1;
						ncds_play_sample(pp->sound[PP_SOUND_SPRING], 128, -1, -1);
					}
					return;
				}
				default:
				{
					return;
				}
			}
		}
	}
}

void player_grab_objects(PLAYER * pp)
{
	int i;

	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		player_grab_object(pp, &pp_game_data.object[i]);
	}
}

void player_guard_territory_x(int i)
{
	int j;

	for(j = 0; j < 4; j++)
	{
		if(j != i)
		{
			if(pp_game_data.player[j].active && !pp_game_data.player[j].tele_out && collide(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], &pp_game_data.player[j].cmap[pp_game_data.player[j].state]))
			{
				if(cmap_vx(&pp_game_data.player[i].cmap[pp_game_data.player[i].state]) == itofix(0))
				{
			    	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_TELE_IN], 128, -1, -1);
					pp_game_data.player[j].x = get_sprite_edge_x(&pp_game_data.player[j].cmap[pp_game_data.player[j].state], &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
					pp_game_data.player[j].fx = itofix(pp_game_data.player[j].x);
					pp_game_data.player[i].fx = itofix(pp_game_data.player[i].x);
	        		player_update_collision_map(&pp_game_data.player[i]);
	        		player_update_collision_map(&pp_game_data.player[j]);
					pp_game_data.player[i].vx += fdiv(pp_game_data.player[j].vx, itofix(2));
					if(pp_game_data.player[i].vx < -pp_game_data.player[i].mx)
					{
						pp_game_data.player[i].vx = -pp_game_data.player[i].mx;
					}
					else if(pp_game_data.player[i].vx > pp_game_data.player[i].mx)
					{
						pp_game_data.player[i].vx = pp_game_data.player[i].mx;
					}
				}
				else
				{
					pp_game_data.player[i].x = get_sprite_edge_x(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], &pp_game_data.player[j].cmap[pp_game_data.player[j].state]);
					pp_game_data.player[i].fx = itofix(pp_game_data.player[i].x);
		       		player_update_collision_map(&pp_game_data.player[i]);
					pp_game_data.player[j].fx = itofix(pp_game_data.player[j].x);
					pp_game_data.player[j].vx += fdiv(pp_game_data.player[i].vx, itofix(2));
					if(pp_game_data.player[j].vx < -pp_game_data.player[j].mx)
					{
						pp_game_data.player[j].vx = -pp_game_data.player[j].mx;
					}
					else if(pp_game_data.player[j].vx > pp_game_data.player[j].mx)
					{
						pp_game_data.player[j].vx = pp_game_data.player[j].mx;
					}
				}
			}
		}
	}
}

void player_guard_territory_y(int i)
{
	int j;

	for(j = 0; j < 4; j++)
	{
		if(j != i)
		{
			if(pp_game_data.player[j].active && !pp_game_data.player[j].tele_out && collide(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], &pp_game_data.player[j].cmap[pp_game_data.player[j].state]))
			{
				pp_game_data.player[i].y = get_sprite_edge_y(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], &pp_game_data.player[j].cmap[pp_game_data.player[j].state]);
				pp_game_data.player[i].fy = itofix(pp_game_data.player[i].y);
				pp_game_data.player[j].fy = itofix(pp_game_data.player[j].y);
				if(cmap_vy(&pp_game_data.player[i].cmap[pp_game_data.player[i].state]) > itofix(0))
				{
					pp_game_data.player[j].vy = itofix(0);
					pp_game_data.player[i].vy = itofix(-5);
                	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
					pp_game_data.player[i].state = PS_JUMP_RIGHT + pp_game_data.player[i].state % 2;
			    	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_JUMP], 128, -1, -1);
					if(pp_game_data.player[j].state == PS_STAND_LEFT || pp_game_data.player[j].state == PS_STAND_RIGHT || pp_game_data.player[j].state == PS_WALK_LEFT || pp_game_data.player[j].state == PS_WALK_RIGHT || pp_game_data.player[j].state == PS_STAND_UP_LEFT || pp_game_data.player[j].state == PS_STAND_UP_RIGHT)
					{
						pp_game_data.player[j].state = PS_DUCK_RIGHT + pp_game_data.player[j].state % 2;
					}
				}
				else if(cmap_vy(&pp_game_data.player[j].cmap[pp_game_data.player[j].state]) > itofix(0))
				{
					pp_game_data.player[i].vy = itofix(0);
					pp_game_data.player[j].vy = itofix(-5);
                	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
					pp_game_data.player[j].state = PS_JUMP_RIGHT + pp_game_data.player[j].state % 2;
			    	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_JUMP], 128, -1, -1);
				}
				else
				{
					if(pp_game_data.player[i].y + pp_game_data.player[i].cmap[pp_game_data.player[i].state].left.point[0].y > pp_game_data.player[j].y + pp_game_data.player[j].cmap[pp_game_data.player[j].state].left.point[0].y)
					{
						pp_game_data.player[j].y = get_sprite_bottom_edge(&pp_game_data.player[j].cmap[pp_game_data.player[j].state], &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
						pp_game_data.player[j].vy = itofix(-5);
						pp_game_data.player[i].vy = itofix(0);
	                	ncds_play_sample(pp_game_data.player[j].sound[PP_SOUND_LAND], 128, -1, -1);
						pp_game_data.player[j].state = PS_JUMP_RIGHT + pp_game_data.player[j].state % 2;
				    	ncds_play_sample(pp_game_data.player[j].sound[PP_SOUND_JUMP], 128, -1, -1);
						if(pp_game_data.player[i].state == PS_STAND_LEFT || pp_game_data.player[i].state == PS_STAND_RIGHT || pp_game_data.player[i].state == PS_WALK_LEFT || pp_game_data.player[i].state == PS_WALK_RIGHT || pp_game_data.player[i].state == PS_STAND_UP_LEFT || pp_game_data.player[i].state == PS_STAND_UP_RIGHT)
						{
							pp_game_data.player[i].state = PS_DUCK_RIGHT + pp_game_data.player[i].state % 2;
						}
					}
					else
					{
						pp_game_data.player[i].y = get_sprite_bottom_edge(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], &pp_game_data.player[j].cmap[pp_game_data.player[j].state]);
						pp_game_data.player[j].vy = itofix(0);
						pp_game_data.player[i].vy = itofix(-5);
	                	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
						pp_game_data.player[i].state = PS_JUMP_RIGHT + pp_game_data.player[i].state % 2;
				    	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_JUMP], 128, -1, -1);
						if(pp_game_data.player[j].state == PS_STAND_LEFT || pp_game_data.player[j].state == PS_STAND_RIGHT || pp_game_data.player[j].state == PS_WALK_LEFT || pp_game_data.player[j].state == PS_WALK_RIGHT || pp_game_data.player[j].state == PS_STAND_UP_LEFT || pp_game_data.player[j].state == PS_STAND_UP_RIGHT)
						{
							pp_game_data.player[j].state = PS_DUCK_RIGHT + pp_game_data.player[j].state % 2;
						}
					}
				}
			}
		}
	}
}

/* handle logic for specified player */
void player_logic(int i, TILEMAP * mp)
{
	int j;

    player_message_logic(&pp_game_data.player[i].message);
    if(pp_game_data.player[i].active)
    {
       	if(pp_game_data.player[i].gun_flash.active)
       	{
	        pp_game_data.player[i].gun_flash.active--;
       	}
	    if(pp_game_data.player[i].tele_in)
	    {
		    if(pp_game_data.player[i].frame == 0)
		    {
		    	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_TELE_IN], 128, -1, -1);
	    	}
	    	pp_game_data.player[i].frame++;
	    	if(pp_game_data.player[i].frame >= pp_game_data.player[i].character.ap[0]->h)
	    	{
			    pp_game_data.player[i].tele_in = 0;
				ncds_queue_sample(pp_game_data.player[i].sound[PP_SOUND_START]);
	    	}
	    	else
	    	{
			    clear_bitmap(pp_game_data.player[i].zap_image);
		    	draw_character_ex(pp_game_data.player[i].zap_image, get_ani(pp_game_data.player[i].character.ap[pp_game_data.player[i].tele_state], pp_game_data.player[i].tele_frame), 0, 0, 31, -1);
		    	for(j = 0; j < pp_game_data.player[i].character.ap[0]->h - pp_game_data.player[i].frame - 1; j++)
	    		{
					hline(pp_game_data.player[i].zap_image, 0, j, pp_game_data.player[i].character.ap[0]->w - 1, 0);
	    		}
    		}
    	}

		else if(pp_game_data.player[i].tele_out)
	    {
		    if(pp_game_data.player[i].frame == 0)
		    {
//		    	ncds_play_sample(&ncds_channels[1], pp_game_data.player[i].sound[PP_SOUND_TELE_OUT], 128, -1);
		    	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_TELE_OUT], 128, -1, -1);
//		    	ncds_queue_sample(pp_game_data.player[i].sound[PP_SOUND_TELE_OUT]);
	    	}
	    	pp_game_data.player[i].frame++;
	    	if(pp_game_data.player[i].frame > pp_game_data.player[i].character.ap[0]->h)
	    	{
			    pp_game_data.player[i].tele_out = 0;
			    pp_game_data.player[i].active = 0;
//				blit(pp_image[PP_IMAGE_ELIMINATED], pp_game_data.player[i].screen.bp, 0, 0, 0, 0, 320, 240);
				if(pp_game_data.player[i].screen.bp)
				{
					stretch_blit(pp_image[PP_IMAGE_ELIMINATED], pp_game_data.player[i].screen.bp, 0, 0, pp_image[PP_IMAGE_ELIMINATED]->w, pp_image[PP_IMAGE_ELIMINATED]->h, 0, 0, pp_game_data.player[i].screen.bp->w, pp_game_data.player[i].screen.bp->h);
				}
				pp_game_data.player[i].just = 1;
	    	}
	    	else
	    	{
			    clear_bitmap(pp_game_data.player[i].zap_image);
		    	draw_character_ex(pp_game_data.player[i].zap_image, get_ani(pp_game_data.player[i].character.ap[pp_game_data.player[i].tele_state], pp_game_data.player[i].tele_frame), 0, 0, 31, -1);
		    	for(j = 0; j < pp_game_data.player[i].frame; j++)
	    		{
					hline(pp_game_data.player[i].zap_image, 0, j, pp_game_data.player[i].character.ap[0]->w - 1, 0);
	    		}
    		}
    	}

		else
		{
    		/* update frame */
        	pp_game_data.player[i].frame++;
        	if(pp_game_data.player[i].flash_time)
        	{
	            pp_game_data.player[i].flash_time--;
        	}
        	if(pp_game_data.player[i].cloak_time)
        	{
		        pp_game_data.player[i].cloak_time--;
        	}
        	if(pp_game_data.player[i].deflect_time)
        	{
		        pp_game_data.player[i].deflect_time--;
        	}
        	if(pp_game_data.player[i].moon_time)
        	{
		        pp_game_data.player[i].moon_time--;
	        	if(pp_game_data.player[i].moon_time <= 0)
	        	{
			        pp_game_data.player[i].gy = ftofix(0.5);
	        	}
        	}
        	if(pp_game_data.player[i].run_time)
        	{
		        pp_game_data.player[i].run_time--;
	        	if(pp_game_data.player[i].run_time <= 0)
	        	{
			        pp_game_data.player[i].mx = itofix(2);
	    		}
        	}
        	if(pp_game_data.player[i].fly_time)
        	{
		        pp_game_data.player[i].fly_time--;
        	}
        	if(pp_game_data.player[i].turbo_time)
        	{
		        pp_game_data.player[i].turbo_time--;
        	}
        	if(pp_game_data.player[i].reload_time)
        	{
		        pp_game_data.player[i].reload_time--;
		        if(pp_game_data.player[i].reload_fast)
		        {
	        		if(pp_game_data.player[i].reload_time == 10)
	        		{
				        ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_RELOAD_A], 128, -1, -1);
	        		}
	        		if(pp_game_data.player[i].reload_time == 5)
	        		{
				        ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_RELOAD_B], 128, -1, -1);
	        		}
		     	}
		     	else
		     	{
	        		if(pp_game_data.player[i].reload_time == 25)
	        		{
				        ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_RELOAD_A], 128, -1, -1);
	        		}
	        		if(pp_game_data.player[i].reload_time == 10)
	        		{
				        ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_RELOAD_B], 128, -1, -1);
	        		}
        		}
        	}
        	if(pp_game_data.player[i].shock_time)
        	{
		        pp_game_data.player[i].shock_time--;
        	}
        	if(pp_option[PP_OPTION_AUTO_TARGET] && pp_game_data.player[i].weapon == PP_AMMO_TYPE_SEEK && pp_game_data.mode != PP_MODE_TARGET && pp_game_data.mode != PP_MODE_SCAVENGER)
        	{
        		pp_find_target(&pp_game_data.player[i]);
        	}

	        /* move player horizontally */
        	player_update_collision_map(&pp_game_data.player[i]);
        	if(!pp_game_data.player[i].col)
        	{
        		if(!tilemap_scrapeb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]))
        		{
	        		pp_game_data.player[i].fx = fixadd(pp_game_data.player[i].fx, tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_CONVEY, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]));
        		}
	        	else
        		{
	        		pp_game_data.player[i].fx = fixadd(pp_game_data.player[i].fx, tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_CONVEY, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]));
        		}
        	}
        	pp_game_data.player[i].col = 0;
        	pp_game_data.player[i].fx = fixadd(pp_game_data.player[i].fx, pp_game_data.player[i].vx);
        	pp_game_data.player[i].x = fixtoi(pp_game_data.player[i].fx);
        	player_update_collision_map(&pp_game_data.player[i]);
        	if(tilemap_collide(mp, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]) || tilemap_nudge(mp, pp_game_data.level->tilemap->il, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]))
        	{
	        	pp_game_data.player[i].col = 1;
            	pp_game_data.player[i].vx = itofix(0);
	            pp_game_data.player[i].x = get_sprite_x(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], mp->ts->w);
	           	pp_game_data.player[i].fx = itofix(pp_game_data.player[i].x);
	        	player_move_collision_map(&pp_game_data.player[i]);
        	}
        	player_guard_territory_x(i);
	        player_move_collision_map(&pp_game_data.player[i]);
        	player_grab_objects(&pp_game_data.player[i]);

        	/* move player vertically */
        	player_update_collision_map(&pp_game_data.player[i]);
        	pp_game_data.player[i].fy = fixadd(pp_game_data.player[i].fy, pp_game_data.player[i].vy);
        	pp_game_data.player[i].y = fixtoi(pp_game_data.player[i].fy);
        	player_update_collision_map(&pp_game_data.player[i]);
        	if(tilemap_collide(mp, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]))
        	{
	            pp_game_data.player[i].y = get_sprite_y(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], mp->ts->h);
            	pp_game_data.player[i].fy = itofix(pp_game_data.player[i].y);
            	player_move_collision_map(&pp_game_data.player[i]);
            	pp_game_data.player[i].vy = itofix(0);
            	if(pp_game_data.player[i].state == PS_FALL_LEFT && pp_game_data.player[i].fly_time <= 0)
            	{
	                pp_game_data.player[i].state = PS_WALK_LEFT;
	                pp_game_data.player[i].frame = 0;
                	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
	            }
            	else if(pp_game_data.player[i].state == PS_FALL_RIGHT && pp_game_data.player[i].fly_time <= 0)
            	{
	                pp_game_data.player[i].state = PS_WALK_RIGHT;
	                pp_game_data.player[i].frame = 0;
                	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
            	}
            	else if(pp_game_data.player[i].state == PS_JUMP_LEFT && pp_game_data.player[i].fly_time <= 0)
            	{
	                pp_game_data.player[i].state = PS_FALL_LEFT;
                	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_BUMP], 128, -1, -1);
            	}
            	else if(pp_game_data.player[i].state == PS_JUMP_RIGHT && pp_game_data.player[i].fly_time <= 0)
            	{
	                pp_game_data.player[i].state = PS_FALL_RIGHT;
                	ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_BUMP], 128, -1, -1);
            	}
	        }
	        player_guard_territory_y(i);
	        player_move_collision_map(&pp_game_data.player[i]);
        	player_grab_objects(&pp_game_data.player[i]);
        	player_update_collision_map(&pp_game_data.player[i]);

        	/* if player isn't on the ground, add gravity */
        	if(!player_grounded(&pp_game_data.player[i], mp))
        	{
	        	if(pp_game_data.player[i].fly_time > 0 && (pp_game_data.player[i].controller.jump || pp_game_data.player[i].controller.wjump))
	        	{
		        	if(pp_game_data.player[i].vy > itofix(-5))
		        	{
		        		pp_game_data.player[i].vy = fixadd(pp_game_data.player[i].vy, ftofix(-0.5));
	            		if(pp_game_data.player[i].vy < itofix(-5))
            			{
			                pp_game_data.player[i].vy = itofix(-5);
            			}
            		}
            		else
            		{
	            		pp_game_data.player[i].vy = fixadd(pp_game_data.player[i].vy, pp_game_data.player[i].gy);
            		}
            		if(pp_game_data.player[i].state < OLD_PLAYER_STATES)
            		{
		        		if(pp_game_data.player[i].state != PS_JUMP_LEFT && pp_game_data.player[i].state != PS_JUMP_RIGHT && pp_game_data.player[i].state != PS_FALL_LEFT && pp_game_data.player[i].state != PS_FALL_RIGHT)
		        		{
				        	pp_game_data.player[i].state = PS_JUMP_RIGHT + pp_game_data.player[i].state % 2;
		        		}
		        		if(pp_game_data.player[i].state == PS_FALL_LEFT)
		        		{
				        	pp_game_data.player[i].state = PS_JUMP_LEFT;
		        		}
		        		else if(pp_game_data.player[i].state == PS_FALL_RIGHT)
		        		{
				        	pp_game_data.player[i].state = PS_JUMP_RIGHT;
		        		}
	        		}
	        		else
	        		{
		        		if(pp_game_data.player[i].state != PS_FJUMP_LEFT && pp_game_data.player[i].state != PS_FJUMP_RIGHT && pp_game_data.player[i].state != PS_FFALL_LEFT && pp_game_data.player[i].state != PS_FFALL_RIGHT)
		        		{
				        	pp_game_data.player[i].state = PS_FJUMP_RIGHT + pp_game_data.player[i].state % 2;
		        		}
		        		if(pp_game_data.player[i].state == PS_FFALL_LEFT)
		        		{
				        	pp_game_data.player[i].state = PS_FJUMP_LEFT;
		        		}
		        		else if(pp_game_data.player[i].state == PS_FFALL_RIGHT)
		        		{
				        	pp_game_data.player[i].state = PS_FJUMP_RIGHT;
		        		}
	        		}
	        		if(pp_game_data.player[i].frame % 6 == 0)
	        		{
			        	generate_object(PP_OBJECT_JET, itofix(pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].x - pp_object_ani[PP_OBJECT_JET]->w / 2), itofix(pp_game_data.player[i].cmap[pp_game_data.player[i].state].y + pp_game_data.player[i].cmap[pp_game_data.player[i].state].ry + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].y - pp_object_ani[PP_OBJECT_JET]->h / 2));
			    		ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_FLY], 128, -1, -1);
	        		}
            	}
            	else
            	{
            		pp_game_data.player[i].vy = fixadd(pp_game_data.player[i].vy, pp_game_data.player[i].gy);
            	}
            	if(pp_game_data.player[i].vy > itofix(0))
            	{
	                if(pp_game_data.player[i].state == PS_JUMP_LEFT)
                	{
	                    pp_game_data.player[i].state = PS_FALL_LEFT;
                	}
                	else if(pp_game_data.player[i].state == PS_JUMP_RIGHT)
                	{
	                    pp_game_data.player[i].state = PS_FALL_RIGHT;
                	}
                	else if(pp_game_data.player[i].state == PS_WALK_LEFT || pp_game_data.player[i].state == PS_DUCK_LEFT || pp_game_data.player[i].state == PS_STAND_LEFT || pp_game_data.player[i].state == PS_STAND_UP_LEFT)
                	{
	                    pp_game_data.player[i].state = PS_FALL_LEFT;
                	}
                	else if(pp_game_data.player[i].state == PS_WALK_RIGHT || pp_game_data.player[i].state == PS_DUCK_RIGHT || pp_game_data.player[i].state == PS_STAND_RIGHT || pp_game_data.player[i].state == PS_STAND_UP_RIGHT)
                	{
	                    pp_game_data.player[i].state = PS_FALL_RIGHT;
                	}
            	}
            	if(pp_game_data.player[i].vy > itofix(7))
            	{
	                pp_game_data.player[i].vy = itofix(7);
            	}
            	if(pp_game_data.player[i].vy < itofix(-7))
            	{
//	                pp_game_data.player[i].vy = itofix(-7);
            	}
        	}
        	else
        	{
            	if(pp_game_data.player[i].state == PS_FALL_LEFT)
           		{
	                pp_game_data.player[i].state = PS_WALK_LEFT;
               		ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
               		pp_game_data.player[i].frame = 0;
           		}
           		else if(pp_game_data.player[i].state == PS_FALL_RIGHT)
           		{
	                pp_game_data.player[i].state = PS_WALK_RIGHT;
               		ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
               		pp_game_data.player[i].frame = 0;
           		}
           		else if(pp_game_data.player[i].state == PS_JUMP_LEFT && pp_game_data.player[i].vy >= itofix(0))
           		{
	                pp_game_data.player[i].state = PS_WALK_LEFT;
               		ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
               		pp_game_data.player[i].frame = 0;
           		}
           		else if(pp_game_data.player[i].state == PS_JUMP_RIGHT && pp_game_data.player[i].vy >= itofix(0))
           		{
	                pp_game_data.player[i].state = PS_WALK_RIGHT;
               		ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_LAND], 128, -1, -1);
               		pp_game_data.player[i].frame = 0;
           		}
        	}
        	if(pp_game_data.player[i].gun_flash.active)
        	{
	            pp_game_data.player[i].gun_flash.x = pp_game_data.player[i].x + pp_game_data.player[i].character.gx_table[pp_game_data.player[i].state];
            	pp_game_data.player[i].gun_flash.y = pp_game_data.player[i].y + pp_game_data.player[i].character.gy_table[pp_game_data.player[i].state];
				create_paintball(pp_game_data.player[i].color, pp_game_data.player[i].x + pp_game_data.player[i].character.px_table[pp_game_data.player[i].state], pp_game_data.player[i].y + pp_game_data.player[i].character.py_table[pp_game_data.player[i].state], a_table[pp_game_data.player[i].state], pp_game_data.player[i].weapon);
				if(pp_game_data.player[i].ammo[pp_game_data.player[i].weapon] == 0)
				{
				    pp_cycle_ammo(&pp_game_data.player[i]);
			    	if(pp_game_data.player[i].ammo[pp_game_data.player[i].weapon] > 0)
		    		{
       					if(pp_game_data.player[i].turbo_time)
       					{
       						pp_game_data.player[i].reload_time = 15;
				       		pp_game_data.player[i].reload_fast = 1;
       					}
       					else
       					{
       						pp_game_data.player[i].reload_time = 35;
       						pp_game_data.player[i].reload_fast = 0;
				       	}
		    		}
		    		else
		    		{
			    		pp_game_data.player[i].target = -1;
		    		}
				}
        	}
    	}
    	pp_game_data.player[i].cx = pp_game_data.player[i].x + pp_game_data.player[i].character.mix;
    	pp_game_data.player[i].cy = pp_game_data.player[i].y + pp_game_data.player[i].character.miy;
    	if(player_grounded(&pp_game_data.player[i], pp_game_data.level->tilemap))
    	{
	    	if(tilemap_flag(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].left.point[0].x, pp_game_data.player[i].cmap[pp_game_data.player[i].state].y + pp_game_data.player[i].cmap[pp_game_data.player[i].state].ry + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].y + 1))
	    	{
		    	if(tilemap_flag(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].right.point[0].x, pp_game_data.player[i].cmap[pp_game_data.player[i].state].y + pp_game_data.player[i].cmap[pp_game_data.player[i].state].ry + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].y + 1))
		    	{
		    		pp_game_data.player[i].tx = (pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].x) / 16;
		    	}
		    	else
		    	{
		    		pp_game_data.player[i].tx = (pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].left.point[0].x) / 16;
		    	}
	    	}
	    	else if(tilemap_flag(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].right.point[0].x, pp_game_data.player[i].cmap[pp_game_data.player[i].state].y + pp_game_data.player[i].cmap[pp_game_data.player[i].state].ry + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].y + 1))
	    	{
	    		pp_game_data.player[i].tx = (pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].right.point[0].x) / 16;
	    	}
		}
		else
		{
    		pp_game_data.player[i].tx = (pp_game_data.player[i].cmap[pp_game_data.player[i].state].x + pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].x) / 16;
		}
    	pp_game_data.player[i].ty = (pp_game_data.player[i].cmap[pp_game_data.player[i].state].y + pp_game_data.player[i].cmap[pp_game_data.player[i].state].ry + pp_game_data.player[i].cmap[pp_game_data.player[i].state].bottom.point[0].y) / 16;
    }
}

void player_draw_screen(BITMAP * bp, PLAYER * pp)
{
	if(pp->screen.active && pp->screen.bp)
	{
		if(pp->screen.w == pp->screen.bp->w && pp->screen.h == pp->screen.bp->h)
		{
			blit(pp->screen.bp, screen, 0, 0, pp->screen.x, pp->screen.y, pp->screen.bp->w, pp->screen.bp->h);
		}
		else
		{
			stretch_blit(pp->screen.bp, screen, 0, 0, pp->screen.bp->w, pp->screen.bp->h, pp->screen.x, pp->screen.y, pp->screen.w, pp->screen.h);
		}
	}
}
