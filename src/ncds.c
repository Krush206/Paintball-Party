/* ------------------------------------------------------------------------ */
/* ---------------- T^3 Audio System (c)2001 T^3 Software. ---------------- */
/* ------------------------------------------------------------------------ */


/* include all necessary files */
#include <allegro.h>
#include <aldumb.h>
#include <string.h>
#include <ctype.h>
#include "ncds.h"
#include "wav.h"

#ifdef NCDS_DUMB
	#include <aldumb.h>
#endif

#ifdef NCDS_ALMP3
	#include <almp3.h>
#endif

#ifdef NCDS_ALOGG
	#include <alogg.h>
#endif

#ifdef NCDS_DUMB
	DUH * ncds_duh = NULL;
	AL_DUH_PLAYER *ncds_dplayer = NULL;
	DUH_SIGRENDERER * ncds_sigrenderer = NULL;
#else
	void * ncds_duh = NULL;
	void * ncds_dplayer = NULL;
#endif

MIDI * ncds_midi = NULL;

#ifdef NCDS_ALMP3
	ALMP3_MP3 * ncds_mp3 = NULL;
#else
	void * ncds_mp3 = NULL;
#endif

#ifdef NCDS_ALOGG
	ALOGG_OGG * ncds_ogg = NULL;
#else
	void * ncds_ogg = NULL;
#endif

char * ncds_data = NULL;

int ncds_sound_volume =    100; // volume of sound effects
int ncds_music_volume =    100; // volume of music effects
int ncds_initialized =       0; // is T3 Sound System initialized?
int ncds_sound_on =          1;
int ncds_music_on =          1;
int ncds_midi_on =           1;
int ncds_loop =              0;
int ncds_done =              1;
SAMPLE * ncds_internal_sample = NULL;
SAMPLE * ncds_sample_queue[NCDS_MAX_QUEUE] = {NULL};
int ncds_queue_channel = -1;
int ncds_compressed_buffer_size = 8192;

char ncds_music_type[NCDS_MAX_MUSIC_TYPES][8] = {0};
int ncds_music_types = 0;

void ncds_set_compressed_buffer_size(int size)
{
	ncds_compressed_buffer_size = size;
}

void ncds_add_music_type(char * ext)
{
	strcpy(ncds_music_type[ncds_music_types], ext);
	ncds_music_types++;
}

char * ncds_get_music(char * fn)
{
	int i;
	char * ret = (char *)malloc(1024);
	
	if(ncds_music_on && ncds_initialized)
	{
		for(i = 0; i < ncds_music_types; i++)
		{
			if(exists(replace_extension(ret, fn, ncds_music_type[i], 1024)))
			{
				return ret;
			}
		}
	}
	free(ret);
	return NULL;
}

char * buffer_file(char * fn)
{
	char * data;
	PACKFILE * fp;

	fp = pack_fopen(fn, "r");
	if(fp == NULL)
	{
		return NULL;
	}
	data = (char *)malloc(file_size(fn));
	pack_fread(data, file_size(fn), fp);
	pack_fclose(fp);
	return data;
}

/* initialize sound system */
int ncds_install(void)
{
	int i;
	int d;
	int good = 0;

	#ifdef ALLEGRO_WINDOWS
//	d = DIGI_DIRECTAMX(0);
	d = DIGI_AUTODETECT;
	#else
	d = DIGI_AUTODETECT;
	#endif
	if(!ncds_initialized)
    {
//	    ncds_midi_on = 1;
        reserve_voices(32, 0);
        if(install_sound(d, MIDI_AUTODETECT, NULL))
        {
	        if(install_sound(d, MIDI_NONE, NULL))
	        {
            	ncds_initialized = 0;
	            return 0;
            }
            else
            {
            	good = 1;
            	ncds_midi_on = 0;
            }
        }
        else
        {
	        good = 1;
	        ncds_midi_on = 1;
        }
        if(good)
        {
			/* set up the sound table */
        	ncds_internal_sample = create_sample(8, 0, 11025, 1);
			for(i = 0; i < ncds_internal_sample->len; i++)
			{
				((char *)ncds_internal_sample->data)[i] = 128;
			}
        	#ifdef NCDS_DUMB
				dumb_resampling_quality = 1;
				atexit(&dumb_exit);
				dumb_register_packfiles();
				ncds_add_music_type("s3m");
				ncds_add_music_type("xm");
				ncds_add_music_type("it");
				ncds_add_music_type("mod");
			#endif
			#ifdef NCDS_ALMP3
				ncds_add_music_type("mp3");
			#endif
			#ifdef NCDS_ALOGG
				ncds_add_music_type("ogg");
			#endif
			if(ncds_midi_on)
			{
				ncds_add_music_type("mid");
			}
        	ncds_initialized = 1;
        }
    }
    return ncds_initialized;
}


/* deinitializes sound system */
void ncds_remove(void)
{
    if(ncds_initialized)
    {
	    destroy_sample(ncds_internal_sample);
        remove_sound();
        ncds_initialized = 0;
    }
}

void ncds_enable_effects(void)
{
    ncds_sound_on = 1;
}

void ncds_enable_music(void)
{
    ncds_music_on = 1;
}

void ncds_disable_effects(void)
{
    ncds_sound_on = 0;
}

void ncds_disable_music(void)
{
    ncds_music_on = 0;
}

void ncds_stop_duh(void)
{
	#ifdef NCDS_DUMB
		al_stop_duh(ncds_dplayer);
		unload_duh(ncds_duh);
		ncds_duh = NULL;
		ncds_dplayer = NULL;
	#endif
}

void ncds_stop_mp3(void)
{
	#ifdef NCDS_ALMP3
		almp3_stop_mp3(ncds_mp3);
		almp3_destroy_mp3(ncds_mp3);
		ncds_mp3 = NULL;
		free(ncds_data);
		ncds_data = NULL;
	#endif
}

void ncds_stop_ogg(void)
{
	#ifdef NCDS_ALOGG
		alogg_stop_ogg(ncds_ogg);
		alogg_destroy_ogg(ncds_ogg);
		ncds_ogg = NULL;
		free(ncds_data);
		ncds_data = NULL;
	#endif
}

void ncds_stop_music(void)
{
	if(ncds_initialized && ncds_music_on)
	{
		if(ncds_duh && ncds_dplayer)
		{
			ncds_stop_duh();
		}
		else if(ncds_midi)
		{
			stop_midi();
			destroy_midi(ncds_midi);
			ncds_midi = NULL;
		}
		else if(ncds_mp3)
		{
			ncds_stop_mp3();
		}
		else if(ncds_ogg)
		{
			ncds_stop_ogg();
		}
		ncds_done = 1;
	}
}

void ncds_play_mp3(char * fn, int loop)
{
	#ifdef NCDS_ALMP3
		if(ncds_music_on && ncds_initialized)
		{
			ncds_stop_music();
			ncds_data = buffer_file(fn);
			if(ncds_data)
			{
				ncds_mp3 = almp3_create_mp3(ncds_data, file_size(fn));
				if(ncds_mp3)
				{
        			if(almp3_play_ex_mp3(ncds_mp3, ncds_compressed_buffer_size, (int)((float)ncds_music_volume * 2.5), 128, 1000, loop) == ALMP3_OK)
        			{
						ncds_set_music_volume(ncds_music_volume);
						ncds_loop = loop;
						ncds_done = 0;
					}
					else
					{
						almp3_destroy_mp3(ncds_mp3);
						ncds_mp3 = NULL;
						free(ncds_data);
						ncds_data = NULL;
					}
				}
				else
				{
					free(ncds_data);
				}
			}
		}
	#endif
}

void ncds_play_ogg(char * fn, int loop)
{
	#ifdef NCDS_ALOGG
		if(ncds_music_on && ncds_initialized)
		{
			ncds_stop_music();
			ncds_data = buffer_file(fn);
			if(ncds_data)
			{
				ncds_ogg = alogg_create_ogg_from_buffer(ncds_data, file_size(fn));
				if(ncds_ogg)
				{
        			if(alogg_play_ex_ogg(ncds_ogg, ncds_compressed_buffer_size, (int)((float)ncds_music_volume * 2.5), 128, 1000, loop) == ALOGG_OK)
        			{
						ncds_set_music_volume(ncds_music_volume);
						ncds_loop = loop;
						ncds_done = 0;
					}
					else
					{
						alogg_destroy_ogg(ncds_ogg);
						ncds_ogg = NULL;
						free(ncds_data);
						ncds_data = NULL;
					}
				}
				else
				{
					free(ncds_data);
				}
			}
		}
	#endif
}

void ncds_play_midi(char * fn, int loop)
{
	if(ncds_music_on && ncds_initialized && ncds_midi_on)
	{
		ncds_stop_music();
		ncds_midi = load_midi(fn);
		ncds_set_music_volume(ncds_music_volume);
		play_midi(ncds_midi, loop);
		ncds_loop = loop;
		ncds_done = 0;
	}
}

int ncds_dumb_loop_killer(void * data)
{
	dumb_it_sr_set_speed((DUMB_IT_SIGRENDERER *)data, 0);
//	ncds_done = 1;
	return 1;
}

void ncds_play_s3m(char * fn, int loop)
{
	#ifdef NCDS_DUMB
		if(ncds_music_on && ncds_initialized)
		{
			ncds_stop_music();
			ncds_duh = dumb_load_s3m(fn);
			ncds_dplayer = al_start_duh(ncds_duh, 2, 0, 1.0, NCDS_BUFFER_SIZE, 22050);
			ncds_set_music_volume(ncds_music_volume);
			ncds_loop = loop;
			ncds_done = 0;
			if(!loop)
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), ncds_dumb_loop_killer, duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)));
			}
			else
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), NULL, NULL);
			}
		}
	#endif
}

void ncds_play_xm(char * fn, int loop)
{
	#ifdef NCDS_DUMB
		if(ncds_music_on && ncds_initialized)
		{
			ncds_stop_music();
			ncds_duh = dumb_load_xm(fn);
			ncds_dplayer = al_start_duh(ncds_duh, 2, 0, 1.0, NCDS_BUFFER_SIZE, 22050);
			ncds_set_music_volume(ncds_music_volume);
			ncds_loop = loop;
			ncds_done = 0;
			if(!loop)
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), ncds_dumb_loop_killer, duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)));
			}
			else
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), NULL, NULL);
			}
		}
	#endif
}

void ncds_play_it(char * fn, int loop)
{
	#ifdef NCDS_DUMB
		if(ncds_music_on && ncds_initialized)
		{
			ncds_stop_music();
			ncds_duh = dumb_load_it(fn);
			ncds_dplayer = al_start_duh(ncds_duh, 2, 0, 1.0, NCDS_BUFFER_SIZE, 22050);
			ncds_set_music_volume(ncds_music_volume);
			ncds_loop = loop;
			ncds_done = 0;
			if(!loop)
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), ncds_dumb_loop_killer, duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)));
			}
			else
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), NULL, NULL);
			}
		}
	#endif
}

void ncds_play_mod(char * fn, int loop)
{
	#ifdef NCDS_DUMB
		if(ncds_music_on && ncds_initialized)
		{
			ncds_stop_music();
			ncds_duh = dumb_load_mod(fn);
			ncds_dplayer = al_start_duh(ncds_duh, 2, 0, 1.0, NCDS_BUFFER_SIZE, 22050);
			ncds_set_music_volume(ncds_music_volume);
			ncds_loop = loop;
			ncds_done = 0;
			if(!loop)
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), ncds_dumb_loop_killer, duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)));
			}
			else
			{
				dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), NULL, NULL);
			}
		}
	#endif
}

void ncds_play_music(char * fn, int loop)
{
	char str[1024] = {0};
	int i;
	int mode = 0;
	
	if(fn[0] != '\0')
	{
		strcpy(str, get_extension(fn));
		for(i = 0; i < strlen(str); i++)
		{
			str[i] = tolower(str[i]);
		}
		if(!strcmp(str, "mid"))
		{
			ncds_play_midi(fn, loop);
		}
		else if(!strcmp(str, "s3m"))
		{
			ncds_play_s3m(fn, loop);
		}
		else if(!strcmp(str, "xm"))
		{
			ncds_play_xm(fn, loop);
		}
		else if(!strcmp(str, "it"))
		{
			ncds_play_it(fn, loop);
		}
		else if(!strcmp(str, "mod"))
		{
			ncds_play_mod(fn, loop);
		}
		else if(!strcmp(str, "mp3"))
		{
			ncds_play_mp3(fn, loop);
		}
		else if(!strcmp(str, "ogg"))
		{
			ncds_play_ogg(fn, loop);
		}
		else
		{
			ncds_play_it(fn, loop);
		}
	}
}

void ncds_update_queue(void)
{
	int i, j;
	
	if(ncds_initialized)
	{
		if(ncds_sample_queue[0])
		{
			/* allocate a queue_channel if necessary */
			if(ncds_queue_channel < 0)
			{
				ncds_queue_channel = allocate_voice(ncds_sample_queue[0]);
	       		voice_set_volume(ncds_queue_channel, (int)(2.55 * (float)ncds_sound_volume));
       			voice_set_frequency(ncds_queue_channel, ncds_sample_queue[0]->freq);
       			voice_set_pan(ncds_queue_channel, 128);
       			voice_start(ncds_queue_channel);
       			for(j = 0; j < NCDS_MAX_QUEUE - 1; j++)
       			{
		        	ncds_sample_queue[j] = ncds_sample_queue[j + 1];
       			}
       			ncds_sample_queue[NCDS_MAX_QUEUE - 1] = NULL;
			}
			else if(voice_get_position(ncds_queue_channel) < 0)
			{
	        	reallocate_voice(ncds_queue_channel, ncds_sample_queue[0]);
	       		voice_set_volume(ncds_queue_channel, (int)(2.55 * (float)ncds_sound_volume));
       			voice_set_frequency(ncds_queue_channel, ncds_sample_queue[0]->freq);
       			voice_set_pan(ncds_queue_channel, 128);
       			voice_start(ncds_queue_channel);
       			for(j = 0; j < NCDS_MAX_QUEUE - 1; j++)
       			{
		        	ncds_sample_queue[j] = ncds_sample_queue[j + 1];
       			}
       			ncds_sample_queue[NCDS_MAX_QUEUE - 1] = NULL;
        	}
		}
		else
		{
			if(ncds_queue_channel >= 0 && voice_get_position(ncds_queue_channel) < 0)
			{
				deallocate_voice(ncds_queue_channel);
				ncds_queue_channel = -1;
			}
		}
	}
}

int queue_in_use(void)
{
	if(ncds_initialized && ncds_queue_channel >= 0)
	{
		if(voice_get_position(ncds_queue_channel) >= 0)
		{
			return 1;
		}
	}
	return 0;
}

void ncds_update_music(void)
{
	int i, ret;
	
	if(ncds_initialized && !ncds_done)
	{
		if(ncds_duh && ncds_dplayer)
		{
			#ifdef NCDS_DUMB
				ret = al_poll_duh(ncds_dplayer);
				if(ret != 0)
				{
					ncds_done = 1;
				}
			#endif
		}
		else if(ncds_mp3)
		{
			#ifdef NCDS_ALMP3
				ret = almp3_poll_mp3(ncds_mp3);
				if(ret == ALMP3_POLL_PLAYJUSTFINISHED)
				{
					ncds_done = 1;
				}
				else if(ret == ALMP3_POLL_NOTPLAYING || ret == ALMP3_POLL_FRAMECORRUPT || ret == ALMP3_POLL_INTERNALERROR)
				{
					ncds_stop_music();
				}
			#endif
		}
		else if(ncds_ogg)
		{
			#ifdef NCDS_ALOGG
				ret = alogg_poll_ogg(ncds_ogg);
				if(ret == ALOGG_POLL_PLAYJUSTFINISHED)
				{
					ncds_done = 1;
				}
				else if(ret == ALOGG_POLL_NOTPLAYING || ret == ALOGG_POLL_FRAMECORRUPT || ret == ALOGG_POLL_INTERNALERROR)
				{
					ncds_stop_music();
				}
			#endif
		}
		else if(ncds_midi)
		{
			if(midi_pos < 0)
			{
				ncds_done = 1;
			}
		}
		ncds_update_queue();
	}
}

void ncds_pause_music(void)
{
	if(ncds_dplayer)
	{
		#ifdef NCDS_DUMB
			al_pause_duh(ncds_dplayer);
		#endif
	}
	else if(ncds_midi)
	{
		midi_pause();
	}
	else if(ncds_mp3)
	{
		#ifdef NCDS_ALMP3
			almp3_stop_mp3(ncds_mp3);
		#endif
	}
	else if(ncds_ogg)
	{
		#ifdef NCDS_ALOGG
			alogg_stop_ogg(ncds_ogg);
		#endif
	}
}

void ncds_resume_music(void)
{
	if(ncds_dplayer)
	{
		#ifdef NCDS_DUMB
			al_resume_duh(ncds_dplayer);
		#endif
	}
	else if(ncds_midi)
	{
		midi_resume();
	}
	else if(ncds_mp3)
	{
		#ifdef NCDS_ALMP3
        	almp3_play_ex_mp3(ncds_mp3, 8192, (int)((float)ncds_music_volume * 2.5), 128, 1000, 1);
        #endif
	}
	else if(ncds_ogg)
	{
		#ifdef NCDS_ALOGG
        	alogg_play_ex_ogg(ncds_ogg, 8192, (int)((float)ncds_music_volume * 2.5), 128, 1000, 1);
        #endif
	}
}

void ncds_set_position(unsigned long pos)
{
	if(ncds_dplayer)
	{
		#ifdef NCDS_DUMB
		al_stop_duh(ncds_dplayer);
		ncds_sigrenderer = dumb_it_start_at_order(ncds_duh, 2, pos);
		ncds_dplayer = al_duh_encapsulate_sigrenderer(ncds_sigrenderer, 1.0, NCDS_BUFFER_SIZE, 22050);
		ncds_set_music_volume(ncds_music_volume);
		ncds_done = 0;
		if(!ncds_loop)
		{
			dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), ncds_dumb_loop_killer, duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)));
		}
		else
		{
			dumb_it_set_loop_callback(duh_get_it_sigrenderer(al_duh_get_sigrenderer(ncds_dplayer)), NULL, NULL);
		}
		#endif
	}
/*	else if(ncds_midi)
	{
		midi_resume();
	}
	else if(ncds_mp3)
	{
		#ifdef NCDS_ALMP3
        	almp3_play_ex_mp3(ncds_mp3, 8192, (int)((float)ncds_music_volume * 2.5), 128, 1000, 1);
        #endif
	}
	else if(ncds_ogg)
	{
		#ifdef NCDS_ALOGG
        	alogg_play_ex_ogg(ncds_ogg, 8192, (int)((float)ncds_music_volume * 2.5), 128, 1000, 1);
        #endif
	} */
}

/* set the music volume */
void ncds_set_music_volume(int volume)
{
	float final = (float)volume / 100.0;

	if(ncds_midi || (ncds_duh && ncds_dplayer) || ncds_mp3 || ncds_ogg)
	{
		#ifdef NCDS_DUMB
			al_duh_set_volume(ncds_dplayer, final);
		#endif
		set_volume(-1, (int)((float)volume * 2.5));
	}
	ncds_music_volume = volume;
}


/* set the sound effects volume */
void ncds_set_sound_volume(int volume)
{
    ncds_sound_volume = volume;
}


/* loads a wav file from a file pointer */
SAMPLE * ncds_load_wav_fp(PACKFILE * f)
{
	char buffer[25];
	int i;
	int length, len;
	int freq = 22050;
	int bits = 8;
	int channels = 1;
	signed short s;
	SAMPLE *spl = NULL;
	int loaded = 0;

	pack_fread(buffer, 12, f);          /* check RIFF header */
	if (memcmp(buffer, "RIFF", 4) || memcmp(buffer+8, "WAVE", 4))
	{
		goto getout;
	}

	while (!pack_feof(f) && !loaded)
	{
		if(pack_fread(buffer, 4, f) != 4)
		{
			break;
		}

		length = pack_igetl(f);          /* read chunk length */

		if (memcmp(buffer, "fmt ", 4) == 0)
		{
			i = pack_igetw(f);            /* should be 1 for PCM data */
			length -= 2;
			if (i != 1) 
			{
				goto getout;
			}

			channels = pack_igetw(f);     /* mono or stereo data */
			length -= 2;
			if ((channels != 1) && (channels != 2))
			{
	    		goto getout;
    		}

			freq = pack_igetl(f);         /* sample frequency */
			length -= 4;

			pack_igetl(f);                /* skip six bytes */
			pack_igetw(f);
			length -= 6;

			bits = pack_igetw(f);         /* 8 or 16 bit data? */
			length -= 2;
			if ((bits != 8) && (bits != 16))
			{
	    		goto getout;
    		}
		}
		else if (memcmp(buffer, "data", 4) == 0)
		{
			len = length / channels;

			if(bits == 16)
			{
				len /= 2;
			}

			spl = create_sample(bits, ((channels == 2) ? TRUE : FALSE), freq, len);
//			clear_keybuf();
//			text_mode(0);
//			textprintf(screen, font, 0, 0, 31, "%d %d %d %d", bits, channels, freq, len);
//			textprintf(screen, font, 0, 8, 31, "%d %d %d %d", spl->bits, spl->stereo, spl->freq, spl->len);
//			readkey();

			if (spl)
			{ 
				if (bits == 8)
				{
					pack_fread(spl->data, len, f);
				}
				else
				{
					for (i=0; i<len*channels; i++)
					{
						s = pack_igetw(f);
						((signed short *)spl->data)[i] = s^0x8000;
					}
				}

				length = 0;
				loaded = 1;

				if (*allegro_errno)
				{
					destroy_sample(spl);
					spl = NULL;
				}
			}
		}

		while (length > 0)              /* skip the remainder of the chunk */
		{
			if (pack_getc(f) == EOF)
			{
				break;
			}

			length--;
		}
	}

	getout:
	return spl;
}

/* loads wave from the specified file and offset */
SAMPLE * ncds_load_wav(char * FileName)
{
    PACKFILE * file;
    SAMPLE * SamPointer;

    /* open the file */
    file = pack_fopen(FileName, "r");
    if(file == NULL)
    {
        return NULL;
    }

    /* load the sample */
    SamPointer = ncds_load_wav_fp(file);
    pack_fclose(file);

    return SamPointer;
}


/* saves a wave file to file pointer */
int ncds_save_wav_fp(SAMPLE * SamplePointer, PACKFILE * f)
{
    int i;

    /* create the header */
    char RiffHeader[4] = {'R', 'I', 'F', 'F'};
    unsigned long FileLength = 36 + SamplePointer->len;
    char WaveHeader[4] = {'W', 'A', 'V', 'E'};
    char FormatHeader[4] = {'f', 'm', 't', ' '};
    unsigned long FormatLength = 0x10;
    unsigned int ExtraByte = 0x01;
    unsigned int Channels = 0x01;
    unsigned long SampleRate = SamplePointer->freq;
    unsigned long BytesPerSecond = SamplePointer->freq;
    unsigned int BytesPerSample = SamplePointer->bits / 8;
    unsigned int BitsPerSample = SamplePointer->bits;
    char DataHeader[4] = {'d', 'a', 't', 'a'};
    unsigned long SampleLength = SamplePointer->len;

    /* write header to file */
    pack_fwrite(RiffHeader, 4, f);
    pack_iputl(FileLength, f);
    pack_fwrite(WaveHeader, 4, f);
    pack_fwrite(FormatHeader, 4, f);
    pack_iputl(FormatLength, f);
    pack_iputw(ExtraByte, f);
    pack_iputw(Channels, f);
    pack_iputl(SampleRate, f);
    pack_iputl(BytesPerSecond, f);
    pack_iputw(BytesPerSample, f);
    pack_iputw(BitsPerSample, f);
    pack_fwrite(DataHeader, 4, f);
    pack_iputl(SampleLength, f);

    /* write sample data to file */
    pack_fwrite(SamplePointer->data, SamplePointer->len * BytesPerSample, f);

    return 1;
}


/* saves wave to the specified file */
int ncds_save_wav(SAMPLE * SamplePointer, char * FileName)
{
    PACKFILE * file;

    /* open file */
    file = pack_fopen(FileName, "w");
    if(file == NULL)
    {
        return 0;
    }

    /* save WAV to the file */
    if(!ncds_save_wav_fp(SamplePointer, file))
    {
        return 0;
    }

    /* close the file */
    pack_fclose(file);

    return 1;
}

/* release sound sample */
void ncds_free_sample(SAMPLE * sp)
{
    if(sp != NULL)
    {
        destroy_sample(sp);
    }
}

/* simplified sample playing function */
void ncds_play_sample(SAMPLE * sp, int pan, int freq, int priority)
{
	if(ncds_initialized && ncds_sound_on)
	{
		if(sp)
		{
			if(priority >= 0)
			{
				sp->priority = priority;
			}
			if(freq < 0)
			{
				freq = 1000;
			}
			play_sample(sp, (int)(2.55 * (float)ncds_sound_volume), pan, freq, 0);
		}
	}
}

void ncds_queue_sample(SAMPLE * sp)
{
	int i;
	
	if(ncds_initialized && ncds_sound_on)
	{
		if(sp && sp != ncds_internal_sample)
		{
			for(i = 0; i < NCDS_MAX_QUEUE; i++)
			{
				if(ncds_sample_queue[i] == NULL)
				{
					ncds_sample_queue[i] = sp;
					break;
				}
			}
		}
	}
}

void ncds_clear_queue(void)
{
	int i;
	
	for(i = 0; i < NCDS_MAX_QUEUE; i++)
	{
		ncds_sample_queue[i] = NULL;
	}
	if(ncds_queue_channel >= 0)
	{
		voice_stop(ncds_queue_channel);
	}
}

void ncds_hijack_queue(SAMPLE * sp)
{
	if(sp == NULL)
	{
		ncds_clear_queue();
	}
	else
	{
		if(queue_in_use())
		{
			ncds_clear_queue();
			ncds_queue_sample(sp);
		}
		else
		{
			ncds_queue_sample(sp);
		}
	}
}

/* figure out sound panning based on X coordinate and width */
int ncds_place_sound(int x, int width)
{
    float fw = width;
    float wr = width / 256;
    float fx = x * wr;

    if(fx < 0)
    {
        return 0;
    }
    else if(fx > 255)
    {
        return 255;
    }
    return (int)fx;
}
