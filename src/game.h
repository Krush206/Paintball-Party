#ifndef GAME_H
#define GAME_H

#define PP_MAX_MESSAGES 128

/* the different game modes */
#define PP_MODE_TARGET       0 /* (1p) target practice */
#define PP_MODE_SCAVENGER    1 /* (1p) scavenger hunt */
#define PP_MODE_SPLAT        2 /* (2p) splat fest */
#define PP_MODE_HUNTER      13 /* (2p) hunter */
#define PP_MODE_TAG          3 /* (2p) tag */
#define PP_MODE_FLAG         4 /* (2p) capture the flag */
#define PP_MODE_1P_TEST     14


void set_default_options(void);
void set_default_game_options(void);
int pp_game_init(int mode);
void pp_game_exit(void);
void pp_game_logic(void);
void pp_game_in_logic(void);
void pp_quit_yes(void);
void pp_quit_no(void);
void pp_instant_replay(void);
void pp_done_no(void);

#endif
