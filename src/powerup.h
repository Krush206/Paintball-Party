#ifndef POWERUP_H
#define POWERUP_H

#include "animate.h"
#include "collide.h"

#define MAX_POWERUPS   32

#define POWERUP_TYPES   4
#define POWERUP_AMMO    0
#define POWERUP_RUN     1
#define POWERUP_JUMP    2
#define POWERUP_TRIPLE  3

/* a powerup */
typedef struct
{

    ANIMATION * ap;
    COLLISION_MAP cm;

    fixed fx, fy, vx, vy;
    int x, y, cx, cy;
    int active, type, frame;

} POWERUP;

/* invisible sprite that spawns powerups */
typedef struct
{

    int active, time, frame, type;
    int x, y;

} POWERUP_SPAWNER;

void generate_powerup(int i, int x, int y, int type);
void spawn_powerups(void);

#endif
