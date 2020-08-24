#ifndef LEVEL_H
#define LEVEL_H

#include <allegro.h>
#include "tilemap.h"

#include "objects.h"

/* for the level selection list */
typedef struct
{
	
	char name[256];
	char file[1024];
	BITMAP * bp;
	
} LEVEL_LIST_ENTRY;

#define LEVEL_LIST_MAX 256


typedef struct
{

    /* tilemap data */
    TILESET * tileset;
    TILEMAP * tilemap;

	/* object positions */
    int object_type[PP_MAX_OBJECTS], object_x[PP_MAX_OBJECTS], object_y[PP_MAX_OBJECTS], object_active[PP_MAX_OBJECTS], object_flag[PP_MAX_OBJECTS][PP_MAX_OBJECT_FLAGS];

} LEVEL;

int load_level_data_fp(LEVEL * lp, PACKFILE * fp);
int load_level_data(LEVEL * lp, char * fn);
int save_level_data_fp(LEVEL * lp, PACKFILE * fp);
int save_level_data(LEVEL * lp, char * fn);
void save_level_fp(LEVEL * lp, PACKFILE * fp);
void save_level(LEVEL * lp, char * fn);
LEVEL * load_level_fp(PACKFILE * fp);
LEVEL * load_level(char * fn);
LEVEL * create_level(PALETTE pal, int w, int h, int tw, int th, int l);
void destroy_level(LEVEL * lp);

#endif
