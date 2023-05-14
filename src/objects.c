#include "includes.h"

int generate_object(int type, fixed x, fixed y)
{
	int i;

	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		if(!pp_game_data.object[i].active)
		{
			pp_game_data.object[i].type = type;
			pp_game_data.object[i].ap = pp_object_ani[pp_game_data.object[i].type];
			pp_game_data.object[i].x = x;
			pp_game_data.object[i].y = y;
			pp_game_data.object[i].active = 1;
			object_initialize(&pp_game_data.object[i]);
			return i;
		}
	}
	return -1;
}

int object_drop(OBJECT * op, int type, fixed ox, fixed oy)
{
	int i;

	switch(type)
	{
		case PP_OBJECT_GENERATOR:
		{
			for(i = 0; i < PP_MAX_OBJECTS; i++)
			{
				if(!pp_game_data.object[i].active)
				{
					pp_game_data.object[i].type = type;
					pp_game_data.object[i].ap = NULL;
					pp_game_data.object[i].x = fixadd(op->x, ox);
					pp_game_data.object[i].y = fixadd(op->y, oy);
					pp_game_data.object[i].active = 1;
					pp_game_data.object[i].flag[PP_GENERATOR_FLAG_OBJECT] = op->type;
					switch(op->type)
					{
						case PP_OBJECT_AMMO_NORMAL:
						case PP_OBJECT_AMMO_X:
						case PP_OBJECT_AMMO_MINE:
						case PP_OBJECT_AMMO_BOUNCE:
						case PP_OBJECT_AMMO_SEEK:
						{
							pp_game_data.object[i].flag[PP_GENERATOR_FLAG_TIME] = pp_option[PP_OPTION_AMMO_TIME];
							break;
						}
						case PP_OBJECT_POWER_CLOAK:
						case PP_OBJECT_POWER_JUMP:
						case PP_OBJECT_POWER_RUN:
						case PP_OBJECT_POWER_DEFLECT:
						case PP_OBJECT_POWER_FLY:
						case PP_OBJECT_POWER_TURBO:
						{
							pp_game_data.object[i].flag[PP_GENERATOR_FLAG_TIME] = pp_option[PP_OPTION_POWER_TIME];
							break;
						}
						default:
						{
							pp_game_data.object[i].flag[PP_GENERATOR_FLAG_TIME] = 600;
							break;
						}
					}
					object_initialize(&pp_game_data.object[i]);
					return i;
				}
			}
			return -1;
		}
		default:
		{
			for(i = 0; i < PP_MAX_OBJECTS; i++)
			{
				if(!pp_game_data.object[i].active)
				{
					pp_game_data.object[i].type = type;
					pp_game_data.object[i].ap = pp_object_ani[pp_game_data.object[i].type];
					pp_game_data.object[i].x = fixadd(op->x, ox);
					pp_game_data.object[i].y = fixadd(op->y, oy);
					pp_game_data.object[i].active = 1;
					object_initialize(&pp_game_data.object[i]);
					return i;
				}
			}
			return -1;
		}
	}
}

/* set the initial state of specified object */
void object_initialize(OBJECT * op)
{
	switch(op->type)
	{
		/* handle target initialization */
		case PP_OBJECT_TARGET:
		{
    		generate_collision_map(&op->cmap, 1, 1, op->ap->w - 2, op->ap->h - 2, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
			update_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
			op->vx = itofix(0);
			op->vy = itofix(0);
			break;
		}

		case PP_OBJECT_POWER_POOF:
		case PP_OBJECT_GEM_POOF:
		case PP_OBJECT_POOF:
		case PP_OBJECT_JET:
		{
			if(op->ap)
			{
		    	generate_collision_map(&op->cmap, 0, 0, op->ap->w, op->ap->h, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
				update_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
			}
			op->flag[PP_POOF_FRAME] = 0;
			break;
		}
		case PP_OBJECT_SPRING_UP:
		case PP_OBJECT_SPRING_DOWN:
		case PP_OBJECT_SPRING_LEFT:
		case PP_OBJECT_SPRING_RIGHT:
		{
    		generate_collision_map(&op->cmap, 0, 0, op->ap->w, op->ap->h, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
			update_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
			break;
		}
		default:
		{
			if(op->ap)
			{
		    	generate_collision_map(&op->cmap, 0, 0, op->ap->w, op->ap->h, pp_game_data.level->tileset->w, pp_game_data.level->tileset->h);
				update_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
			}
			break;
		}
	}
}

void object_friction(OBJECT * op, fixed amount)
{
	if(op->vx > itofix(0))
	{
		op->vx -= amount;
		if(op->vx < 0)
		{
			op->vx = 0;
		}
	}
	else if(op->vx < itofix(0))
	{
		op->vx += amount;
		if(op->vx > 0)
		{
			op->vx = 0;
		}
	}
}

/* handles the logic for specified object */
void object_logic(OBJECT * op)
{
	int i;
	fixed temp;

	if(op->active)
	{
		switch(op->type)
		{

			/* handle stationary targets */
			case PP_OBJECT_TARGET:
			{
				update_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
				return;
			}

			case PP_OBJECT_GENERATOR:
			{
				if(op->flag[PP_GENERATOR_FLAG_TIME])
				{
					op->flag[PP_GENERATOR_FLAG_TIME]--;
					if(op->flag[PP_GENERATOR_FLAG_TIME] <= 0)
					{
						object_drop(op, op->flag[PP_GENERATOR_FLAG_OBJECT], 0, 0);
						ncds_play_sample(pp_sound[PP_SOUND_RESPAWN], 128, -1, -1);
						op->active = 0;
					}
				}
				return;
			}

			case PP_OBJECT_FLAG:
			{
				if(op->flag[PP_FLAG_FLAG_DROP])
				{
					move_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
					if(!tilemap_scrapeb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &op->cmap))
					{
						temp = tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_CONVEY, &op->cmap);
					}
					else
					{
						temp = tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_CONVEY, &op->cmap);
					}
					op->x += op->vx + temp;
					update_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
					if(tilemap_collide(pp_game_data.level->tilemap, &op->cmap))
					{
						op->x = itofix(get_sprite_x(&op->cmap, pp_game_data.level->tileset->w));
						op->vx = -op->vx;
					}
					op->y += op->vy;
					update_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
					if(tilemap_collide(pp_game_data.level->tilemap, &op->cmap))
					{
						op->y = itofix(get_sprite_y(&op->cmap, pp_game_data.level->tileset->h));
						op->vy = itofix(0);
						move_collision_map(&op->cmap, fixtoi(op->x), fixtoi(op->y));
//						op->flag[PP_FLAG_FLAG_DROP] = 0;
					}
					if(tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SOLIDT, &op->cmap))
					{
						if(!tilemap_scrapeb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &op->cmap))
						{
							if(!tilemap_touchb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &op->cmap))
							{
								object_friction(op, ftofix(0.5));
							}
						}
						else
						{
							if(!tilemap_rubb_check(pp_game_data.level->tilemap, pp_game_data.level->tilemap->il, TILEMAP_FLAG_SLICK, &op->cmap))
							{
								object_friction(op, ftofix(0.5));
							}
						}
					}
					op->vy += ftofix(0.5);
					if(op->vy > itofix(7))
					{
						op->vy = itofix(7);
					}
				}
				break;
			}

			case PP_OBJECT_POWER_POOF:
			case PP_OBJECT_GEM_POOF:
			case PP_OBJECT_POOF:
			case PP_OBJECT_JET:
			{
				op->flag[PP_POOF_FRAME]++;
				if(op->flag[PP_POOF_FRAME] >= op->ap->f * op->ap->d)
				{
					op->active = 0;
				}
				break;
			}
			
			case PP_OBJECT_SPRING_UP:
			case PP_OBJECT_SPRING_LEFT:
			case PP_OBJECT_SPRING_RIGHT:
			{
				if(op->flag[PP_POOF_FRAME] > 0)
				{
					op->flag[PP_POOF_FRAME] -= 2;
					if(op->flag[PP_POOF_FRAME] < 0)
					{
						op->flag[PP_POOF_FRAME] = 0;
					}
				}
				else if(op->flag[PP_POOF_FRAME] < 0)
				{
					op->flag[PP_POOF_FRAME] = 14;
				}
				break;
			}
			case PP_OBJECT_SPRING_DOWN:
			{
				if(op->flag[PP_POOF_FRAME] > 0)
				{
					op->flag[PP_POOF_FRAME] -= 2;
					if(op->flag[PP_POOF_FRAME] < 0)
					{
						op->flag[PP_POOF_FRAME] = 0;
					}
				}
				else if(op->flag[PP_POOF_FRAME] < 0)
				{
					op->flag[PP_POOF_FRAME] = 7;
				}
				break;
			}

			default:
			{
				return;
			}

		}
	}
}

void object_draw(BITMAP * bp, OBJECT * op, int ox, int oy)
{
	switch(op->type)
	{
		case PP_OBJECT_POWER_POOF:
		case PP_OBJECT_GEM_POOF:
		case PP_OBJECT_POOF:
		case PP_OBJECT_JET:
		{
			draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy, op->flag[PP_POOF_FRAME]);
			break;
		}
		case PP_OBJECT_SPRING_UP:
		{
			draw_ani(bp, pp_object_ani[PP_OBJECT_SPRING_BAR_V], fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			if(op->flag[PP_POOF_FRAME] > 0)
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy - op->flag[PP_POOF_FRAME], pp_game_data.frame);
			}
			else
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			}
 			break;
		}
		case PP_OBJECT_SPRING_DOWN:
		{
			draw_ani(bp, pp_object_ani[PP_OBJECT_SPRING_BAR_V], fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			if(op->flag[PP_POOF_FRAME] > 0)
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy + op->flag[PP_POOF_FRAME], pp_game_data.frame);
			}
			else
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			}
			break;
		}
		case PP_OBJECT_SPRING_LEFT:
		{
			draw_ani(bp, pp_object_ani[PP_OBJECT_SPRING_BAR_H], fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			if(op->flag[PP_POOF_FRAME] > 0)
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox - op->flag[PP_POOF_FRAME], fixtoi(op->y) + oy, pp_game_data.frame);
			}
			else
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			}
			break;
		}
		case PP_OBJECT_SPRING_RIGHT:
		{
			draw_ani(bp, pp_object_ani[PP_OBJECT_SPRING_BAR_H], fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			if(op->flag[PP_POOF_FRAME] > 0)
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox + op->flag[PP_POOF_FRAME], fixtoi(op->y) + oy, pp_game_data.frame);
			}
			else
			{
				draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			}
			break;
		}
		default:
		{
			draw_ani(bp, op->ap, fixtoi(op->x) + ox, fixtoi(op->y) + oy, pp_game_data.frame);
			break;
		}
	}
}
