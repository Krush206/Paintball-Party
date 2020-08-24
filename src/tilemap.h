#ifndef TILEMAP_H
#define TILEMAP_H

#include <allegro.h>
#include "collide.h"
#include "animate.h"

#ifdef __cplusplus
extern "C"{
#endif

/* misc defines */
#define TILEMAP_MAX_SPRITES            256
#define TILEMAP_MAX_TILES              256
#define TILEMAP_MAX_FLAGS               50
#define TILEMAP_MAX_LAYERS               5

/* tile flags */
#define TILEMAP_FLAG_SOLIDT              0 /* solid on top */
#define TILEMAP_FLAG_SOLIDB              1 /* solid on bottom */
#define TILEMAP_FLAG_SOLIDL              2 /* solid on left */
#define TILEMAP_FLAG_SOLIDR              3 /* solid on right */

#define TILEMAP_FLAG_NEXTFRAME           4 /* next animation frame */
#define TILEMAP_FLAG_NEXTWAIT            5 /* how long to wait */
#define TILEMAP_FLAG_BRUTE               6 /* use brute-force animation */

#define TILEMAP_FLAG_TRANSPARENT         7 /* tile is see-through */

#define TILEMAP_FLAG_SLICK               8
#define TILEMAP_FLAG_V_GRAVITY           9
#define TILEMAP_FLAG_H_GRAVITY          10
#define TILEMAP_FLAG_CONVEY             11

#define TILEMAP_FLAG_LIGHT              12

extern COLOR_MAP * tilemap_trans_color_map;
extern COLOR_MAP * tilemap_light_map;

/* tile structure (single tile) */
typedef struct
{
    ANIMATION * ap;
    int flag[TILEMAP_MAX_FLAGS];
//    float fgx, fgy;
//    float ffx, ffy;
} TILE;


/* tileset structure */
typedef struct
{
    TILE * tile[TILEMAP_MAX_TILES];
    PALETTE palette;
    short tile_index[TILEMAP_MAX_TILES]; /* for animation purposes */
    short w, h;
    int frame_count;
    int trans_flag;
} TILESET;


/* map layer structure */
typedef struct
{
    BITMAP * mp; /* contains map data */
    BITMAP * brute_data; /* for brute animations */
    int x, y;
    float xm, ym;
    short flag[TILEMAP_MAX_FLAGS];
} TILEMAP_LAYER;


/* this is what programmer wants to use */
typedef struct
{
    TILESET * ts;
    TILEMAP_LAYER * layer[TILEMAP_MAX_LAYERS];
//    BITMAP * message;
    short w, h, l, il;
    int x, y;
    long cx, cy, cb, cr;
} TILEMAP;


/* disk functions for single tiles (mainly helpers for the stuff below) */
TILE * load_tile_fp(PACKFILE * fp);
TILE * load_tile(char * fn);
int save_tile_fp(TILE * tp, PACKFILE * fp);
int save_tile(TILE * tp, char * fn);
void destroy_tile(TILE * tp);
void copy_tile(TILE * tp1, TILE * tp2);
void clear_tile(TILE * tp);

/* for use by editor */
TILESET * create_tileset(PALETTE pp, short w, short h);

/* disk functions */
TILESET * load_tileset_fp(PACKFILE * fp);
TILESET * load_tileset(char * fn);
int save_tileset_fp(TILESET * tsp, PACKFILE * fp);
int save_tileset(TILESET * tsp, char * fn);
void destroy_tileset(TILESET * tsp);

/* programmer functions */
void reset_tileset(TILESET * tsp);
void tileset_set_trans(TILESET * tsp, int n);
void animate_tileset(TILESET * tsp);

/* mostly for internal purposes */
TILEMAP_LAYER * create_tilemap_layer(short w, short h);
void draw_tilemap_layer(BITMAP * bp, TILEMAP_LAYER * tlp, TILESET * tsp);

/* same here */
TILEMAP_LAYER * load_tilemap_layer_fp(PACKFILE * fp);
TILEMAP_LAYER * load_tilemap_layer(char * fn);
int save_tilemap_layer_fp(TILEMAP_LAYER * tlp, PACKFILE * fp);
int save_tilemap_layer(TILEMAP_LAYER * tlp, char * fn);
void destroy_tilemap_layer(TILEMAP_LAYER * tlp);

/* mostly for internal stuff */
TILEMAP * create_tilemap(short w, short h, short l);

/* disk functions */
TILEMAP * load_tilemap_fp(PACKFILE * fp);
TILEMAP * load_tilemap(char * fn);
int save_tilemap_fp(TILEMAP * tmp, PACKFILE * fp);
int save_tilemap(TILEMAP * tmp, char * fn);
void destroy_tilemap(TILEMAP * tmp);

/* helper functions for the programmer */
void initialize_tilemap(TILEMAP * tmp, TILESET * tsp);
void tilemap_change_tile(TILEMAP * tmp, int layer, int t1, int t2);
void center_tilemap(BITMAP * bp, TILEMAP * tmp, int x, int y);
void center_tilemap_smooth(BITMAP * bp, TILEMAP * tmp, int x, int y, int speed);
int tilemap_getcx(TILEMAP * tmp, BITMAP * bp, int x);
int tilemap_getcy(TILEMAP * tmp, BITMAP * bp, int y);
int tilemap_getrx(TILEMAP * tmp, int x);
int tilemap_getry(TILEMAP * tmp, int y);
int tilemap_flag_tile(TILEMAP * tmp, int layer, int flag, int x, int y);
int tilemap_flag(TILEMAP * tmp, int layer, int flag, int x, int y);
int tilemap_collidet_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp);
int tilemap_collideb_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp);
int tilemap_collidel_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp);
int tilemap_collider_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp);

/* gets the flag from the CMAP center point */
int tilemap_toucht_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_touchb_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_touchl_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_touchr_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);

/* get flag value if at least one point in the CMAP is touching specified flag */
int tilemap_rubt_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_rubb_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_rubl_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_rubr_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);

/* get flag value only if all points in the CMAP are touching specified flag */
int tilemap_scrapet_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_scrapeb_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_scrapel_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);
int tilemap_scraper_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp);

int tilemap_collide(TILEMAP * tmp, COLLISION_MAP * crp);
int tilemap_nudge(TILEMAP * tmp, int layer, COLLISION_MAP * crp);
int get_tile(TILEMAP * tmp, int layer, int x, int y);

/* draw a layer of the map */
void draw_tilemap(BITMAP * bp, TILEMAP * tmp, int layer);

/* draw the tilemap background */
void draw_tilemap_bg(BITMAP * bp, TILEMAP * tmp);

/* draw the tilemap foreground */
void draw_tilemap_fg(BITMAP * bp, TILEMAP * tmp);


#ifdef __cplusplus
}
#endif

#endif
