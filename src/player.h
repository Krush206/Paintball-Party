#ifndef PLAYER_H
#define PLAYER_H

#include <allegro.h>
#include "animate.h"
#include "collide.h"
#include "tilemap.h"
#include "control.h"
#include "level.h"

/* player states */
#define OLD_PLAYER_STATES       12
#define PLAYER_STATES           24
#define PS_STAND_RIGHT           0
#define PS_STAND_LEFT            1
#define PS_STAND_UP_RIGHT        2
#define PS_STAND_UP_LEFT         3
#define PS_WALK_RIGHT            4
#define PS_WALK_LEFT             5
#define PS_DUCK_RIGHT            6
#define PS_DUCK_LEFT             7
#define PS_JUMP_RIGHT            8
#define PS_JUMP_LEFT             9
#define PS_FALL_RIGHT           10
#define PS_FALL_LEFT            11
#define PS_FSTAND_RIGHT         12
#define PS_FSTAND_LEFT          13
#define PS_FSTAND_UP_RIGHT      14
#define PS_FSTAND_UP_LEFT       15
#define PS_FWALK_RIGHT          16
#define PS_FWALK_LEFT           17
#define PS_FDUCK_RIGHT          18
#define PS_FDUCK_LEFT           19
#define PS_FJUMP_RIGHT          20
#define PS_FJUMP_LEFT           21
#define PS_FFALL_RIGHT          22
#define PS_FFALL_LEFT           23

#define GUN_FLASH_TYPES        4
#define GUN_FLASH_RIGHT        0
#define GUN_FLASH_LEFT         1
#define GUN_FLASH_UP_RIGHT     2
#define GUN_FLASH_UP_LEFT      3

/* ammo types */
#define PP_AMMO_TYPES       5
#define PP_AMMO_TYPE_NORMAL 0
#define PP_AMMO_TYPE_X      1
#define PP_AMMO_TYPE_MINE   2
#define PP_AMMO_TYPE_BOUNCE 3
#define PP_AMMO_TYPE_SEEK   4

#define PLAYER_DATA_MAX 1024

/* for the character selection list */
typedef struct
{
	
	char name[256];
	char file[1024];
	ANIMATION * ap;
	
} CHAR_LIST_ENTRY;

#define CHAR_LIST_MAX 1024

typedef struct
{
	
	char name[17];
	int played, won;
	int fired, hit;
	
} PLAYER_DATA;

typedef struct
{

	char * text;
	char buffer[128];
	int ox, oy, vx, vy, life, active;

} PLAYER_MESSAGE;

/* player structure (1 for each player) */
typedef struct
{

    ANIMATION * ap;
    int x, y, active, frame;

} GUN_FLASH;

/* a player's screen */
typedef struct
{

	BITMAP * bp, * mp, * b[3], * m[3];
	int x, y, w, h, active;

} PLAYER_SCREEN;

typedef struct
{
	
	/* animation data */
	ANIMATION * ap[PLAYER_STATES]; /* player animations */
	ANIMATION * pap;    /* paintball animation */
	ANIMATION * ppap;   /* paintball particle animation */
	ANIMATION * gap[GUN_FLASH_TYPES]; /* gun flash animations */
	
    /* positioning tables */
	int cx_table[PLAYER_STATES], cy_table[PLAYER_STATES], cw_table[PLAYER_STATES], ch_table[PLAYER_STATES];
	int px_table[PLAYER_STATES], py_table[PLAYER_STATES];
	int gx_table[PLAYER_STATES], gy_table[PLAYER_STATES];
	int mix, miy, mir; /* center of character */
	int ix, iy; /* location of pointer relative to character */
	int pf;

} PP_CHARACTER;

typedef struct
{

	PP_CHARACTER character;
	
	/* player data */
    int color;
    int reload_time, reload_fast;

    COLLISION_MAP cmap[PLAYER_STATES];
    PLAYER_CONTROLLER controller;
    GUN_FLASH gun_flash;
    PLAYER_SCREEN screen;
    BITMAP * radar_image;
    BITMAP * zap_image;

    fixed fx, fy, vx, vy, evx, evy, mx, gy;
    int x, y, cx, cy, tx, ty;
    int state, frame, ground;
    int flash_time;
    int active, tele_in, tele_out, tele_state, tele_frame;

    /* other stuff */
    int score, hits, shots;
    int cloak_time;
    int moon_time;
    int run_time;
    int deflect_time;
    int fly_time;
    int turbo_time;
    int weapon;
    int ammo[PP_AMMO_TYPES];
    int target;
    int it, flag, time, shock_time, loose, item, gem[5], gems, just;
    PLAYER_MESSAGE message;
    int remote;
    int col;
    int slip;
//    int data;
    char name[224];
	SAMPLE * sound[64];
	
	int entered;
	int char_pick;
	unsigned long char_sum;
	int data_pick;

} PLAYER;

int player_can_reach(int p, int x, int y);
int player_generate_screen(PLAYER * dp, LEVEL * lp);
int load_char(PLAYER * dp, const char * fn);
int load_char_ns(PLAYER * dp, const char * fn);
int load_sounds(PLAYER * dp, const char * fn);
void destroy_char(PLAYER * pp);
void erase_char(PLAYER * pp);
int player_init(PLAYER * pp, int reload);
void player_message_generate(PLAYER_MESSAGE * mp, char * text, int ox, int oy, int vx, int vy, int life);
void player_message_logic(PLAYER_MESSAGE * mp);
void player_message_draw(BITMAP * bp, PLAYER_MESSAGE * mp, int x, int y);
int player_grounded(PLAYER * pp, TILEMAP * mp);
void pp_cycle_ammo(PLAYER * pp);
void pp_cycle_target(PLAYER * pp);
void pp_select_target(PLAYER * pp);
void player_guard_territory_x(int i);
void player_guard_territory_y(int i);
void player_logic(int i, TILEMAP * mp);
void player_draw_screen(BITMAP * bp, PLAYER * pp);

#endif
