#ifndef PARTICLE_H
#define PARTICLE_H

#include "paintbal.h"

#define MAX_PARTICLES 256

typedef struct
{

    fixed fx, fy, vx, vy;
    int x, y, color;
    int life, active;
    ANIMATION * ap;

} PARTICLE;

void particle_logic(PARTICLE * pp);
void generate_particle(int x, int y, float vx, float vy, int color);
void generate_paintball_splat_particles(PAINTBALL * pp);
void reset_particles(void);

#endif
