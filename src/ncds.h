#ifndef SOUND_H
#define SOUND_H

#include <allegro.h>

#ifdef __cplusplus
extern "C" {
#endif

/* undefine these to disable portions of NCDS */
#define NCDS_DUMB

/* sound system channel modes */
#define NCDS_MAX_CHANNELS     32
#define NCDS_MAX_QUEUE        32
#define NCDS_MAX_MUSIC_TYPES  32

#define NCDS_BUFFER_SIZE    4096

extern int ncds_num_patterns;
extern int ncds_initialized;
extern int ncds_midi_on;
extern int ncds_sound_volume;
extern int ncds_music_volume;
extern int ncds_queue_channel;

extern int ncds_num_patterns;
extern char ncds_music_type[NCDS_MAX_MUSIC_TYPES][8];
extern int ncds_music_types;
extern SAMPLE * ncds_sample_queue[NCDS_MAX_QUEUE];

/* initialization functions */
void ncds_set_compressed_buffer_size(int size);
int ncds_install(void);
void ncds_remove(void);
void ncds_set_effects_channels(int number);
void ncds_set_reserved_channels(int number);
void ncds_enable_effects(void);
void ncds_enable_music(void);
void ncds_disable_effects(void);
void ncds_disable_music(void);
void ncds_set_music_volume(int volume);
void ncds_set_sound_volume(int volume);

/* loading functions */
void ncds_stop_music(void);
void ncds_play_s3m(char * fn, int loop);
void ncds_play_xm(char * fn, int loop);
void ncds_play_it(char * fn, int loop);
void ncds_play_music(char * fn, int loop);
char * ncds_get_music(char * fn);

void ncds_update_music(void);
void ncds_pause_music(void);
void ncds_resume_music(void);
void ncds_set_position(unsigned long pos);

SAMPLE * ncds_load_wav_fp(PACKFILE * fp);
SAMPLE * ncds_load_wav(char * fn);
int ncds_save_wav_fp(SAMPLE * sp, PACKFILE * fp);
int ncds_save_wav(SAMPLE * sp, char * fn);
void ncds_free_sample(SAMPLE * sp);

/* playing functions */
void ncds_play_sample(SAMPLE * sp, int pan, int freq, int priority);
void ncds_queue_sample(SAMPLE * sp);
void ncds_hijack_queue(SAMPLE * sp);
int queue_in_use(void);
int ncds_place_sound(int x, int width);

#ifdef __cplusplus
}
#endif

#endif
