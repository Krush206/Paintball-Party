#include <allegro.h>
#include <winalleg.h>
#include "itools.h"

/* get bitmap from clipboard automatically converting to specified palette */
BITMAP * cb_get_bitmap(PALETTE pal)
{
	BITMAP * result = NULL;
	BITMAP * temp = NULL;
	PALETTE temp_pal;
	
	/* the windows-specific image and palette storage */
	HBITMAP hbmp;
	HPALETTE hpal;
    
	/* if there's not bitmap, exit */
	if(!IsClipboardFormatAvailable(CF_BITMAP))
	{
		return NULL;
	}
    
	/* attempt to open the clipboard */
	if(OpenClipboard(win_get_window()))
	{
		
		/* get the bitmap data from the clipboard */
		hbmp = GetClipboardData(CF_BITMAP);
		if(hbmp)
		{
			
			/* import palette if there's one on clipboard */
//			if(!IsClipboardFormatAvailable(CF_PALETTE))
//			{
//				hpal = GetClipboardData(CF_PALETTE);
//				if(hpal)
//				{
//					convert_hpalette_to_palette(hpal, temp_pal);
//				}
//			}
    
			/* get the image and copy it to our result bitmap */
			temp = convert_hbitmap_to_bitmap(hbmp);
			bgr_to_rgb(temp);
//			blit(temp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
//			textprintf(screen, font, 0, 16, 31, "%dbpp", bitmap_color_depth(temp));
//			readkey();
			result = create_bitmap(temp->w, temp->h);
			blit(temp, result, 0, 0, 0, 0, result->w, result->h);
//			if(hpal)
//			{
//				convert_palette(result, temp_pal, pal);
//			}
			
			destroy_bitmap(temp);
		} 
		
		/* close the clipboard */
		CloseClipboard();
    } 
    return result;
}

/* puts specified image on the clipboard */
void cb_put_bitmap(BITMAP * bmp, PALETTE pal)
{
	BITMAP * temp;
	HBITMAP hbmp = NULL;
	HPALETTE hpal;
	
	temp = create_bitmap_ex(16, bmp->w, bmp->h);
	blit(bmp, temp, 0, 0, 0, 0, temp->w, temp->h);
	if(OpenClipboard(win_get_window()))
	{
		if(EmptyClipboard())
		{
			hbmp = convert_bitmap_to_hbitmap(temp);
			if(hbmp)
			{
				if(!SetClipboardData(CF_BITMAP, hbmp))
				{
					DeleteObject(hbmp);
				}
//				hpal = convert_palette_to_hpalette(pal);
//				if(hpal)
//				{
//					if(!SetClipboardData(CF_PALETTE, hpal))
//					{
//						DeleteObject(hpal);
//					}
//				}
				CloseClipboard();
			}
		} 
	}
	destroy_bitmap(temp);
}

int cb_full(void)
{
	if(!IsClipboardFormatAvailable(CF_BITMAP))
	{
		return 0;
	}
	return 1;
}
