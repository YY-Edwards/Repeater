#ifndef MYREPEATER_H
#define MYREPEATER_H

#define ON	1
#define OFF 0


#include "Monitor_Interface.h"
#include "GPIO_App.h"
#include "AudioQueue.h"
#include "myspeex.h"
#include "AudioAlsa.h"
#include "Myrtp.h"
#include "Common.h"
#include "CMaster.h"
#include "CSlave.h"
#include "Protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h> 
#include <poll.h>
#include <signal.h> 
#include <stdint.h>
#include <errno.h>


typedef enum
{
	WAIT_SLAVE_GET_CHANNEL,
	WAIT_SLAVE_RELEASE_CHANNEL,

}master_state_t;

typedef enum
{
	
	WAIT_REMOTE_AUDIO_BEGIN,
	WAIT_REMOTE_AUDIO_END,
	WAIT_MASTER_REPLY_CHANENNL_STATUS,

}slave_state_t;

class MyRepeater
{

 public :
	 MyRepeater();
	 ~MyRepeater();
	void Start();
	
private :


	Monitor_Interface * my_baseinfo ;
	GPIO_App *my_gpio_app;
	AudioAlsa *my_alsa ;
	//Myrtp *my_rtp;
	Myrtp *my_recvrtp;
	Myspeex *my_speex;
	
	CProtocol *proto;
	int basedevice_ID;
	
	
	struct pollfd *fdset;
	list<uint32_t> localip;
	std::string baseip_str;
	std::string masterip_str;

	struct  timeval start;
	struct  timeval end;


	//init mutex, cond, sem
	 pthread_mutex_t CD_cond_mutex;
	 pthread_cond_t CD_trigger_cond;

	 pthread_mutex_t poll_cond_mutex;
	 pthread_cond_t mulcast_poll_cond;


	pthread_mutex_t send_rtp_cond_mutex;
	pthread_cond_t send_rtp_cond;

	//pthread_mutex_t playback_cond_mutex;
	pthread_cond_t playback_cond;
		
	//init queue
	AudioQueue 	m_RtpSendQueue;
	AudioQueue 	m_PlayBackQueue;
	AudioQueue 	m_EncodeQueue;
	AudioQueue 	m_DecodeQueue;


	//init pthread ID
	pthread_t id_CDpoll;
	pthread_t id_record;
	pthread_t id_playback;
	pthread_t id_rtppoll;
	pthread_t id_rtpsend;
	pthread_t id_time;
	pthread_t id_led;
	pthread_t id_encode;
	pthread_t id_decode;

	pthread_attr_t attr1, attr2, attr3, attr4;
	struct sched_param param;


	//init buffer point 
	char *capture_buffer;
	char *playback_buffer;

	//Global flag
	pthread_mutex_t flag_mutex ;
	volatile unsigned int CD_Trigger ;//0: invalid; 1:actived
	volatile unsigned int Mulcast_Trigger;//0: invalid; 1:actived
	/****master-use****/
	volatile unsigned int channel_busy_flag ;
	/****master-use****/

	pthread_mutex_t playback_start_flag_mutex;
	volatile unsigned int playback_start_flag;//wait 5p to playback

	volatile unsigned int audio_codec_err_counter;


	pthread_mutex_t map_mutex;
	//std::map <std::string, std::string> devicemap;

	std::map <std::string, std::string> base_masterMap;
	std::map <std::string, Myrtp *> sessionmap;
	void rtp_session_create(std::map<std::string, std::string> slavemap);

	volatile unsigned int timeout_flag;//0: invalid; 1:actived
	volatile unsigned int rtp_can_send;

	/****slave-use****/
	volatile unsigned int stop_send_rtp_flag;
	volatile unsigned int channel_applied_flag;
	volatile unsigned int slave_busy;
	/****slave-use****/

	//void config_hw(uint8_t userport, uint32_t baseip, uint32_t mulcastip);
	void config_hw(uint32_t baseip, uint32_t mulcastip);
						
	static void*/* DWORD WINAPI*/ CDPollThread(void*/*LPVOID*/ p);
	static void*/* DWORD WINAPI*/ RecordThread(void*/*LPVOID*/ p);
	static void*/* DWORD WINAPI*/ PlaybackThread(void*/*LPVOID*/ p);
	static void*/* DWORD WINAPI*/ RTPsendThread(void*/*LPVOID*/ p);
	static void*/* DWORD WINAPI*/ MulcastPortPollThread(void*/*LPVOID*/ p);
	static void*/* DWORD WINAPI*/ TimePollThread(void*/*LPVOID*/ p);
	static void*/* DWORD WINAPI*/ EncodeThread(void*/*LPVOID*/ p);
	static void*/* DWORD WINAPI*/ DecodeThread(void*/*LPVOID*/ p);

	//static void*/* DWORD WINAPI*/ LedIndicatorThread(void*/*LPVOID*/ p);
	
	
	void CDPollThreadFunc();
	void RecordThreadFunc();
	void PlaybackThreadFunc();
	void RTPsendThreadFunc();
	void MulcastPortPollThreadFunc();
	void TimePollThreadFunc();
	void LedIndicatorThreadFunc();
	void EncodeThreadFunc();
	void DecodeThreadFunc();
	
	static void  MasterOnData(int command, ResponeData data);
	void  MasterOnDataFunc(int command, ResponeData data);

	static void  SlaveOnData(int command, ResponeData data);
	void  SlaveOnDataFunc(int command, ResponeData data);
	
	void checkerror(int rtperr);
	void config_rtp_params(Myrtp *sess, uint32_t destip, uint16_t destport,
	uint32_t baseip, uint16_t baseport);
	
	void setTimer(int seconds, int useconds);
	void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
	
	void Connect(bool isMaster, const char *baseip, const char *masterip);
	void repeater_task_start();

	void Set_RecordAndTimePoll_Event();
	void Reset_RecordAndTimePoll_Event();
	int Wait_Record_Event();
	int Wait_TimePoll_Event();

	void Set_Playback_Event();
	void Reset_Playback_Event();
	int Wait_Playback_Event();

	void Set_RTPRecv_Event();
	void Reset_RTPRecv_Event();
	int Wait_RTPRecv_Event();

	static MyRepeater * pThis;   //静态对象指针

	volatile unsigned int Configurable_delay_time;

	bool set_thread_exit_flag;

	/*
	设置线程退出标志
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }

	
};


#endif





