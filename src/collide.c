#include <allegro.h>
#include "collide.h"

int cmap_h_dir(COLLISION_MAP * cp)
{
	if(cp->fx < cp->fox)
	{
		return -1;
	}
	else if(cp->fx > cp->fox)
	{
		return 1;
	}
	return 0;
}

int cmap_v_dir(COLLISION_MAP * cp)
{
	if(cp->fy < cp->foy)
	{
		return -1;
	}
	else if(cp->fy > cp->foy)
	{
		return 1;
	}
	return 0;
}

fixed cmap_vx(COLLISION_MAP * cp)
{
	return cp->fx - cp->fox;
}

fixed cmap_vy(COLLISION_MAP * cp)
{
	return cp->fy - cp->foy;
}

/* does pixel-perfect collision detection */
int detect_collision(int x1, int y1, BITMAP * sprite1, int x2, int y2, BITMAP * sprite2)
{
    int has_collided;
    int bx1, by1, bx2, by2, bx3, by3; // Our rectangular intersection points
    int x, y, lenx, leny;  // Used for the detection loop

    has_collided = FALSE;  // Let's start with no preconceptions

    // First, do bounding-box detection
    if(((x1 + sprite1->w) >= x2) && (x1 < (x2 + sprite2->w)) && ((y1 + sprite1->h) >= y2) && (y1 < (y2 + sprite2->h)))
    {
        // Compute the intersection point
        bx1 = x1 - x2;
        if(bx1 < 0)
        {
            bx1 = 0;
        }
        by1 = y1 - y2;
        if(by1 < 0)
        {
            by1 = 0;
        }
        if(x1 < x2)
        {
            bx2 = x2 - x1;
        }
        else
        {
            bx2 = 0;
        }
        if(y1 < y2)
        {
            by2 = y2 - y1;
        }
        else
        {
            by2 = 0;
        }
        if(x1 > x2 + sprite2->w - sprite1->w)
        {
            bx3 = x2 + sprite2->w - x1;
        }
        else
        {
            bx3 = sprite1->w;
        }

        if(y1 > y2 + sprite2->h - sprite1->h)
        {
            by3 = y2 + sprite2->h - y1;
        }
        else
        {
            by3 = sprite1->h;
        }

        lenx = abs (bx2 - bx3);
        leny = abs (by2 - by3);

        // Now, do pixel-perfect detection on the intersection rectangle
        for(y = 0; y < leny; y++)
        {
            for(x = 0; x < lenx; x++)
            {
                if(sprite1->line[by2 + y][bx2 + x] != 0)
                {
                    if(sprite2->line[by1 + y][bx1 + x] != 0)
                    {
                        has_collided = TRUE;
                    }
                }
            }
        }
    }
    return has_collided;
}

int collide_rect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return ((y1 <= y2 + h2 - 1) && (y2 <= y1 + h1 - 1) && (x1 <= x2 + w2 - 1) && (x2 <= x1 + w1 - 1));
}

/* does simple rectangle collision */
int collide(COLLISION_MAP * rp1, COLLISION_MAP * rp2)
{
    return ((rp1->y + rp1->ry <= rp2->y + rp2->ry + rp2->h - 1) && (rp2->y + rp2->ry <= rp1->y + rp1->ry + rp1->h - 1) && (rp1->x + rp1->rx <= rp2->x + rp2->rx + rp2->w - 1) && (rp2->x + rp2->rx <= rp1->x + rp1->rx + rp1->w - 1));
}

int point_collide(int x, int y, COLLISION_MAP * rp)
{
    return (y <= rp->bottom.point[0].y + rp->y + rp->ry) && (rp->top.point[0].y + rp->y + rp->ry <= y) && (x <= rp->right.point[0].x + rp->x + rp->rx) && (rp->left.point[0].x + rp->x + rp->rx <= x);
}

int collide_top(COLLISION_MAP * rp1, COLLISION_MAP * rp2)
{
	if(collide(rp1, rp2))
	{
		if(rp1->y + rp1->ry <= rp2->y + rp2->ry + rp2->h - 1 && rp1->oy + rp1->ry > rp2->y + rp2->ry + rp2->h - 1)
		{
			return 1;
		}
	}
	return 0;
}

int collide_bottom(COLLISION_MAP * rp1, COLLISION_MAP * rp2)
{
	if(collide(rp1, rp2))
	{
		if(rp1->y + rp1->ry + rp1->h - 1 >= rp2->y + rp2->ry && rp1->oy + rp1->ry + rp1->h - 1 < rp2->y + rp2->ry)
		{
			return 1;
		}
	}
	return 0;
}

int collide_left(COLLISION_MAP * rp1, COLLISION_MAP * rp2)
{
	if(collide(rp1, rp2))
	{
		if(rp1->x + rp1->rx <= rp2->x + rp2->rx + rp2->w - 1 && rp1->ox + rp1->rx > rp2->x + rp2->rx + rp2->w - 1)
		{
			return 1;
		}
	}
	return 0;
}

int collide_right(COLLISION_MAP * rp1, COLLISION_MAP * rp2)
{
	if(collide(rp1, rp2))
	{
		if(rp1->x + rp1->rx + rp1->w - 1 >= rp2->x + rp2->rx && rp1->ox + rp1->rx + rp1->w - 1 < rp2->x + rp2->rx)
		{
			return 1;
		}
	}
	return 0;
}

/* adds a point to a collision list */
void add_collision_point(COLLISION_LIST * lp, int x, int y)
{
    lp->point[lp->point_count].x = x;
    lp->point[lp->point_count].y = y;
    lp->point_count++;
}

/* use this function in initialization process for each of your sprites */
void generate_collision_map(COLLISION_MAP * rp, int x, int y, int w, int h, int tw, int th)
{
    int i;

    /* set sprite relativity */
    rp->rx = x;
    rp->ry = y;
    rp->w = w;
    rp->h = h;

    /* clear the lists */
    rp->top.point_count = 0;
    rp->bottom.point_count = 0;
    rp->left.point_count = 0;
    rp->right.point_count = 0;

    /* add center point first (special purpose) */
    add_collision_point(&rp->top, w >> 1, 0);

    /* do top edge */
    for(i = 0; i < w; i += tw)
    {
        add_collision_point(&rp->top, i, 0);
    }

    /* add top right point */
    add_collision_point(&rp->top, w - 1, 0);
    rp->top.fluff_start = -1;


    /* add center point first (special purpose) */
    add_collision_point(&rp->bottom, w >> 1, h - 1);

    /* do bottom edge */
    for(i = 0; i < w; i += tw)
    {
        add_collision_point(&rp->bottom, i, h - 1);
    }

    /* add bottom right point */
    add_collision_point(&rp->bottom, w - 1, h - 1);
    rp->bottom.fluff_start = -1;


    /* add center point first (special purpose) */
    add_collision_point(&rp->left, 0, h >> 1);

    /* do left edge */
    for(i = 0; i < h; i += th)
    {
        add_collision_point(&rp->left, 0, i);
    }

    /* add bottom left point */
    add_collision_point(&rp->left, 0, h - 1);
    rp->left.fluff_start = -1;


    /* add center point first (special purpose) */
    add_collision_point(&rp->right, w - 1, h >> 1);

    /* do right edge */
    for(i = 0; i < h; i += th)
    {
        add_collision_point(&rp->right, w - 1, i);
    }

    /* add bottom right point */
    add_collision_point(&rp->right, w - 1, h - 1);
    rp->right.fluff_start = -1;
    rp->tnudge = 0;
    rp->bnudge = 0;
    rp->lnudge = 0;
    rp->rnudge = 0;
}

/* fillet the top of a cmap (useful for more lenient jumps) */
void cmap_fillet_top(COLLISION_MAP * rp, int amount)
{
	if(amount > rp->w / 2)
	{
		amount = rp->w / 2;
	}
	/* adjust the top of the map */
	rp->top.point[1].x += amount;
	rp->top.point[rp->top.point_count - 1].x -= amount;
    add_collision_point(&rp->top, 0, amount);
    add_collision_point(&rp->top, rp->w - 1, amount);

	/* adjust the left edge */
	rp->left.point[1].y += amount;
    add_collision_point(&rp->left, amount, 0);

	/* adjust the right edge */
	rp->right.point[1].y += amount;
    add_collision_point(&rp->right, rp->w - amount - 1, 0);
}

void cmap_fluff_bottom(COLLISION_MAP * rp, int amount)
{
	int i, count;

	count = rp->bottom.point_count;
	rp->bottom.fluff_start = rp->bottom.point_count;
	/* do bottom edge */
    for(i = 0; i < count; i++)
    {
        add_collision_point(&rp->bottom, rp->bottom.point[i].x, rp->bottom.point[i].y - amount);
    }
}

void cmap_clear_hits(COLLISION_MAP * rp)
{
	int i;

	for(i = 0; i < rp->top.point_count; i++)
	{
		rp->top.hit = -1;
	}
	for(i = 0; i < rp->bottom.point_count; i++)
	{
		rp->bottom.hit = -1;
	}
	for(i = 0; i < rp->left.point_count; i++)
	{
		rp->left.hit = -1;
	}
	for(i = 0; i < rp->right.point_count; i++)
	{
		rp->right.hit = -1;
	}
}

void cmap_clear_hits_top(COLLISION_MAP * rp)
{
	int i;

	for(i = 0; i < rp->top.point_count; i++)
	{
		rp->top.hit = -1;
	}
}

void cmap_clear_hits_bottom(COLLISION_MAP * rp)
{
	int i;

	for(i = 0; i < rp->bottom.point_count; i++)
	{
		rp->bottom.hit = -1;
	}
}

void cmap_clear_hits_left(COLLISION_MAP * rp)
{
	int i;

	for(i = 0; i < rp->left.point_count; i++)
	{
		rp->left.hit = -1;
	}
}

void cmap_clear_hits_right(COLLISION_MAP * rp)
{
	int i;

	for(i = 0; i < rp->right.point_count; i++)
	{
		rp->right.hit = -1;
	}
}

void cmap_draw(BITMAP * bp, COLLISION_MAP * rp, int x, int y, int color)
{
	int i;

	for(i = 0; i < rp->top.point_count; i++)
	{
		putpixel(bp, x + rp->rx + rp->top.point[i].x, y + rp->ry + rp->top.point[i].y, color);
	}
	if(rp->top.nudge)
	{
		textout_ex(bp, font, "Top", x, y, 31, -1);
	}
	for(i = 0; i < rp->bottom.point_count; i++)
	{
		putpixel(bp, x + rp->rx + rp->bottom.point[i].x, y + rp->ry + rp->bottom.point[i].y, color);
	}
	if(rp->bottom.nudge)
	{
		textout_ex(bp, font, "Bottom", x, y + 8, 31, -1);
	}
	for(i = 0; i < rp->left.point_count; i++)
	{
		putpixel(bp, x + rp->rx + rp->left.point[i].x, y + rp->ry + rp->left.point[i].y, color);
	}
	if(rp->left.nudge)
	{
		textout_ex(bp, font, "Left", x, y + 16, 31, -1);
	}
	for(i = 0; i < rp->right.point_count; i++)
	{
		putpixel(bp, x + rp->rx + rp->right.point[i].x, y + rp->ry + rp->right.point[i].y, color);
	}
	if(rp->right.nudge)
	{
		textout_ex(bp, font, "Right", x, y + 24, 31, -1);
	}
}

/* updates the position of a collision rectangle */
void update_collision_map(COLLISION_MAP * rp, int x, int y)
{
    int i;

    /* store old position so we know where we were */
    rp->ox = rp->x;
    rp->oy = rp->y;
    rp->fox = itofix(rp->x);
    rp->foy = itofix(rp->y);

    /* set the new position */
    rp->x = x;
    rp->y = y;
    rp->fx = itofix(rp->x);
    rp->fy = itofix(rp->y);
}

void fupdate_collision_map(COLLISION_MAP * rp, fixed x, fixed y)
{
    int i;

    /* store old position so we know where we were */
    rp->fox = rp->fx;
    rp->foy = rp->fy;
    rp->ox = fixtoi(rp->fx);
    rp->oy = fixtoi(rp->fy);

    /* set the new position */
    rp->fx = x;
    rp->fy = y;
    rp->x = fixtoi(x);
    rp->y = fixtoi(y);
}

/* updates the position of a collision rectangle */
void move_collision_map(COLLISION_MAP * rp, int x, int y)
{
    /* set the new position */
    rp->x = x;
    rp->y = y;
    rp->fx = itofix(rp->x);
    rp->fy = itofix(rp->y);
}

void fmove_collision_map(COLLISION_MAP * rp, fixed x, fixed y)
{
    /* set the new position */
    rp->fx = x;
    rp->fy = y;
    rp->x = fixtoi(x);
    rp->y = fixtoi(y);
}

int get_sprite_left_edge(COLLISION_MAP * cp, COLLISION_MAP * dp)
{
	return dp->x + dp->rx + dp->w - cp->rx;
}

int get_sprite_right_edge(COLLISION_MAP * cp, COLLISION_MAP * dp)
{
	return dp->x + dp->rx - (cp->rx + cp->w);
}

int get_sprite_top_edge(COLLISION_MAP * cp, COLLISION_MAP * dp)
{
	return dp->y + dp->ry + dp->h - cp->ry;
}

int get_sprite_bottom_edge(COLLISION_MAP * cp, COLLISION_MAP * dp)
{
	return dp->y + dp->ry - (cp->ry + cp->h);
}

int get_sprite_edge_x(COLLISION_MAP * srp, COLLISION_MAP * drp)
{
//	if(collide(srp, drp) && !collide_rect(srp->ox + srp->rx, srp->y + srp->ry, srp->w, srp->h, drp->x + drp->rx, drp->y + drp->ry, drp->w, drp->h))
	if(collide(srp, drp))
	{
		if(srp->fx < srp->fox)
		{
			return drp->x + drp->rx + drp->w - srp->rx;
		}
		else if(srp->fx > srp->fox)
		{
			return drp->x + drp->rx - (srp->rx + srp->w);
		}
		else
		{
			return srp->x;
		}
	}
	else
	{
		return srp->x;
	}
}

int get_sprite_edge_y(COLLISION_MAP * srp, COLLISION_MAP * drp)
{
	if(collide(srp, drp) && !collide_rect(srp->x + srp->rx, srp->oy + srp->ry, srp->w, srp->h, drp->x + drp->rx, drp->y + drp->ry, drp->w, drp->h))
	{
		if(srp->fy < srp->foy)
		{
			return drp->y + drp->ry + drp->h - srp->ry;
		}
		else if(srp->fy > srp->foy)
		{
			return drp->y + drp->ry - (srp->ry + srp->h);
		}
		else
		{
			return srp->y;
		}
	}
	else
	{
		return srp->y;
	}
}

/* gets sprite solid edge alignment value (use after collision) */
int get_sprite_x(COLLISION_MAP * rp, int tw)
{
    /* if sprite was moving left */
    if(rp->fx < rp->fox && rp->left.hit >= 0)
    {
//        return ((rp->x / tw) * tw) + tw - rp->rx;
        return (((rp->x + rp->left.point[rp->left.hit].x + rp->rx) / tw) * tw) + tw - (rp->rx + rp->left.point[rp->left.hit].x);
    }

    /* if sprite was moving right */
    else if(rp->fx > rp->fox && rp->right.hit >= 0)
    {
        return (((rp->x + rp->rx + rp->right.point[rp->right.hit].x) / tw) * tw) - (rp->right.point[rp->right.hit].x - rp->left.point[0].x) - 1 - rp->rx;
//        return (((rp->x + rp->rx + rp->right.point[0].x) / tw) * tw) - (rp->right.point[0].x - rp->left.point[0].x) - rp->rx;
    }

    else if(rp->lnudge && rp->left.nudge && !rp->right.nudge)
    {
        return (((rp->x + rp->rx) / tw) * tw) + tw - (rp->rx);
    }

    else if(rp->rnudge && rp->right.nudge && !rp->left.nudge)
    {
        return (((rp->x + rp->rx + rp->w - 1) / tw) * tw) - (rp->w - 1 - rp->left.point[0].x) - 1 - rp->rx;
    }

    /* if sprite wasn't moving */
    else
    {
        return rp->x;
    }
}

int cmap_find_bottom(COLLISION_MAP * rp)
{
	int i;
	int lowest = 0;

	for(i = 0; i < rp->bottom.point_count; i++)
	{
		if(rp->bottom.point[i].y > lowest)
		{
			lowest = rp->bottom.point[i].y;
		}
	}
	return lowest;
}

/* gets sprite solid edge alignment value (use after collision) */
int get_sprite_y(COLLISION_MAP * rp, int th)
{
	int lx, rx, ty, by;

    /* if sprite was moving up */
    if(rp->fy < rp->foy && rp->top.hit >= 0)
    {
        return (((rp->y + rp->top.point[rp->top.hit].y + rp->ry) / th) * th) + th - (rp->ry + rp->top.point[rp->top.hit].y);
    }

    /* if sprite was moving down */
    else if(rp->fy > rp->foy && rp->bottom.hit >= 0)
    {
		by = cmap_find_bottom(rp);
        return (((rp->y + rp->ry + by) / th) * th) - (by - rp->top.point[0].y) - 1 - (rp->ry);
    }

    else if(rp->tnudge && rp->top.nudge && !rp->bottom.nudge)
    {
        return (((rp->y + rp->ry) / th) * th) + th - (rp->ry);
    }

    else if(rp->bnudge && rp->bottom.nudge && !rp->top.nudge)
    {
        return (((rp->y + rp->ry + rp->h - 1) / th) * th) - (rp->h - rp->top.point[0].y) - 1 - (rp->ry);
    }

    /* if sprite wasn't moving */
    else
    {
        return rp->y;
    }
}
