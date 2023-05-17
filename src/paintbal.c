#include <math.h>
#include "includes.h"

#define PAINTBALL_WHO_TIME 4

PAINTBALL *create_paintball(int color, int x, int y, int angle, int type)
{
	int i;

	for(i = 0; i < MAX_PAINTBALLS; i++)
	{
		if(!pp_game_data.paintball[i].active)
		{
			if(type == AMMO_TYPE_SEEK)
			{
				pp_game_data.paintball[i].frame = 0;
            	pp_game_data.paintball[i].ap = pp_game_data.player[color].character.pap;
            	pp_game_data.paintball[i].x = x;
            	pp_game_data.paintball[i].y = y;
            	pp_game_data.paintball[i].fx = itofix(pp_game_data.paintball[i].x);
            	pp_game_data.paintball[i].fy = itofix(pp_game_data.paintball[i].y);
            	pp_game_data.paintball[i].angle = itofix(angle);
            	pp_game_data.paintball[i].vx = fixmul(cosf(pp_game_data.paintball[i].angle), itofix(8));
            	pp_game_data.paintball[i].vy = fixmul(sinf(pp_game_data.paintball[i].angle), itofix(8));
            	pp_game_data.paintball[i].who = color;
            	if(angle % 64 == 0)
            	{
            		pp_game_data.paintball[i].who_time = PAINTBALL_WHO_TIME;
            	}
            	else
            	{
            		pp_game_data.paintball[i].who_time = 0;
            	}
            	pp_game_data.paintball[i].dwho = -1;
            	pp_game_data.paintball[i].dtime = 0;
            	pp_game_data.paintball[i].type = type;
            	pp_game_data.paintball[i].misc = 0;
            	if(pp_game_data.mode == PP_MODE_TARGET)
            	{
	            	if(pp_game_data.player[color].target < 0)
	            	{
	            		pp_game_data.paintball[i].target = -1;
	            	}
	            	else if(pp_game_data.object[pp_game_data.player[color].target].active)
	            	{
	            		pp_game_data.paintball[i].target = pp_game_data.player[color].target;
            		}
            	}
            	else
            	{
	            	if(pp_game_data.player[color].target < 0)
	            	{
		            	pp_game_data.paintball[i].target = -1;
	            	}
	            	else if(pp_game_data.player[pp_game_data.player[color].target].active && !pp_game_data.player[pp_game_data.player[color].target].tele_in && !pp_game_data.player[pp_game_data.player[color].target].tele_out && !pp_game_data.player[pp_game_data.player[color].target].cloak_time)
					{
	            		pp_game_data.paintball[i].target = pp_game_data.player[color].target;
            		}
            		else
            		{
	            		pp_game_data.paintball[i].target = -1;
            		}
            	}
//				generate_collision_map(&pp_game_data.paintball[i].cmap, 1, 1, pp_game_data.player[color].pap->w - 2, pp_game_data.player[color].pap->h - 2, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
				generate_collision_map(&pp_game_data.paintball[i].cmap, pp_game_data.player[color].character.pf, pp_game_data.player[color].character.pf, pp_game_data.player[color].character.pap->w - pp_game_data.player[color].character.pf * 2, pp_game_data.player[color].character.pap->h - pp_game_data.player[color].character.pf * 2, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
            	update_collision_map(&pp_game_data.paintball[i].cmap, pp_game_data.paintball[i].x, pp_game_data.paintball[i].y);
            	update_collision_map(&pp_game_data.paintball[i].cmap, pp_game_data.paintball[i].x, pp_game_data.paintball[i].y);
            	pp_game_data.paintball[i].active = 1;
			}
			else
			{
            	pp_game_data.paintball[i].ap = pp_game_data.player[color].character.pap;
            	pp_game_data.paintball[i].x = x;
            	pp_game_data.paintball[i].y = y;
            	pp_game_data.paintball[i].fx = itofix(pp_game_data.paintball[i].x);
            	pp_game_data.paintball[i].fy = itofix(pp_game_data.paintball[i].y);
            	pp_game_data.paintball[i].angle = itofix(angle);
            	pp_game_data.paintball[i].vx = fixmul(cosf(pp_game_data.paintball[i].angle), itofix(8));
            	pp_game_data.paintball[i].vy = fixmul(sinf(pp_game_data.paintball[i].angle), itofix(8));
            	pp_game_data.paintball[i].who = color;
            	if(angle % 64 == 0)
            	{
            		pp_game_data.paintball[i].who_time = PAINTBALL_WHO_TIME;
            	}
            	else
            	{
            		pp_game_data.paintball[i].who_time = 0;
            	}
            	pp_game_data.paintball[i].dwho = -1;
            	pp_game_data.paintball[i].dtime = 0;
            	pp_game_data.paintball[i].type = type;
            	pp_game_data.paintball[i].misc = 0;
//				generate_collision_map(&pp_game_data.paintball[i].cmap, 1, 1, pp_game_data.player[color].pap->w - 2, pp_game_data.player[color].pap->h - 2, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
				generate_collision_map(&pp_game_data.paintball[i].cmap, pp_game_data.player[color].character.pf, pp_game_data.player[color].character.pf, pp_game_data.player[color].character.pap->w - pp_game_data.player[color].character.pf * 2, pp_game_data.player[color].character.pap->h - pp_game_data.player[color].character.pf * 2, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
            	update_collision_map(&pp_game_data.paintball[i].cmap, pp_game_data.paintball[i].x, pp_game_data.paintball[i].y);
            	update_collision_map(&pp_game_data.paintball[i].cmap, pp_game_data.paintball[i].x, pp_game_data.paintball[i].y);
            	pp_game_data.paintball[i].active = 1;
            }
            return &pp_game_data.paintball[i];
		}
	}
}

void generate_paintball(PLAYER * pp)
{
    int i, j;

	if(!pp->gun_flash.active)
	{
		pp->gun_flash.ap = pp->character.gap[g_table[pp->state]];
        pp->gun_flash.x = pp->x + pp->character.gx_table[pp->state];
        pp->gun_flash.y = pp->y + pp->character.gy_table[pp->state];
		pp->gun_flash.active = 2;
		pp->gun_flash.frame = rand();
	}
}

void generate_paintball_splat(PAINTBALL * pp)
{
	PAINTBALL *pp_split;

	generate_paintball_splat_particles(pp);
	if(pp->type == AMMO_TYPE_EXPLOSIVE)
	{
		pp_split = create_paintball(pp->who, pp->x, pp->y, fixtoi(pp->angle) + 96, AMMO_TYPE_NORMAL);
		pp_split->split = 1;
		pp_split = create_paintball(pp->who, pp->x, pp->y, fixtoi(pp->angle) + 160, AMMO_TYPE_NORMAL);
		pp_split->split = 1;
	}
	pp->active = 0;
}

void reset_paintballs(void)
{
	int i;

	for(i = 0; i < MAX_PAINTBALLS; i++)
	{
		pp_game_data.paintball[i].active = pp_game_data.paintball[i].split = 0;
	}
}

void paintball_do_hit(PAINTBALL * pp, int i)
{
	int j;

	/* if player has deflect, bounce off of him */
	if(pp_game_data.player[i].deflect_time)
	{
		if(pp->dwho != i)
		{
			/* set paintball at edge of player it hits */
			if(!(pp->type == AMMO_TYPE_MINE && pp->misc))
			{
				pp->x = get_sprite_edge_x(&pp->cmap, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
				pp->y = get_sprite_edge_y(&pp->cmap, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
				pp->fx = itofix(pp->x);
				pp->fy = itofix(pp->y);
				update_collision_map(&pp->cmap, pp->x, pp->y);
			}

			switch(pp->type)
			{
				case AMMO_TYPE_NORMAL:
				case AMMO_TYPE_EXPLOSIVE:
				{
					if(fixtoi(pp->angle) % 64 == 0)
					{
						pp->angle += itofix(128);
					}
					else
					{
						pp->angle = itofix(128) - pp->angle;
					}
	            	pp->vx = fixmul(cosf(pp->angle), itofix(8));
    	        	pp->vy = fixmul(sinf(pp->angle), itofix(8));
					pp->dwho = i;
					ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_RICOCHET], 128, -1, -1);
					break;
				}
				case AMMO_TYPE_BOUNCE:
				case AMMO_TYPE_MINE:
				{
					if(fixtoi(pp->angle) % 64 == 0)
					{
						if(pp->angle == itofix(64) || pp->angle == itofix(192))
						{
							if(pp->vy > 0)
							{
								pp->angle = itofix(192);
							}
							else
							{
								pp->angle = itofix(64);
							}
						}
						else
						{
							pp->angle += itofix(128);
						}
					}
					else
					{
						pp->angle = itofix(128) - pp->angle;
					}
	            	pp->vx = fixmul(cosf(pp->angle), itofix(8));
    	        	pp->vy = fixmul(sinf(pp->angle), itofix(8));
					pp->dwho = i;
					ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_RICOCHET], 128, -1, -1);
					break;
				}
				case AMMO_TYPE_SEEK:
				{
					if(i != pp->who)
					{
						pp->target = pp->who;
					}
					if(fixtoi(pp->angle) % 64 == 0)
					{
						pp->angle += itofix(128);
					}
					else
					{
						pp->angle = itofix(128) - pp->angle;
					}
	            	pp->vx = fixmul(cosf(pp->angle), itofix(8));
   	        		pp->vy = fixmul(sinf(pp->angle), itofix(8));
					pp->dtime = 2;
					pp->dwho = i;
					ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_RICOCHET], 128, -1, -1);
					break;
				}
			}
		}

		/* splat the paintball if it hits player it just bounced off of */
		else
		{
			generate_paintball_splat(pp);
			ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_SPLAT], 128, 1000 + rand() % 100 - 50, -1);
		}
	}

	/* if isn't still flashing, give points appropriately */
	else if(pp_game_data.player[i].flash_time <= 0 && pp_game_data.winner < 0)
	{
		/* set paintball at edge of player it hits */
		pp->x = get_sprite_edge_x(&pp->cmap, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
		pp->y = get_sprite_edge_y(&pp->cmap, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
		pp->fx = itofix(pp->x);
		pp->fy = itofix(pp->y);
		update_collision_map(&pp->cmap, pp->x, pp->y);

		generate_paintball_splat(pp);
		ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_SPLAT], 128, 1000 + rand() % 100 - 50, -1);
//		ncds_play_sample(&ncds_channels[0], pp_game_data.player[i].sound[PP_SOUND_HIT], 128, 22050 + rand() % 2000 - 1000);
		ncds_play_sample(pp_game_data.player[i].sound[PP_SOUND_HIT], 128, -1, -1);
		pp_game_data.player[i].flash_time = 15;
		if(pp_game_data.mode == PP_MODE_SPLAT)
		{
			if(pp_players_left())
			{
				if(pp->who == i)
				{
					pp_game_data.player[pp->who].score--;
				}
				else
				{
					pp_game_data.player[pp->who].score++;
				}
			}
		}
		else if(pp_game_data.mode == PP_MODE_TAG)
		{
			pp_game_data.player[pp->who].it = 0;
			pp_game_data.player[i].it = 1;
		}
		else if(pp_game_data.mode == PP_MODE_FLAG)
		{
			if(pp_game_data.player[i].flag)
			{
				pp_game_data.flag_sprite = generate_object(PP_OBJECT_FLAG, pp_game_data.player[i].fx + itofix(pp_game_data.player[i].cmap[pp_game_data.player[i].state].rx + pp_game_data.player[i].cmap[pp_game_data.player[i].state].w / 2 - pp_object_ani[PP_OBJECT_FLAG]->w / 2), pp_game_data.player[i].fy + itofix(pp_game_data.player[i].cmap[pp_game_data.player[i].state].ry + pp_game_data.player[i].cmap[pp_game_data.player[i].state].h / 2 - pp_object_ani[PP_OBJECT_FLAG]->h / 2));
				if(pp_game_data.flag_sprite >= 0)
				{
					if(pp_game_data.object[pp_game_data.flag_sprite].y + itofix(pp_object_ani[PP_OBJECT_FLAG]->h) > pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.ap[pp_game_data.player[i].state]->h))
					{
						pp_game_data.object[pp_game_data.flag_sprite].y = pp_game_data.player[i].fy + itofix(pp_game_data.player[i].character.ap[pp_game_data.player[i].state]->h) - itofix(pp_object_ani[PP_OBJECT_FLAG]->h);
					}
					if(pp_game_data.player[i].fx < pp_game_data.player[pp->who].fx)
					{
						pp_game_data.object[pp_game_data.flag_sprite].flag[PP_FLAG_FLAG_DROP] = 1;
						pp_game_data.object[pp_game_data.flag_sprite].vx = itofix(4);
						pp_game_data.object[pp_game_data.flag_sprite].vy = itofix(0);
					}
					else
					{
						pp_game_data.object[pp_game_data.flag_sprite].flag[PP_FLAG_FLAG_DROP] = 1;
						pp_game_data.object[pp_game_data.flag_sprite].vx = itofix(-4);
						pp_game_data.object[pp_game_data.flag_sprite].vy = itofix(0);
					}
				}
				pp_game_data.player[i].flag = 0;
				pp_game_data.player[i].shock_time = 60;
			}
		}
		else if(pp_game_data.mode == PP_MODE_HUNTER)
		{
			pp_game_data.player[i].hits++;
		}
	}

	/* otherwise, just splat the paintball */
	else
	{
		/* set paintball at edge of player it hits */
		pp->x = get_sprite_edge_x(&pp->cmap, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
		pp->y = get_sprite_edge_y(&pp->cmap, &pp_game_data.player[i].cmap[pp_game_data.player[i].state]);
		pp->fx = itofix(pp->x);
		pp->fy = itofix(pp->y);
		update_collision_map(&pp->cmap, pp->x, pp->y);

		generate_paintball_splat(pp);
		ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_SPLAT], 128, 1000 + rand() % 100 - 50, -1);
	}
}

/* handle movement according to ammo type */
void paintball_move(PAINTBALL * pp)
{
	fixed target_angle = -itofix(9876);
	fixed temp;
	int ox, oy;
	static int rev_vx, rev_vy, up;

	if(pp_game_data.player[pp->who].gun_flash.active)
		switch(pp_game_data.player[pp->who].state)
		{
			case PS_STAND_RIGHT:
			case PS_WALK_RIGHT:
			case PS_JUMP_RIGHT:
			case PS_DUCK_RIGHT:
			case PS_FALL_RIGHT:
			case PS_FFALL_RIGHT:
			case PS_FWALK_RIGHT:
			case PS_FJUMP_RIGHT:
			case PS_FDUCK_RIGHT:
			case PS_FSTAND_RIGHT:
				rev_vx = -(rev_vy = pp->vx = itofix(10)), up = 0;
				break;
			case PS_STAND_LEFT:
			case PS_WALK_LEFT:
			case PS_JUMP_LEFT:
			case PS_DUCK_LEFT:
			case PS_FALL_LEFT:
			case PS_FFALL_LEFT:
			case PS_FWALK_LEFT:
			case PS_FJUMP_LEFT:
			case PS_FDUCK_LEFT:
			case PS_FSTAND_LEFT:
				rev_vx = -(rev_vy = pp->vx = -itofix(10)), up = 0;
				break;
			case PS_STAND_UP_RIGHT:
			case PS_STAND_UP_LEFT:
			case PS_FSTAND_UP_RIGHT:
			case PS_FSTAND_UP_LEFT:
				rev_vx = rev_vy = -(pp->vy = -itofix(10)), up = 1;
		}

	switch(pp->type)
	{
		case AMMO_TYPE_NORMAL:
		case AMMO_TYPE_EXPLOSIVE:
		{
			if(pp->split)
			{
				if(up) rev_vx = -(pp->vx = rev_vx), pp->vy = rev_vy;
				else pp->vx = rev_vx, rev_vy = -(pp->vy = rev_vy);

				pp->split = 0;
			}

			pp->fx = fixadd(pp->fx, pp->vx);
			pp->fy = fixadd(pp->fy, pp->vy);
			pp->x = fixtoi(pp->fx);
			pp->y = fixtoi(pp->fy);
			break;
		}
		case AMMO_TYPE_BOUNCE:
		{
			pp->fy = fixadd(pp->fy, pp->vy);
			pp->vy = fixadd(pp->vy, ftofix(0.5));
			pp->x = fixtoi(pp->fx);
			pp->y = fixtoi(pp->fy);
			update_collision_map(&pp->cmap, pp->x, pp->y);
			if(tilemap_collide(pp_game_data.level->tilemap, &pp->cmap))
			{
				if(pp->vx != itofix(0))
				{
					pp->y = get_sprite_y(&pp->cmap, pp_game_data.level->tileset->h);
					pp->fy = itofix(pp->y);
					pp->vy = itofix(-3);
					ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_BOUNCE], 128, -1, -1);
				}
				else
				{
					pp->active = 0;
					generate_paintball_splat(pp);
					ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_SPLAT], 128, 1000 + rand() % 100 - 50, -1);
				}
			}
			pp->fx = fixadd(pp->fx, pp->vx);
			if(pp->vy > itofix(7))
			{
				pp->vy = itofix(7);
			}
			pp->x = fixtoi(pp->fx);
			pp->y = fixtoi(pp->fy);
			break;
		}
		case AMMO_TYPE_MINE:
		{
			ox = fixtoi(pp->fx);
			oy = fixtoi(pp->fy);
			
			pp->x = fixtoi(pp->fx);
			pp->y = fixtoi(pp->fy);
			move_collision_map(&pp->cmap, pp->x, pp->y);
			if(!tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap) && !tilemap_rubt_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDB, &pp->cmap) && !tilemap_rubl_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDR, &pp->cmap) && !tilemap_rubr_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDL, &pp->cmap))
			{
				pp->misc = 0;
			}
			pp->fy = fixadd(pp->fy, pp->vy);
			if(pp->misc != 1)
			{
				pp->vy = fixadd(pp->vy, ftofix(0.5));
				if(pp->vy > itofix(0) && pp->angle >= itofix(192))
				{
					pp->angle -= itofix(128);
				}
			}
			pp->x = fixtoi(pp->fx);
			pp->y = fixtoi(pp->fy);
			update_collision_map(&pp->cmap, pp->x, pp->y);
			if(tilemap_collide(pp_game_data.level->tilemap, &pp->cmap))
			{
				pp->y = get_sprite_y(&pp->cmap, pp_game_data.level->tileset->h);
				pp->fy = itofix(pp->y);
				pp->vy = itofix(0);
				move_collision_map(&pp->cmap, pp->x, pp->y);
				oy = pp->y;
				pp->misc = 1;
			}
			if(pp->misc)
			{
				if(tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap))
				{
					if(!tilemap_scrapeb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &pp->cmap))
					{
						if(!tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &pp->cmap))
						{
							if(tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap))
							{
								pp->vy = itofix(0);
								pp->vx = itofix(0);
							}
						}
						else
						{
							pp->vy = itofix(0);
							pp->misc = 2;
						}
					}
					else
					{
						if(!tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &pp->cmap))
						{
							if(tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap))
							{
								pp->vy = itofix(0);
								pp->vx = itofix(0);
							}
						}
						else
						{
							pp->vy = itofix(0);
							pp->misc = 2;
						}
					}
				}
			}
			if(pp->vy > itofix(7))
			{
				pp->vy = itofix(7);
			}
			pp->fx = fixadd(pp->fx, pp->vx);
			pp->x = fixtoi(pp->fx);
			pp->y = fixtoi(pp->fy);
			if(pp->misc)
			{
				move_collision_map(&pp->cmap, pp->x, pp->y);
				if(!tilemap_scrapeb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap))
				{
					temp = tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_CONVEY, &pp->cmap);
				}
				else
				{
					temp = tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_CONVEY, &pp->cmap);
				}
				pp->fx = fixadd(pp->fx, temp);
				pp->x = fixtoi(pp->fx);
//				update_collision_map(&pp->cmap, pp->x, pp->y);
				move_collision_map(&pp->cmap, pp->x, pp->y);
				if(temp != itofix(0) && !tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &pp->cmap))
				{
					pp->misc = 0;
				}
			}
			update_collision_map(&pp->cmap, ox, pp->y);
//			update_collision_map(&pp->cmap, pp->x, pp->y);
			break;
		}
		case AMMO_TYPE_SEEK:
		{
			if(pp->target >= 0)
			{
				if(pp_game_data.mode == PP_MODE_TARGET)
				{
					if(pp_game_data.object[pp->target].active && !pp->who_time)
					{
						target_angle = atan2f(fixsub(pp_game_data.object[pp->target].y + itofix(pp_game_data.object[pp->target].ap->h / 2), pp->fy + itofix(pp_game_data.player[pp->who].character.pap->h / 2)), fixsub(pp_game_data.object[pp->target].x + itofix(pp_game_data.object[pp->target].ap->w / 2), pp->fx + itofix(pp_game_data.player[pp->who].character.pap->w / 2)));
					}
				}
				else
				{
					if(pp_game_data.player[pp->target].active && !pp_game_data.player[pp->target].tele_in && !pp_game_data.player[pp->target].tele_out && !pp->who_time && !pp_game_data.player[pp->target].cloak_time)
					{
						target_angle = atan2f(fixsub(pp_game_data.player[pp->target].fy + itofix(pp_game_data.player[pp->target].character.ap[pp_game_data.player[pp->target].state]->h / 2), pp->fy + itofix(pp_game_data.player[pp->who].character.pap->h / 2)), fixsub(pp_game_data.player[pp->target].fx + itofix(pp_game_data.player[pp->target].character.ap[pp_game_data.player[pp->target].state]->w / 2), pp->fx + itofix(pp_game_data.player[pp->who].character.pap->w / 2)));
					}
				}
				if(target_angle != -itofix(9876))
				{
					if(target_angle - pp->angle <= itofix(4) && target_angle - pp->angle >= itofix(-4))
					{
						pp->angle = target_angle;
					}
					else
					{
						if (((pp->angle - target_angle) & 0xFFFFFF) < itofix(128))
						{
		           			pp->angle = (pp->angle - itofix(4)) & 0xFFFFFF;
           				}
       					else
       					{
		           			pp->angle = (pp->angle + itofix(4)) & 0xFFFFFF;
           				}
       				}
					pp->vx = fixmul(itofix(4), cosf(pp->angle));
					pp->vy = fixmul(itofix(4), sinf(pp->angle));
       			}
				pp->fx = fixadd(pp->fx, pp->vx);
				pp->fy = fixadd(pp->fy, pp->vy);
				pp->x = fixtoi(pp->fx);
				pp->y = fixtoi(pp->fy);
			}
			else
			{
				pp->fx = fixadd(pp->fx, pp->vx);
				pp->fy = fixadd(pp->fy, pp->vy);
				pp->x = fixtoi(pp->fx);
				pp->y = fixtoi(pp->fy);
			}
			break;
		}
	}
}

void generate_new_target(void)
{
}

void paintball_collide(PAINTBALL * pp)
{
	int i, who = -1;

	switch(pp->type)
	{
		case AMMO_TYPE_NORMAL:
		case AMMO_TYPE_SEEK:
		case AMMO_TYPE_EXPLOSIVE:
		case AMMO_TYPE_BOUNCE:
		{
			update_collision_map(&pp->cmap, pp->x, pp->y);
			if(tilemap_collide(pp_game_data.level->tilemap, &pp->cmap))
			{
   				pp->x = get_sprite_x(&pp->cmap, pp_game_data.level->tileset->w);
				pp->y = get_sprite_y(&pp->cmap, pp_game_data.level->tileset->h);
   				generate_paintball_splat(pp);
   				ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_SPLAT], 128, 1000 + rand() % 100 - 50, -1);
			}
			break;
		}
		case AMMO_TYPE_MINE:
		{
			update_collision_map(&pp->cmap, pp->x, pp->y);
			if(tilemap_collide(pp_game_data.level->tilemap, &pp->cmap))
			{
				pp->x = get_sprite_x(&pp->cmap, pp_game_data.level->tileset->w);
				pp->y = get_sprite_y(&pp->cmap, pp_game_data.level->tileset->h);
				pp->fx = itofix(pp->x);
				pp->fy = itofix(pp->y);
				pp->vx = itofix(0);
				pp->vy = itofix(0);
				pp->misc = 1;
			}
			break;
		}
	}

	/* see if paintball is hitting player */
	for(i = 0; i < 4; i++)
	{
		if(pp_game_data.player[i].active && !pp_game_data.player[i].tele_in && !pp_game_data.player[i].tele_out)
		{
			if(collide(&pp_game_data.player[i].cmap[pp_game_data.player[i].state], &pp->cmap) && !(i == pp->who && pp->who_time && pp->dwho == -1))
			{
				paintball_do_hit(pp, i);
				pp_player_data[pp_game_data.player[pp->who].data_pick].hit++;
				break;
			}
		}
	}

	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		if(pp_game_data.object[i].active && pp_game_data.object[i].type == PP_OBJECT_TARGET)
		{
			if(collide(&pp_game_data.object[i].cmap, &pp->cmap))
			{
				pp->x = get_sprite_edge_x(&pp->cmap, &pp_game_data.object[i].cmap);
				pp->y = get_sprite_edge_y(&pp->cmap, &pp_game_data.object[i].cmap);
				pp->fx = itofix(pp->x);
				pp->fy = itofix(pp->y);
				update_collision_map(&pp->cmap, pp->x, pp->y);
				generate_paintball_splat(pp);
				ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_SPLAT], 128, 1000 + rand() % 100 - 50, -1);
				pp_game_data.object[i].active = 0;
				
				/* drop new target */
				who = pp_game_data.object[i].flag[PP_TARGET_FLAG_WHO];
				while(who == pp_game_data.object[i].flag[PP_TARGET_FLAG_WHO])
				{
					who = pp_game_data.target_list[rand() % pp_game_data.targets];
				}
				pp_game_data.target_sprite = object_drop(&pp_game_data.object[who], PP_OBJECT_TARGET, 0, 0);
				if(pp_game_data.target_sprite >= 0)
				{
					pp_game_data.object[pp_game_data.target_sprite].flag[PP_TARGET_FLAG_WHO] = who;
					pp_game_data.player[pp->who].target = pp_game_data.target_sprite;
				}
				
				pp_game_data.player[pp->who].score++;
				pp_game_data.time += 60;
				pp_player_data[pp_game_data.player[pp->who].data_pick].hit++;
				if(pp_game_data.player[pp->who].weapon == PP_AMMO_TYPE_SEEK)
				{
					ncds_play_sample(pp_game_data.player[pp->who].sound[PP_SOUND_TARGET], 128, -1, -1);
				}
				break;
			}
		}
//		else if(pp_game_data.object[i].active && pp_game_data.object[i].type == PP_OBJECT_SPRING_UP)
//		{
//			if(collide_bottom(&pp->cmap, &pp_game_data.object[i].cmap))
//			if(collide(&pp->cmap, &pp_game_data.object[i].cmap))
//			{
//				pp->y = fixtoi(pp_game_data.object[i].y) - pp->ap->h;
//				pp->fy = itofix(pp->y);
//				pp->vx = itofix(0);
//				pp->vy = itofix(-14);
//				pp->angle = itofix(-64);
  //      		update_collision_map(&pp->cmap, pp->x, pp->y);
//	       		pp_game_data.object[i].flag[PP_POOF_FRAME] = -1;
//				ncds_play_sample(&ncds_channels[3], pp_game_data.player[pp->who].sound[PP_SOUND_SPRING], 128, -1);
//			}
//		}
	}
}

void paintball_logic(PAINTBALL * pp)
{
	if(pp->active)
	{
		if(pp->who_time)
		{
			pp->who_time--;
		}
		if(pp->dtime)
		{
			pp->dtime--;
		}

		paintball_move(pp);
		paintball_collide(pp);
		pp->frame++;
	}
}

void paintball_remove(int color)
{
	int i, s = 0;

	for(i = 0; i < MAX_PAINTBALLS; i++)
	{
		if(pp_game_data.paintball[i].active)
		{
			if(pp_game_data.paintball[i].who == color)
			{
				generate_paintball_splat(&pp_game_data.paintball[i]);
				s = 1;
			}
		}
	}
	if(s)
	{
		ncds_play_sample(pp_game_data.player[color].sound[PP_SOUND_SPLAT], 128, 1000 + rand() % 100 - 50, -1);
	}
}
