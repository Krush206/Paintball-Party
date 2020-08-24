#ifndef OBJECTS_H
#define OBJECTS_H

#define PP_MAX_OBJECTS              256
#define PP_MAX_OBJECT_FLAGS          16

/* main objects */
#define PP_OBJECT_PORTAL              0
#define PP_OBJECT_GENERATOR           1

/* ammo power ups */
#define PP_OBJECT_AMMO_NORMAL         2
#define PP_OBJECT_AMMO_X              3
#define PP_OBJECT_AMMO_MINE           4
#define PP_OBJECT_AMMO_BOUNCE         5
#define PP_OBJECT_AMMO_SEEK           6

/* special power ups */
#define PP_OBJECT_POWER_CLOAK         7
#define PP_OBJECT_POWER_JUMP          8
#define PP_OBJECT_POWER_RUN           9
#define PP_OBJECT_POWER_DEFLECT      10

/* capture the flag objects */
#define PP_OBJECT_FLAG               11
#define PP_OBJECT_FLAG_PORTAL        12
#define PP_OBJECT_BASE               13
#define PP_OBJECT_BASE_PORTAL        14
#define PP_OBJECT_BANK               15

/* scavenger hunt objects */
#define PP_OBJECT_GEM_1              16
#define PP_OBJECT_GEM_2              17
#define PP_OBJECT_GEM_3              18
#define PP_OBJECT_GEM_4              19
#define PP_OBJECT_GEM_5              20
#define PP_OBJECT_HUNT_PORTAL        21
#define PP_OBJECT_HUNT_BASE          22

/* target practice objects */
#define PP_OBJECT_TARGET             23
#define PP_OBJECT_TARGET_PORTAL      24

/* extra objects */
#define PP_OBJECT_POWER_POOF         25
#define PP_OBJECT_GEM_POOF           26
#define PP_OBJECT_POOF               27

/* new objects for v1.1 */
#define PP_OBJECT_POWER_FLY          28
#define PP_OBJECT_JET                29
#define PP_OBJECT_SPRING_UP          30
#define PP_OBJECT_SPRING_DOWN        31
#define PP_OBJECT_SPRING_LEFT        32
#define PP_OBJECT_SPRING_RIGHT       33
#define PP_OBJECT_SPRING_BAR_V       34
#define PP_OBJECT_SPRING_BAR_H       35
#define PP_OBJECT_POWER_TURBO        36

/* generator flags */
#define PP_GENERATOR_FLAG_OBJECT      0 // object to drop when it's time
#define PP_GENERATOR_FLAG_TIME        1 // amount of time 'til object is generated

/* flag flags */
#define PP_FLAG_FLAG_DROP             0 // flag was dropped by player

/* target flags */
#define PP_TARGET_FLAG_TYPE           0 // type of target (tells us how to move it)
#define PP_TARGET_FLAG_WHO            1 // which portal generated the target
#define PP_TARGET_STATIC              0 // non-moving target
#define PP_TARGET_COSINE              1 // rotates around point along x-axis
#define PP_TARGET_SINE                2 // rotates around point along y-axis
#define PP_TARGET_CIRCLE              3 // rotates around point
#define PP_TARGET_BOUNCE_H            4 // bounce horizontally
#define PP_TARGET_BOUNCE_V            5 // bounce vertically
#define PP_TARGET_BOUNCE              6 // bounce horizontally and vertically

/* poof flags */
#define PP_POOF_FRAME                 0

typedef struct
{

	ANIMATION * ap;
	COLLISION_MAP cmap;
	int type, active;

	/* for normal moving objects */
	fixed x, y, vx, vy;

	/* for circular moving objects */
	int angle;
	int speed, cx, cy;
	fixed radius;

	/* special flags */
	int flag[PP_MAX_OBJECT_FLAGS];

} OBJECT;

int generate_object(int type, fixed x, fixed y);
int object_drop(OBJECT * op, int type, fixed ox, fixed oy);
void object_initialize(OBJECT * op);
void object_logic(OBJECT * op);
void object_draw(BITMAP * bp, OBJECT * op, int ox, int oy);

#endif
