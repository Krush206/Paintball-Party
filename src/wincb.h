#ifndef WINCB_H
#define WINCB_H

#include <allegro.h>

BITMAP * cb_get_bitmap(PALETTE pal);
void cb_put_bitmap(BITMAP * bmp, PALETTE pal);
int cb_full(void);

#endif
