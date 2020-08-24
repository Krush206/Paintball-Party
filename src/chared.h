#ifndef CHARED_H
#define CHARED_H

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

int tool_cut(void);
int tool_copy(void);
int tool_paste(void);
int tool_mirror(void);
int tool_flip(void);
int tool_turn(void);
void peg_item(void);
int file_menu_new(void);
int file_menu_load(void);
int file_menu_save(void);
int file_menu_web_update(void);
int file_menu_exit(void);
int view_menu_char(void);
int view_menu_ani(void);
int view_menu_pos(void);
int view_menu_scratch(void);
int view_menu_sounds(void);
int ani_menu_new(void);
int ani_sub_menu_import(void);
int ani_sub_menu_export(void);
int ani_menu_qo(void);
int ani_menu_fire(void);
int ani_menu_fire_all(void);
int ani_menu_rotate(void);
int frame_menu_insert(void);
int frame_menu_add(void);
int frame_menu_delete(void);
int frame_menu_import(void);
int frame_menu_import_sequence(void);
int frame_menu_export(void);
int frame_menu_clear(void);
int speed_menu_decrease(void);
int speed_menu_increase(void);
int frame_shift_left(void);
int frame_shift_right(void);
int frame_shift_up(void);
int frame_shift_down(void);
int ani_shift_left(void);
int ani_shift_right(void);
int ani_shift_up(void);
int ani_shift_down(void);
int menu_nop(void);
int item_menu_pf(void);
int item_menu_cmap(void);
int item_menu_flash(void);
int item_menu_it(void);
int item_menu_arrow(void);
int item_menu_paintball(void);
int item_menu_lock(void);
int sound_menu_patch(void);
int sound_menu_unpatch(void);
int sound_menu_blank(void);
int sound_menu_export(void);
int sound_menu_listen(void);
void control(void);
void update(void);

#endif
