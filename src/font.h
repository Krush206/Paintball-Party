#ifndef NCD_H
#define NCD_H

#include <allegro.h>

#ifdef __cplusplus
extern "C"{
#endif

/* fixed size font structure */
typedef struct
{
    unsigned char w; /* width */
    unsigned char h; /* height */
    BITMAP * ch[256];
} NCD_FONT;

/* internal function used in editor */
NCD_FONT * create_ncd_font(int w, int h);

/* disk functions */
NCD_FONT * load_ncd_font_fp(PACKFILE * fp);
NCD_FONT * load_ncd_font(char * FileName);
int save_ncd_font_fp(NCD_FONT * nfp, PACKFILE * fp);
int save_ncd_font(NCD_FONT * nfp, char * FileName);
void destroy_ncd_font(NCD_FONT * nfp);

/* set some options */
void ncd_text_mode(int mode);
void ncd_text_color(int color);

/* text output functions */
void ncd_textout(BITMAP * b, NCD_FONT * f, int x, int y, char * s);
void ncd_textout_2x(BITMAP * b, NCD_FONT * f, int x, int y, char * s);
void ncd_textout_center(BITMAP * b, NCD_FONT * f, int y, char * s);
void ncd_textout_center_pivot(BITMAP * b, NCD_FONT * f, int px, int y, char * s);
void ncd_printf(BITMAP *b, NCD_FONT *f, int x, int y, char *format, ...);
void ncd_printf_center(BITMAP *b, NCD_FONT *f, int y, char *format, ...);
void ncd_printf_center_pivot(BITMAP *b, NCD_FONT *f, int px, int y, char *format, ...);
void ncd_printf_truncated(BITMAP *b, NCD_FONT *f, int x, int y, int length, char *format, ...);
void ncd_draw_number(BITMAP * b, NCD_FONT * f, int x, int y, long n, int d);
void ncd_draw_number_right(BITMAP * b, NCD_FONT * f, int x, int y, long n, int d);
void ncd_draw_number_left(BITMAP * b, NCD_FONT * f, int x, int y, long n, int d);
void ncd_show_text(BITMAP * bp, NCD_FONT * fp, int x, int y, char * tp);

#ifdef __cplusplus
}
#endif

#endif
