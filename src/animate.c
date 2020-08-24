#include <allegro.h>
#include "animate.h"

/* free an animation from memory */
void destroy_ani(ANIMATION * ap)
{
    int i;

    /* destroy it */
    if(ap != NULL)
    {
        /* destroy the bitmaps */
        for(i = 0; i < ap->f; i++)
        {
            if(ap->image[i] != NULL)
            {
                destroy_bitmap(ap->image[i]);
            }
        }

        /* free the structure */
        free(ap);
        ap = NULL;
    }
}

/* load animation from a file pointer */
ANIMATION * load_ani_fp(PACKFILE * fp, PALETTE pal)
{
    ANIMATION * ap;
    char header[4];
    int i, j, k;

    /* check format */
    pack_fread(header, 4, fp);
    if(header[0] != 'A' || header[1] != 'N' || header[2] != 'I' || header[3] != 21)
    {
        return NULL;
    }

    ap = (ANIMATION *)malloc(sizeof(ANIMATION));

    /* load header */
    ap->w = pack_igetl(fp);
    ap->h = pack_igetl(fp);
    ap->f = pack_igetl(fp);
    ap->d = pack_igetl(fp);

    /* load palette data */
    j = pack_getc(fp);
    if(j)
    {
        if(pal != NULL)
        {
            for(i = 0; i < 256; i++)
            {
                pal[i].r = pack_getc(fp);
                pal[i].g = pack_getc(fp);
                pal[i].b = pack_getc(fp);
            }
        }
        else
        {
            for(i = 0; i < 256 * 3; i++)
            {
                pack_getc(fp);
            }
        }
    }

    /* load animation data */
    for(i = 0; i < ap->f; i++)
    {
        ap->image[i] = create_bitmap(ap->w, ap->h);
        for(j = 0; j < ap->h; j++)
        {
            for(k = 0; k < ap->w; k++)
            {
                ap->image[i]->line[j][k] = pack_getc(fp);
            }
        }
    }

    return ap;
}

/* load an animation from a filename */
ANIMATION * load_ani(char * fn, PALETTE pal)
{
    PACKFILE * fp;
    ANIMATION * ap;

    /* open the file */
    fp = pack_fopen(fn, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    /* read the animation data */
    ap = load_ani_fp(fp, pal);
    pack_fclose(fp);

    return ap;
}

ANIMATION * duplicate_ani(ANIMATION * oap)
{
	ANIMATION * ap;
	int i, j, k;

	ap = (ANIMATION *)malloc(sizeof(ANIMATION));

    /* load header */
    ap->w = oap->w;
    ap->h = oap->h;
    ap->f = oap->f;
    ap->d = oap->d;

    /* load animation data */
    for(i = 0; i < ap->f; i++)
    {
        ap->image[i] = create_bitmap(ap->w, ap->h);
        for(j = 0; j < ap->h; j++)
        {
            for(k = 0; k < ap->w; k++)
            {
                ap->image[i]->line[j][k] = oap->image[i]->line[j][k];
            }
        }
    }

    return ap;
}

/* save animation to a file pointer */
int save_ani_fp(ANIMATION * ap, PACKFILE * fp, PALETTE pal)
{
    char header[4] = {'A', 'N', 'I', 21};
    int i, j, k;

    /* write format header */
    pack_fwrite(header, 4, fp);

    /* save header */
    pack_iputl(ap->w, fp);
    pack_iputl(ap->h, fp);
    pack_iputl(ap->f, fp);
    pack_iputl(ap->d, fp);

    /* save palette data */
    if(pal != NULL)
    {
        pack_putc(1, fp);
        for(i = 0; i < 256; i++)
        {
            pack_putc(pal[i].r, fp);
            pack_putc(pal[i].g, fp);
            pack_putc(pal[i].b, fp);
        }
    }
    else
    {
        pack_putc(0, fp);
    }

    /* save animation data */
    for(i = 0; i < ap->f; i++)
    {
        for(j = 0; j < ap->h; j++)
        {
            for(k = 0; k < ap->w; k++)
            {
                pack_putc(ap->image[i]->line[j][k], fp);
            }
        }
    }
    return 1;
}

/* save an animation */
int save_ani(ANIMATION * ap, char * fn, PALETTE pal)
{
    PACKFILE * fp;

    fp = pack_fopen(fn, "w");
    if(fp == NULL)
    {
        return 0;
    }

    /* save the animation */
    if(save_ani_fp(ap, fp, pal))
    {
        pack_fclose(fp);
        return 1;
    }
    pack_fclose(fp);

    return 0;
}

/* create an empty animation of the specified width and height */
ANIMATION * create_ani(int w, int h)
{
    ANIMATION * ap;

    ap = (ANIMATION *)malloc(sizeof(ANIMATION));

    ap->w = w;
    ap->h = h;
    ap->f = 0;
    ap->d = 1;

    return ap;
}

/* clip a portion of an animation */
ANIMATION * clip_ani(ANIMATION * ap, int x, int y, int w, int h)
{
	ANIMATION * rap;
	int i;

	rap = create_ani(w, h);
	rap->f = ap->f;
	rap->d = ap->d;
	for(i = 0; i < ap->f; i++)
	{
		rap->image[i] = create_bitmap(w, h);
		if(!rap->image[i])
		{
			return NULL;
		}
		clear_bitmap(rap->image[i]);
		draw_sprite(rap->image[i], ap->image[i], -x, -y);
	}
	return rap;
}

/* mask a portion of an animation */
ANIMATION * mask_ani(ANIMATION * ap, int x, int y, int w, int h)
{
	ANIMATION * rap;
	BITMAP * scratch;
	int i;

	rap = create_ani(ap->w, ap->h);
	rap->f = ap->f;
	rap->d = ap->d;
	scratch = create_bitmap(w, h);
	for(i = 0; i < ap->f; i++)
	{
		rap->image[i] = create_bitmap(ap->w, ap->h);
		if(!rap->image[i])
		{
			return NULL;
		}
		clear_bitmap(scratch);
		draw_sprite(scratch, ap->image[i], -x, -y);
		draw_sprite(rap->image[i], scratch, x, y);
	}
	destroy_bitmap(scratch);
	return rap;
}

/* add frame to end of animation */
int animation_add_frame(ANIMATION * ap, BITMAP * bp)
{
    if(ap->f < ANIMATION_MAX_FRAMES && ap != NULL)
    {
        /* create the new frame */
        ap->image[ap->f] = create_bitmap(ap->w, ap->h);
        if(ap->image[ap->f] == NULL)
        {
            return 0;
        }
        clear_bitmap(ap->image[ap->f]);
        blit(bp, ap->image[ap->f], 0, 0, 0, 0, ap->w, ap->h);
        ap->f++;

        return 1;
    }
    return 0;
}

/* insert a frame of animation */
int animation_insert_frame(ANIMATION * ap, int f, BITMAP * bp)
{
    int i;

    /* make sure we are inserting image to appropriate place */
    if(f < ap->f && ap->f < ANIMATION_MAX_FRAMES && ap != NULL)
    {
        ap->image[ap->f] = create_bitmap(ap->w, ap->h);

        /* make room for the inserted frame */
        for(i = ap->f; i > f; i--)
        {
            blit(ap->image[i - 1], ap->image[i], 0, 0, 0, 0, ap->w, ap->h);
        }

        /* insert the new image */
        clear_bitmap(ap->image[f]);
        blit(bp, ap->image[f], 0, 0, 0, 0, ap->w, ap->h);
        ap->f++;

        return 1;
    }
    return 0;
}

int animation_replace_frame(ANIMATION * ap, int f, BITMAP * bp)
{
    if(f < ap->f && ap != NULL)
    {
        clear_bitmap(ap->image[f]);
        blit(bp, ap->image[f], 0, 0, 0, 0, ap->w, ap->h);
        return 1;
    }
    return 0;
}

int animation_delete_frame(ANIMATION * ap, int f)
{
    int i;

    if(f < ap->f && ap != NULL)
    {
        /* move other frames back one position */
        for(i = f; i < ap->f - 1; i++)
        {
            blit(ap->image[i + 1], ap->image[i], 0, 0, 0, 0, ap->w, ap->h);
        }
        destroy_bitmap(ap->image[ap->f - 1]);

        /* adjust frame counter */
        ap->f--;

        return 1;
    }
    return 0;
}

int animation_copy_frame(ANIMATION * ap, int f, BITMAP * bp)
{
    if(f < ap->f && ap != NULL)
    {
        /* create and copy */
        bp = create_bitmap(ap->w, ap->h);
        blit(ap->image[f], bp, 0, 0, 0, 0, ap->w, ap->h);

        return 1;
    }
    return 0;
}

BITMAP * get_ani(ANIMATION * ap, int f)
{
    int i = !ap->d ? f % ap->f : (f / ap->d) % ap->f;

    return ap->image[i];
}

BITMAP * get_ani_frame(ANIMATION * ap, int f)
{
    return ap->image[f];
}

/* draw the specified frame */
int blit_ani_frame(ANIMATION * ap, BITMAP * bp, int x, int y, int f)
{
    if(f < ap->f && ap != NULL)
    {
        blit(ap->image[f], bp, 0, 0, x, y, bp->w, bp->h);

        return 1;
    }
    return 0;
}

/* similar to blit but for animations */
int blit_ani(ANIMATION * ap, BITMAP * bp, int x, int y, unsigned long f)
{
    int i;

    if(ap != NULL)
    {
        /* make sure there are frames to draw */
        if(ap->f > 0)
        {
            /* decide which frame to draw */
            i = !ap->d ? f % ap->f : (f / ap->d) % ap->f;
            blit(ap->image[i], bp, 0, 0, x, y, bp->w, bp->h);

            return 1;
        }
    }
    return 0;
}

/* like draw_sprite but for animations (pick frame based on f) */
int draw_ani(BITMAP * bp, ANIMATION * ap, int x, int y, unsigned long f)
{
    int i;

    if(ap != NULL)
    {
        /* make sure there are frames to draw */
        if(ap->f > 0)
        {
            /* decide which frame to draw */
            i = !ap->d ? f % ap->f : (f / ap->d) % ap->f;
            draw_sprite(bp, ap->image[i], x, y);

            return 1;
        }
    }
    return 0;
}

/* like draw_ani but doesn't calculate which frame to draw */
int draw_ani_frame(BITMAP * bp, ANIMATION * ap, int x, int y, int f)
{
    if(ap != NULL)
    {
        if(f < ap->f)
        {
            draw_sprite(bp, ap->image[f], x, y);

            return 1;
        }
    }
    return 0;
}

/* like draw_trans_sprite but for animations */
int draw_trans_ani(BITMAP * bp, ANIMATION * ap, int x, int y, unsigned long f)
{
    int i;

    if(ap != NULL)
    {
        /* make sure there are frames to draw */
        if(ap->f > 0)
        {
            /* decide which frame to draw */
            i = !ap->d ? f % ap->f : (f / ap->d) % ap->f;
            draw_trans_sprite(bp, ap->image[i], x, y);

            return 1;
        }
    }
    return 0;
}

/* like draw_trans_ani but doesn't calculate which frame to draw */
int draw_trans_ani_frame(BITMAP * bp, ANIMATION * ap, int x, int y, unsigned long f)
{
    if(ap != NULL)
    {
        if(f < ap->f)
        {
            draw_trans_sprite(bp, ap->image[f], x, y);

            return 1;
        }
    }
    return 0;
}

/* like draw_character but for animations */
int draw_solid_ani(BITMAP * bp, ANIMATION * ap, int x, int y, int color, unsigned long f)
{
    int i;

    if(ap != NULL)
    {
        /* make sure there are frames to draw */
        if(ap->f > 0)
        {
            /* decide which frame to draw */
            i = !ap->d ? f % ap->f : (f / ap->d) % ap->f;
            draw_character_ex(bp, ap->image[i], x, y, color, -1);

            return 1;
        }
    }
    return 0;
}

/* like draw_solid_ani but doesn't calculate frame */
int draw_solid_ani_frame(BITMAP * bp, ANIMATION * ap, int x, int y, int color, unsigned long f)
{
    if(ap != NULL)
    {
        if(f < ap->f)
        {
            draw_character_ex(bp, ap->image[f], x, y, color, -1);

            return 1;
        }
    }
    return 0;
}
