//----------------------------------------------------------------------------
//
// ALLEGTTF - True type font loader for Allegro
//
// Douglas Eleveld (D.J.Eleveld@anest.azg.nl)
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// User TTF loading functions
//
//----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "../include/allegttf.h"
#include "internal.h"


/* Calling the function is different for DOS/WIN or LINUX */
#if defined(__DJGPP__) || defined(__WIN32__)
static char* system_string = "ttf2pcx.exe -ph%i -pv%i -s%c -a -cs%i -ce%i %s %s";
#else
static char* system_string = "./ttf2pcx -ph%i -pv%i -s%c -a -cs%i -ce%i %s %s >/dev/null";
#endif


//----------------------------------------------------------------------------
/* Load a ttf font from a file */
FONT* load_ttf_font_ex2_mem (AL_CONST char* filename,
                             AL_CONST int pw,
                             AL_CONST int ph,
                             AL_CONST int begin,
                             AL_CONST int end,
                             AL_CONST int smooth)
   {
   FONT* ret = NULL;
   char buf[L_tmpnam+6];
   char sysbuf[512];

   char smoothchar = 'n';

   if (smooth == ALLEGTTF_TTFSMOOTH)
      smoothchar = 't';
   else if (smooth == ALLEGTTF_REALSMOOTH)
      smoothchar = 'r';
      
   /* Make a temporary filename and run TTF2PCX on that file */
   tmpnam(buf);
   strcat(buf,".pcx");

   sprintf(sysbuf, system_string,
      ph,pw,
      smoothchar,
      begin,end,
      filename,
      buf);
   system(sysbuf);

   // Load the font data in
   ret = attf_load_font(buf);
   unlink(buf);

   return ret;
   }

//----------------------------------------------------------------------------
/* Load a ttf font from a file */
FONT* load_ttf_font_ex_mem (AL_CONST char* filename,
                            AL_CONST int pw,
                            AL_CONST int ph,
                            AL_CONST int smooth)
   {
   return load_ttf_font_ex2_mem(filename,pw,ph,32,128,smooth);
   }
//----------------------------------------------------------------------------
/* Load a ttf font from a file */
FONT* load_ttf_font_mem (AL_CONST char* filename,
                         AL_CONST int points,
                         AL_CONST int smooth)
   {
   return load_ttf_font_ex2_mem(filename,points,points,32,128,smooth);
   }
//----------------------------------------------------------------------------

