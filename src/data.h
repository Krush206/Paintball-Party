#ifndef DATA_H
#define DATA_H

#include <allegro.h>
#include "ncds.h"
#include "message.h"
#include "player.h"
#include "particle.h"
#include "level.h"
#include "paintbal.h"
#include "title.h"
#include "objects.h"
#include "game.h"
#include "logo.h"

#define PP_OPTIONS                 128

/* graphics options */
#define PP_CONFIG_LOGIC              0 /* logic method */
#define PP_CONFIG_VSYNC              1 /* syncronize graphics with monitor */
#define PP_CONFIG_ICONS              2 /* show helper icons in game for certain modes */
#define PP_CONFIG_RADAR              3 /* display the radar */
#define PP_CONFIG_TRANS              4 /* enable translucency effects on maps */
#define PP_CONFIG_LOWRES             5

/* audio options */
#define PP_CONFIG_MUSIC_VOLUME       6 /* the music volume */
#define PP_CONFIG_SOUND_VOLUME       7 /* the sound volume */

/* misc options */
#define PP_CONFIG_JOY                8
#define PP_CONFIG_REPLAY             9
#define PP_CONFIG_RANDOM_MUSIC      10
#define PP_CONFIG_CHAR_THEMES       11
#define PP_CONFIG_DMUSIC            12
#define PP_CONFIG_SPLIT_MODE        13
#define PP_CONFIG_G_PLAYED          14
//#define PP_OPTION_L_PLAYED          15
#define PP_CONFIG_P_P1              16
#define PP_CONFIG_P_P2              17
#define PP_CONFIG_P_P3              18
#define PP_CONFIG_P_P4              19
#define PP_CONFIG_NET_THROTTLE      20

#define PP_CONFIG_LOGIC_SMOOTH   0 /* logic and graphics run once each frame */
#define PP_CONFIG_LOGIC_TIMED    1 /* logic is timed to ~60hz */
#define PP_CONFIG_LOGIC_FAST     2 /* logic is timed a little faster than 60hz */

#define PP_SCREEN_MODES              3
#define PP_SCREEN_MODE_QUARTER       0
#define PP_SCREEN_MODE_VERTICAL      1
#define PP_SCREEN_MODE_HORIZONTAL    2

/* game rule options */
#define PP_OPTION_GAME_TYPE          0 /* type of game */
#define PP_OPTION_GAME_TIME          1 /* time measured in frames (60 = 1s) */
#define PP_OPTION_SF_HITS            2 /* number of hits for winner in splat fest */
#define PP_OPTION_H_HITS             3 /* number of hits before elimination in hunter */
#define PP_OPTION_T_TIME             4 /* amount of "it" time before elimination in tag */
#define PP_OPTION_F_FLAGS            5 /* number of flags to capture before winning */

/* power up options (multi-player) */
#define PP_OPTION_AMMO_TIME          6 /* time it takes for ammo to reappear */
#define PP_OPTION_POWER_TIME         7 /* time it takes for power ups to reappear */
#define PP_OPTION_AMMO_NORMAL        8 /* amount of normal ammo to start with */
#define PP_OPTION_AMMO_NORMAL_FLAG   9 /* enable normal ammo */
#define PP_OPTION_AMMO_X            10 /* amount of explosive ammo to start with */
#define PP_OPTION_AMMO_X_FLAG       11 /* enable explosive ammo */
#define PP_OPTION_AMMO_MINE         12 /* amount of mines to start with */
#define PP_OPTION_AMMO_MINE_FLAG    13 /* enable mines */
#define PP_OPTION_AMMO_BOUNCE       14 /* amount of bounce ammo to start with */
#define PP_OPTION_AMMO_BOUNCE_FLAG  15 /* enable bounce ammo */
#define PP_OPTION_AMMO_SEEK         16 /* amount of homing ammo to start with */
#define PP_OPTION_AMMO_SEEK_FLAG    17 /* enable homing ammo */
#define PP_OPTION_POWER_CLOAK       18 /* enable cloak power up */
#define PP_OPTION_POWER_DEFLECT     19 /* enable deflect power up */
#define PP_OPTION_POWER_JUMP        20 /* enable jump power up */
#define PP_OPTION_POWER_RUN         21 /* enable run power up */
#define PP_OPTION_POWER_FLY         22 /* enable fly power up */
#define PP_OPTION_POWER_TURBO       23 /* enable turbo power up */
#define PP_OPTION_POWER_RAND        24
#define PP_OPTION_AUTO_TARGET       25

#define PP_OPTION_GAME_NORMAL    0
#define PP_OPTION_GAME_TIMED     1 /* game length is limited to a certain time */

#define PP_MAX_FONTS 16
#define PP_FONT_GAME          0
#define PP_FONT_TINY          1

#define PP_MUSIC_FILE_INTRO "pp.dat#music/intro.it"
#define PP_MUSIC_FILE_MENU "pp.dat#music/menu.it"

typedef struct
{
	
	PLAYER player[4];
	PARTICLE particle[MAX_PARTICLES];
	PAINTBALL paintball[MAX_PAINTBALLS];
	LEVEL * level;
	int flag_list[PP_MAX_OBJECTS];
	int flags;
	int base_list[PP_MAX_OBJECTS];
	int bases;
	int portal_list[PP_MAX_OBJECTS];
	int portals;
	int target_list[PP_MAX_OBJECTS];
	int targets;
	int hunt_list[PP_MAX_OBJECTS];
	int hunts;
	int flag_sprite;
	int base_sprite;
	int target_sprite;
	int gem_sprite;
	OBJECT object[PP_MAX_OBJECTS];
	int mode;
	int frame;
	int time;
	int winner;

} PP_GAME_DATA;

extern int logic_switch;
extern int pp_typing;
extern char pp_text[1024];
extern int pp_text_pos;
extern NCDMSG_QUEUE pp_messages;
extern PP_GAME_DATA pp_game_data;
extern int connect_time;
extern char network_id[256];
extern int server_closing;
extern int client_closing;
extern int char_ok;
extern int level_ok;
extern int net_debug;
extern int frame_okay;
extern char input_updated[4];
extern unsigned long char_sum[4][CHAR_LIST_MAX];
extern int char_sums[4];
extern unsigned long char_avail[CHAR_LIST_MAX];
extern int char_avails;
extern unsigned long char_pick[4];
extern unsigned long level_sum[4][CHAR_LIST_MAX];
extern int level_sums[4];
extern unsigned long level_avail[CHAR_LIST_MAX];
extern int level_avails;
extern int net_select;

extern PLAYER_DATA pp_player_data[PLAYER_DATA_MAX];
extern int pp_players;
extern char pp_new_data_text[256];
extern char pp_ip_text[256];
extern char pp_id_text[256];
extern char pp_new_data_key;
extern int pp_new_data_pos;
extern int pp_new_one;

extern BITMAP *  pp_screen;
extern BITMAP *  static_screen;
extern PALETTE   pp_palette;
extern PALETTE   logo_palette;
extern PALETTE   blue_palette;
extern COLOR_MAP pp_color_map;
extern COLOR_MAP pp_light_map;
//extern PLAYER    pp_player[4];
extern FONT *    al_font[PP_MAX_FONTS];
extern int       pp_state;
extern int       resume_state;
void(*net_yes_proc)();
void(*net_no_proc)();
extern int       pp_quit;
extern int       pp_logic_frames;
extern int       pp_timed;
extern int       pp_option[PP_OPTIONS];
extern int       pp_config[PP_OPTIONS];
extern char      pp_level_name[1024];
extern int       pp_global_frame;
extern char      pp_replay_name[1024];
extern int       pp_fade_step;
extern int       pp_fade_dir;
extern int       pp_fading;
extern PALETTE   pp_fade_start_pal;
extern PALETTE   pp_fade_end_pal;
extern void (*pp_fade_proc)();
extern int       pp_random_seed;

extern BITMAP *  pp_title_image;
extern BITMAP *  pp_menu_image;
extern PP_MENU   pp_menu[MAX_MENUS];
extern int       pp_current_menu;
extern BITMAP *  pp_menu_main_image[MAX_MENU_ITEMS];
extern char * key_names[];
#define MAX_CHARACTERS 6
extern char * character_name[MAX_CHARACTERS + 1];
extern char * character_file[MAX_CHARACTERS];
extern ANIMATION * character_place;
#define MAX_LEVELS     6
extern char * music_file[1024];
extern int music_index;
extern char temp_file[1024];
extern char pp_title_char_name[4][1024];
extern char pp_title_level_name[1024];
extern char pp_title_music_name[1024];
#define MAX_DEMOS 256
extern char demo_list[MAX_DEMOS][1024];
extern int demo_count;
extern int pp_stat;

extern CHAR_LIST_ENTRY char_entry[CHAR_LIST_MAX];
extern int char_entries;
extern LEVEL_LIST_ENTRY level_entry[LEVEL_LIST_MAX];
extern int level_entries;

/* font definitions */
extern NCD_FONT * pp_font[PP_MAX_FONTS];

extern int       single_player;

/* image definitions */
#define PP_MAX_IMAGES        32
#define PP_IMAGE_TITLE        0
#define PP_IMAGE_MENU         1
#define PP_IMAGE_ELIMINATED   2
#define PP_IMAGE_CLOCK        3
#define PP_IMAGE_SCORE        4
#define PP_IMAGE_TARGETS      5
#define PP_IMAGE_AMMO         6
#define PP_IMAGE_TIME         7
#define PP_IMAGE_FLAGS        8
#define PP_IMAGE_HITS         9
#define PP_IMAGE_POWER_C     10
#define PP_IMAGE_POWER_D     11
#define PP_IMAGE_POWER_J     12
#define PP_IMAGE_POWER_R     13
#define PP_IMAGE_GEMS_0      14
#define PP_IMAGE_GEMS_1      15
#define PP_IMAGE_GEMS_2      16
#define PP_IMAGE_GEMS_3      17
#define PP_IMAGE_GEMS_4      18
#define PP_IMAGE_GEMS_5      19
#define PP_IMAGE_LOGO        20
#define PP_IMAGE_POWER_F     21
#define PP_IMAGE_POWER_T     22
extern BITMAP *  pp_image[PP_MAX_IMAGES];

/* animation definitions */
#define PP_MAX_ANIMATIONS    32
//#define PP_ANI_MENU_SELECTOR  0
#define PP_ANI_IT_POINTER     1
#define PP_ANI_POINTER        2
#define PP_ANI_TARGET         3
extern ANIMATION * pp_ani[PP_MAX_ANIMATIONS];

extern ANIMATION * pp_object_ani[256];
extern COLLISION_MAP pp_object_cmap[256];

#define PP_MAX_SOUNDS       64
#define PP_SOUND_FIRE        0
#define PP_SOUND_SPLAT       1
#define PP_SOUND_RELOAD_A    2
#define PP_SOUND_RELOAD_B    3
#define PP_SOUND_AMMO        4
#define PP_SOUND_CLOAK       5
#define PP_SOUND_GEM         6
#define PP_SOUND_JUMP        7
#define PP_SOUND_LAND        8
#define PP_SOUND_HIT         9
#define PP_SOUND_BUMP       10
#define PP_SOUND_TIME       11
#define PP_SOUND_MENU_PICK  12
#define PP_SOUND_MENU_NEXT  13
#define PP_SOUND_TELE_IN    14
#define PP_SOUND_TELE_OUT   15
#define PP_SOUND_RUN        16
#define PP_SOUND_DEFLECT    17
#define PP_SOUND_FLAG       18
#define PP_SOUND_BANK       19
#define PP_SOUND_BASE       20
#define PP_SOUND_RESPAWN    21
#define PP_SOUND_PJUMP      22
#define PP_SOUND_MENU_LEFT  23
#define PP_SOUND_MENU_RIGHT 24
#define PP_SOUND_MENU_FLAG  25
#define PP_SOUND_MENU_ENTER 26
#define PP_SOUND_MENU_ESC   27
#define PP_SOUND_MENU_MOVE  28
#define PP_SOUND_CRUSH      29
#define PP_SOUND_TARGET     30
#define PP_SOUND_BOUNCE     31
#define PP_SOUND_RICOCHET   32
#define PP_SOUND_START      33
#define PP_SOUND_WIN        34
#define PP_SOUND_ENTER      35
#define PP_SOUND_FLY        36
#define PP_SOUND_PFLY       37
#define PP_SOUND_SPRING     38
#define PP_SOUND_PTURBO     39

extern SAMPLE * pp_sound[PP_MAX_SOUNDS];
extern SAMPLE * ncds_internal_sample;

extern int g_table[PLAYER_STATES];
extern int a_table[PLAYER_STATES];
extern char *    weapon_table[5];
extern char * weapon_info[5];

extern PP_CREDIT pp_credit[PP_MAX_CREDITS];
extern int pp_credits;

void text_enter(void);
void pp_build_music_list(void);
void pp_add_player_data(char * name);
void pp_delete_player_data(int p);
void pp_load_data(void);
void pp_free_data(void);

#endif
