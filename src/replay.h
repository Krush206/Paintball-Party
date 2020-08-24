#ifndef REPLAY_H
#define REPLAY_H

extern int demo_playback;
extern PACKFILE * pp_replay_file;
extern int replay_pause;
extern int replay_step;
extern int replay_camera;
extern int replay_zoom;

void pp_replay_set_camera(int num);
void pp_replay_exit(void);
void pp_copy_file(char * fn, char * dfn);
int pp_replay_start_play(char * fn, int rew);
int pp_replay_restart(char * fn);
void pp_replay_start_record(char * fn);
void pp_replay_stop_record(void);
void pp_replay_read_header(void);
void pp_replay_write_header(void);
void pp_replay_read_controls(void);
void pp_replay_write_controls(void);
void pp_replay_stop(void);

#endif
