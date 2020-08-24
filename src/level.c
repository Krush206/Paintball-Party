#include <allegro.h>
#include "tilemap.h"
#include "level.h"
#include "objects.h"

int load_level_data_fp(LEVEL * lp, PACKFILE * fp)
{
	int i, j;

	if(fp && lp)
	{
		for(i = 0; i < PP_MAX_OBJECTS; i++)
		{
			lp->object_type[i] = pack_igetl(fp);
			lp->object_x[i] = pack_igetl(fp);
			lp->object_y[i] = pack_igetl(fp);
			lp->object_active[i] = pack_igetl(fp);
			for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
			{
				lp->object_flag[i][j] = pack_igetl(fp);
			}
		}
		return 1;
	}
	return 0;
}

int load_level_data(LEVEL * lp, char * fn)
{
    PACKFILE * fp;
    int rval;

    fp = pack_fopen(fn, "rb");
    if(!fp)
    {
        return 0;
    }
    rval = load_level_data_fp(lp, fp);
    pack_fclose(fp);

    return rval;
}

int save_level_data_fp(LEVEL * lp, PACKFILE * fp)
{
	int i, j;

	if(fp && lp)
	{
		for(i = 0; i < PP_MAX_OBJECTS; i++)
		{
			pack_iputl(lp->object_type[i], fp);
			pack_iputl(lp->object_x[i], fp);
			pack_iputl(lp->object_y[i], fp);
			pack_iputl(lp->object_active[i], fp);
			for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
			{
				pack_iputl(lp->object_flag[i][j], fp);
			}
		}
		return 1;
	}
	return 0;
}

int save_level_data(LEVEL * lp, char * fn)
{
    PACKFILE * fp;
    int rval;

    fp = pack_fopen(fn, "wb");
    if(!fp)
    {
        return 0;
    }
    rval = save_level_data_fp(lp, fp);
    pack_fclose(fp);

    return rval;
}

void save_level_fp(LEVEL * lp, PACKFILE * fp)
{
	save_tileset_fp(lp->tileset, fp);
	save_tilemap_fp(lp->tilemap, fp);
	save_level_data_fp(lp, fp);
}

void save_level(LEVEL * lp, char * fn)
{
	PACKFILE * fp;

	fp = pack_fopen(fn, "w");
	if(!fp)
	{
		return;
	}
	save_level_fp(lp, fp);
	pack_fclose(fp);
}

LEVEL * load_level_fp(PACKFILE * fp)
{
	int i;
	LEVEL * lp;

    lp = malloc(sizeof(LEVEL));
    if(!lp)
    {
        return NULL;
    }
//    for(i = 0; i < 128; i++)
//    {
//	    pack_getc(fp);
//    }
	lp->tileset = load_tileset_fp(fp);
	if(!lp->tileset)
	{
		free(lp);
		return NULL;
	}
	lp->tilemap = load_tilemap_fp(fp);
	if(!lp->tilemap)
	{
		return NULL;
	}
	load_level_data_fp(lp, fp);
	initialize_tilemap(lp->tilemap, lp->tileset);
	return lp;
}

LEVEL * load_level(char * fn)
{
	PACKFILE * fp;
	LEVEL * lp;
	char * nfn;

	if(!exists(fn))
	{
		nfn = get_filename(fn);
	}
	else
	{
		nfn = fn;
	}
	if(!exists(nfn))
	{
	}
	else
	{
		fp = pack_fopen(nfn, "r");
		if(!fp)
		{
			return NULL;
		}
		lp = load_level_fp(fp);
		pack_fclose(fp);

		return lp;
	}
	return NULL;
}

LEVEL * create_level(PALETTE pal, int w, int h, int tw, int th, int l)
{
	int i, j;
	LEVEL * lp;

    clear_keybuf();
    lp = (LEVEL *)malloc(sizeof(LEVEL));
    if(!lp)
    {
        return NULL;
    }
    lp->tileset = create_tileset(pal, tw, th);
    if(!lp->tileset)
    {
	    free(lp);
	    return NULL;
    }
    lp->tilemap = create_tilemap(w, h, l);
    if(!lp->tilemap)
    {
	    destroy_tileset(lp->tileset);
	    free(lp);
	    return NULL;
    }
    initialize_tilemap(lp->tilemap, lp->tileset);
	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		lp->object_type[i] = 0;
		lp->object_x[i] = 0;
		lp->object_y[i] = 0;
		lp->object_active[i] = 0;
		for(j = 0; j < PP_MAX_OBJECT_FLAGS; j++)
		{
			lp->object_flag[i][j] = 0;
		}
	}
    return lp;
}

void destroy_level(LEVEL * lp)
{
    if(lp)
    {
    	destroy_tileset(lp->tileset);
       	destroy_tilemap(lp->tilemap);
        free(lp);
    }
}
