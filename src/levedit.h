#ifndef LEVEDIT_H
#define LEVEDIT_H

void split_bitmap(BITMAP * bp, TILESET * tp, int start, int f, int d);
void split_ani(ANIMATION * ap, TILESET * tp, int start);
int tool_cut(void);
int tool_copy(void);
int tool_paste(void);
int tool_mirror(void);
int tool_flip(void);
int tool_turn(void);
void update(void);
char * layer_list_get_text(int index, int * size);
int menu_nop(void);
int file_menu_new(void);
int file_menu_load(void);
int file_menu_save(void);
int file_menu_exit(void);
int import_menu_tilemap(void);
int import_menu_tileset(void);
int import_menu_objectmap(void);
int export_menu_tilemap(void);
int export_menu_tileset(void);
int export_menu_objectmap(void);
void fix_view_menu(void);
int view_menu_tilemap(void);
int view_menu_tileset(void);
int view_menu_objectmap(void);
int object_menu_previous(void);
int object_menu_next(void);
int object_menu_delete(void);
int map_menu_properties(void);
int map_menu_resize(void);
int layer_menu_properties(void);
int tile_menu_properties(void);
void fix_layer_menu(void);
int layer_select_1(void);
int layer_select_2(void);
int layer_select_3(void);
int layer_select_4(void);
int layer_select_5(void);
int layer_shift_left(void);
int layer_shift_right(void);
int layer_shift_up(void);
int layer_shift_down(void);
int ani_sub_menu_import(void);
int ani_sub_menu_replace(void);
int ani_sub_menu_export(void);
int ani_sub_menu_insert(void);
int ani_sub_menu_delete(void);
int frame_menu_insert(void);
int frame_menu_add(void);
int frame_menu_delete(void);
int frame_menu_import(void);
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
void level_logic(void);
void level_draw(void);
void insert_tile(int pos, int num);
void delete_tile(int pos);
int bitmap_equal(BITMAP * bp1, BITMAP * bp2);
int tiles_consumed(BITMAP * bp, TILESET * tp);
void split_bitmap(BITMAP * bp, TILESET * tp, int start, int f, int d);
void split_ani(ANIMATION * ap, TILESET * tp, int start);
void get_tiles(char * fn, int ow);

#endif
