#ifndef COLLIDE_H
#define COLLIDE_H

#include <allegro.h>

#ifdef __cplusplus
extern "C"{
#endif

#define CR_LEFT  0
#define CR_RIGHT 1
#define CR_UP    2
#define CR_DOWN  3

typedef struct
{
    int x;
    int y;
} COLLISION_POINT;

typedef struct
{
    COLLISION_POINT point[32];
    int point_count, fluff_start;
    int hit, nudge;
} COLLISION_LIST;


/* this is what the user will generally be concerned with */
typedef struct
{

    /* the collision edges */
    COLLISION_LIST top;
    COLLISION_LIST bottom;
    COLLISION_LIST left;
    COLLISION_LIST right;
    int tnudge, bnudge, lnudge, rnudge;

    /* the current absolute position of the rectangle */
    int x, y, w, h;
    fixed fx, fy, fox, foy;

    /* the old position */
    int ox, oy;

    /* position relative to upper left corner of sprite */
    int rx, ry;

    int dir;

} COLLISION_MAP;

int cmap_h_dir(COLLISION_MAP * cp);
int cmap_v_dir(COLLISION_MAP * cp);
int detect_collision(int x1, int y1, BITMAP * sprite1, int x2, int y2, BITMAP * sprite2);
int collide(COLLISION_MAP * mp1, COLLISION_MAP * mp2);
int collide_top(COLLISION_MAP * mp1, COLLISION_MAP * mp2);
int collide_bottom(COLLISION_MAP * mp1, COLLISION_MAP * mp2);
int collide_left(COLLISION_MAP * mp1, COLLISION_MAP * mp2);
int collide_right(COLLISION_MAP * mp1, COLLISION_MAP * mp2);
int point_collide(int x, int y, COLLISION_MAP * rp);
void generate_collision_map(COLLISION_MAP * rp, int x, int y, int w, int h, int tw, int th);
void update_collision_map(COLLISION_MAP * rp, int x, int y);
void fupdate_collision_map(COLLISION_MAP * rp, fixed x, fixed y);
void move_collision_map(COLLISION_MAP * rp, int x, int y);
void fmove_collision_map(COLLISION_MAP * rp, fixed x, fixed y);
int get_sprite_left_edge(COLLISION_MAP * cp, COLLISION_MAP * dp);
int get_sprite_right_edge(COLLISION_MAP * cp, COLLISION_MAP * dp);
int get_sprite_top_edge(COLLISION_MAP * cp, COLLISION_MAP * dp);
int get_sprite_bottom_edge(COLLISION_MAP * cp, COLLISION_MAP * dp);
int get_sprite_edge_x(COLLISION_MAP * srp, COLLISION_MAP * drp);
int get_sprite_edge_y(COLLISION_MAP * srp, COLLISION_MAP * drp);
int get_sprite_x(COLLISION_MAP * rp, int tw);
int get_sprite_y(COLLISION_MAP * rp, int th);

#ifdef __cplusplus
}
#endif

#endif
