/*
 * AudioAlsa.h
 *
 * Created: 2016/12/16
 * Author: EDWARDS
 */ 
#ifndef AudioAlsa_h_
#define AudioAlsa_h_ 

#include <alsa/asoundlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <syslog.h>

#define RESUME 0
#define PAUSE 1

class AudioAlsa 
{
	

public:

	AudioAlsa();
	~AudioAlsa();

	void config_alsa(){ alsa_params_config(); }
	uint16_t get_record_period_size(){ return c_size; }
	uint16_t get_playback_period_size(){ return p_size; }
	void get_record_buf(void *buff);//void *outbuf
	int8_t send_buf_playback(void *inputbuf);
	void do_pause(uint8_t action);
	void alsa_abort(void);
	void record_prepare(void){ snd_pcm_prepare(capture_handle); }
	void record_start(void){ snd_pcm_start(capture_handle); }
	void play_prepare(void){ snd_pcm_prepare(playback_handle); }
	void play_start(void){ snd_pcm_start(playback_handle); }


private:

	uint8_t alsa_config_flag;
	/*	Handle for the PCM device	 */
	snd_pcm_t *playback_handle;
	snd_pcm_t *capture_handle;

	snd_pcm_uframes_t playback_frames;
	snd_pcm_uframes_t capture_frames;

	uint16_t p_size;
	uint16_t c_size;

	uint16_t playback_can_pause;
	uint16_t capture_can_pause;

	void alsa_params_config();

	
	
};

#endif
