/*  pcm-audio-record_playback-test */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
// #include <speex/speex.h>
// #include <speex/speex_preprocess.h>    
// #include <speex/speex_echo.h> 
#include<sys/time.h>
#include<unistd.h>
#include <sys/types.h>
#include <pthread.h>

#define Swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)) 

/*	Handle for the PCM device	 */
snd_pcm_t *playback_handle; 		
snd_pcm_t *capture_handle;

signed short *capture_buffer  = NULL;
signed short *playback_buffer = NULL;

snd_pcm_uframes_t playback_frames; 
snd_pcm_uframes_t capture_frames; 

int p_size;
int c_size;


int EXIT_flag = 0;
int client_playback_flag =0;

struct  timeval    begin_record;
struct  timeval    end_record;

signed short  *buffer =NULL;
int size = 0;


void alsa_params_config(void )
{

	int i ;
	int err;
	int dir =0;  
	int periods = 20 ;/*Number of periods*/ 
	int write_count = 0;
	
    snd_pcm_uframes_t periodsize;
	snd_pcm_hw_params_t *p_hwparams; 	/*  Hardware information and PCM stream configerution*/	
	snd_pcm_hw_params_t *c_hwparams;

	snd_pcm_sw_params_t *p_swparams;	/*  Software information and PCM stream configerution*/

	/* 1.Open PCM , the last parameter of this function is the mode */
	/* If this is set to 0, the standard mode is used.*/
	

		if((err = snd_pcm_open(&capture_handle, "plughw:0,0" , SND_PCM_STREAM_CAPTURE, 0)) < 0){

			fprintf(stderr, "cannot open audio device  (%s) \n",  snd_strerror(err));
			fprintf(stderr," Sound card SGTL5000 does not work...\n");
			exit(1);
		}


	
	/* 2.Allocate the snd_pcm_hw_params_t structure on the stack */

		if((err = snd_pcm_hw_params_malloc(&c_hwparams)) < 0){

			fprintf(stderr, "cannot allocate hardware parameter structure (%s) \n", snd_strerror(err));
			exit(1);
		}

	/* 3.Init hw_params */

		if((err = snd_pcm_hw_params_any(capture_handle, c_hwparams)) < 0){
		
			fprintf(stderr, "cannot initialize capture hardware parameter structure (%s) \n ", snd_strerror(err));
			exit(1);
		}

	/* 4.Set access type */

		if((err = snd_pcm_hw_params_set_access(capture_handle, c_hwparams,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0){
			fprintf(stderr, "cannot set capture access type (%s) \n", snd_strerror(err));
			exit(1);	
		}
	
	
	/* 5.Set sample format */

		if((err = snd_pcm_hw_params_set_format(capture_handle, c_hwparams,
			SND_PCM_FORMAT_S16_LE)) < 0){
			 fprintf(stderr, "cannot set capture sample format (%s) \n", snd_strerror(err)); 
			 exit(1);    
		}

		
	/* 6.Set exact rate */
		int rate = 8000;


		if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, c_hwparams, &rate, 0)) < 0) {
			fprintf(stderr, "cannot set capture sample rate (%s)\n", snd_strerror(err));
			exit(1);
		}
		if (rate != 8000) {  
			fprintf(stderr, "The rate %d Hz is not supported by your hardware. Using %d Hz instead.\n", 8000, rate);  
		}
	

	/* 7.Set channel count */

		if((err =  snd_pcm_hw_params_set_channels(capture_handle, c_hwparams, 1)) < 0){

			fprintf(stderr, "cannot set channel count (%s) \n", snd_strerror(err)); 
			exit(1);    	
		}
  
	 /* Set buffer size (in frames). The resulting latency is given by */  
   	 /* latency = periodsize * periods / (rate * bytes_per_frame)     */
	    
		int temp = 0;
		periodsize = 160;
		//periodsize = 20;//可以更改为40bytes/2.5ms
		temp = periods * periodsize;
		unsigned capture_buffer_time = 0;
		unsigned capture_period_time = 0;


		/****capture test*************************************************************/

		if (snd_pcm_hw_params_set_periods(capture_handle, c_hwparams, periods, 0) < 0) {
			fprintf(stderr, "Error setting capture_handle periods.\n");
			exit(-1);
		}
		
		periodsize = periodsize*periods;

		err = snd_pcm_hw_params_set_buffer_size_near(capture_handle, c_hwparams, &periodsize);  
    	if (err < 0){  
			//fprintf(stderr, "Unable to set buffer_size %di : %s\n", periodsize,  snd_strerror(err));
			exit(1);
    	}
		
		periodsize = periodsize/periods;
		err = snd_pcm_hw_params_set_period_size_near(capture_handle, c_hwparams, &periodsize, 0);  
    	if (err < 0){  	
        	//fprintf(stderr, "Unable to set periodsize %di : %s\n", periodsize,  snd_strerror(err));
			exit(1);
    	}
		
		err = snd_pcm_hw_params_get_buffer_time(c_hwparams, &capture_buffer_time, 0);
		//printf("capture_buffer_time is : %d us\n", capture_buffer_time);

		
		err = snd_pcm_hw_params_get_period_time(c_hwparams, &capture_period_time, 0);
		//printf("capture_period_time is : %d us \n", capture_period_time);

			
		 	
    /**/  

	fprintf(stderr, "state: okay \n");

	/* 8.Set hw_params */	

		if((err = snd_pcm_hw_params(capture_handle, c_hwparams)) < 0 ){
			fprintf(stderr, "cannot set parameters (%s) \n", snd_strerror(err)); 
			exit(1);    
		}

	/* Free snd_pcm_hw_params_t stucture */
		snd_pcm_hw_params_free(c_hwparams);

	fprintf(stderr, "capture are ready...\n");


	 /* Use a buffer large enough to hold one period */  
		snd_pcm_hw_params_get_period_size(c_hwparams, &capture_frames, &dir);  


	c_size = capture_frames * 2 * 1; /* 2 bytes/sample,  channels */ 
	
	if(NULL == (capture_buffer = (signed short *) malloc(c_size))){

		printf("capture_buffer malloc error..\n");
		exit(1);
	}  
	

	// fprintf(stderr,  
				// "capture_size = %d\n",	
					// c_size);
					
	memset(capture_buffer, 0x00, c_size);

	//fprintf(stderr, "alsa-audio-params config  success\n");


}




main(int argc, char *argv[])
{
	int err;
	int write_count = 0;
	int record_counter = 0;
	int temp = 0;
	
	int zheng_sum = 0;
	int zheng_counter = 0;
	int zheng_average =0 ;
	
	int fu_sum = 0;
	int fu_counter = 0;
	int fu_average =0 ;
	int i =0;
	
	char tmp[50];
	bzero(tmp, 50);
	
   	fprintf(stderr," sgtl5000 okay test \n");

	alsa_params_config();
	
	sprintf(tmp, "amixer -c 0 cset numid=7 1");
	system(tmp);
	bzero(tmp, 50);
	
	
while(record_counter < 150) {

		//gettimeofday(&begin_record, NULL);
		err = snd_pcm_readi(capture_handle, (signed short *)capture_buffer, capture_frames);
		if(err == -EPIPE){
				
				fprintf(stderr, "an overrun occurred (%s)\n" ,snd_strerror (err));
				if((err = snd_pcm_prepare(capture_handle)) < 0 ){

               		 fprintf(stderr, "cannot prepare audio interface for  use (%s) \n", snd_strerror(err));
				}
		
		}
		else if (err == -EBADFD){

			 fprintf(stderr, "PCM is not in the right state (%s)\n", snd_strerror (err));
		}
		else if(err == -ESTRPIPE){

			fprintf(stderr, "a suspend event occurred (%s)\n", snd_strerror (err));			
		}
		else{

			//if(record_counter > 5){
				for (i; i < 160; i++){

					//capture_buffer[i] = Swap16(capture_buffer[i]);
					if (capture_buffer[i] > 0)
					{
						zheng_sum+= capture_buffer[i];
						zheng_counter++;
					}
					else if(capture_buffer[i] < 0)
					{
						fu_sum+= capture_buffer[i];
						fu_counter++;
					
					}
					else
					{
						//continue;
					}
					//fprintf(stderr, "capture_buffer[%d] : %d\r\n", i, capture_buffer[i]);

				}
			//}
				
			record_counter++;
			memset(capture_buffer, 0x00, c_size);//reset  0x00
			
			//if(record_counter > 10)break;//20ms * 10次
			//fprintf(stderr, "record_counter : %d\r\n", record_counter);
			i = 0;

		}
					


	}

	
	
OUT:
	
	
	//printf("1\n");
	if(capture_buffer != NULL){
		
		free(capture_buffer);
		capture_buffer = NULL;
	}
	
	/*  关闭PCM设备句柄 */
	snd_pcm_close(capture_handle);
	fprintf(stderr, "codes is over \n");
	
	fprintf(stderr, "record_counter : %d\r\n", record_counter);
	fprintf(stderr, "reboot threshold : 100\r\n");
	 if (zheng_counter != 0){
		printf("zheng_counter: %d, zheng_sum: %d, zheng_average: %d\r\n", zheng_counter, zheng_sum, (zheng_sum / zheng_counter));
		zheng_average = zheng_sum / zheng_counter;
	}
	else
	{
		printf("zheng_counter: 0 \r\n");

	}
	if (fu_counter != 0){
		printf("fu_counter: %d, fu_sum: %d, fu_average: %d\r\n", fu_counter, fu_sum, (fu_sum / fu_counter));
		fu_average = fu_sum / fu_counter;
	}
	else
	{
		printf("fu_counter: 0 \r\n");
	}
	
	
	
	// if(zheng_average > 2500)printf("zheng_average > \r\n");
	// else printf("zheng_average okay\r\n");
	
	// if(fu_average < -1000)printf("fu_average < \r\n");
	// else printf("fu_average okay\r\n");
	
	//if(fu_average < (-1000) || (zheng_average > 1000)){
		
		if(fu_average < (-100) && (zheng_average > 100)){
		
			printf("audio record fail... \r\n");
			sprintf(tmp, "reboot");//restart the device
			system(tmp);
			sleep(1);
		
		}
		// else{
			
			
			
			
		// }
		
		
	//}
	
	// if((zheng_average > 2500) && (fu_average < (-2000))){
		
		// printf("audio record fail... \r\n");
		// sprintf(tmp, "reboot");//restart the device
		// system(tmp);
		// sleep(2);
	// }
	else printf("audio record okay... \r\n");
	
	exit(0);				

}





