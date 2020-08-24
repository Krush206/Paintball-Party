#include <allegro.h>
#include "animate.h"

#define OLD_PLAYER_STATES       12
#define PLAYER_STATES           24
#define GUN_FLASH_TYPES        4

/* this data can be saved to a file */
typedef struct
{

	ANIMATION * ap[PLAYER_STATES]; /* all animations should have same dimensions */
	ANIMATION * pap;
	ANIMATION * ppap;
	ANIMATION * gap[GUN_FLASH_TYPES];
	int cx_table[PLAYER_STATES], cy_table[PLAYER_STATES], cb_table[PLAYER_STATES], cr_table[PLAYER_STATES], cw_table[PLAYER_STATES], ch_table[PLAYER_STATES];
	int px_table[PLAYER_STATES], py_table[PLAYER_STATES];
	int gx_table[PLAYER_STATES], gy_table[PLAYER_STATES];
	int mx, my, mr; /* center of character */
	int ix, iy; /* location of pointer relative to character */
	int pf;

} PLAYER_DATA;

PLAYER_DATA player;
PALETTE pal;

void load_char_fp(PLAYER_DATA * dp, PACKFILE * fp)
{
	int i;
	int temp;

	temp = pack_igetl(fp);
	if(temp)
	{
		dp->pap = load_ani_fp(fp, NULL);
	}
	else
	{
		dp->pap = NULL;
	}
	temp = pack_igetl(fp);
	if(temp)
	{
		dp->ppap = load_ani_fp(fp, NULL);
	}
	else
	{
		dp->ppap = NULL;
	}
	for(i = 0; i < GUN_FLASH_TYPES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->gap[i] = load_ani_fp(fp, NULL);
		}
		else
		{
			dp->gap[i] = NULL;
		}
	}
	for(i = 0; i < PLAYER_STATES; i++)
	{
		temp = pack_igetl(fp);
		if(temp)
		{
			dp->ap[i] = load_ani_fp(fp, NULL);
		}
		else
		{
			dp->ap[i] = NULL;
		}
		dp->cx_table[i] = pack_igetl(fp);
		dp->cy_table[i] = pack_igetl(fp);
		dp->cw_table[i] = pack_igetl(fp);
		dp->ch_table[i] = pack_igetl(fp);
		dp->px_table[i] = pack_igetl(fp);
		dp->py_table[i] = pack_igetl(fp);
		dp->gx_table[i] = pack_igetl(fp);
		dp->gy_table[i] = pack_igetl(fp);
	}
	dp->mx = pack_igetl(fp);
	dp->my = pack_igetl(fp);
	dp->mr = pack_igetl(fp);
	dp->ix = pack_igetl(fp);
	dp->iy = pack_igetl(fp);
	dp->pf = pack_igetl(fp);
}

int load_char(PLAYER_DATA * dp, char * fn)
{
	PACKFILE * fp;
	int type;
	char ffn[1024];

	fp = pack_fopen(fn, "r");
	if(!fp)
	{
		return 0;
	}
	pack_igetl(fp);
	load_char_fp(dp, fp);
	pack_fclose(fp);
	return 1;
}

void web_update(const char * fn, int attrib, int param)
{
	int i, pos;
	char * cfn;
	int ext;
//	char fn[1024];
	char basefn[1024] = {0};
	char folderfn[1024] = {0};
	char sfilename[1024] = {0};
	char ifilename[1024] = {0};
	char tfilename[1024] = {0};
	char webfn[1024] = {0};
	char command[1024] = {0};
	BITMAP * webbp;
	
	load_char(&player, (char *)fn);
	webbp = create_bitmap(player.ap[0]->w * 2, player.ap[0]->h * 2);
	clear_bitmap(webbp);
	stretch_sprite(webbp, player.ap[0]->image[0], 0, 0, webbp->w, webbp->h);
	cfn = get_filename(fn);
	strcpy(basefn, cfn);
	for(i = 1023; i >= 0; i--)
	{
		if(basefn[i] == '.')
		{
			basefn[i] = 0;
			break;
		}
	}
	for(i = 0, pos = 0; i < 1024; i++)
	{
		if(cfn[i] >= 'a' && cfn[i] <= 'z')
		{
			webfn[pos] = cfn[i];
			pos++;
		}
		else if(cfn[i] >= 'A' && cfn[i] <= 'Z')
		{
			webfn[pos] = cfn[i] + 32;
			pos++;
		}
		else if(cfn[i] >= '0' && cfn[i] <= '9')
		{
			webfn[pos] = cfn[i];
			pos++;
		}
		else if(cfn[i] == '.')
		{
			ext = pos;
		}
		else if(cfn[i] == ' ')
		{
			webfn[pos] = '_';
			pos++;
		}
		else if(cfn[i] == 0)
		{
			webfn[ext] = 0;
			webfn[pos] = 0;
			pos++;
			break;
		}
	}
	for(i = ext - 1; i >= 0 && webfn[i] == '_'; i--)
	{
		webfn[i] = 0;
	}
	strcpy(folderfn, "C:\\newchars\\");
//	strcpy(fn, folderfn);
//	strcat(fn, webfn);
//	replace_extension(fn, fn, "", 1023);
	strcpy(sfilename, cfn);
	replace_extension(sfilename, sfilename, "pps", 1023);
	strcpy(ifilename, fn);
	replace_extension(ifilename, ifilename, "png", 1023);
	strcpy(tfilename, fn);
	replace_extension(tfilename, tfilename, "txt", 1023);
		
	/* create the image */
	sprintf(command, "c:\\pa\\pacomp.exe -c2 -a c:\\newchars\\ppc_%s.zip \"%s.ppc\" \"%s.pps\" \"%s.txt\" \"%s.mid\" \"%s.mp3\" \"%s.ogg\" \"%s.mod\" \"%s.s3m\" \"%s.xm\" \"%s.it\"", webfn, basefn, basefn, basefn, basefn, basefn, basefn, basefn, basefn, basefn, basefn);
	printf(command);
//	getch();
//	textprintf(screen, font, 0, 32, 31, "%s", command);
//	readkey();
	system(command);
	sprintf(command, "c:\\newchars\\ppc_%s.png", webfn);
	save_png(command, webbp, pal);
	destroy_bitmap(webbp);
}

int main(void)
{
	allegro_init();
	destroy_bitmap(load_pcx("palette.pcx", pal));
	for_each_file("*.ppc", FA_ARCH | FA_RDONLY, web_update, 0);
//	system("dist.bat");
	return 0;
}
END_OF_MAIN()
