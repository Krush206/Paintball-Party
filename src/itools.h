#ifndef ITOOLS_H
#define ITOOLS_H

#include <allegro.h>
#include "animate.h"
#include "tilemap.h"

#define TOOL_PIXEL   0
#define TOOL_LINE    1
#define TOOL_RECT    2
#define TOOL_FRECT   3
#define TOOL_CIRCLE  4
#define TOOL_FCIRCLE 5
#define TOOL_FILL    6
#define TOOL_REPLACE 7
#define TOOL_SELECT  8

void bgr_to_rgb(BITMAP * bp);
void convert_palette(BITMAP * bp, PALETTE p1, PALETTE p2);
void convert_palette_ani(ANIMATION * ap, PALETTE p1, PALETTE p2);
BITMAP * turn_bitmap(BITMAP * bp);
void tool_replace(BITMAP * bp, int x, int y, int c);
void tool_pixel(BITMAP * bp, int x, int y, int c);
void tool_fill(BITMAP * bp, int x, int y, int c);
void tool_line(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)());
void tool_rect(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)());
void tool_frect(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)());
void tool_circle(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)());
void tool_fcircle(BITMAP * bp, int b, int x, int y, int dx, int dy, int s, int c, void (*proc)());
void tool(void);

#endif
