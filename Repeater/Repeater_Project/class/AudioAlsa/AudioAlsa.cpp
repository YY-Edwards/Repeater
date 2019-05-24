/*
* AudioAlsa.c
*
* Created: 2016/12/19
* Author: EDWARDS
*/

#include "AudioAlsa.h"
//extern int pause_flag;

AudioAlsa::AudioAlsa()
	: playback_handle(NULL)
	, capture_handle(NULL)
	, p_size(0)
	, c_size(0)
	, playback_frames(0)
	, capture_frames(0)
	, alsa_config_flag(0)
	, playback_can_pause(0)
	, capture_can_pause(0)
{

	config_alsa();

}

AudioAlsa::~AudioAlsa()
{

	snd_pcm_nonblock(playback_handle, 0);
	snd_pcm_drain(playback_handle);
	snd_pcm_close(playback_handle);
	//free(playback_handle);
	playback_handle = NULL;

	snd_pcm_nonblock(capture_handle, 0);
	snd_pcm_drain(capture_handle);
	snd_pcm_close(capture_handle);
	//free(capture_handle);
	capture_handle = NULL;



	fprintf(stderr,"delete class AudioAlsa\n");

}

void AudioAlsa::alsa_params_config()
{
		int i;
		int err;
		int dir = 0;
		int periods = 12;/*Number of periods*/
		int write_count = 0;

		char tem[50];
		bzero(tem, 50);

		snd_pcm_uframes_t periodsize;
		snd_pcm_hw_params_t *p_hwparams; 	/*  Hardware information and PCM stream configerution*/
		snd_pcm_hw_params_t *c_hwparams;
		snd_pcm_sw_params_t *p_swparams;	/*  Software information and PCM stream configerution*/
		//snd_pcm_sw_params_t *c_swparams;

		if (alsa_config_flag == 0)
			alsa_config_flag = 1;

		/* 1.Open PCM , the last parameter of this function is the mode */
		/* If this is set to 0, the standard mode is used.*/

		if ((err = snd_pcm_open(&playback_handle, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0)) < 0){

			fprintf(stderr, "cannot open audio device (%s) \n", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_open(&capture_handle, "plughw:0,0", SND_PCM_STREAM_CAPTURE, 0)) < 0){

			fprintf(stderr, "cannot open audio device  (%s) \n", snd_strerror(err));
			exit(1);
		}

		/* 2.Allocate the snd_pcm_hw_params_t structure on the stack */
		if ((err = snd_pcm_hw_params_malloc(&p_hwparams)) < 0){

			fprintf(stderr, "cannot allocate hardware parameter structure (%s) \n", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_hw_params_malloc(&c_hwparams)) < 0){

			fprintf(stderr, "cannot allocate hardware parameter structure (%s) \n", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_sw_params_malloc(&p_swparams)) < 0){

			fprintf(stderr, "cannot allocate software parameter structure (%s) \n", snd_strerror(err));
			exit(1);
		}


		/* 3.Init hw_params */
		if ((err = snd_pcm_hw_params_any(playback_handle, p_hwparams)) < 0){

			fprintf(stderr, "cannot initialize playback hardware parameter structure (%s) \n ", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_hw_params_any(capture_handle, c_hwparams)) < 0){

			fprintf(stderr, "cannot initialize capture hardware parameter structure (%s) \n ", snd_strerror(err));
			exit(1);
		}

		/* 4.Set access type */
		if ((err = snd_pcm_hw_params_set_access(playback_handle, p_hwparams,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0){
			fprintf(stderr, "cannot set playback access type (%s) \n", snd_strerror(err));
			exit(1);
		}
		if ((err = snd_pcm_hw_params_set_access(capture_handle, c_hwparams,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0){
			fprintf(stderr, "cannot set capture access type (%s) \n", snd_strerror(err));
			exit(1);
		}


		/* 5.Set sample format */
		if ((err = snd_pcm_hw_params_set_format(playback_handle, p_hwparams,
			SND_PCM_FORMAT_S16_LE)) < 0){
			fprintf(stderr, "cannot set playback sample format (%s) \n", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_hw_params_set_format(capture_handle, c_hwparams,
			SND_PCM_FORMAT_S16_LE)) < 0){
			fprintf(stderr, "cannot set capture sample format (%s) \n", snd_strerror(err));
			exit(1);
		}


		/* 6.Set exact rate */
		unsigned int rate = 8000;


		if ((err = snd_pcm_hw_params_set_rate_near(playback_handle, p_hwparams, &rate, 0)) < 0) {

			fprintf(stderr, "cannot set playback sample rate (%s)\n", snd_strerror(err));
			exit(1);
		}
		if (rate != 8000) {
			fprintf(stderr, "The rate %d Hz is not supported by your hardware. Using %d Hz instead.\n", 8000, rate);
		}

		if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, c_hwparams, &rate, 0)) < 0) {
			fprintf(stderr, "cannot set capture sample rate (%s)\n", snd_strerror(err));
			exit(1);
		}
		if (rate != 8000) {
			fprintf(stderr, "The rate %d Hz is not supported by your hardware. Using %d Hz instead.\n", 8000, rate);
		}


		/* 7.Set channel count */
		if ((err = snd_pcm_hw_params_set_channels(playback_handle, p_hwparams, 1)) < 0){

			fprintf(stderr, "cannot set channel count (%s) \n", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_hw_params_set_channels(capture_handle, c_hwparams, 1)) < 0){

			fprintf(stderr, "cannot set channel count (%s) \n", snd_strerror(err));
			exit(1);
		}
		
		//sample	= 2bytes(LSB,MSB)-SND_PCM_FORMAT_S16_LE
		//frame		= 1*sample(channel)
		//period	= x*frames
		//buffer	= y*periods
		//采样率(rate)：每秒钟采样次数，该次数是针对帧而言，常用的采样率如8KHz的人声

		/* Set buffer size (in frames). The resulting latency is given by */
		/* latency = periodsize * periods / (rate * bytes_per_frame)     */

		int temp = 0;
		periodsize = 160;
		temp = periods * periodsize;
		unsigned capture_buffer_time = 0;
		unsigned capture_period_time = 0;
		unsigned playback_buffer_time = 0;
		unsigned playback_period_time = 0;


		/****capture test*************************************************************/

	/*	if (snd_pcm_hw_params_set_periods(capture_handle, c_hwparams, periods, 0) < 0) {
			fprintf(stderr, "Error setting capture_handle periods.\n");
			exit(-1);
		}
		if (snd_pcm_hw_params_set_periods(playback_handle, p_hwparams, periods, 0) < 0) {
			fprintf(stderr, "Error setting capture_handle periods.\n");
			exit(-1);
		}*/


		periodsize = periodsize*periods;
		err = snd_pcm_hw_params_set_buffer_size_near(playback_handle, p_hwparams, &periodsize);
		if (err < 0){
			fprintf(stderr, "Unable to set buffer_size %ld : %s\n", periodsize, snd_strerror(err));
			exit(1);
		}
		err = snd_pcm_hw_params_set_buffer_size_near(capture_handle, c_hwparams, &periodsize);
		if (err < 0){
			fprintf(stderr, "Unable to set buffer_size %ld : %s\n", periodsize, snd_strerror(err));
			exit(1);
		}

		periodsize = periodsize/periods;
		err = snd_pcm_hw_params_set_period_size_near(playback_handle, p_hwparams, &periodsize, 0);
		if (err < 0){
			fprintf(stderr, "Unable to set periodsize %ld : %s\n", periodsize, snd_strerror(err));
			exit(1);
		}

		err = snd_pcm_hw_params_set_period_size_near(capture_handle, c_hwparams, &periodsize, 0);
		if (err < 0){
			fprintf(stderr, "Unable to set periodsize %ld : %s\n", periodsize, snd_strerror(err));
			exit(1);
		}

		err = snd_pcm_hw_params_get_buffer_time(c_hwparams, &capture_buffer_time, 0);
		fprintf(stderr,"capture_buffer_time is : %d us\n", capture_buffer_time);
		err = snd_pcm_hw_params_get_buffer_time(p_hwparams, &playback_buffer_time, 0);
		fprintf(stderr,"playback_buffer_time is : %d us\n", playback_buffer_time);

		err = snd_pcm_hw_params_get_period_time(c_hwparams, &capture_period_time, 0);
		fprintf(stderr,"capture_period_time is : %d us \n", capture_period_time);
		err = snd_pcm_hw_params_get_period_time(p_hwparams, &playback_period_time, 0);
		fprintf(stderr,"playback_period_time is : %d us \n", playback_period_time);

		/**/

		//fprintf(stderr, "state: okay \n");

		playback_can_pause = snd_pcm_hw_params_can_pause(p_hwparams);
		capture_can_pause = snd_pcm_hw_params_can_pause(c_hwparams);

		if (!capture_can_pause) {
			fprintf(stderr, "\rCapture PAUSE command ignored (no hw support)\n");
		}
		if (!playback_can_pause) {
			fprintf(stderr, "\rPlayback PAUSE command ignored (no hw support)\n");
		}


		/* 8.Set hw_params */
		if ((err = snd_pcm_hw_params(playback_handle, p_hwparams)) < 0){
			fprintf(stderr, "cannot set parameters (%s) \n", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_hw_params(capture_handle, c_hwparams)) < 0){
			fprintf(stderr, "cannot set parameters (%s) \n", snd_strerror(err));
			exit(1);
		}

		/* Free snd_pcm_hw_params_t stucture */
		snd_pcm_hw_params_free(p_hwparams);
		snd_pcm_hw_params_free(c_hwparams);

		//fprintf(stderr, "capture and playback are ready...\n");


		/* Use a buffer large enough to hold one period */
		snd_pcm_hw_params_get_period_size(c_hwparams, &capture_frames, &dir);
		snd_pcm_hw_params_get_period_size(p_hwparams, &playback_frames, &dir);


		c_size = capture_frames * 2 * 1; /* 2 bytes/sample,  channels */
		p_size = playback_frames * 2 * 1;

		//if (NULL == (capture_buffer = (signed short *)malloc(c_size))){

		//	fprintf(stderr,"capture_buffer malloc error..\n");
		//	exit(1);
		//}

		//if (NULL == (playback_buffer = (signed short *)malloc(p_size))){

		//	fprintf(stderr,"playback_buffer malloc error..\n");
		//	exit(1);
		//}


		/*fprintf(stderr,
			"capture_size = %d\n",
			c_size);*/


		//fprintf(stderr,
		//	"playback_size = %d\n",
		//	p_size);

		/*9.Return current software configuration for a playback_handle */
		if ((err = snd_pcm_sw_params_current(playback_handle, p_swparams)) < 0){
			fprintf(stderr, "cannot get playback_handle sw parameters (%s) \n", snd_strerror(err));
			exit(1);
		}

		///*10.Set start threshold */
		if ((err = snd_pcm_sw_params_set_start_threshold(playback_handle, p_swparams, 1)) < 0){
			fprintf(stderr, "cannot set start_threshold (%s) \n", snd_strerror(err));
			exit(1);
		}


		snd_pcm_uframes_t p_boundary;
		///*11.get boundary for ring  */
		if ((err = snd_pcm_sw_params_get_boundary(p_swparams, &p_boundary))<0)
		{
			fprintf(stderr, "sw params get boundary failed, errorcode = %s", snd_strerror(err));
			exit(1);
		}
		//fprintf(stderr, " get p_boundary: %ld \n", p_boundary);

		/*12.disable underrun reporting */
		if ((err = snd_pcm_sw_params_set_stop_threshold(playback_handle, p_swparams, p_boundary))<0)
		{
			fprintf(stderr, "sw params stop threshold failed, errorcode = %s", snd_strerror(err));
			exit(1);
		}

		///*13.play silence when there is an underrun */
		if ((err = snd_pcm_sw_params_set_silence_size(playback_handle, p_swparams, p_boundary))<0)
		{
			fprintf(stderr, "sw params set silence size failed, errorcode = %s", snd_strerror(err));
			exit(1);
		}

		if ((err = snd_pcm_sw_params(playback_handle, p_swparams))<0)
		{
			fprintf(stderr, "sw params failed, errorcode = %s", snd_strerror(err));
			exit(1);
		}

		snd_pcm_sw_params_free(p_swparams);

		//if ((err = snd_pcm_prepare(playback_handle)) < 0){
		//	fprintf(stderr, "cannot prepare audio interface for  use (%s) \n", snd_strerror(err));
		//	exit(1);
		//}

		//if ((err = snd_pcm_prepare(capture_handle)) < 0){
		//	fprintf(stderr, "cannot prepare audio interface for  use (%s) \n", snd_strerror(err));
		//	exit(1);
		//}

		fprintf(stderr, "alsa-audio-params config  success\n");

}

void AudioAlsa::get_record_buf(void *buff)
{
	if (alsa_config_flag != 1){
		fprintf(stderr,"alsa is not configed!!!\n");
		exit(-1);
	}
	int err = 0;

		err = snd_pcm_readi(capture_handle, (signed short *)buff, capture_frames);
		//gettimeofday(&end_record, NULL);
		//这里只能测试出DMA传输320bytes所需要的时间
		//首先硬件层声卡1个period时间到了，会通知内核和驱动来读/写数据，然后驱动再传输/拷贝application-buffer(内存到buffer的传输是通过DMA传输)
		//fprintf(stderr,"3.Record transmit time: %ld,%ld \n", (end_record.tv_sec - begin_record.tv_sec), (end_record.tv_usec - begin_record.tv_usec));
		if (err == -EPIPE){
			fprintf(stderr, "\r\n an overrun occurred (%s) \r\n", snd_strerror(err));
			if ((err = snd_pcm_prepare(capture_handle)) < 0){
				fprintf(stderr, "cannot prepare audio interface for  use (%s) \n", snd_strerror(err));
			}
		}
		else if (err == -EBADFD){
			fprintf(stderr, "PCM is not in the right state (%s)\n", snd_strerror(err));
		}
		else if (err == -ESTRPIPE){
			fprintf(stderr, "a suspend event occurred (%s)\n", snd_strerror(err));
		}
		else{

	
		}

}

int8_t AudioAlsa::send_buf_playback(void *inputbuf)
{
	int8_t err = 0;
	int8_t err_p = 0;

	err = snd_pcm_writei(playback_handle, (signed short *)inputbuf, playback_frames);

	if (err == -EPIPE){
		/* EPIPE means underrun */
		fprintf(stderr, "\r\n underrun occurred \r\n");
		/* Finish hardware parameters set ,and make device prepared */
		if ((err_p = snd_pcm_prepare(playback_handle)) < 0){
			fprintf(stderr, "cannot prepare audio interface for  use (%s) \n", snd_strerror(err));
		}
		//fprintf(stderr, "underrun err value is :%d \n", err);
		return err;
	}
	else if (err == -EBADFD){

		fprintf(stderr, "write:EBADFD  failed (%s)\n", snd_strerror(err));
		err = snd_pcm_state(playback_handle);

		switch (err){

			case SND_PCM_STATE_SETUP:

				fprintf(stderr,"pcm state is : SNDRV_PCM_STATE_SETUP\n");
				snd_pcm_prepare(playback_handle);
				break;

			case SND_PCM_STATE_SUSPENDED:

				fprintf(stderr,"pcm state is : SNDRV_PCM_STATE_SUSPENDED\n");
				while ((err = snd_pcm_resume(playback_handle)) == -EAGAIN)
					sleep(1);

				snd_pcm_prepare(playback_handle);

				break;

			default:

				fprintf(stderr,"unkonow err \n");
				break;

			}
	}

	else if (err == -ESTRPIPE){

			fprintf(stderr, "write:ESTRPIPE failed (%s)\n", snd_strerror(err));
			while ((err = snd_pcm_resume(playback_handle)) == -EAGAIN)
				sleep(1);
			if (err < 0){
				err = snd_pcm_prepare(playback_handle);
				if (err < 0)fprintf(stderr,"can not recovery\n");
				//break;
			}
	}
	else{
	
		return 0;

	}
}
void AudioAlsa::do_pause(uint8_t action)
{
	int err;

	if (!playback_can_pause) {
		fprintf(stderr, "\rPAUSE command ignored (no hw support)\n");
		return;
	}
	if (!capture_can_pause) {
		fprintf(stderr, "\rPAUSE command ignored (no hw support)\n");
		return;
	}

	err = snd_pcm_state(playback_handle);
	if (((err == SND_PCM_STATE_RUNNING) && (action == PAUSE))
		|| ((err == SND_PCM_STATE_PAUSED) && (action == RESUME)))
	{
		err = snd_pcm_pause(playback_handle, action);
		if (err < 0) {
			fprintf(stderr, "playback_handle pause push error: %s\n", snd_strerror(err));
			return;
		}
		if (action == PAUSE)
		{
			fprintf(stderr, "Pause playback...\n");
		}
		if (action == RESUME)
		{
			fprintf(stderr, "Resume playback...\n");
		}
	}

	err = snd_pcm_state(capture_handle);
	if (((err == SND_PCM_STATE_RUNNING) && (action == PAUSE))
		|| ((err == SND_PCM_STATE_PAUSED) && (action == RESUME)))
	{
		err = snd_pcm_pause(capture_handle, action);
		if (err < 0) {
			fprintf(stderr, "capture_handle pause push error: %s\n", snd_strerror(err));
			return;
		}
		if (action == PAUSE)
		{
			fprintf(stderr, "Pause capture...\n");
		}
		if (action == RESUME)
		{
			fprintf(stderr, "Resume capture...\n");
		}
	}
	
}
void AudioAlsa::alsa_abort(void)
{
	if ((playback_handle != NULL) || (capture_handle != NULL)){

		snd_pcm_abort(playback_handle); 
		snd_pcm_abort(capture_handle);

		snd_pcm_drain(capture_handle);
		snd_pcm_drain(playback_handle);

		snd_pcm_close(playback_handle);
		snd_pcm_close(capture_handle);

		playback_handle = NULL;
		capture_handle = NULL;
		fprintf(stderr, "Close alsa...\n");
	}

}
