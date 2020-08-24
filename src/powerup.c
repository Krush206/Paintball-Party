#include <allegro.h>
#include "animate.h"
#include "powerup.h"

extern ANIMATION *     powerup_ani[POWERUP_TYPES];
extern POWERUP         powerup[MAX_POWERUPS];
extern POWERUP_SPAWNER powerup_spawner[MAX_POWERUPS];

void generate_powerup(int i, int x, int y, int type)
{
    if(!powerup[i].active)
    {
        powerup[i].ap = powerup_ani[type];
        powerup[i].type = type;
        powerup[i].x = x;
        powerup[i].y = y;
        powerup[i].fx = itofix(x);
        powerup[i].fy = itofix(y);
        powerup[i].vx = itofix(0);
        powerup[i].vy = itofix(0);
        powerup[i].active = 1;
    }
}

void spawn_powerups(void)
{
    int i;

    for(i = 0; i < MAX_POWERUPS; i++)
    {
        if(powerup_spawner[i].active)
        {
            if(!powerup[i].active)
            {
                powerup_spawner[i].frame++;
                if(powerup_spawner[i].frame >= powerup_spawner[i].time)
                {
                    generate_powerup(i, powerup_spawner[i].x, powerup_spawner[i].y, powerup_spawner[i].type);
                    powerup_spawner[i].frame = 0;
                }
            }
        }
    }
}
