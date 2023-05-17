#ifndef PAINTBALL_H
#define PAINTBALL_H

#include "animate.h"
#include "collide.h"

#define MAX_PAINTBALLS    256
#define AMMO_TYPE_NORMAL    0
#define AMMO_TYPE_EXPLOSIVE 1
#define AMMO_TYPE_MINE      2
#define AMMO_TYPE_BOUNCE    3
#define AMMO_TYPE_SEEK      4

typedef struct
{

    ANIMATION * ap;
    COLLISION_MAP cmap;
    fixed fx, fy, vx, vy;
    int x, y, who, dwho, type, misc, who_time, dtime, frame, iced;
    int active, split;

	/* seeker data */
    int target;
    fixed angle;

} PAINTBALL;

PAINTBALL *create_paintball(int color, int x, int y, int angle, int type);
void generate_paintball(PLAYER * pp);
void reset_paintballs(void);
void paintball_logic(PAINTBALL * pp);
void paintball_remove(int color);

#endif
