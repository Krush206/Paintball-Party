#ifndef ANIMATE_H
#define ANIMATE_H

#include <allegro.h>

#ifdef __cplusplus
extern "C"{
#endif

#define ANIMATION_MAX_FRAMES 256

/* animation structure */
typedef struct
{
    BITMAP * image[ANIMATION_MAX_FRAMES];
    int w; /* width */
    int h; /* height */
    int f; /* frames */
    int d; /* speed (in delay frames) */
} ANIMATION;

/* disk functions */
ANIMATION * load_ani_fp(PACKFILE * fp, PALETTE pal);
ANIMATION * load_ani(char * fn, PALETTE pal);
ANIMATION * duplicate_ani(ANIMATION * ap);
int save_ani_fp(ANIMATION * ap, PACKFILE * fp, PALETTE pal);
int save_ani(ANIMATION * ap, char * fn, PALETTE pal);
void destroy_ani(ANIMATION * ap);

/* used to construct animations (used in editor) */
void initialize_ani(ANIMATION * ap);
ANIMATION * create_ani(int w, int h);
ANIMATION * clip_ani(ANIMATION * ap, int x, int y, int w, int h);
ANIMATION * mask_ani(ANIMATION * ap, int x, int y, int w, int h);
int animation_add_frame(ANIMATION * ap, BITMAP * bp);
int animation_insert_frame(ANIMATION * ap, int f, BITMAP * bp);
int animation_replace_frame(ANIMATION * ap, int f, BITMAP * bp);
int animation_delete_frame(ANIMATION * ap, int f);
int animation_copy_frame(ANIMATION * ap, int f, BITMAP * bp);

/* helper functions for the programmer */
BITMAP * get_ani(ANIMATION * ap, int f);
BITMAP * get_ani_frame(ANIMATION * ap, int f);

/* drawing functions */
int blit_ani(ANIMATION * ap, BITMAP * bp, int x, int y, unsigned long f);
int blit_ani_frame(ANIMATION * ap, BITMAP * bp, int x, int y, int f);
int draw_ani(BITMAP * bp, ANIMATION * ap, int x, int y, unsigned long f);
int draw_ani_frame(BITMAP * bp, ANIMATION * ap, int x, int y, int f);
int draw_trans_ani(BITMAP * bp, ANIMATION * ap, int x, int y, unsigned long f);
int draw_trans_ani_frame(BITMAP * bp, ANIMATION * ap, int x, int y, unsigned long f);
int draw_solid_ani(BITMAP * bp, ANIMATION * ap, int x, int y, int color, unsigned long f);
int draw_solid_ani_frame(BITMAP * bp, ANIMATION * ap, int x, int y, int color, unsigned long f);

#ifdef __cplusplus
}
#endif

#endif
