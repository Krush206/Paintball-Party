#include <allegro.h>
#include "collide.h"
#include "tilemap.h"

COLOR_MAP * tilemap_trans_color_map = NULL;
COLOR_MAP * tilemap_light_map = NULL;

/* loads a single tile from a file pointer */
TILE * load_tile_fp(PACKFILE * fp)
{
    char header[4]; /* identifier buffer */
    int tw, th;     /* temporary width and height holders */
    int i;       /* looping variables */
    TILE * tp;

    /* load the header */
    pack_fread(header, 4, fp);
    if(header[0] != 'T' || header[1] != 'I' ||header[2] != 'L' || header[3] != 21)
    {
        return NULL;
    }

    tp = (TILE *)malloc(sizeof(TILE));

    /* load dimensions */
    tw = pack_igetw(fp);
    th = pack_igetw(fp);

    /* load flags */
    for(i = 0; i < TILEMAP_MAX_FLAGS; i++)
    {
        tp->flag[i] = pack_igetl(fp);
    }

    /* create and load image */
    tp->ap = load_ani_fp(fp, NULL);

    /* return success */
    return tp;
}

/* loads a tile from the specified file */
TILE * load_tile(char * fn)
{
    PACKFILE * fp; /* file pointer */
    TILE * tp;

    /* attempt to open file */
    fp = pack_fopen(fn, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    tp = load_tile_fp(fp);

    /* close the file */
    pack_fclose(fp);

    /* return success */
    return tp;
}

/* saves tile to the specified file pointer */
int save_tile_fp(TILE * tp, PACKFILE * fp)
{
    char header[4] = {'T', 'I', 'L', 21}; /* identify as a tile */
    int i; /* looping variables */

    /* save the header */
    pack_fwrite(header, 4, fp);

    /* save dimensions */
    pack_iputw(tp->ap->w, fp);
    pack_iputw(tp->ap->h, fp);

    /* save flags */
    for(i = 0; i < TILEMAP_MAX_FLAGS; i++)
    {
        pack_iputl(tp->flag[i], fp);
    }

    save_ani_fp(tp->ap, fp, NULL);

    /* return success */
    return 1;
}

/* saves tile to the specified file */
int save_tile(TILE * tp, char * fn)
{
    PACKFILE * fp; /* file pointer */

    /* attempt to open file */
    fp = pack_fopen(fn, "w");
    if(fp == NULL)
    {
        return 0;
    }

    /* attemp to save tile data */
    if(!save_tile_fp(tp, fp))
    {
        return  0;
    }

    /* close the file */
    pack_fclose(fp);

    /* return success */
    return 1;
}

TILE * create_tile(int w, int h)
{
    int i;
    TILE * tp;

    tp = (TILE *)malloc(sizeof(TILE));

    tp->ap = create_ani(w, h);
    animation_add_frame(tp->ap, create_bitmap(w, h));
    clear_bitmap(tp->ap->image[0]);
    for(i = 0; i < TILEMAP_MAX_FLAGS; i++)
    {
        tp->flag[i] = 0;
    }
    return tp;
}

void destroy_tile(TILE * tp)
{
	if(tp)
	{
		if(tp->ap)
		{
	    	destroy_ani(tp->ap);
	    }
	    free(tp);
	    tp = NULL;
    }
}

void copy_tile(TILE * tp1, TILE * tp2)
{
	int i;

	destroy_ani(tp2->ap);
	tp2->ap = duplicate_ani(tp1->ap);
	for(i = 0; i < TILEMAP_MAX_FLAGS; i++)
	{
		tp2->flag[i] = tp1->flag[i];
	}
}

void clear_tile(TILE * tp)
{
	int w, h;

	w = tp->ap->w;
	h = tp->ap->h;
	destroy_ani(tp->ap);
	tp->ap = create_ani(w, h);
	animation_add_frame(tp->ap, create_bitmap(w, h));
//	clear_bitmap(tp->ap->image[0]);
}

/* holds a set of tiles (all should be same dimensions) */
TILESET * load_tileset_fp(PACKFILE * fp)
{
    char header[3]; /* tileset identifier */
    int i;          /* looping variable */
    TILESET * tsp;

    /* load the header */
    pack_fread(header, 3, fp);
    if(header[0] != 'T' || header[1] != 'S' || header[2] != 21)
    {
        return NULL;
    }

    tsp = (TILESET *)malloc(sizeof(TILESET));

    /* load the palette */
    for(i = 0; i < 256; i++)
    {
        tsp->palette[i].r = pack_getc(fp);
        tsp->palette[i].g = pack_getc(fp);
        tsp->palette[i].b = pack_getc(fp);
    }

    /* load tileset dimensions */
    tsp->w = pack_igetw(fp);
    tsp->h = pack_igetw(fp);

    /* load the tile image data */
    for(i = 0; i < TILEMAP_MAX_TILES; i++)
    {
        tsp->tile[i] = load_tile_fp(fp);
        if(!tsp->tile[i])
        {
	        return NULL;
        }

        /* make sure tile starts on the right animation frame */
        tsp->tile_index[i] = i;
    }

    /* frame counter for doing tile animations */
    tsp->frame_count = 0;

    /* create a transparency map with the tileset's palette */
//    create_trans_table(&tilemap_trans_color_map, tsp->palette, 128, 128, 128, NULL);
    tsp->trans_flag = 1;

    /* return success */
    return tsp;
}

TILESET * load_tileset(char * fn)
{
    PACKFILE * fp;
    TILESET * tsp;

    /* attempt to open file */
    fp = pack_fopen(fn, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    /* attemp to load tileset data */
    tsp = load_tileset_fp(fp);

    /* close the file */
    pack_fclose(fp);

    return tsp;
}

int save_tileset_fp(TILESET * tsp, PACKFILE * fp)
{
    char header[4] = {'T', 'S', 21};
    int i;

    /* load the header */
    pack_fwrite(header, 3, fp);

    /* save the palette data */
    for(i = 0; i < 256; i++)
    {
        pack_putc(tsp->palette[i].r, fp);
        pack_putc(tsp->palette[i].g, fp);
        pack_putc(tsp->palette[i].b, fp);
    }

    /* save the dimensions */
    pack_iputw(tsp->w, fp);
    pack_iputw(tsp->h, fp);

    /* save tile data */
    for(i = 0; i < TILEMAP_MAX_TILES; i++)
    {
        save_tile_fp(tsp->tile[i], fp);
    }

    /* return success */
    return 1;
}

int save_tileset(TILESET * tsp, char * fn)
{
    PACKFILE * fp;

    /* attempt to open file */
    fp = pack_fopen(fn, "w");
    if(fp == NULL)
    {
        return 0;
    }

    /* attemp to save tile data */
    if(!save_tileset_fp(tsp, fp))
    {
        return  0;
    }

    /* close the file */
    pack_fclose(fp);

    return 1;
}

/* create an empty tileset with the specified palette and dimensions */
TILESET * create_tileset(PALETTE pp, short w, short h)
{
    int i, j;
    TILESET * tsp;

    tsp = (TILESET *)malloc(sizeof(TILESET));

    /* copy palette */
    for(i = 0; i < 256; i++)
    {
        tsp->palette[i].r = pp[i].r;
        tsp->palette[i].g = pp[i].g;
        tsp->palette[i].b = pp[i].b;
    }

    /* set dimensions */
    tsp->w = w;
    tsp->h = h;

    /* create the tiles */
    for(i = 0; i < TILEMAP_MAX_TILES; i++)
    {
        tsp->tile[i] = create_tile(tsp->w, tsp->h);

        /* set proper animation frame */
        tsp->tile_index[i] = i;
    }

    /* reset animation */
    tsp->frame_count = 0;

    return tsp;
}

/* frees a tileset from memory */
void destroy_tileset(TILESET * tsp)
{
    int i;

    /* free the tile images */
    if(tsp)
    {
    	for(i = 0; i < TILEMAP_MAX_TILES; i++)
    	{
	        destroy_tile(tsp->tile[i]);
	    }

	    free(tsp);
	    tsp = NULL;
    }
}

/* reset the tileset animation variables */
void reset_tileset(TILESET * tsp)
{
    int i;

    tsp->frame_count = 0;
    for(i = 0; i < TILEMAP_MAX_TILES; i++)
    {
        tsp->tile_index[i] = i;
    }
}

void tileset_set_trans(TILESET * tsp, int n)
{
	tsp->trans_flag = n;
}

/* animate the tileset */
void animate_tileset(TILESET * tsp)
{
    int i;

    tsp->frame_count++;
    for(i = 0; i < TILEMAP_MAX_TILES; i++)
    {
        if(tsp->tile[tsp->tile_index[i]]->flag[TILEMAP_FLAG_NEXTWAIT] != 0 && tsp->tile[tsp->tile_index[i]]->flag[TILEMAP_FLAG_BRUTE] == 0)
        {
            if(tsp->frame_count % tsp->tile[tsp->tile_index[i]]->flag[TILEMAP_FLAG_NEXTWAIT] == 0)
            {
                tsp->tile_index[i] = tsp->tile[tsp->tile_index[i]]->flag[TILEMAP_FLAG_NEXTFRAME];
            }
        }
    }
}

/* load a layer from an open file */
TILEMAP_LAYER * load_tilemap_layer_fp(PACKFILE * fp)
{
    char header[4];
    int i, j;
    int tw, th;
    TILEMAP_LAYER * tlp;

    pack_fread(header, 4, fp);
    if(header[0] != 'T' || header[1] != 'M' || header[2] != 'L' || header[3] != 23)
    {
        return NULL;
    }

    tlp = (TILEMAP_LAYER *)malloc(sizeof(TILEMAP_LAYER));

    tw = pack_igetw(fp);
    th = pack_igetw(fp);
    tlp->xm = fixtof(pack_igetl(fp));
    tlp->ym = fixtof(pack_igetl(fp));
    for(i = 0; i < TILEMAP_MAX_FLAGS; i++)
    {
        tlp->flag[i] = pack_igetw(fp);
    }

    tlp->mp = create_bitmap(tw, th);
    tlp->brute_data = create_bitmap(tw, th);
    clear_bitmap(tlp->brute_data);
    for(i = 0; i < th; i++)
    {
        for(j = 0; j < tw; j++)
        {
            tlp->mp->line[i][j] = pack_getc(fp);
        }
    }

    return tlp;
}

/* load a layer */
TILEMAP_LAYER * load_tilemap_layer(char * fn)
{
    PACKFILE * fp;
    TILEMAP_LAYER * tlp;

    fp = pack_fopen(fn, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    tlp = load_tilemap_layer_fp(fp);

    pack_fclose(fp);

    return tlp;
}

/* save a layer to an open file */
int save_tilemap_layer_fp(TILEMAP_LAYER * tlp, PACKFILE * fp)
{
    char header[4] = {'T', 'M', 'L', 23};
    int i, j;

    pack_fwrite(header, 4, fp);

    pack_iputw(tlp->mp->w, fp);
    pack_iputw(tlp->mp->h, fp);
    pack_iputl(ftofix(tlp->xm), fp);
    pack_iputl(ftofix(tlp->ym), fp);
    for(i = 0; i < TILEMAP_MAX_FLAGS; i++)
    {
        pack_iputw(tlp->flag[i], fp);
    }

    for(i = 0; i < tlp->mp->h; i++)
    {
        for(j = 0; j < tlp->mp->w; j++)
        {
            pack_putc(tlp->mp->line[i][j], fp);
        }
    }

    return 1;
}

/* save a layer */
int save_tilemap_layer(TILEMAP_LAYER * tlp, char * fn)
{
    PACKFILE * fp;

    fp = pack_fopen(fn, "w");
    if(fp == NULL)
    {
        return 0;
    }

    if(!save_tilemap_layer_fp(tlp, fp))
    {
        return 0;
    }

    pack_fclose(fp);

    return 1;
}

/* create a tilemap layer */
TILEMAP_LAYER * create_tilemap_layer(short w, short h)
{
    TILEMAP_LAYER * tlp;

    tlp = (TILEMAP_LAYER *)malloc(sizeof(TILEMAP_LAYER));

	if(!tlp)
	{
		return NULL;
	}
    tlp->mp = create_bitmap(w * 2, h * 2);
    tlp->brute_data = create_bitmap(w, h);
    tlp->xm = 1.0;
    tlp->ym = 1.0;
    clear_bitmap(tlp->mp);
    clear_bitmap(tlp->brute_data);

    return tlp;
}

/* free a tilemap layer */
void destroy_tilemap_layer(TILEMAP_LAYER * tlp)
{
	if(tlp)
	{
		if(tlp->mp)
		{
    		destroy_bitmap(tlp->mp);
    	}
    	if(tlp->brute_data)
    	{
    		destroy_bitmap(tlp->brute_data);
    	}
    	free(tlp);
    	tlp = NULL;
    }
}

/* draws a layer of a tilemap to the specified drawing surface (bp) */
void draw_tilemap_layer(BITMAP * bp, TILEMAP_LAYER * tlp, TILESET * tsp)
{
    int i, j; /* looping variables */
    int oi;   /* first tile to draw in the y direction */
    int oj;   /* first tile to draw in the x direction */
    int oei;  /* last tile in the y direction (actually last + 1) */
    int oej;  /* last tile in the x direction (actually last + 1) */

    /* if tiles are 16x16 do this quicker method */
    if(tsp->w == 16 && tsp->h == 16)
    {
        oi = tlp->y >> 4;
        oei = oi + (bp->h >> 4) + 2;
        oj = tlp->x >> 4;
        oej = oj + (bp->w >> 4) + 2;
    }
    else if(tsp->w == 32 && tsp->h == 32)
    {
        oi = tlp->y >> 5;
        oei = oi + (bp->h >> 5) + 2;
        oj = tlp->x >> 5;
        oej = oj + (bp->w >> 5) + 2;
    }

    /* otherwise just use regular divides */
    else
    {
        oi = tlp->y / tsp->h;
        oei = oi + bp->h / tsp->h + 2;
        oj = tlp->x / tsp->w;
        oej = oj + bp->h / tsp->h + 2;
    }

    /* drawing loop */
    for(i = oi; i < oei && i < tlp->mp->h; i++)
    {
        for(j = oj; j < oej && j < tlp->mp->w; j++)
        {
            if(tsp->tile_index[tlp->mp->line[i][j]] != 0)
            {
                /* if brute animation is on for this tile, do the brute animation */
                if(tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_BRUTE])
                {
                    tlp->brute_data->line[i][j]++;
                    if(tlp->brute_data->line[i][j] >= tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_NEXTWAIT] && tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_NEXTWAIT] > 0)
                    {
                        tlp->mp->line[i][j] = tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_NEXTFRAME];
                        tlp->brute_data->line[i][j] = 0;
                    }
                }

                /* if tile is transparent, draw in transparent mode */
                if(tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_TRANSPARENT] && tsp->trans_flag)
                {
                    color_map = tilemap_trans_color_map;
                    draw_trans_ani(bp, tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->ap, (j - oj) * tsp->w - tlp->x % tsp->w, (i - oi) * tsp->h - tlp->y % tsp->h, tsp->frame_count);
                }

                /* if tile is transparent, draw in transparent mode */
                else if(tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_LIGHT])
                {
                    color_map = tilemap_light_map;
                    draw_trans_ani(bp, tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->ap, (j - oj) * tsp->w - tlp->x % tsp->w, (i - oi) * tsp->h - tlp->y % tsp->h, tsp->frame_count);
                }

                /* otherwise, just use a regular blit */
                else
                {
                    draw_ani(bp, tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->ap, (j - oj) * tsp->w - tlp->x % tsp->w, (i - oi) * tsp->h - tlp->y % tsp->h, tsp->frame_count);
                }
            }
        }
    }
}

/* draws a layer in solid mode */
void draw_tilemap_layer_solid(BITMAP * bp, TILEMAP_LAYER * tlp, TILESET * tsp)
{
    int i, j, oi, oj, oei, oej; /* see draw_tilemap_layer() */

    /* if tiles are 16x16 do this quicker method */
    if(tsp->w == 16 && tsp->h == 16)
    {
        oi = tlp->y >> 4;
        oei = oi + (bp->h >> 4) + 2;
        oj = tlp->x >> 4;
        oej = oj + (bp->w >> 4) + 2;
    }

    /* otherwise just use regular divides */
    else
    {
        oi = tlp->y / tsp->h;
        oei = oi + bp->h / tsp->h + 2;
        oj = tlp->x / tsp->w;
        oej = oj + bp->h / tsp->h + 2;
    }

    for(i = oi; i < oei && i < tlp->mp->h; i++)
    {
        for(j = oj; j < oej && j < tlp->mp->w; j++)
        {
            /* handle brute animations for onscreen tiles */
            if(tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_BRUTE])
            {
                tlp->brute_data->line[i][j]++;
                if(tlp->brute_data->line[i][j] >= tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_NEXTWAIT] && tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_NEXTWAIT] > 0)
                {
                    tlp->mp->line[i][j] = tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->flag[TILEMAP_FLAG_NEXTFRAME];
                    tlp->brute_data->line[i][j] = 0;
                }
            }
            blit_ani(tsp->tile[tsp->tile_index[tlp->mp->line[i][j]]]->ap, bp, (j - oj) * tsp->w - tlp->x % tsp->w, (i - oi) * tsp->h - tlp->y % tsp->h, tsp->frame_count);
        }
    }
}

/* load tilemap from open file */
TILEMAP * load_tilemap_fp(PACKFILE * fp)
{
    char header[3];
    int i;
    TILEMAP * tmp;

    /* read the ID and check it */
    pack_fread(header, 3, fp);
    if(header[0] != 'T' || header[1] != 'M' || header[2] != 21)
    {
        return NULL;
    }

    /* allocate memory */
    tmp = (TILEMAP *)malloc(sizeof(TILEMAP));

    /* read dimensions */
    tmp->w = pack_igetw(fp);
    tmp->h = pack_igetw(fp);
    tmp->l = pack_igetw(fp);

    /* read clipping data */
    tmp->il = pack_igetw(fp);
    tmp->cx = pack_igetl(fp);
    tmp->cy = pack_igetl(fp);
    tmp->cb = pack_igetl(fp);
    tmp->cr = pack_igetl(fp);

    /* read layers */
    for(i = 0; i < tmp->l; i++)
    {
        tmp->layer[i] = load_tilemap_layer_fp(fp);
        if(!tmp->layer[i])
        {
	        return NULL;
        }
    }
//    tmp->message = create_bitmap(tmp->w, tmp->h);
//    clear_bitmap(tmp->message);

    /* nullify tileset (user must set this before drawing) */
    tmp->ts = NULL;

    return tmp;
}

/* load a tilemap */
TILEMAP * load_tilemap(char * fn)
{
    PACKFILE * fp;
    TILEMAP * tmp;

    /* attempt to open the file */
    fp = pack_fopen(fn, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    /* attemp to load the map */
    tmp = load_tilemap_fp(fp);

    /* close the file */
    pack_fclose(fp);

    return tmp;
}

/* save tilemap to open file */
int save_tilemap_fp(TILEMAP * tmp, PACKFILE * fp)
{
    char header[3] = {'T', 'M', 21};
    int i;

    /* write ID */
    pack_fwrite(header, 3, fp);

    /* write dimensions */
    pack_iputw(tmp->w, fp);
    pack_iputw(tmp->h, fp);
    pack_iputw(tmp->l, fp);

    /* write clipping data */
    pack_iputw(tmp->il, fp);
    pack_iputl(tmp->cx, fp);
    pack_iputl(tmp->cy, fp);
    pack_iputl(tmp->cb, fp);
    pack_iputl(tmp->cr, fp);

    /* save the layers */
    for(i = 0; i < tmp->l; i++)
    {
        save_tilemap_layer_fp(tmp->layer[i], fp);
    }

    return 1;
}

/* save a tilemap */
int save_tilemap(TILEMAP * tmp, char * fn)
{
    PACKFILE * fp;

    /* attempt to open the file */
    fp = pack_fopen(fn, "w");
    if(fp == NULL)
    {
        return 0;
    }

    /* attempt to save */
    if(!save_tilemap_fp(tmp, fp))
    {
        pack_fclose(fp);
        return 0;
    }

    /* close the file */
    pack_fclose(fp);

    return 1;
}

/* create an empty tilemap */
TILEMAP * create_tilemap(short w, short h, short l)
{
    int i, j;
    TILEMAP * tmp;

    /* allocate memory */
    tmp = (TILEMAP *)malloc(sizeof(TILEMAP));
	if(!tmp)
	{
		return NULL;
	}
    /* set dimensions */
    tmp->w = w;
    tmp->h = h;
    tmp->l = l;

    /* set map clipping area */
    tmp->cx = 0;
    tmp->cy = 0;
    tmp->cb = h - 1;
    tmp->cr = w - 1;

    /* create the layers */
    for(i = 0; i < l && i < TILEMAP_MAX_LAYERS; i++)
    {
        tmp->layer[i] = create_tilemap_layer(w, h);
        if(!tmp->layer[i])
        {
	        for(j = 0; j < i; j++)
	        {
		        destroy_tilemap_layer(tmp->layer[j]);
	        }
	        free(tmp);
	        return NULL;
        }
    }

    /* nullify the tileset (user must set this before drawing anything) */
    tmp->ts = NULL;

    return tmp;
}

/* free a tilemap */
void destroy_tilemap(TILEMAP * tmp)
{
    int i;

    /* free the layers */
    if(tmp)
    {
    	for(i = 0; i < tmp->l; i++)
    	{
		    if(tmp->layer[i])
	    	{
	        	destroy_tilemap_layer(tmp->layer[i]);
        	}
    	}
	    /* free the memory */
	    free(tmp);
	    tmp = NULL;
    }
}

/* function to gloss up linking a tileset to a tilemap */
void initialize_tilemap(TILEMAP * tmp, TILESET * tsp)
{
    tmp->ts = tsp;
}

/* centers tilemap around specified point */
void center_tilemap(BITMAP * bp, TILEMAP * tmp, int x, int y)
{
    int xpos = x - (bp->w >> 1);
    int ypos = y - (bp->h >> 1);

//    tmp->x += (xpos - tmp->x) >> 1;
    tmp->x = xpos;
    if(tmp->x > (tmp->cr + 1) * tmp->ts->w - bp->w)
    {
        tmp->x = (tmp->cr + 1) * tmp->ts->w - bp->w;
    }
    if(tmp->x < tmp->cx * tmp->ts->w)
    {
        tmp->x = tmp->cx * tmp->ts->w;
    }

//    tmp->y += (ypos - tmp->y) >> 1;
    tmp->y = ypos;
    if(tmp->y > (tmp->cb + 1) * tmp->ts->h - bp->h)
    {
        tmp->y = (tmp->cb + 1) * tmp->ts->h - bp->h;
    }
    if(tmp->y < tmp->cy * tmp->ts->h)
    {
        tmp->y = tmp->cy * tmp->ts->h;
    }
}

/* centers tilemap around specified point */
void center_tilemap_smooth(BITMAP * bp, TILEMAP * tmp, int x, int y, int speed)
{
    if(tmp->x < x - (bp->w >> 1))
    {
        tmp->x += speed;
        if(tmp->x >= x - (bp->w >> 1))
        {
            tmp->x = x - (bp->w >> 1);
        }
        if(tmp->x > (tmp->cr + 1) * tmp->ts->w - bp->w)
        {
            tmp->x = (tmp->cr + 1) * tmp->ts->w - bp->w;
        }
    }
    if(tmp->x > x - (bp->w >> 1))
    {
        tmp->x -= speed;
        if(tmp->x <= x - (bp->w >> 1))
        {
            tmp->x = x - (bp->w >> 1);
        }
        if(tmp->x < tmp->cx * tmp->ts->w)
        {
            tmp->x = tmp->cx * tmp->ts->w;
        }
    }
    if(tmp->y < y - (bp->h >> 1))
    {
        tmp->y += speed;
        if(tmp->y >= y - (bp->h >> 1))
        {
            tmp->y = y - (bp->h >> 1);
        }
        if(tmp->y > (tmp->cb + 1) * tmp->ts->h - bp->h)
        {
            tmp->y = (tmp->cb + 1) * tmp->ts->h - bp->h;
        }
    }
    if(tmp->y > y - (bp->h >> 1))
    {
        tmp->y -= speed;
        if(tmp->y <= y - (bp->h >> 1))
        {
            tmp->y = y - (bp->h >> 1);
        }
        if(tmp->y < tmp->cy * tmp->ts->h)
        {
            tmp->y = tmp->cy * tmp->ts->h;
        }
    }
}

int tilemap_getcx(TILEMAP * tmp, BITMAP * bp, int x)
{
    int xpos = x - (bp->w >> 1);
    int tmp_x;
    
    tmp_x = xpos;
    if(tmp_x > (tmp->cr + 1) * tmp->ts->w - bp->w)
    {
        tmp_x = (tmp->cr + 1) * tmp->ts->w - bp->w;
    }
    if(tmp_x < tmp->cx * tmp->ts->w)
    {
        tmp_x = tmp->cx * tmp->ts->w;
    }
    return tmp_x;
}

int tilemap_getcy(TILEMAP * tmp, BITMAP * bp, int y)
{
    int ypos = y - (bp->h >> 1);
    int tmp_y;
    
    tmp_y = ypos;
    if(tmp_y > (tmp->cb + 1) * tmp->ts->h - bp->h)
    {
        tmp_y = (tmp->cb + 1) * tmp->ts->h - bp->h;
    }
    if(tmp_y < tmp->cy * tmp->ts->h)
    {
        tmp_y = tmp->cy * tmp->ts->h;
    }
    return tmp_y;
}

int tilemap_getrx(TILEMAP * tmp, int x)
{
    return x - tmp->x;
}

int tilemap_getry(TILEMAP * tmp, int y)
{
    return y - tmp->y;
}

/* check a point for a specific flag */
int tilemap_flag(TILEMAP * tmp, int layer, int flag, int x, int y)
{
    int ctx, cty;

	ctx = x / tmp->ts->w;
	cty = y / tmp->ts->h;
    if(ctx >= 0 && ctx < tmp->layer[layer]->mp->w && cty >= 0 && cty < tmp->layer[layer]->mp->h)
    {
        return tmp->ts->tile[tmp->ts->tile_index[tmp->layer[layer]->mp->line[cty][ctx]]]->flag[flag];
    }
    return 0;
}

int tilemap_flag_tile(TILEMAP * tmp, int layer, int flag, int x, int y)
{
    if(x >= 0 && x < tmp->layer[layer]->mp->w && y >= 0 && y < tmp->layer[layer]->mp->h)
    {
        return tmp->ts->tile[tmp->ts->tile_index[tmp->layer[layer]->mp->line[y][x]]]->flag[flag];
    }
    return 0;
}

int tile_flag(TILESET * tsp, int tile, int flag)
{
	return tsp->tile[tile]->flag[flag];
}

int get_tile(TILEMAP * tmp, int layer, int x, int y)
{
	if(x >= 0 && x < tmp->layer[layer]->mp->w * tmp->ts->w && y >= 0 && y < tmp->layer[layer]->mp->h * tmp->ts->h)
	{
		return tmp->ts->tile_index[tmp->layer[layer]->mp->line[y / tmp->ts->h][x / tmp->ts->w]];
	}
	return 0;
}

void tilemap_change_tile(TILEMAP * tmp, int layer, int t1, int t2)
{
    int i, j;

    for(i = 0; i < tmp->h; i++)
    {
        for(j = 0; j < tmp->w; j++)
        {
            if(tmp->ts->tile_index[tmp->layer[layer]->mp->line[i][j]] == t1)
            {
                tmp->layer[layer]->mp->line[i][j] = t2;
                tmp->ts->tile_index[t2] = t2;
            }
        }
    }
}

int tilemap_over_edge_h(TILEMAP * tmp, int x1, int x2)
{
	return x1 / tmp->ts->w != x2 / tmp->ts->w;
}

int tilemap_over_edge_v(TILEMAP * tmp, int y1, int y2)
{
	return y1 / tmp->ts->h != y2 / tmp->ts->h;
}

int tilemap_collidet_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    cmap_clear_hits_top(crp);
    if(crp->top.point_count > 0)
    {
	    if(crp->fy < crp->foy)
	    {
	       	/* check the points */
        	for(i = 0; i < crp->top.point_count; i++)
        	{
				/* see if we need to check collision */
		    	if((crp->y + crp->ry + crp->top.point[i].y) % tmp->ts->h > (crp->oy + crp->ry + crp->top.point[i].y) % tmp->ts->h)
		    	{
	                if(tilemap_flag(tmp, layer, TILEMAP_FLAG_SOLIDB, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y))
                	{
	                	crp->top.hit = i;
                    	return 1;
                	}
            	}
        	}
        }
    }
    return 0;
}

int tilemap_collideb_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    cmap_clear_hits_bottom(crp);
    if(crp->bottom.point_count > 0)
    {
	    if(crp->y > crp->oy)
	    {
           	for(i = 0; i < crp->bottom.point_count; i++)
           	{
	        	/* see if we need to check collision */
        		if((crp->y + crp->ry + crp->bottom.point[i].y) % tmp->ts->h < (crp->oy + crp->ry + crp->bottom.point[i].y) % tmp->ts->h)
        		{
		            /* check the points */
	                if(tilemap_flag(tmp, layer, TILEMAP_FLAG_SOLIDT, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y))
					{
	                	crp->bottom.hit = i;
                    	return 1;
                	}
            	}
        	}
//       		if((crp->y + crp->ry + crp->bottom.point[0].y) % tmp->ts->h < (crp->y + crp->ry + crp->bottom.point[crp->bottom.point_count - 2].y) % tmp->ts->h)
//       		{
	            /* check the points */
//                if(tilemap_flag(tmp, layer, TILEMAP_FLAG_SOLIDT, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y))
//				{
  //              	crp->bottom.hit = i;
    //               	return 1;
      //         	}
        //   	}
    	}
    }
    if(crp->bottom.fluff_start >= 0)
    {
	    if(crp->y >= crp->oy)
	    {
           	for(i = crp->bottom.fluff_start; i < crp->bottom.point_count; i++)
           	{
	       		if((crp->y + crp->ry + crp->bottom.point[i].y) % tmp->ts->h < (crp->y + crp->ry + crp->bottom.point[i - crp->bottom.fluff_start].y) % tmp->ts->h)
       			{
	                if(tilemap_flag(tmp, layer, TILEMAP_FLAG_SOLIDT, crp->x + crp->rx + crp->bottom.point[i - crp->bottom.fluff_start].x, crp->y + crp->ry + crp->bottom.point[i - crp->bottom.fluff_start].y))
					{
	                	crp->bottom.hit = i - crp->bottom.fluff_start;
                   		return 1;
					}
           		}
           	}
	    }
    }
    return 0;
}

int tilemap_collidel_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    cmap_clear_hits_left(crp);
    if(crp->left.point_count > 0)
    {
	    if(crp->fx < crp->fox)
	    {
            for(i = 0; i < crp->left.point_count; i++)
            {
	        	/* see if we need to check collision */
        		if((crp->x + crp->rx + crp->left.point[i].x) % tmp->ts->w > (crp->ox + crp->rx + crp->left.point[i].x) % tmp->ts->w)
        		{
		            /* check the points */
                	if(tilemap_flag(tmp, layer, TILEMAP_FLAG_SOLIDR, crp->x + crp->rx + crp->left.point[i].x, crp->y + crp->ry + crp->left.point[i].y))
                	{
		                crp->left.hit = i;
                    	return 1;
                	}
            	}
        	}
        }
    }
    return 0;
}

int tilemap_collider_check(TILEMAP * tmp, int layer, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    cmap_clear_hits_right(crp);
    if(crp->right.point_count > 0)
    {
	    if(crp->fx > crp->fox)
	    {
            /* check sprite right to tile left */
            for(i = 0; i < crp->right.point_count; i++)
            {
		        /* see if we need to check collision */
		        if((crp->x + crp->rx + crp->right.point[i].x) % tmp->ts->w < (crp->ox + crp->rx + crp->right.point[i].x) % tmp->ts->w)
		        {
	                if(tilemap_flag(tmp, layer, TILEMAP_FLAG_SOLIDL, crp->x + crp->rx + crp->right.point[i].x, crp->y + crp->ry + crp->right.point[i].y))
                	{
		                crp->right.hit = i;
                    	return 1;
                	}
                }
            }
        }
    }
    return 0;
}

int tilemap_toucht_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    if(crp->top.point_count > 0)
    {
		return tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[0].x, crp->y + crp->ry + crp->top.point[0].y - 1);
	}
	return 0;
}

int tilemap_touchb_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    if(crp->bottom.point_count > 0)
    {
		return tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[0].x, crp->y + crp->ry + crp->bottom.point[0].y + 1);
	}
	return 0;
}

int tilemap_touchl_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    if(crp->left.point_count > 0)
    {
		return tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[0].x - 1, crp->y + crp->ry + crp->left.point[0].y);
	}
	return 0;
}

int tilemap_touchr_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;

    /* see if there's at least one point to check */
    if(crp->right.point_count > 0)
    {
		return tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[0].x + 1, crp->y + crp->ry + crp->right.point[0].y);
	}
	return 0;
}

int tilemap_rubt_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int c = 0;

    /* see if there's at least one point to check */
    if(crp->top.point_count > 0)
    {
	    for(i = 0; i < crp->top.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y - 1) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y - 1) && tilemap_over_edge_v(tmp, crp->y + crp->ry + crp->top.point[i].y, crp->y + crp->ry + crp->top.point[i].y - 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y - 1);
			    return r;
		    }
	    }
	}
	return r;
}

int tilemap_rubb_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int c = 0;

    /* see if there's at least one point to check */
    if(crp->bottom.point_count > 0)
    {
	    for(i = 0; i < crp->bottom.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y + 1) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y + 1) && tilemap_over_edge_v(tmp, crp->y + crp->ry + crp->bottom.point[i].y, crp->y + crp->ry + crp->bottom.point[i].y + 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y + 1);
			    return r;
		    }
	    }
	}
	return r;
}

int tilemap_rubl_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int c = 0;

    /* see if there's at least one point to check */
    if(crp->left.point_count > 0)
    {
	    for(i = 0; i < crp->left.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x - 1, crp->y + crp->ry + crp->left.point[i].y) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x, crp->y + crp->ry + crp->left.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x - 1, crp->y + crp->ry + crp->left.point[i].y) && tilemap_over_edge_h(tmp, crp->x + crp->rx + crp->left.point[i].x, crp->x + crp->rx + crp->left.point[i].x - 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x - 1, crp->y + crp->ry + crp->left.point[i].y);
			    return r;
		    }
	    }
	}
	return r;
}

int tilemap_rubr_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int c = 0;

    /* see if there's at least one point to check */
    if(crp->right.point_count > 0)
    {
	    for(i = 0; i < crp->right.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x + 1, crp->y + crp->ry + crp->right.point[i].y) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x, crp->y + crp->ry + crp->right.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x + 1, crp->y + crp->ry + crp->right.point[i].y) && tilemap_over_edge_h(tmp, crp->x + crp->rx + crp->right.point[i].x, crp->x + crp->rx + crp->right.point[i].x + 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x + 1, crp->y + crp->ry + crp->right.point[i].y);
			    return r;
		    }
	    }
	}
	return r;
}

int tilemap_scrapet_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int no = 0;

    /* see if there's at least one point to check */
    if(crp->top.point_count > 0)
    {
	    for(i = 0; i < crp->top.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y - 1) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y - 1) && tilemap_over_edge_v(tmp, crp->y + crp->ry + crp->top.point[i].y, crp->y + crp->ry + crp->top.point[i].y - 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y - 1);
		    }
		    else
		    {
			    no = 1;
		    }
	    }
	}
	if(!no)
	{
		return r;
	}
	else
	{
		return 0;
	}
}

int tilemap_scrapeb_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int no = 0;

    /* see if there's at least one point to check */
    if(crp->bottom.point_count > 0)
    {
	    for(i = 0; i < crp->bottom.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y + 1) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y + 1) && tilemap_over_edge_v(tmp, crp->y + crp->ry + crp->bottom.point[i].y, crp->y + crp->ry + crp->bottom.point[i].y + 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y + 1);
		    }
		    else
		    {
			    no = 1;
		    }
	    }
	}
	if(!no)
	{
		return r;
	}
	else
	{
		return 0;
	}
}

int tilemap_scrapel_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int no = 0;

    /* see if there's at least one point to check */
    if(crp->left.point_count > 0)
    {
	    for(i = 0; i < crp->left.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x - 1, crp->y + crp->ry + crp->left.point[i].y) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x, crp->y + crp->ry + crp->left.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x - 1, crp->y + crp->ry + crp->left.point[i].y) && tilemap_over_edge_h(tmp, crp->x + crp->rx + crp->left.point[i].x, crp->x + crp->rx + crp->left.point[i].x - 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->left.point[i].x - 1, crp->y + crp->ry + crp->left.point[i].y);
		    }
		    else
		    {
			    no = 1;
		    }
	    }
	}
	if(!no)
	{
		return r;
	}
	else
	{
		return 0;
	}
}

int tilemap_scraper_check(TILEMAP * tmp, int layer, int flag, COLLISION_MAP * crp)
{
    int i;
    int r = 0;
    int no = 0;

    /* see if there's at least one point to check */
    if(crp->right.point_count > 0)
    {
	    for(i = 0; i < crp->right.point_count; i++)
	    {
//		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x + 1, crp->y + crp->ry + crp->right.point[i].y) && !tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x, crp->y + crp->ry + crp->right.point[i].y))
		    if(tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x + 1, crp->y + crp->ry + crp->right.point[i].y) && tilemap_over_edge_h(tmp, crp->x + crp->rx + crp->right.point[i].x, crp->x + crp->rx + crp->right.point[i].x + 1))
		    {
			    r = tilemap_flag(tmp, layer, flag, crp->x + crp->rx + crp->right.point[i].x + 1, crp->y + crp->ry + crp->right.point[i].y);
		    }
		    else
		    {
			    no = 1;
		    }
	    }
	}
	if(!no)
	{
		return r;
	}
	else
	{
		return 0;
	}
}

int tilemap_nudge(TILEMAP * tmp, int layer, COLLISION_MAP * crp)
{
	int ctile = 0, n = 0;

	crp->top.nudge = 0;
	crp->bottom.nudge = 0;
	crp->left.nudge = 0;
	crp->right.nudge = 0;

    /* check for nudging */
    ctile = get_tile(tmp, layer, crp->x + crp->rx, crp->y + crp->ry);
   	if(tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDT) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDB) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDL) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDR))
   	{
   		crp->top.nudge = 1;
   		crp->left.nudge = 1;
    	n = 1;
   	}
    ctile = get_tile(tmp, layer, crp->x + crp->rx + crp->w - 1, crp->y + crp->ry);
   	if(tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDT) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDB) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDL) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDR))
   	{
    	crp->top.nudge = 1;
    	crp->right.nudge = 1;
    	n = 1;
   	}
    ctile = get_tile(tmp, layer, crp->x + crp->rx, crp->y + crp->ry + crp->h - 1);
   	if(tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDT) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDB) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDL) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDR))
   	{
    	crp->bottom.nudge = 1;
    	crp->left.nudge = 1;
    	n = 1;
   	}
    ctile = get_tile(tmp, layer, crp->x + crp->rx + crp->w - 1, crp->y + crp->ry + crp->h - 1);
   	if(tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDT) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDB) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDL) && tile_flag(tmp->ts, ctile, TILEMAP_FLAG_SOLIDR))
   	{
    	crp->bottom.nudge = 1;
    	crp->right.nudge = 1;
    	n = 1;
   	}
   	return n;
}

int tilemap_collide(TILEMAP * tmp, COLLISION_MAP * crp)
{
    if(tilemap_collidet_check(tmp, tmp->il, crp) || tilemap_collideb_check(tmp, tmp->il, crp) || tilemap_collidel_check(tmp, tmp->il, crp) || tilemap_collider_check(tmp, tmp->il, crp))
    {
        return 1;
    }
    return 0;
}

int tilemap_fused(TILEMAP * tmp, COLLISION_MAP * crp)
{
	int i;

	if(crp->x < crp->ox)
	{
		for(i = 0; i < crp->left.point_count; i++)
		{
			if(tilemap_flag(tmp, tmp->il, TILEMAP_FLAG_SOLIDR, crp->x + crp->rx + crp->left.point[i].x, crp->y + crp->ry + crp->left.point[i].y))
			{
				crp->dir = CR_LEFT;
				return 1;
			}
		}
	}
	if(crp->x > crp->ox)
	{
		for(i = 0; i < crp->right.point_count; i++)
		{
			if(tilemap_flag(tmp, tmp->il, TILEMAP_FLAG_SOLIDL, crp->x + crp->rx + crp->right.point[i].x, crp->y + crp->ry + crp->right.point[i].y))
			{
				crp->dir = CR_RIGHT;
				return 1;
			}
		}
	}
	if(crp->y < crp->oy)
	{
		for(i = 0; i < crp->top.point_count; i++)
		{
			if(tilemap_flag(tmp, tmp->il, TILEMAP_FLAG_SOLIDB, crp->x + crp->rx + crp->top.point[i].x, crp->y + crp->ry + crp->top.point[i].y))
			{
				crp->dir = CR_UP;
				return 1;
			}
		}
	}
	if(crp->y > crp->oy)
	{
		for(i = 0; i < crp->bottom.point_count; i++)
		{
			if(tilemap_flag(tmp, tmp->il, TILEMAP_FLAG_SOLIDL, crp->x + crp->rx + crp->bottom.point[i].x, crp->y + crp->ry + crp->bottom.point[i].y))
			{
				crp->dir = CR_DOWN;
				return 1;
			}
		}
	}
    return 0;
}

/* draws specified layer of a tilemap */
void draw_tilemap(BITMAP * bp, TILEMAP * tmp, int layer)
{

    /* get the layer's screen coordinates */
    tmp->layer[layer]->x = (int)(tmp->x * tmp->layer[layer]->xm);
    tmp->layer[layer]->y = (int)(tmp->y * tmp->layer[layer]->ym);

    /* make sure we have a tileset before we try to draw */
    if(tmp)
    {
        if(!tmp->ts)
        {
            return;
        }
    }
    else
    {
        return;
    }

    /* draw in solid mode if it's the back layer */
    if(layer == 0)
    {
        draw_tilemap_layer_solid(bp, tmp->layer[layer], tmp->ts);
    }

    /* otherwise we draw normally */
    else
    {
        draw_tilemap_layer(bp, tmp->layer[layer], tmp->ts);
    }
}

/* draws layers before sprite layer */
void draw_tilemap_bg(BITMAP * bp, TILEMAP * tmp)
{
    int i;

    for(i = 0; i <= tmp->il && i < tmp->l ; i++)
    {
        draw_tilemap(bp, tmp, i);
    }
}

/* draws layers after sprite layer */
void draw_tilemap_fg(BITMAP * bp, TILEMAP * tmp)
{
    int i;

    for(i = tmp->il + 1; i < tmp->l; i++)
    {
        draw_tilemap(bp, tmp, i);
    }
}
