#ifndef CONTROL_H
#define CONTROL_H

#include <joynet.h>

/* control bit defines for net play */
#define CONTROL_BIT_LEFT     0
#define CONTROL_BIT_RIGHT    1
#define CONTROL_BIT_UP       2
#define CONTROL_BIT_DOWN     3
#define CONTROL_BIT_FIRE     4
#define CONTROL_BIT_JUMP     5
#define CONTROL_BIT_WJUMP    6
#define CONTROL_BIT_OPTION   7
#define CONTROL_BIT_WOPTION  8
#define CONTROL_BIT_SELECT   9
#define CONTROL_BIT_WSELECT 10

#define MAX_JOY_KEYS     256
#define KEY_J0_LEFT      KEY_MAX + 0
#define KEY_J0_RIGHT     KEY_MAX + 1
#define KEY_J0_UP        KEY_MAX + 2
#define KEY_J0_DOWN      KEY_MAX + 3
#define KEY_J0_B0        KEY_MAX + 4
#define KEY_J0_B1        KEY_MAX + 5
#define KEY_J0_B2        KEY_MAX + 6
#define KEY_J0_B3        KEY_MAX + 7
#define KEY_J0_B4        KEY_MAX + 8
#define KEY_J0_B5        KEY_MAX + 9
#define KEY_J0_B6        KEY_MAX + 10
#define KEY_J0_B7        KEY_MAX + 11
#define KEY_J0_B8        KEY_MAX + 12
#define KEY_J0_B9        KEY_MAX + 13
#define KEY_J0_B10       KEY_MAX + 14
#define KEY_J1_LEFT      KEY_MAX + 15
#define KEY_J1_RIGHT     KEY_MAX + 16
#define KEY_J1_UP        KEY_MAX + 17
#define KEY_J1_DOWN      KEY_MAX + 18
#define KEY_J1_B0        KEY_MAX + 19
#define KEY_J1_B1        KEY_MAX + 20
#define KEY_J1_B2        KEY_MAX + 21
#define KEY_J1_B3        KEY_MAX + 22
#define KEY_J1_B4        KEY_MAX + 23
#define KEY_J1_B5        KEY_MAX + 24
#define KEY_J1_B6        KEY_MAX + 25
#define KEY_J1_B7        KEY_MAX + 26
#define KEY_J1_B8        KEY_MAX + 27
#define KEY_J1_B9        KEY_MAX + 28
#define KEY_J1_B10       KEY_MAX + 29
#define KEY_J2_LEFT      KEY_MAX + 30
#define KEY_J2_RIGHT     KEY_MAX + 31
#define KEY_J2_UP        KEY_MAX + 32
#define KEY_J2_DOWN      KEY_MAX + 33
#define KEY_J2_B0        KEY_MAX + 34
#define KEY_J2_B1        KEY_MAX + 35
#define KEY_J2_B2        KEY_MAX + 36
#define KEY_J2_B3        KEY_MAX + 37
#define KEY_J2_B4        KEY_MAX + 38
#define KEY_J2_B5        KEY_MAX + 39
#define KEY_J2_B6        KEY_MAX + 40
#define KEY_J2_B7        KEY_MAX + 41
#define KEY_J2_B8        KEY_MAX + 42
#define KEY_J2_B9        KEY_MAX + 43
#define KEY_J2_B10       KEY_MAX + 44
#define KEY_J3_LEFT      KEY_MAX + 45
#define KEY_J3_RIGHT     KEY_MAX + 46
#define KEY_J3_UP        KEY_MAX + 47
#define KEY_J3_DOWN      KEY_MAX + 48
#define KEY_J3_B0        KEY_MAX + 49
#define KEY_J3_B1        KEY_MAX + 50
#define KEY_J3_B2        KEY_MAX + 51
#define KEY_J3_B3        KEY_MAX + 52
#define KEY_J3_B4        KEY_MAX + 53
#define KEY_J3_B5        KEY_MAX + 54
#define KEY_J3_B6        KEY_MAX + 55
#define KEY_J3_B7        KEY_MAX + 56
#define KEY_J3_B8        KEY_MAX + 57
#define KEY_J3_B9        KEY_MAX + 58
#define KEY_J3_B10       KEY_MAX + 59

#define CONTROL_TYPES      8
#define CONTROL_TYPE_KEY_1 0
#define CONTROL_TYPE_KEY_2 1
#define CONTROL_TYPE_KEY_3 2
#define CONTROL_TYPE_KEY_4 3
#define CONTROL_TYPE_JOY_1 4
#define CONTROL_TYPE_JOY_2 5
#define CONTROL_TYPE_JOY_3 6
#define CONTROL_TYPE_JOY_4 7

/* control offsets */
#define CONTROL_KEY_START     0
#define CONTROL_JOY1_START  256
#define CONTROL_JOY2_START  512
#define CONTROL_JOY3_START  768
#define CONTROL_JOY4_START 1024

#define PP_KEY_UP     0
#define PP_KEY_DOWN   1
#define PP_KEY_LEFT   2
#define PP_KEY_RIGHT  3
#define PP_KEY_FIRE   4
#define PP_KEY_JUMP   5
#define PP_KEY_OPTION 6
#define PP_KEY_SELECT 7

typedef struct
{

    /* which keys are which */
    int left_key;
    int right_key;
    int up_key;
    int down_key;
    int fire_key;
    int jump_key;
    int option_key;
    int select_key;

    /* controller state */
    int left, right, up, down;
    int fire, wfire;
    int jump, wjump;
    int option, woption;
    int select, wselect;
    
	/* data for the internet play stuff */
	int button[8];
	unsigned char bit;

} PLAYER_CONTROLLER;

int cant_wait(void);
int get_key_all(void);
int get_joy_all(int jn);
void reset_controller(PLAYER_CONTROLLER * cp, int p);
void junk_controller(PLAYER_CONTROLLER * cp);
void clear_controller(PLAYER_CONTROLLER * cp);
void get_key_state(PLAYER_CONTROLLER * cp);
void read_controller(PLAYER_CONTROLLER * cp);
void read_controller_net(PLAYER_CONTROLLER * cp);

extern int joy_key[MAX_JOY_KEYS];
void clear_keys(void);
void read_joy_keys(void);
void copy_controller(PLAYER_CONTROLLER * cp1, PLAYER_CONTROLLER * cp2);
void compress_controller(PLAYER_CONTROLLER * cp, unsigned char * bit);
void uncompress_controller(PLAYER_CONTROLLER * cp, unsigned char * bit);

#endif
