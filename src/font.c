#include <allegro.h>
#include <stdarg.h>
#include <string.h>
#include "font.h"

int text_mode_ncd = 0;    /* default to full color drawing */
int text_color_ncd = 255; /* default text color */

/* create a blank font, useful for importing bitmaps */
NCD_FONT * create_ncd_font(int w, int h)
{
    int i;
    BITMAP * image;
    NCD_FONT * nfp;

    nfp = (NCD_FONT *)malloc(sizeof(NCD_FONT));
    if(nfp == NULL)
    {
        return NULL;
    }
    nfp->w = w;
    nfp->h = h;
    for(i = 0; i < 256; i++)
    {
        nfp->ch[i] = create_bitmap(w, h);
        if(nfp->ch[i] == NULL)
        {
            return NULL;
        }
        clear(nfp->ch[i]);
    }

    image = create_bitmap(8, 8);
    if(image == NULL)
    {
        return NULL;
    }
    for(i = 0; i < 256; i++)
    {
        clear(image);
        textprintf_ex(image, font, 0, 0, 15, -1, "%c", i);
        stretch_sprite(nfp->ch[i], image, 0, 0, nfp->w, nfp->h);
    }
    destroy_bitmap(image);

    return nfp;
}

NCD_FONT * load_ncd_font_fp(PACKFILE * fp)
{
    int i, j, k;
    char header[4];
    NCD_FONT * nfp;

    /* read header */
    pack_fread(&header, 4, fp);

    /* if header is wrong, return 0 */
    if(header[0] != 'M' || header[1] != 'F' || header[2] != 'T' || header[3] != 21)
    {
        return NULL;
    }

    nfp = (NCD_FONT *)malloc(sizeof(NCD_FONT));
    if(nfp == NULL)
    {
        return NULL;
    }

    /* read font dimensions */
    nfp->w = pack_getc(fp);
    nfp->h = pack_getc(fp);

    /* read font data to file */
    for(i = 0; i < 256; i++)
    {
        nfp->ch[i] = create_bitmap(nfp->w, nfp->h);
        if(nfp->ch[i] == NULL)
        {
            return NULL;
        }
        for(j = 0; j < nfp->h; j++)
        {
            for(k = 0; k < nfp->w; k++)
            {
                nfp->ch[i]->line[j][k] = pack_getc(fp);
            }
        }
    }

    return nfp;
}

/* load a font from specified file */
NCD_FONT * load_ncd_font(char * fn)
{
    PACKFILE * fp;
    NCD_FONT * nfp;

    /* open file */
    fp = pack_fopen(fn, "r");

    /* return 0 if file couldn't be opened */
    if(fp == NULL)
    {
        return NULL;
    }

    nfp = load_ncd_font_fp(fp);
    pack_fclose(fp);

    return nfp;
}

int save_ncd_font_fp(NCD_FONT * nfp, PACKFILE * fp)
{
    int i, j, k;
    char header[4] = {'M', 'F', 'T', 21};

    /* write header */
    pack_fwrite(&header, 4, fp);

    /* write font dimensions */
    pack_putc(nfp->w, fp);
    pack_putc(nfp->h, fp);

    /* write font data to file */
    for(i = 0; i < 256; i++)
    {
        for(j = 0; j < nfp->h; j++)
        {
            for(k = 0; k < nfp->w; k++)
            {
                pack_putc(nfp->ch[i]->line[j][k], fp);
            }
        }
    }

    return 1;
}

/* create a font file with specified filename */
int save_ncd_font(NCD_FONT * nfp, char * fn)
{
    PACKFILE * fp;

    /* open file */
    fp = pack_fopen(fn, "w");

    /* return 0 if file couldn't be opened */
    if(fp == NULL)
    {
        return 0;
    }

    if(!save_ncd_font_fp(nfp, fp))
    {
        pack_fclose(fp);
        return 0;
    }

    /* close the file and return 1 on success */
    pack_fclose(fp);
    return 1;
}

/* free font from memory */
void destroy_ncd_font(NCD_FONT * fp)
{
    int i;

    for(i = 0; i < 256; i++)
    {
        if(fp->ch[i] != NULL)
        {
            destroy_bitmap(fp->ch[i]);
        }
    }
    free(fp);
}

void ncd_text_mode(int mode)
{
    text_mode_ncd = mode;
}

void ncd_text_color(int color)
{
    text_color_ncd = color;
}

/* prints a string using specified NCD_FONT */
void ncd_textout(BITMAP * b, NCD_FONT * f, int x, int y, char * s)
{
    int i;

    for(i = 0; s[i] != '\0'; i++)
    {
        if(text_mode_ncd == 0)
        {
            draw_sprite(b, f->ch[(int)s[i]], x + i * f->w, y);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch[(int)s[i]], x + i * f->w, y, text_color_ncd, -1);
        }
    }
}

void ncd_textout_2x(BITMAP * b, NCD_FONT * f, int x, int y, char * s)
{
    int i;

    for(i = 0; s[i] != '\0'; i++)
    {
        if(text_mode_ncd == 0)
        {
            stretch_sprite(b, f->ch[(int)s[i]], x + i * (f->w * 2), y, f->w * 2, f->h * 2);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch[(int)s[i]], x + i * (f->w * 2), y, text_color_ncd, -1);
        }
    }
}

/* prints a string using specified NCD_FONT */
void ncd_textout_center(BITMAP * b, NCD_FONT * f, int y, char * s)
{
    int i, x;

    x = (b->w / 2) - ((strlen(s) * f->w) / 2);

    for(i = 0; s[i] != '\0'; i++)
    {
        if(text_mode_ncd == 0)
        {
            draw_sprite(b, f->ch[(int)s[i]], x + i * f->w, y);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch[(int)s[i]], x + i * f->w, y, text_color_ncd, -1);
        }
    }
}

/* prints a string using specified NCD_FONT */
void ncd_textout_center_pivot(BITMAP * b, NCD_FONT * f, int px, int y, char * s)
{
    int i, x;

//    x = (b->w / 2) - ((strlen(s) * f->w) / 2);
    x = px - ((strlen(s) * f->w) / 2);

    for(i = 0; s[i] != '\0'; i++)
    {
        if(text_mode_ncd == 0)
        {
            draw_sprite(b, f->ch[(int)s[i]], x + i * f->w, y);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch[(int)s[i]], x + i * f->w, y, text_color_ncd, -1);
        }
    }
}

/* formatted output using a NCD_FONT */
void ncd_printf(BITMAP *b, NCD_FONT *f, int x, int y, char *format, ...)
{
   char buf[1024];

   va_list ap;
   va_start(ap, format);
   vsprintf(buf, format, ap);
   va_end(ap);

   ncd_textout(b, f, x, y, buf);
}

/* formatted output using a NCD_FONT */
void ncd_printf_center(BITMAP *b, NCD_FONT *f, int y, char *format, ...)
{
   char buf[1024];

   va_list ap;
   va_start(ap, format);
   vsprintf(buf, format, ap);
   va_end(ap);

   ncd_textout_center(b, f, y, buf);
}

/* formatted output using a NCD_FONT */
void ncd_printf_center_pivot(BITMAP *b, NCD_FONT *f, int px, int y, char *format, ...)
{
   char buf[1024];

   va_list ap;
   va_start(ap, format);
   vsprintf(buf, format, ap);
   va_end(ap);

   ncd_textout_center_pivot(b, f, px, y, buf);
}

/* formatted output using a NCD_FONT (limited length) */
void ncd_printf_truncated(BITMAP *b, NCD_FONT *f, int x, int y, int length, char *format, ...)
{
    char buf[1024];

    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);

    buf[length] = '\0';
    ncd_textout(b, f, x, y, buf);
}

/* draws number using specified NCD_FONT */
void ncd_draw_number(BITMAP * b, NCD_FONT * f, int x, int y, long n, int d)
{
    char NUMBER[11]; /* array with maximum of 10 digits */
    int i;          /* looping variable */
    int j;           /* horizontal drawing position */

    /* converts <Number> to string <NUMBER> using base 10 */
    if(n < 0)
    {
	    sprintf(NUMBER, "%d", -n);
    }
    else
    {
	    sprintf(NUMBER, "%d", n);
    }

    /* find total leading 0s (Digits - i after this loop) */
    for(i = 0; i < d; i++)
    {
        if(NUMBER[i] == '\0')
        {
            break;
        }
    }

    j = x; /* start drawing at x */
    if(n < 0)
    {
        draw_sprite(b, f->ch['-'], j, y);
	    j += f->w;
	    i++;
    }

    /* draw leading 0s */
    while(i < d)
    {
        if(text_mode_ncd == 0)
        {
            draw_sprite(b, f->ch['0'], j, y);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch['0'], j, y, text_color_ncd, -1);
        }
        i++;
        j += f->w;
    }

    /* draw the number */
    for(i = 0; NUMBER[i] != '\0'; i++)
    {
        if(text_mode_ncd == 0)
        {
            draw_sprite(b, f->ch[(int)NUMBER[i]], j, y);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch[(int)NUMBER[i]], j, y, text_color_ncd, -1);
        }
        j += f->w;
    }
}

/* draws number using specified NCD_FONT (right aligned) */
void ncd_draw_number_right(BITMAP * b, NCD_FONT * f, int x, int y, long n, int d)
{
    char NUMBER[11]; /* array with maximum of 10 digits */
    int i;          /* looping variable */
    int j;           /* horizontal drawing position */

    /* converts <Number> to string <NUMBER> using base 10 */
    sprintf(NUMBER, "%d", n);

    /* find total leading 0s (Digits - i after this loop) */
    for(i = 0; i < d; i++)
    {
        if(NUMBER[i] == '\0')
        {
            break;
        }
    }

    j = x; /* start drawing at x */

    /* draw leading 0s */
    while(i < d)
    {
        i++;
        j += f->w;
    }

    /* draw the number */
    for(i = 0; NUMBER[i] != '\0'; i++)
    {
        if(text_mode_ncd == 0)
        {
            draw_sprite(b, f->ch[(int)NUMBER[i]], j, y);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch[(int)NUMBER[i]], j, y, text_color_ncd, -1);
        }
        j += f->w;
    }
}

/* left justified */
void ncd_draw_number_left(BITMAP * b, NCD_FONT * f, int x, int y, long n, int d)
{
    char NUMBER[11]; /* array with maximum of 10 digits */
    int i;          /* looping variable */
    int j;           /* horizontal drawing position */

    /* converts <Number> to string <NUMBER> using base 10 */
    sprintf(NUMBER, "%d", n);

    /* find total leading 0s (Digits - i after this loop) */
    for(i = 0; i < d; i++)
    {
        if(NUMBER[i] == '\0')
        {
            break;
        }
    }

    j = x; /* start drawing at x */

    /* draw the number */
    for(i = 0; NUMBER[i] != '\0'; i++)
    {
        if(text_mode_ncd == 0)
        {
            draw_sprite(b, f->ch[(int)NUMBER[i]], j, y);
        }
        else if(text_mode_ncd == 1)
        {
            draw_character_ex(b, f->ch[(int)NUMBER[i]], j, y, text_color_ncd, -1);
        }
        j += f->w;
    }
}

void ncd_show_text(BITMAP * bp, NCD_FONT * fp, int x, int y, char * tp)
{
    int ox = x;
    int oy = y;
    int i;
    char stp[3] = " ";

    for(i = 0; i < strlen(tp); i++)
    {
        if(tp[i] == '\n')
        {
            ox = x;
            oy += fp->h + 1;
            if(oy >= bp->h)
            {
                break;
            }
        }
        else
        {
            stp[0] = tp[i];
            if(ox < bp->w && oy > -fp->h && oy < bp->h)
            {
                ncd_textout(bp, fp, ox, oy, stp);
                ox += fp->w;
            }
        }
    }
}
