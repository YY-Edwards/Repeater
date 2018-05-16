#ifndef MYREPEATER_H
#define MYREPEATER_H

#define ON	1
#define OFF 0

#include "config.h"
#include "syninterface.h"
#include "fifoqueue.h"
#include "Monitor_Interface.h"
#include "GPIO_App.h"
//#include "AudioQueue.h"
#include "myspeex.h"
#include "AudioAlsa.h"
#include "Myrtp.h"
#include "Common.h"
#include "CMaster.h"
#include "CSlave.h"
#include "Protocol.h"


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
	void Stop();

	
private :

	bool	m_PleaseStopRepeater;

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



	MySynCond *CD_trigger_cond;
	MySynCond *mulcast_poll_cond;
	MySynCond *send_rtp_cond;
	MySynCond *playback_cond;
	

	//init mutex, cond, sem
	// pthread_mutex_t CD_cond_mutex;
	// pthread_cond_t CD_trigger_cond;

	// pthread_mutex_t poll_cond_mutex;
	// pthread_cond_t mulcast_poll_cond;


	//pthread_mutex_t send_rtp_cond_mutex;
	//pthread_cond_t send_rtp_cond;

	////pthread_mutex_t playback_cond_mutex;
	//pthread_cond_t playback_cond;
	

	FifoQueue m_RtpSendQueue;
	RingQueue m_PlayBackQueue;
	//FifoQueue m_PlayBackQueue;
	FifoQueue m_EncodeQueue;
	FifoQueue m_DecodeQueue;

	//init queue
	//AudioQueue 	m_RtpSendQueue;
	//AudioQueue 	m_PlayBackQueue;
	//AudioQueue 	m_EncodeQueue;
	//AudioQueue 	m_DecodeQueue;


	MyCreateThread * cd_poll_thread_p;
	MyCreateThread * record_thread_p;
	MyCreateThread * playback_thread_p;
	MyCreateThread * rtp_poll_thread_p;
	MyCreateThread * rtp_send_thread_p;
	MyCreateThread * timer_thread_p;
	MyCreateThread * encode_thread_p;
	MyCreateThread * decode_thread_p;

	//init pthread ID
	//pthread_t id_CDpoll;
	//pthread_t id_record;
	//pthread_t id_playback;
	//pthread_t id_rtppoll;
	//pthread_t id_rtpsend;
	//pthread_t id_time;
	//pthread_t id_led;
	//pthread_t id_encode;
	//pthread_t id_decode;

	/*pthread_attr_t attr1, attr2, attr3, attr4;
	struct sched_param param;*/


	//init buffer point 
	char *capture_buffer;
	char *playback_buffer;

	//Global flag
	Mutex *flag_mutex;
	//pthread_mutex_t flag_mutex ;
	volatile unsigned int CD_Trigger_flag ;//0: invalid; 1:actived
	volatile unsigned int Mulcast_Trigger_flag;//0: invalid; 1:actived
	/****master-use****/
	volatile unsigned int channel_busy_flag ;
	/****master-use****/

	Mutex *playback_start_flag_mutex;
	//pthread_mutex_t playback_start_flag_mutex;
	volatile unsigned int playback_start_flag;//wait 5p to playback

	volatile unsigned int audio_codec_err_counter;

	Mutex *map_mutex;
	//pthread_mutex_t map_mutex;
	//std::map <std::string, std::string> devicemap;

	std::map <std::string, std::string> base_masterMap;
	std::map <std::string, Myrtp *> sessionmap;
	void rtp_session_create(std::map<std::string, std::string> slavemap);

	volatile unsigned int timeout_flag;//0: invalid; 1:actived
	volatile unsigned int rtp_can_send;

	/****slave-use****/
	volatile unsigned int stop_send_rtp_flag;
	volatile unsigned int channel_applied_flag;
	volatile unsigned int slave_busy_flag;
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
	void Wait_Record_Event();
	void Wait_TimePoll_Event();

	void Set_Playback_Event();
	void Reset_Playback_Event();
	void Wait_Playback_Event();

	void Set_RTPRecv_Event();
	void Reset_RTPRecv_Event();
	void Wait_RTPRecv_Event();

	static MyRepeater * pThis;   //静态对象指针

	volatile unsigned int Configurable_delay_time;
	/*
	设置线程退出标志
	*/
	void SetThreadExitFlag()   { m_PleaseStopRepeater = true; }
	
};


#endif





