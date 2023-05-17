#include <math.h>
#include "includes.h"

void particle_logic(PARTICLE * pp)
{
	int i;

    if(pp->active)
    {
        pp->fx = fixadd(pp->fx, pp->vx);
        pp->fy = fixadd(pp->fy, pp->vy);
        pp->vy = fixadd(pp->vy, ftofix(0.5));
        if(pp->vy > itofix(7))
        {
            pp->vy = itofix(7);
        }
        pp->life--;
        if(pp->life <= 0)
        {
            pp->active = 0;
        }
        pp->x = fixtoi(pp->fx);
        pp->y = fixtoi(pp->fy);
    }
}

void generate_particle(int x, int y, float vx, float vy, int color)
{
    int i;

    for(i = 0; i < MAX_PARTICLES; i++)
    {
        if(!pp_game_data.particle[i].active)
        {
            pp_game_data.particle[i].fx = itofix(x);
            pp_game_data.particle[i].fy = itofix(y);
            pp_game_data.particle[i].vx = ftofix(vx);
            pp_game_data.particle[i].vy = ftofix(vy);
            pp_game_data.particle[i].x = x;
            pp_game_data.particle[i].y = y;
            pp_game_data.particle[i].color = color;
            pp_game_data.particle[i].life = 15 + (rand() % 8) * 2;
            pp_game_data.particle[i].ap = pp_game_data.player[color].character.ppap;
            pp_game_data.particle[i].active = 1;
            break;
        }
    }
}

void generate_paintball_splat_particles(PAINTBALL * pp)
{
	int i;
	int angle;
	float length;

	for(i = 0; i < (pp_game_data.player[pp->who].character.pap->w * pp_game_data.player[pp->who].character.pap->h) / pp_game_data.player[pp->who].character.ppap->w; i++)
	{
		angle = rand() % 256;
		length = rand() % (pp_game_data.player[pp->who].character.pap->w / 2);
		generate_particle(pp->x + pp_game_data.player[pp->who].character.pap->w / 2, pp->y + pp_game_data.player[pp->who].character.pap->h / 2, fixtof(fixmul(ftofix(length), cosf(itofix(angle)))), fixtof(fixmul(ftofix(length), sinf(itofix(angle))) - itofix(1)), pp->who);
	}
}

void reset_particles(void)
{
	int i;

	for(i = 0; i < MAX_PARTICLES; i++)
	{
		pp_game_data.particle[i].active = 0;
	}
}
