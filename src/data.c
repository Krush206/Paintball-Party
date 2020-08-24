#include <allegro.h>
#include "allegttf/include/allegttf.h"
#include "message.h"
#include "ncds.h"
#include "player.h"
#include "particle.h"
#include "level.h"
#include "paintbal.h"
#include "tilemap.h"
#include "font.h"
#include "title.h"
#include "level.h"
#include "objects.h"
#include "data.h"
#include "aastr.h"

int logic_switch = 0;

/* networking data */
int pp_typing = 0;
char pp_text[1024] = {0};
int pp_text_pos = 0;
NCDMSG_QUEUE pp_messages;
int connect_time = 0;
char network_id[256] = {0};
int server_closing = 0;
int client_closing = 0;
int char_ok = 0;
int level_ok = 0;
int net_debug = 0;
int frame_okay = -1;
char input_updated[4];
unsigned long char_sum[4][CHAR_LIST_MAX];
int char_sums[4] = {0};
unsigned long char_avail[CHAR_LIST_MAX];
int char_avails = 0;
unsigned long char_pick[4];
unsigned long level_sum[4][CHAR_LIST_MAX];
int level_sums[4] = {0};
unsigned long level_avail[CHAR_LIST_MAX];
int level_avails = 0;
int net_select = 0;

/* main data */
PP_GAME_DATA pp_game_data;

/* player statistics data */
PLAYER_DATA pp_player_data[PLAYER_DATA_MAX];
int pp_players = 0;
char pp_new_data_text[256] = {0};
char pp_new_data_key = 0x0;
int pp_new_data_pos = 0;
int pp_new_one;

char pp_ip_text[256] = {0};
char pp_id_text[256] = {0};

BITMAP *  pp_screen = NULL;
BITMAP *  static_screen = NULL;
PALETTE   pp_palette;
PALETTE   logo_palette;
PALETTE   blue_palette;
COLOR_MAP pp_color_map;
COLOR_MAP pp_light_map;
//PLAYER    pp_player[4];
FONT *    al_font[PP_MAX_FONTS] = {NULL};
int       pp_state;
int       resume_state;
int       pp_quit = 0;
int       pp_logic_frames;
int       pp_timed = 0;
int       pp_option[PP_OPTIONS];
int       pp_config[PP_OPTIONS];
char      pp_level_name[1024] = {0};
int       pp_global_frame = 0;

/* for the fading routines */
int       pp_fade_step = 0;
int       pp_fade_dir = 0;
int       pp_fading = 0;
PALETTE   pp_fade_start_pal;
PALETTE   pp_fade_end_pal;
void (*pp_fade_proc)() = NULL;
int       pp_random_seed;

/* title screen data */
BITMAP    *  pp_image[PP_MAX_IMAGES] = {NULL};
ANIMATION *  pp_ani[PP_MAX_ANIMATIONS] = {NULL};
PP_MENU      pp_menu[MAX_MENUS];
int          pp_current_menu;
NCD_FONT *   pp_font[PP_MAX_FONTS] = {NULL};
char * key_names[] = {" ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Num 0", "Num 1", "Num 2", "Num 3", "Num 4", "Num 5", "Num 6", "Num 7", "Num 8", "Num 9", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "Escape", "Tilde", "Minus", "Equals", "Backspace", "Tab", "Left Brace", "Right Brace", "Enter", "Semicolon", "Quote", "Backslash", "Backslash", "Comma", "Period", "Slash", "Space", "Insert", "Delete", "Home", "End", "Page Up", "Page Down", "Left", "Right", "Up", "Down", "Divide", "Multiply", "Subtract", "Add", "Point", "Num Enter", "Print Screen", "Pause", "?", "Yen", "Kana", "Convert", "No Convert", "At", "Circumflex", "Colon", "Kanji", "Left Shift", "Right Shift", "Left Control", "Right Control", "Alt", "Alt", "Left Start", "Right Start", "Menu", "Scroll Lock", "Num Lock", "Caps Lock",
"Joy 0 Left", "Joy 0 Right", "Joy 0 Up", "Joy 0 Down", "Joy 0 Button 0", "Joy 0 Button 1", "Joy 0 Button 2", "Joy 0 Button 3", "Joy 0 Button 4", "Joy 0 Button 5", "Joy 0 Button 6", "Joy 0 Button 7", "Joy 0 Button 8", "Joy 0 Button 9", "Joy 0 Button 10",
"Joy 1 Left", "Joy 1 Right", "Joy 1 Up", "Joy 1 Down", "Joy 1 Button 0", "Joy 1 Button 1", "Joy 1 Button 2", "Joy 1 Button 3", "Joy 1 Button 4", "Joy 1 Button 5", "Joy 1 Button 6", "Joy 1 Button 7", "Joy 1 Button 8", "Joy 1 Button 9", "Joy 1 Button 10",
"Joy 2 Left", "Joy 2 Right", "Joy 2 Up", "Joy 2 Down", "Joy 2 Button 0", "Joy 2 Button 1", "Joy 2 Button 2", "Joy 2 Button 3", "Joy 2 Button 4", "Joy 2 Button 5", "Joy 2 Button 6", "Joy 2 Button 7", "Joy 2 Button 8", "Joy 2 Button 9", "Joy 2 Button 10",
"Joy 3 Left", "Joy 3 Right", "Joy 3 Up", "Joy 3 Down", "Joy 3 Button 0", "Joy 3 Button 1", "Joy 3 Button 2", "Joy 3 Button 3", "Joy 3 Button 4", "Joy 3 Button 5", "Joy 3 Button 6", "Joy 3 Button 7", "Joy 3 Button 8", "Joy 3 Button 9", "Joy 3 Button 10"};
ANIMATION * character_place = NULL;
char * music_file[1024] = {NULL};
int    music_index = 0;
char temp_file[1024] = {0};
char pp_title_char_name[4][1024] = {0};
char pp_title_level_name[1024] = {0};
char pp_title_music_name[1024] = {0};
char pp_replay_name[1024] = {0};
char demo_list[MAX_DEMOS][1024] = {0};
int demo_count = 0;
int pp_stat = -1;

CHAR_LIST_ENTRY char_entry[CHAR_LIST_MAX];
int char_entries = 0;
LEVEL_LIST_ENTRY level_entry[LEVEL_LIST_MAX];
int level_entries = 0;

int       single_player = 0;

ANIMATION * pp_object_ani[256] = {NULL};
COLLISION_MAP pp_object_cmap[256];

SAMPLE * pp_sound[PP_MAX_SOUNDS] = {NULL};

/* some info tables */
/*                              SR   SL  SUR  SUL   WR   WL   DR   DL   JR   JL   FR   FL */
int g_table[PLAYER_STATES] =  {  0,   1,   2,   3,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   2,   3,   0,   1,   0,   1,   0,   1,   0,   1};
int a_table[PLAYER_STATES] =  {  0, 128, 192, 192,   0, 128,   0, 128,   0, 128,   0, 128,   0, 128, 192, 192,   0, 128,   0, 128,   0, 128,   0, 128};

char * weapon_table[5] = {"A", "S", "M", "B", "H"};
char * weapon_info[5] = {"Normal", "Splitter", "Mine", "Bouncer", "Homing"};

PP_CREDIT pp_credit[PP_MAX_CREDITS];
int pp_credits;

void text_enter(void)
{
	char k;
	
	if(keypressed())
	{
		k = readkey() & 0xFF;
		switch(k)
		{
			/* enter */
			case 0xD:
			{
            	pp_text[pp_text_pos] = '\0';
				if(pp_typing != 3)
				{
					key[KEY_ENTER] = 0;
				}
				pp_typing = 0;
				break;
			}
		
			/* escape */
			case 0x1B:
			{
				strcpy(pp_text, "");
				key[KEY_ESC] = 0;
				pp_typing = 0;
				break;
			}	
		
			/* backspace */
			case 0x8:
			{
       			pp_text[pp_text_pos] = '\0';
				if(pp_text_pos > 0)
				{	
           			pp_text_pos--;
   					pp_text[pp_text_pos] = '\0';
				}
				break;
			}
				
			/* normal key */
			default:
			{
       			pp_text[pp_text_pos] = k;
				pp_text_pos++;
				pp_text[pp_text_pos] = '\0';
				break;
			}
		}
	}
}

void pp_player_data_save(char * fn)
{
	PACKFILE * fp;
	int i;
	int count = pp_players;
	
	fp = pack_fopen(fn, "w");
	if(!fp)
	{
		return;
	}
	pack_iputl(count, fp);
	for(i = 0; i < count; i++)
	{
		pack_fwrite(pp_player_data[i].name, 17, fp);
		pack_iputl(pp_player_data[i].played, fp);
		pack_iputl(pp_player_data[i].won, fp);
		pack_iputl(pp_player_data[i].fired, fp);
		pack_iputl(pp_player_data[i].hit, fp);
	}
	pack_fclose(fp);
}

void pp_player_data_load(char * fn)
{
	PACKFILE * fp;
	int i;
	int count;
	
	fp = pack_fopen(fn, "r");
	if(!fp)
	{
		return;
	}
	count = pack_igetl(fp);
	for(i = 0; i < count; i++)
	{
		pack_fread(pp_player_data[i].name, 17, fp);
		pp_player_data[i].played = pack_igetl(fp);
		pp_player_data[i].won = pack_igetl(fp);
		pp_player_data[i].fired = pack_igetl(fp);
		pp_player_data[i].hit = pack_igetl(fp);
	}
	pack_fclose(fp);
	pp_players = count;
}

void pp_add_player_data(char * name)
{
//	name[strlen(name)] = '\0';
	strcpy(pp_player_data[pp_players].name, name);
	pp_player_data[pp_players].played = 0;
	pp_player_data[pp_players].won = 0;
	pp_player_data[pp_players].fired = 0;
	pp_player_data[pp_players].hit = 0;
	pp_players++;
}

void pp_delete_player_data(int p)
{
	int i;
	
	for(i = p; i < pp_players - 1; i++)
	{
		strcpy(pp_player_data[i].name, pp_player_data[i + 1].name);
		pp_player_data[i].played = pp_player_data[i + 1].played;
		pp_player_data[i].won = pp_player_data[i + 1].won;
		pp_player_data[i].fired = pp_player_data[i + 1].fired;
		pp_player_data[i].hit = pp_player_data[i + 1].hit;
	}
	pp_players--;
}

int char_list_add(const char * fn, int attrib, void * param)
{
	if(load_char_ns(&pp_game_data.player[0], fn))
	{
		strcpy(char_entry[char_entries].file, fn);
		strcpy(char_entry[char_entries].name, get_filename(fn));
		delete_extension(char_entry[char_entries].name);
		char_entry[char_entries].ap = duplicate_ani(pp_game_data.player[0].character.ap[PS_WALK_RIGHT]);
		char_entries++;
	}
	return 0;
}

int demo_list_add(const char * fn, int attrib, void * param)
{
	if(demo_count < MAX_DEMOS)
	{
		strcpy(demo_list[demo_count], fn);
		demo_count++;
	}
	return 0;
}

void char_list_destroy(void)
{
	int i;
	
	for(i = 0; i < char_entries; i++)
	{
		destroy_ani(char_entry[i].ap);
	}
	char_entries = 0;
}

BITMAP * get_level_thumbnail(const char * fn)
{
	int pw, ph, i, tw, th;
	BITMAP * preview, * pre2;
	BITMAP * layer[5];
	BITMAP * thumbnail, * thumb2;
	
	set_palette(pp_palette);
	destroy_level(pp_game_data.level);
	pp_game_data.level = load_level((char *)fn);
	if(!pp_game_data.level)
	{
		return NULL;
	}
	pw = (pp_game_data.level->tilemap->cr - pp_game_data.level->tilemap->cx + 1) * pp_game_data.level->tileset->w;
	ph = (pp_game_data.level->tilemap->cb - pp_game_data.level->tilemap->cy + 1) * pp_game_data.level->tileset->h;
	tw = ((float)pw * (240.0 / (float)ph));
	if(tw > 600)
	{
		tw = 600;
		th = ((float)ph * (600.0 / (float)pw));
	}
	else
	{
		th = 240;
	}
	preview = create_bitmap(pw, ph);
	if(!preview)
	{
		return NULL;
	}
	clear_bitmap(preview);
	thumb2 = create_bitmap(tw, th);
	if(!thumb2)
	{
		return NULL;
	}
	clear_bitmap(thumb2);
	set_color_depth(16);
	pre2 = create_bitmap(pw, ph);
	if(!pre2)
	{
		return NULL;
	}
	clear_bitmap(pre2);
	thumbnail = create_bitmap(tw, th);
	if(!thumbnail)
	{
		return NULL;
	}
	clear_bitmap(thumbnail);
	set_color_depth(8);
	center_tilemap(preview, pp_game_data.level->tilemap, 0, 0);
	for(i = 0; i < pp_game_data.level->tilemap->l; i++)
	{
		layer[i] = create_bitmap(320 + (pw - 320) * pp_game_data.level->tilemap->layer[i]->xm, 240 + (ph - 240) * pp_game_data.level->tilemap->layer[i]->ym);
		if(!layer[i])
		{
			return NULL;
		}
		clear_bitmap(layer[i]);
		draw_tilemap(layer[i], pp_game_data.level->tilemap, i);
	}
	for(i = 0; i < pp_game_data.level->tilemap->l; i++)
	{
		stretch_sprite(preview, layer[i], 0, 0, preview->w, preview->h);
		destroy_bitmap(layer[i]);
	}
	aa_set_mode(AA_MASKED);
	blit(preview, pre2, 0, 0, 0, 0, preview->w, preview->h);
	aa_stretch_blit(pre2, thumbnail, 0, 0, pre2->w, pre2->h, 0, 0, thumbnail->w, thumbnail->h);
   	destroy_bitmap(preview);
   	destroy_bitmap(pre2);
	blit(thumbnail, thumb2, 0, 0, 0, 0, preview->w, preview->h);
	destroy_bitmap(thumbnail);
   	return thumb2;
}

int in_datafile(char * fn)
{
	int i;
	
	for(i = 0; i < strlen(fn); i++)
	{
		if(fn[i] == '#')
		{
			return 1;
		}
	}
	return 0;
}

int level_list_add(const char * fn, int attrib, void * param)
{
	char nfn[256] = {0};
	BITMAP * bp;
	
	replace_extension(nfn, fn, "pcx", 255);
	if(!exists(nfn) && !in_datafile((char *)fn))
	{
		bp = get_level_thumbnail(fn);
		if(bp)
		{
			save_pcx(nfn, bp, pp_palette);
			destroy_bitmap(bp);
		}
	}
	if(exists(nfn))
	{
		strcpy(level_entry[level_entries].file, fn);
		strcpy(level_entry[level_entries].name, get_filename(fn));
		delete_extension(level_entry[level_entries].name);
		level_entry[level_entries].bp = load_pcx(nfn, NULL);
		level_entries++;
	}
	return 0;
}

void level_list_destroy(void)
{
	int i;
	
	for(i = 0; i < level_entries; i++)
	{
		destroy_bitmap(level_entry[i].bp);
	}
	level_entries = 0;
}

int pp_add_music_file(const char * fn, int attrib, void * param)
{
	char f[512];
	
	if(pp_config[PP_CONFIG_CHAR_THEMES])
	{
		strcpy(f, fn);
		replace_extension(f, f, "ppc", 511);
	}
	if(!exists(f) || !pp_config[PP_CONFIG_CHAR_THEMES])
	{
		music_file[music_index] = malloc(strlen(fn) + 1);
		strcpy(music_file[music_index], fn);
		music_index++;
	}
	return 0;
}

void pp_destroy_music_list(void)
{
	int i;

	for(i = 0; i < music_index; i++)
	{
		free(music_file[i]);
	}
}

void pp_build_music_list(void)
{
	if(music_index)
	{
		pp_destroy_music_list();
		music_index = 0;
	}
	
	#ifdef NCDS_DUMB
	if(pp_config[PP_CONFIG_DMUSIC])
	{
		pp_add_music_file("pp.dat#levels/The Box.it", 0, 0);
		pp_add_music_file("pp.dat#levels/The Factory.it", 0, 0);
		pp_add_music_file("pp.dat#levels/Mountain Arena.it", 0, 0);
		pp_add_music_file("pp.dat#levels/Jesse's Subamarine.it", 0, 0);
		pp_add_music_file("pp.dat#levels/City Blocks.it", 0, 0);
		pp_add_music_file("pp.dat#levels/Arena.it", 0, 0);
		pp_add_music_file("pp.dat#levels/Fun House.xm", 0, 0);
	}
	#endif
	
	/* DUMB supported files */
	#ifdef NCDS_DUMB
		for_each_file_ex("*.it", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, pp_add_music_file, NULL);
		for_each_file_ex("*.xm", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, pp_add_music_file, NULL);
		for_each_file_ex("*.s3m", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, pp_add_music_file, NULL);
		for_each_file_ex("*.mod", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, pp_add_music_file, NULL);
	#endif
	
	/* standard MIDI */
	if(ncds_midi_on)
	{
		for_each_file_ex("*.mid", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, pp_add_music_file, NULL);
	}
	
	/* compressed audio */
	#ifdef NCDS_ALMP3
		for_each_file_ex("*.mp3", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, pp_add_music_file, NULL);
	#endif
	#ifdef NCDS_ALOGG
		for_each_file_ex("*.ogg", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, pp_add_music_file, NULL);
	#endif
}

void pp_read_config(char * fn)
{
	PACKFILE * fp;
	int i;

	set_default_options();
	set_default_game_options();
	
	/* make sure we are not reading an old config file */
	if(exists(fn))
	{
		if(file_size(fn) < 5780)
		{
			delete_file(fn);
		}
	}
	fp = pack_fopen(fn, "r");
	if(!fp)
	{
		pack_fclose(fp);
		return;
	}
	for(i = 0; i < PP_OPTIONS; i++)
	{
		pp_option[i] = pack_igetl(fp);
	}
	for(i = 0; i < PP_OPTIONS; i++)
	{
		pp_config[i] = pack_igetl(fp);
	}
	for(i = 0; i < 4; i++)
	{
		pp_game_data.player[i].controller.left_key = pack_igetl(fp);
		pp_game_data.player[i].controller.right_key = pack_igetl(fp);
		pp_game_data.player[i].controller.up_key = pack_igetl(fp);
		pp_game_data.player[i].controller.down_key = pack_igetl(fp);
		pp_game_data.player[i].controller.fire_key = pack_igetl(fp);
		pp_game_data.player[i].controller.jump_key = pack_igetl(fp);
		pp_game_data.player[i].controller.select_key = pack_igetl(fp);
		pp_game_data.player[i].controller.option_key = pack_igetl(fp);
	}
	for(i = 0; i < 4; i++)
	{
		pp_game_data.player[i].char_pick = pack_igetl(fp);
		pack_fread(pp_title_char_name[i], 1024, fp);
	}
	pp_slev = pack_igetl(fp);
	pack_fread(pp_title_level_name, 1024, fp);
	pack_fclose(fp);
	pp_game_data.player[0].data_pick = pp_config[PP_CONFIG_P_P1];
	pp_game_data.player[1].data_pick = pp_config[PP_CONFIG_P_P2];
	pp_game_data.player[2].data_pick = pp_config[PP_CONFIG_P_P3];
	pp_game_data.player[3].data_pick = pp_config[PP_CONFIG_P_P4];
}

void pp_write_config(char * fn)
{
	PACKFILE * fp;
	int i;

	fp = pack_fopen(fn, "w");
	if(!fp)
	{
		pack_fclose(fp);
		return;
	}
	for(i = 0; i < PP_OPTIONS; i++)
	{
		pack_iputl(pp_option[i], fp);
	}
	for(i = 0; i < PP_OPTIONS; i++)
	{
		pack_iputl(pp_config[i], fp);
	}
	for(i = 0; i < 4; i++)
	{
		pack_iputl(pp_game_data.player[i].controller.left_key, fp);
		pack_iputl(pp_game_data.player[i].controller.right_key, fp);
		pack_iputl(pp_game_data.player[i].controller.up_key, fp);
		pack_iputl(pp_game_data.player[i].controller.down_key, fp);
		pack_iputl(pp_game_data.player[i].controller.fire_key, fp);
		pack_iputl(pp_game_data.player[i].controller.jump_key, fp);
		pack_iputl(pp_game_data.player[i].controller.select_key, fp);
		pack_iputl(pp_game_data.player[i].controller.option_key, fp);
	}
	for(i = 0; i < 4; i++)
	{
		pack_iputl(pp_game_data.player[i].char_pick, fp);
		pack_fwrite(pp_title_char_name[i], 1024, fp);
	}
	pack_iputl(pp_slev, fp);
	pack_fwrite(pp_title_level_name, 1024, fp);
	pack_fclose(fp);
}

void pp_set_controllers(void)
{
	int i;

	for(i = 0; i < 4; i++)
	{
		reset_controller(&pp_game_data.player[i].controller, i);
	}
}

void lighting_blender(PALETTE pal, int x, int y, RGB *rgb)
{
	int light;
	fixed light_level;
	fixed col_level;
	int final;
	fixed left;
	
	/* calculate the total luminence of the color */
	light = (pal[x].r + pal[x].g + pal[x].b) / 3;
	
	/* do shadow blend */
	if(light == 0)
	{
		rgb->r = 0;
		rgb->g = 0;
		rgb->b = 0;
	}
	else if(light < 32)
	{
		rgb->r = fixtoi(fmul(itofix(pal[y].r), fdiv(itofix(light * 2 + 1), itofix(63))));
		rgb->g = fixtoi(fmul(itofix(pal[y].g), fdiv(itofix(light * 2 + 1), itofix(63))));
		rgb->b = fixtoi(fmul(itofix(pal[y].b), fdiv(itofix(light * 2 + 1), itofix(63))));
	}
	
	/* do light blend */
	else if(light == 32)
	{
		rgb->r = pal[y].r;
		rgb->g = pal[y].g;
		rgb->b = pal[y].b;
	}
	else
	{
		light_level = fdiv(itofix((light - 32) * 2 + 1), itofix(63));
		left = itofix(63 - pal[y].r);
		final = pal[y].r + fixtoi(fmul(left, light_level));
		rgb->r = final;
		left = itofix(63 - pal[y].g);
		final = pal[y].g + fixtoi(fmul(left, light_level));
		rgb->g = final;
		left = itofix(63 - pal[y].b);
		final = pal[y].b + fixtoi(fmul(left, light_level));
		rgb->b = final;
	}
}

void pp_load_data(void)
{
	int i, j;
	char temp_string[256] = {0};

	/* set the default controllers */
	pp_set_controllers();

	pp_add_player_data("Player 1");
	pp_add_player_data("Player 2");
	pp_add_player_data("Player 3");
	pp_add_player_data("Player 4");
	
	/* read the user configuration */
	pp_read_config("pp.cfg");
	pp_player_data_load("pp.pdt");

	/* set the default options for stash, powerups, etc. */
//	set_default_game_options();

	/* load title screen data */
	pp_image[PP_IMAGE_TITLE] = load_pcx("pp.dat#images/title.pcx", pp_palette);
	if(!pp_image[PP_IMAGE_TITLE])
	{
		allegro_message("Out of memory!");
		exit(0);
	}
	pp_image[PP_IMAGE_LOGO] = load_pcx("pp.dat#images/logo.pcx", logo_palette);
	if(!pp_image[PP_IMAGE_LOGO])
	{
		allegro_message("Out of memory!");
		exit(0);
	}
	pp_image[PP_IMAGE_MENU] = load_pcx("pp.dat#images/menu.pcx", NULL);
	if(!pp_image[PP_IMAGE_MENU])
	{
		allegro_message("Out of memory!");
		exit(0);
	}

	/* create the translucency map */
	set_palette(pp_palette);
	pp_load_color_map(&pp_color_map, "pp.dat#trans_color_map");
//    create_trans_table(&pp_color_map, pp_palette, 128, 128, 128, NULL);
	color_map = &pp_color_map;
	create_color_table(&pp_light_map, pp_palette, (void *)lighting_blender, NULL);
	tilemap_trans_color_map = &pp_color_map;
	tilemap_light_map = &pp_light_map;

	/* create the music list */
	if(ncds_initialized && ncds_music_types > 0)
	{
		pp_text2screen("  Building music database... ");
		pp_build_music_list();
		pp_text2screen("done\n");
	}
	
	/* build character list */
	set_color_depth(8);
	pp_text2screen("  Building character database... ");
	char_list_add("pp.dat#characters/Billy.ppc", 0, 0);
	char_list_add("pp.dat#characters/Damien.ppc", 0, 0);
	char_list_add("pp.dat#characters/Josh.ppc", 0, 0);
	char_list_add("pp.dat#characters/Jason.ppc", 0, 0);
	char_list_add("pp.dat#characters/Trey.ppc", 0, 0);
	char_list_add("pp.dat#characters/Pete.ppc", 0, 0);
	char_list_add("pp.dat#characters/Meatwad.ppc", 0, 0);
	char_list_add("pp.dat#characters/Gohan Super Sayian.ppc", 0, 0);
	char_list_add("pp.dat#characters/Samus.ppc", 0, 0);
	char_list_add("pp.dat#characters/Block.ppc", 0, 0);
	for_each_file_ex("*.ppc", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, char_list_add, NULL);
	pp_text2screen("done\n");
	
	/* build level list */
	set_color_depth(8);
	pp_text2screen("  Building level database... ");
	level_list_add("pp.dat#levels/The Box.ppl", 0, 0);
	level_list_add("pp.dat#levels/The Factory.ppl", 0, 0);
	level_list_add("pp.dat#levels/Mountain Arena.ppl", 0, 0);
	level_list_add("pp.dat#levels/Jesse's Subamarine.ppl", 0, 0);
	level_list_add("pp.dat#levels/City Blocks.ppl", 0, 0);
	level_list_add("pp.dat#levels/Arena.ppl", 0, 0);
	level_list_add("pp.dat#levels/Fun House.ppl", 0, 0);
	level_list_add("pp.dat#levels/Tower.ppl", 0, 0);
	level_list_add("pp.dat#levels/Metroid.ppl", 0, 0);
	for_each_file_ex("*.ppl", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, level_list_add, NULL);
	pp_text2screen("done\n");

	for_each_file_ex("*.ppr", 0, FA_DIREC | FA_LABEL | FA_SYSTEM, demo_list_add, NULL);
	
	/* create the blue palette used in the intro sequence */
	for(i = 0; i < 256; i++)
	{
		blue_palette[i].r = pp_palette[pp_image[PP_IMAGE_TITLE]->line[0][0]].r;
		blue_palette[i].g = pp_palette[pp_image[PP_IMAGE_TITLE]->line[0][0]].g;
		blue_palette[i].b = pp_palette[pp_image[PP_IMAGE_TITLE]->line[0][0]].b;
	}

	/* load the in-game status images */
	pp_text2screen("  Loading images... ");
	pp_image[PP_IMAGE_CLOCK] = load_pcx("pp.dat#images/clock.pcx", NULL);
	pp_image[PP_IMAGE_SCORE] = load_pcx("pp.dat#images/score.pcx", NULL);
	pp_image[PP_IMAGE_TARGETS] = load_pcx("pp.dat#images/targets.pcx", NULL);
	pp_image[PP_IMAGE_AMMO] = load_pcx("pp.dat#images/ammo.pcx", NULL);
	pp_image[PP_IMAGE_TIME] = load_pcx("pp.dat#images/time.pcx", NULL);
	pp_image[PP_IMAGE_FLAGS] = load_pcx("pp.dat#images/flags.pcx", NULL);
	pp_image[PP_IMAGE_HITS] = load_pcx("pp.dat#images/hits.pcx", NULL);
	pp_image[PP_IMAGE_POWER_C] = load_pcx("pp.dat#images/power_c.pcx", NULL);
	pp_image[PP_IMAGE_POWER_D] = load_pcx("pp.dat#images/power_d.pcx", NULL);
	pp_image[PP_IMAGE_POWER_J] = load_pcx("pp.dat#images/power_j.pcx", NULL);
	pp_image[PP_IMAGE_POWER_R] = load_pcx("pp.dat#images/power_r.pcx", NULL);
	pp_image[PP_IMAGE_POWER_F] = load_pcx("pp.dat#images/power_f.pcx", NULL);
	pp_image[PP_IMAGE_POWER_T] = load_pcx("pp.dat#images/power_t.pcx", NULL);
	pp_image[PP_IMAGE_GEMS_0] = load_pcx("pp.dat#images/gems_0.pcx", NULL);
	pp_image[PP_IMAGE_GEMS_1] = load_pcx("pp.dat#images/gems_1.pcx", NULL);
	pp_image[PP_IMAGE_GEMS_2] = load_pcx("pp.dat#images/gems_2.pcx", NULL);
	pp_image[PP_IMAGE_GEMS_3] = load_pcx("pp.dat#images/gems_3.pcx", NULL);
	pp_image[PP_IMAGE_GEMS_4] = load_pcx("pp.dat#images/gems_4.pcx", NULL);
	pp_image[PP_IMAGE_GEMS_5] = load_pcx("pp.dat#images/gems_5.pcx", NULL);
	pp_image[PP_IMAGE_ELIMINATED] = load_pcx("pp.dat#images/eliminated.pcx", NULL);
	for(i = 0; i <= PP_IMAGE_POWER_T; i++)
	{
		if(!pp_image[i])
		{
			allegro_message("Out of memory!");
			exit(0);
		}
	}

	/* load in-game icons */
	pp_ani[PP_ANI_IT_POINTER] = load_ani("pp.dat#animations/it_pointer.ani", NULL);
	pp_ani[PP_ANI_POINTER] = load_ani("pp.dat#animations/pointer.ani", NULL);
	pp_ani[PP_ANI_TARGET] = load_ani("pp.dat#animations/target.ani", NULL);
	if(!pp_ani[PP_ANI_IT_POINTER] || !pp_ani[PP_ANI_POINTER] || !pp_ani[PP_ANI_TARGET])
	{
		allegro_message("Out of memory!");
		exit(0);
	}
	pp_text2screen("done\n");

	/* load and initialize all the font stuff */
	pp_text2screen("  Loading fonts... ");
	set_color_conversion(COLORCONV_NONE);
	pp_font[PP_FONT_GAME] = load_ncd_font("pp.dat#fonts/game.mft");
	pp_font[PP_FONT_TINY] = load_ncd_font("pp.dat#fonts/tiny.mft");
	if(!pp_font[PP_FONT_GAME] || !pp_font[PP_FONT_TINY])
	{
		allegro_message("Out of memory!");
		exit(0);
	}
	#ifdef USE_ALLEGTTF
		al_font[0] = attf_load_font("pp.dat#fonts/comic_large.pcx");
		al_font[1] = attf_load_font("pp.dat#fonts/comic_medium.pcx");
		al_font[2] = attf_load_font("pp.dat#fonts/comic_small.pcx");
		al_font[3] = attf_load_font("pp.dat#fonts/comic_tiny.pcx");
		al_font[4] = load_font("pp.dat#fonts/guifont.pcx", NULL, NULL);
		if(!al_font[0] || !al_font[1] || !al_font[2] || !al_font[3] || !al_font[4])
		{
			allegro_message("Out of memory!");
			exit(0);
		}
		antialias_init(pp_palette);
	#else
		al_font[0] = font;
		al_font[1] = font;
		al_font[2] = font;
	#endif
	pp_text2screen("done\n");

	character_place = load_ani("pp.dat#characters/place_holder.ani", NULL);
	if(!character_place)
	{
		allegro_message("Out of memory!");
		exit(0);
	}

	/* load all the sound effects */
	pp_text2screen("  Loading sounds...");
	pp_sound[PP_SOUND_FIRE] = ncds_load_wav("pp.dat#sounds/fire.wav");
	pp_sound[PP_SOUND_SPLAT] = ncds_load_wav("pp.dat#sounds/splat.wav");
	pp_sound[PP_SOUND_RELOAD_A] = ncds_load_wav("pp.dat#sounds/reload_a.wav");
	pp_sound[PP_SOUND_RELOAD_B] = ncds_load_wav("pp.dat#sounds/reload_b.wav");
	pp_sound[PP_SOUND_AMMO] = ncds_load_wav("pp.dat#sounds/ammo.wav");
	pp_sound[PP_SOUND_CLOAK] = ncds_load_wav("pp.dat#sounds/cloak.wav");
	pp_sound[PP_SOUND_GEM] = ncds_load_wav("pp.dat#sounds/gem.wav");
	pp_sound[PP_SOUND_JUMP] = ncds_load_wav("pp.dat#sounds/jump.wav");
	pp_sound[PP_SOUND_LAND] = ncds_load_wav("pp.dat#sounds/land.wav");
	pp_sound[PP_SOUND_HIT] = ncds_load_wav("pp.dat#sounds/hit.wav");
	pp_sound[PP_SOUND_BUMP] = ncds_load_wav("pp.dat#sounds/bump.wav");
	pp_sound[PP_SOUND_TIME] = ncds_load_wav("pp.dat#sounds/time.wav");
	pp_sound[PP_SOUND_MENU_PICK] = ncds_load_wav("pp.dat#sounds/menu_pick.wav");
	pp_sound[PP_SOUND_MENU_NEXT] = ncds_load_wav("pp.dat#sounds/menu_next.wav");
	pp_sound[PP_SOUND_TELE_IN] = ncds_load_wav("pp.dat#sounds/tele_in.wav");
	pp_sound[PP_SOUND_TELE_OUT] = ncds_load_wav("pp.dat#sounds/tele_in.wav");
	pp_sound[PP_SOUND_RUN] = ncds_load_wav("pp.dat#sounds/run.wav");
	pp_sound[PP_SOUND_DEFLECT] = ncds_load_wav("pp.dat#sounds/deflect.wav");
	pp_sound[PP_SOUND_FLAG] = ncds_load_wav("pp.dat#sounds/flag.wav");
	pp_sound[PP_SOUND_BANK] = ncds_load_wav("pp.dat#sounds/bank.wav");
	pp_sound[PP_SOUND_BASE] = ncds_load_wav("pp.dat#sounds/base.wav");
	pp_sound[PP_SOUND_RESPAWN] = ncds_load_wav("pp.dat#sounds/respawn.wav");
	pp_sound[PP_SOUND_PJUMP] = ncds_load_wav("pp.dat#sounds/pjump.wav");
	pp_sound[PP_SOUND_FLY] = ncds_load_wav("pp.dat#sounds/fly.wav");
	pp_sound[PP_SOUND_PFLY] = ncds_load_wav("pp.dat#sounds/pfly.wav");
	pp_sound[PP_SOUND_PTURBO] = ncds_load_wav("pp.dat#sounds/pturbo.wav");
	pp_sound[PP_SOUND_MENU_LEFT] = ncds_load_wav("pp.dat#sounds/menu_left.wav");
	pp_sound[PP_SOUND_MENU_RIGHT] = ncds_load_wav("pp.dat#sounds/menu_right.wav");
	pp_sound[PP_SOUND_MENU_FLAG] = ncds_load_wav("pp.dat#sounds/menu_flag.wav");
	pp_sound[PP_SOUND_MENU_ENTER] = ncds_load_wav("pp.dat#sounds/menu_enter.wav");
	pp_sound[PP_SOUND_MENU_ESC] = ncds_load_wav("pp.dat#sounds/menu_esc.wav");
	pp_sound[PP_SOUND_MENU_MOVE] = ncds_load_wav("pp.dat#sounds/menu_move.wav");
	pp_sound[PP_SOUND_CRUSH] = ncds_load_wav("pp.dat#sounds/crush.wav");
	pp_sound[PP_SOUND_TARGET] = ncds_load_wav("pp.dat#sounds/target.wav");
	pp_sound[PP_SOUND_BOUNCE] = ncds_load_wav("pp.dat#sounds/bounce.wav");
	pp_sound[PP_SOUND_RICOCHET] = ncds_load_wav("pp.dat#sounds/ricochet.wav");
	pp_sound[PP_SOUND_SPRING] = ncds_load_wav("pp.dat#sounds/spring.wav");
	pp_sound[PP_SOUND_ENTER] = ncds_load_wav("pp.dat#sounds/menu_enter.wav");
	for(i = 0; i <= PP_SOUND_PTURBO; i++)
	{
		if(!pp_sound[i] && i != PP_SOUND_START && i != PP_SOUND_WIN)
		{
			allegro_message("Out of memory!");
			exit(0);
		}
	}
	pp_text2screen("done\n");

	/* initialize all the objects animations to NULL */
	/* object animations not loaded below will cause those objects to be ignored in the game */
	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		pp_game_data.object[i].ap = NULL;
	}

	/* load all the in-game object animations that we want */
	pp_text2screen("  Loading game objects... ");
    pp_object_ani[PP_OBJECT_TARGET] = load_ani("pp.dat#objects/target.ani", NULL);
    pp_object_ani[PP_OBJECT_AMMO_NORMAL] = load_ani("pp.dat#objects/ammo_normal.ani", NULL);
    pp_object_ani[PP_OBJECT_AMMO_X] = load_ani("pp.dat#objects/ammo_x.ani", NULL);
    pp_object_ani[PP_OBJECT_AMMO_MINE] = load_ani("pp.dat#objects/ammo_mine.ani", NULL);
    pp_object_ani[PP_OBJECT_AMMO_BOUNCE] = load_ani("pp.dat#objects/ammo_bounce.ani", NULL);
    pp_object_ani[PP_OBJECT_AMMO_SEEK] = load_ani("pp.dat#objects/ammo_seek.ani", NULL);
    pp_object_ani[PP_OBJECT_POWER_CLOAK] = load_ani("pp.dat#objects/power_cloak.ani", NULL);
    pp_object_ani[PP_OBJECT_POWER_JUMP] = load_ani("pp.dat#objects/power_jump.ani", NULL);
    pp_object_ani[PP_OBJECT_POWER_RUN] = load_ani("pp.dat#objects/power_run.ani", NULL);
    pp_object_ani[PP_OBJECT_POWER_DEFLECT] = load_ani("pp.dat#objects/power_deflect.ani", NULL);
    pp_object_ani[PP_OBJECT_POWER_FLY] = load_ani("pp.dat#objects/power_fly.ani", NULL);
    pp_object_ani[PP_OBJECT_POWER_TURBO] = load_ani("pp.dat#objects/power_turbo.ani", NULL);
    pp_object_ani[PP_OBJECT_FLAG] = load_ani("pp.dat#objects/flag.ani", NULL);
    pp_object_ani[PP_OBJECT_BASE] = load_ani("pp.dat#objects/base.ani", NULL);
    pp_object_ani[PP_OBJECT_BANK] = load_ani("pp.dat#objects/bank.ani", NULL);
    pp_object_ani[PP_OBJECT_GEM_1] = load_ani("pp.dat#objects/gem_1.ani", NULL);
    pp_object_ani[PP_OBJECT_GEM_2] = load_ani("pp.dat#objects/gem_2.ani", NULL);
    pp_object_ani[PP_OBJECT_GEM_3] = load_ani("pp.dat#objects/gem_3.ani", NULL);
    pp_object_ani[PP_OBJECT_GEM_4] = load_ani("pp.dat#objects/gem_4.ani", NULL);
    pp_object_ani[PP_OBJECT_GEM_5] = load_ani("pp.dat#objects/gem_5.ani", NULL);
    pp_object_ani[PP_OBJECT_HUNT_BASE] = load_ani("pp.dat#objects/hunt_base.ani", NULL);
    pp_object_ani[PP_OBJECT_POOF] = load_ani("pp.dat#objects/poof.ani", NULL);
    pp_object_ani[PP_OBJECT_GEM_POOF] = load_ani("pp.dat#objects/gem_poof.ani", NULL);
    pp_object_ani[PP_OBJECT_POWER_POOF] = load_ani("pp.dat#objects/power_poof.ani", NULL);
    pp_object_ani[PP_OBJECT_JET] = load_ani("pp.dat#objects/jet.ani", NULL);
    pp_object_ani[PP_OBJECT_SPRING_UP] = load_ani("pp.dat#objects/spring_up.ani", NULL);
    pp_object_ani[PP_OBJECT_SPRING_DOWN] = load_ani("pp.dat#objects/spring_down.ani", NULL);
    pp_object_ani[PP_OBJECT_SPRING_LEFT] = load_ani("pp.dat#objects/spring_left.ani", NULL);
    pp_object_ani[PP_OBJECT_SPRING_RIGHT] = load_ani("pp.dat#objects/spring_right.ani", NULL);
    pp_object_ani[PP_OBJECT_SPRING_BAR_V] = load_ani("pp.dat#objects/spring_bar_v.ani", NULL);
    pp_object_ani[PP_OBJECT_SPRING_BAR_H] = load_ani("pp.dat#objects/spring_bar_h.ani", NULL);
    for(i = 0; i <= PP_OBJECT_POWER_TURBO; i++)
    {
	    if(!pp_object_ani)
	    {
			allegro_message("Out of memory!");
			exit(0);
	    }
    }
	pp_text2screen("done\n");

	/* initialize the player data */
    for(i = 0; i < 4; i++)
	{
		erase_char(&pp_game_data.player[i]);
    	pp_game_data.player[i].color = i;
    }
    pp_game_data.level = NULL;
}

void pp_free_data(void)
{
	int i;

	pp_destroy_music_list();
	char_list_destroy();
	level_list_destroy();
	for(i = 0; i < PP_MAX_SOUNDS; i++)
	{
		if(pp_sound[i])
		{
			ncds_free_sample(pp_sound[i]);
		}
	}
	for(i = 0; i < PP_MAX_IMAGES; i++)
	{
		if(pp_image[i])
		{
			destroy_bitmap(pp_image[i]);
		}
	}

	for(i = 0; i < PP_MAX_ANIMATIONS; i++)
	{
		if(pp_ani[i])
		{
			destroy_ani(pp_ani[i]);
		}
	}

	for(i = 0; i < PP_MAX_OBJECTS; i++)
	{
		if(pp_object_ani[i])
		{
			destroy_ani(pp_object_ani[i]);
		}
	}

	for(i = 0; i < PP_MAX_FONTS; i++)
	{
		if(pp_font[i])
		{
			destroy_ncd_font(pp_font[i]);
		}
		if(al_font[i])
		{
			destroy_font(al_font[i]);
		}
	}

	destroy_level(pp_game_data.level);

	for(i = 0; i < 4; i++)
	{
		destroy_char(&pp_game_data.player[i]);
	}
	destroy_ani(character_place);
}
