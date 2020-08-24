#include <allegro.h>
#include "tilemap.h"
#include "itools.h"

extern int my_mouse_x, my_mouse_y, my_mouse_b;

void bgr_to_rgb(BITMAP * bp)
{
	int i, j, d;
	unsigned long pixel;
	int r, g, b;
	
	d = bitmap_color_depth(bp);
	if(d > 8)
	{
		for(i = 0; i < bp->h; i++)
		{
			for(j = 0; j < bp->w; j++)
			{
				pixel = getpixel(bp, j, i);
				r = getr_depth(d, pixel);
				g = getg_depth(d, pixel);
				b = getb_depth(d, pixel);
				putpixel(bp, j, i, makecol_depth(d, b, g, r));
			}
		}
	}
}

void convert_palette(BITMAP * bp, PALETTE p1, PALETTE p2)
{
    int i, j;
    int Closest;
    int Difference[3];
    unsigned char ColorMap[256];

    /* in case someone is using non-transparent black */
    int other_black = 0;

    /* figure out the color map */
    for(i = 0; i < 256; i++)
    {
        Closest = 1000;
        for(j = 0; j < 256; j++)
        {
            Difference[0] = p1[i].r - p2[j].r;
            if(Difference[0] < 0)
            {
                Difference[0] = -Difference[0];
            }
            Difference[1] = p1[i].g - p2[j].g;
            if(Difference[1] < 0)
            {
                Difference[1] = -Difference[1];
            }
            Difference[2] = p1[i].b - p2[j].b;
            if(Difference[2] < 0)
            {
                Difference[2] = -Difference[2];
            }
            if((Difference[0] + Difference[1] + Difference[2]) < Closest)
            {
                Closest = Difference[0] + Difference[1] + Difference[2];
                ColorMap[i] = j;
            }
        }
        /* map the "other" black */
        if(p2[i].r == 0 && p2[i].g == 0 && p2[i].b == 0)
        {
            other_black = i;
        }
    }

    /* map the colors to the picture */
    for(i = 0; i < bp->h; i++)
    {
        for(j = 0; j < bp->w; j++)
        {
            /* do this if non-transparent black */
            if(bp->line[i][j] != 0 && p1[bp->line[i][j]].r == 0 && p1[bp->line[i][j]].g == 0 && p1[bp->line[i][j]].b == 0)
            {
                bp->line[i][j] = other_black;
            }

            /* otherwise convert normally */
            else
            {
                bp->line[i][j] = ColorMap[bp->line[i][j]];
            }
        }
    }
}

void convert_palette_ani(ANIMATION * ap, PALETTE p1, PALETTE p2)
{
	int i;

	for(i = 0; i < ap->f; i++)
	{
	    convert_palette(ap->image[i], p1, p2);
	}
}

BITMAP * turn_bitmap(BITMAP * bp)
{
	BITMAP * image;
	int i, j, k, l;
	
	image = create_bitmap(bp->h, bp->w);
	clear_bitmap(image);
	for(i = 0; i < image->h; i++)
	{
		for(j = 0; j < image->w; j++)
		{
			image->line[i][j] = bp->line[j][bp->w - i - 1];
		}
	}
	destroy_bitmap(bp);
	return image;
}

void tool_replace(BITMAP * bp, int x, int y, int c)
{
    int i, j, oc;

    oc = bp->line[y][x];
    for(i = 0; i < bp->h; i++)
    {
        for(j = 0; j < bp->w; j++)
        {
            if(bp->line[i][j] == oc)
            {
                bp->line[i][j] = c;
            }
        }
    }
}

void tool_pixel(BITMAP * bp, int x, int y, int c)
{
    bp->line[y][x] = c;
}

void tool_fill(BITMAP * bp, int x, int y, int c)
{
    floodfill(bp, x, y, c);
}

void tool_line(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int px = (mouse_x - dx) / s;
    int py = (mouse_y - dy) / s;
    BITMAP * cbp = create_bitmap(bp->w, bp->h);
    int mbp = 1;
    
    blit(bp, cbp, 0, 0, 0, 0, bp->w, bp->h);
    while(mbp)
    {
	    read_mouse();
	    mbp = (b == 1 ? my_mouse_b & 1 : my_mouse_b & 2);
        blit(cbp, bp, 0, 0, 0, 0, bp->w, bp->h);
        line(bp, px, py, (mouse_x - dx) / s, (mouse_y - dy) / s, c);
        proc();
    }
    destroy_bitmap(cbp);
}

void tool_rect(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int px = (mouse_x - dx) / s;
    int py = (mouse_y - dy) / s;
    BITMAP * cbp = create_bitmap(bp->w, bp->h);
    int mbp = 1;

    blit(bp, cbp, 0, 0, 0, 0, bp->w, bp->h);
    while(mbp)
    {
	    read_mouse();
	    mbp = (b == 1 ? my_mouse_b & 1 : my_mouse_b & 2);
        blit(cbp, bp, 0, 0, 0, 0, bp->w, bp->h);
        rect(bp, px, py, (mouse_x - dx) / s, (mouse_y - dy) / s, c);
        proc();
    }
    destroy_bitmap(cbp);
}

void tool_frect(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int px = (mouse_x - dx) / s;
    int py = (mouse_y - dy) / s;
    BITMAP * cbp = create_bitmap(bp->w, bp->h);
    int mbp = 1;

    blit(bp, cbp, 0, 0, 0, 0, bp->w, bp->h);
    while(mbp)
    {
	    read_mouse();
	    mbp = (b == 1 ? my_mouse_b & 1 : my_mouse_b & 2);
        blit(cbp, bp, 0, 0, 0, 0, bp->w, bp->h);
        rectfill(bp, px, py, (mouse_x - dx) / s, (mouse_y - dy) / s, c);
        proc();
    }
    destroy_bitmap(cbp);
}

void tool_circle(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int px = (mouse_x - dx) / s;
    int py = (mouse_y - dy) / s;
    int nx, ny;
    BITMAP * cbp = create_bitmap(bp->w, bp->h);
    int mbp = 1;

    blit(bp, cbp, 0, 0, 0, 0, bp->w, bp->h);
    while(mbp)
    {
	    read_mouse();
	    mbp = (b == 1 ? my_mouse_b & 1 : my_mouse_b & 2);
        nx = (mouse_x - dx) / s - px;
        ny = (mouse_y - dy) / s - py;
        blit(cbp, bp, 0, 0, 0, 0, bp->w, bp->h);
        if(nx != 0 && ny != 0)
        {
            if(nx / 2 == ny / 2)
            {
                circle(bp, nx / 2 + px, ny / 2 + py, nx < 0 ? -nx / 2 : nx / 2, c);
            }
            else
            {
                ellipse(bp, nx / 2 + px, ny / 2 + py, nx < 0 ? -nx / 2 : nx / 2, ny < 0 ? -ny / 2 : ny / 2, c);
            }
        }
        proc();
    }
    destroy_bitmap(cbp);
}

void tool_fcircle(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)())
{
    int px = (mouse_x - dx) / s;
    int py = (mouse_y - dy) / s;
    int nx, ny;
    BITMAP * cbp = create_bitmap(bp->w, bp->h);
    int mbp = 1;

    blit(bp, cbp, 0, 0, 0, 0, bp->w, bp->h);
    while(mbp)
    {
	    read_mouse();
	    mbp = (b == 1 ? my_mouse_b & 1 : my_mouse_b & 2);
        nx = (mouse_x - dx) / s - px;
        ny = (mouse_y - dy) / s - py;
        blit(cbp, bp, 0, 0, 0, 0, bp->w, bp->h);
        if(nx != 0 && ny != 0)
        {
            if(nx / 2 == ny / 2)
            {
                circlefill(bp, nx / 2 + px, ny / 2 + py, nx < 0 ? -nx / 2 : nx / 2, c);
            }
            else
            {
                ellipsefill(bp, nx / 2 + px, ny / 2 + py, nx < 0 ? -nx / 2 : nx / 2, ny < 0 ? -ny / 2 : ny / 2, c);
            }
        }
        proc();
    }
    destroy_bitmap(cbp);
}
