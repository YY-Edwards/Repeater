#include <stdio.h>
#include "MyRepeater.h"

MyRepeater *MyRepeater::pThis = NULL;

MyRepeater::MyRepeater()
:basedevice_ID(0)
, CD_Trigger_flag(0)
, Mulcast_Trigger_flag(0)
, channel_busy_flag(0)
, playback_start_flag(0)
, stop_send_rtp_flag(0)
, channel_applied_flag(0)
, audio_codec_err_counter(0)
, slave_busy_flag(0)
, timeout_flag(0)
, Configurable_delay_time(0)
, capture_buffer(NULL)
, playback_buffer(NULL)
, rtp_can_send(0)

{

	pThis = this;

	m_PleaseStopRepeater = false;

	my_baseinfo = NULL;
	my_gpio_app = NULL;
	my_alsa = NULL;
	my_recvrtp = NULL;
	my_speex = NULL;
	proto = NULL;

	CD_trigger_cond = NULL;
	mulcast_poll_cond = NULL;
	send_rtp_cond = NULL;
	playback_cond = NULL;

	flag_mutex = NULL;
	playback_start_flag_mutex = NULL;
	map_mutex = NULL;

	fdset = NULL;

	my_baseinfo = new Monitor_Interface;
	my_gpio_app = new GPIO_App;
	my_alsa = new AudioAlsa;
	my_recvrtp = new Myrtp;
	my_speex = new Myspeex;
	//my_rtp = new Myrtp;

	////init mutex
	//pthread_mutex_init(&CD_cond_mutex, NULL);
	//pthread_mutex_init(&poll_cond_mutex, NULL);
	//pthread_mutex_init(&flag_mutex, NULL);
	//pthread_mutex_init(&playback_start_flag_mutex, NULL);
	//pthread_mutex_init(&map_mutex, NULL);
	//pthread_mutex_init(&send_rtp_cond_mutex, NULL);
	////pthread_mutex_init(&playback_cond_mutex, NULL);
	//

	////init cond
	//pthread_cond_init(&CD_trigger_cond, NULL);
	//pthread_cond_init(&mulcast_poll_cond, NULL);
	//pthread_cond_init(&send_rtp_cond, NULL);
	//pthread_cond_init(&playback_cond, NULL);

	//pthread_attr_init(&attr1);
	//pthread_attr_init(&attr2);
	//pthread_attr_init(&attr3);
	//pthread_attr_init(&attr4);
	fprintf(stderr, "New: Repeater\n");
}

MyRepeater::~MyRepeater()
{
	
	/*pthread_cancel(id_CDpoll);
	pthread_cancel(id_rtppoll);
	pthread_cancel(id_rtpsend);
	pthread_cancel(id_encode);
	pthread_cancel(id_decode);
	pthread_cancel(id_record);
	pthread_cancel(id_playback);
	pthread_cancel(id_time);*/


	//fprintf(stderr, "pthread_cancel(id_CDpoll) \n");
	//sleep(10);
	//fprintf(stderr, "sleep 10s \n");

	//map<string, Myrtp*>::iterator it;
	//while (sessionmap.size() > 0)
	//{
	//	it = sessionmap.begin();
	//	if (it->second)
	//	{
	//		delete it->second;
	//		it->second = NULL;
	//	}
	//	sessionmap.erase(it);
	//}
	//std::map <std::string, std::string> ::iterator at;
	//while (base_masterMap.size() > 0)
	//{
	//	at = base_masterMap.begin();
	//	base_masterMap.erase(at);
	//}

	if (my_baseinfo != NULL)delete my_baseinfo;
	if (my_gpio_app != NULL)delete my_gpio_app;
	if (my_alsa != NULL)delete my_alsa;
	if (proto != NULL)delete proto;
	if (my_recvrtp != NULL)delete my_recvrtp;
	if (my_speex != NULL)delete my_speex;

	//pthread_mutex_destroy(&CD_cond_mutex);
	//pthread_mutex_destroy(&poll_cond_mutex);
	//pthread_mutex_destroy(&flag_mutex);
	//pthread_mutex_destroy(&playback_start_flag_mutex);
	//pthread_mutex_destroy(&map_mutex);
	//pthread_mutex_destroy(&send_rtp_cond_mutex);

	//pthread_cond_destroy(&CD_trigger_cond);
	//pthread_cond_destroy(&mulcast_poll_cond);
	//pthread_cond_destroy(&send_rtp_cond);
	//pthread_cond_destroy(&playback_cond);

	//free(playback_buffer);
	//playback_buffer = NULL;

	//free(capture_buffer);
	//capture_buffer = NULL;

	//free(fdset);
	//fdset = NULL;

	//pthread_attr_destroy(&attr1);
	//pthread_attr_destroy(&attr2);
	//pthread_attr_destroy(&attr3);
	//pthread_attr_destroy(&attr4);

	fprintf(stderr, "Destory: Repeater\n");

}

void MyRepeater::Stop()
{
	fprintf(stderr, "release Repeater resource\n");
	SetThreadExitFlag();
	proto->DisConnect();//断开通信

	//delete pthreads
	if (cd_poll_thread_p != NULL)
	{
		delete cd_poll_thread_p;
		cd_poll_thread_p = NULL;
	}
	if (record_thread_p != NULL)
	{
		Set_RecordAndTimePoll_Event();
		delete record_thread_p;
		record_thread_p = NULL;
	}
	if (timer_thread_p != NULL)
	{
		delete timer_thread_p;
		timer_thread_p = NULL;
	}


	if (playback_thread_p != NULL)
	{
		Set_Playback_Event();
		delete playback_thread_p;
		playback_thread_p = NULL;
	}
	if (rtp_poll_thread_p != NULL)
	{
		Set_RTPRecv_Event();
		delete rtp_poll_thread_p;
		rtp_poll_thread_p = NULL;
	}
	if (rtp_send_thread_p != NULL)
	{
		delete rtp_send_thread_p;
		rtp_send_thread_p = NULL;
	}
	
	if (encode_thread_p != NULL)
	{
		delete encode_thread_p;
		encode_thread_p = NULL;
	}
	if (decode_thread_p != NULL)
	{
		delete decode_thread_p;
		decode_thread_p = NULL;
	}

	//delete sem
	if (CD_trigger_cond != NULL)
	{
		delete CD_trigger_cond;
		CD_trigger_cond = NULL;
	}
	if (mulcast_poll_cond != NULL)
	{
		delete mulcast_poll_cond;
		mulcast_poll_cond = NULL;
	}
	if (send_rtp_cond != NULL)
	{
		delete send_rtp_cond;
		send_rtp_cond = NULL;
	}
	if (playback_cond != NULL)
	{
		delete playback_cond;
		playback_cond = NULL;
	}

	//delete mutex
	if (flag_mutex != NULL)
	{
		delete flag_mutex;
		flag_mutex = NULL;
	}
	if (playback_start_flag_mutex != NULL)
	{
		delete playback_start_flag_mutex;
		playback_start_flag_mutex = NULL;
	}
	if (map_mutex != NULL)
	{
		delete map_mutex;
		map_mutex = NULL;
	}

	//clear map
	map<string, Myrtp*>::iterator it;
	while (sessionmap.size() > 0)
	{
		it = sessionmap.begin();
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
		sessionmap.erase(it);
	}
	std::map <std::string, std::string> ::iterator at;
	while (base_masterMap.size() > 0)
	{
		at = base_masterMap.begin();
		base_masterMap.erase(at);
	}

	if (playback_buffer != NULL)
	{
		free(playback_buffer);
		playback_buffer = NULL;

	}

	if (capture_buffer != NULL)
	{
		free(capture_buffer);
		capture_buffer = NULL;
	}

	if (fdset != NULL)
	{
		free(fdset);
		fdset = NULL;
	}

}
void MyRepeater::Start()
{

	//uint8_t user_port;
	//std::string baseip_str;
	//std::string masterip_str;
	std::string mulcastip_str;

	uint32_t baseip;
	uint32_t mulcastip;

	//init cond 
	CD_trigger_cond = new MySynCond;
	mulcast_poll_cond = new MySynCond;
	send_rtp_cond = new MySynCond;
	playback_cond = new MySynCond;

	//init mutex
	flag_mutex = new Mutex("flag_m");
	playback_start_flag_mutex = new Mutex("play_m");
	map_mutex = new Mutex("map_m");


	char tmp[50];
	bzero(tmp, 50);

	start.tv_sec = 0;
	start.tv_usec = 0;
	end.tv_sec = 0;
	start.tv_usec = 0;

	//get basic config info
	mulcastip_str = "228.221.2.1";
	baseip_str = my_baseinfo->get_base_ip();
	masterip_str = my_baseinfo->get_master_ip();
	baseip = inet_addr(baseip_str.c_str());
	mulcastip = inet_addr(mulcastip_str.c_str());

	basedevice_ID = my_baseinfo->get_repeater_is_master();//get device-id

	Connect(basedevice_ID, baseip_str.c_str(), masterip_str.c_str());

	if (basedevice_ID){//master
		fprintf(stderr, "Repeater ID is : Master\r\n");
	}
	else{//slave
		fprintf(stderr, "Repeater ID is : Slave\r\n");
	}

	/*****************************config repeater hw info****************************************/

	config_hw(baseip, mulcastip);

	repeater_task_start();

	//if (pthread_join(id_playback, NULL) != 0){
	//	fprintf(stderr, "id_playback thread is no exit..\n");
	//	exit(1);
	//}
	//fprintf(stderr, "reboot the system\n");
	//sprintf(tmp, "reboot");//restart the device
	//system(tmp);

}

void MyRepeater::config_hw(uint32_t baseip, uint32_t mulcastip)
{
	int baseport = 8000;
	int mulcastport = 8000;//baseport == mulcastport
	std::string null_str = "";

	my_gpio_app->config_GPIO(0);//defalut
	my_recvrtp->init(8000, null_str, baseip_str);

	int fd = 0;
	int ret = 0;
	char databuf[4];
	bzero(databuf, 4);

	fd = open("/opt/delay_config", O_RDONLY);
	if (fd != -1){
		printf("open delay_config okay!\n");
		ret = lseek(fd, 0, SEEK_SET);
		if (ret <0)fprintf(stderr, "lseek delay_config err\n");
		ret = read(fd, databuf, 4);
		if (ret <0)fprintf(stderr, "read delay_config err\n");

		Configurable_delay_time = ((databuf[0] - 0x30) * 1000) + ((databuf[1] - 0x30) * 100) 
			+ ((databuf[2] - 0x30)* 10) + (databuf[3] - 0x30);

		if (Configurable_delay_time < 20)fprintf(stderr, "delay_time err: The lowest configuration unit is 20ms\n");
		printf("Set delay_time : %d ms\n", Configurable_delay_time);
	}
	else
		fprintf(stderr, "open delay_config err\n");

	close(fd);



	//config_rtp_params(my_rtp, mulcastip, mulcastport, baseip, baseport);


}

void MyRepeater::checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		//while (1);
		exit(-1);
	}
}


void MyRepeater::config_rtp_params(Myrtp *sess, uint32_t destip, uint16_t destport,
	uint32_t baseip, uint16_t baseport)
{
	int status = 0;
	uint32_t ssrc = 0;
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;


	destip = ntohl(destip);//将32位网络字节序转换成主机字节序  
	baseip = ntohl(baseip);

	ssrc = (baseip & 0x0000FFFF);//取低八位

	//fprintf(stderr,"baseport :%d\n", baseport);
	//fprintf(stderr,"destport :%d\n", destport);
	//fprintf(stderr,"baseip :0x%x\n", baseip);
	//fprintf(stderr,"ssrc :0x%x\n", ssrc);

	//localip.push_back(baseip);

	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be just use 8000 samples per second.
	sessparams.SetOwnTimestampUnit(1.0 / 8000.0);

	sessparams.SetAcceptOwnPackets(false);

	//transparams.SetBindIP(baseip);

	sessparams.SetUsePredefinedSSRC(true);//set SSRC for rtp-send	
	sessparams.SetPredefinedSSRC(ssrc);

	transparams.SetPortbase(baseport);
	//transparams.SetLocalIPList(localip);

	transparams.SetMulticastInterfaceIP(baseip);

	//设置多播组数据的TTL值，范围为0～255之间的任何值
	transparams.SetMulticastTTL(231);

	status = sess->Create(sessparams, &transparams);
	checkerror(status);

	//更改为单播，1对1模式
	//status = sess->SupportsMulticasting();
	//checkerror(status);

	//RTPIPv4Address addr(destip, destport);

	////if(define recviver)
	//status = sess->JoinMulticastGroup(addr);
	//checkerror(status);

	//status = sess->AddDestination(addr);
	//checkerror(status);

	fprintf(stderr, "RTP init successfully...\n");


}

void MyRepeater::setTimer(int seconds, int useconds)
{

	//int counter = 0;
	struct timeval temp;

	temp.tv_sec = seconds;

	temp.tv_usec = useconds;

	select(0, NULL, NULL, NULL, &temp);

	//counter++;

	//fprintf(stderr,"timer: %d\n", counter);
	return;




}

void MyRepeater::ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack)
{

	static int counter = 0;
	static int s_counter = 0;
	int current_ssrc = 0;
	int temp = 0;
	int length = 0;
	char buffer[320];
	bzero(buffer, 320);


	current_ssrc = srcdat.GetSSRC();
	length = rtppack.GetPayloadLength();

	if (length != 20)fprintf(stderr, "length: %d\n", length);
	memcpy(buffer, rtppack.GetPayloadData(), length);

	//m_PlayBackQueue.PushToQueue((char *)buffer, length);
	m_DecodeQueue.PushToQueue((char *)buffer, length);
	counter++;
	if (counter > 139){
		counter = 0;
		s_counter++;
		fprintf(stderr, " RTP-recv 139p from SSRC:0x%x, tip:%d\n", current_ssrc, s_counter);
	}

}

void*/* DWORD WINAPI*/ MyRepeater::DecodeThread(void*/*LPVOID*/ p)
{

	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->DecodeThreadFunc();
	}
	return (void*)0;

}
void MyRepeater::DecodeThreadFunc()
{

	int temp = 0;
	int waitTime = 0xFFFF;
	int size = 0;
	int nbyte = 0;
	struct  timeval dec_start;
	struct  timeval dec_end;
	static int rtp_recv_count = 0;
	short decode_buff[160];
	char encode_buff[100];
	//pthread_detach(pthread_self());
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	fprintf(stderr, "DecodeThread is running...\n");
	while (!m_PleaseStopRepeater)
	{
		//pthread_testcancel();
		//temp = m_DecodeQueue.TakeFromQueueForSpeex((char *)encode_buff, nbyte);
		//pthread_testcancel();
		temp = m_DecodeQueue.TakeFromQueue((char *)encode_buff, nbyte);
		if (temp == 0){

			if (Mulcast_Trigger_flag == 0){

				if (m_DecodeQueue.QueueIsEmpty() == false)m_DecodeQueue.ClearQueue();
				usleep(2000);//2ms
				continue;

			}
			//gettimeofday(&dec_start, NULL);
			my_speex->decode_audio((char *)encode_buff, (short *)decode_buff);//解压
			//gettimeofday(&dec_end, NULL);
			m_PlayBackQueue.PushToQueue((char *)decode_buff, 320);//推送到发送队列
			//fprintf(stderr, "Decode Time is :%ld s,%ld us\n", (dec_end.tv_sec - dec_start.tv_sec), (dec_end.tv_usec - dec_start.tv_usec));
			usleep(300);
			bzero(decode_buff, sizeof(decode_buff));
			bzero(encode_buff, sizeof(encode_buff));

		}
		else if (temp > 0){//Queue empty
			usleep(20000);//20ms
			//fprintf(stderr, "play Queue is empty\n");
			//my_alsa->send_buf_playback(playback_buffer);;
			//continue;
		}
		else{

			fprintf(stderr, "m_DecodeQueue.TakeFromQueue err : %d\n", temp);//nerver happened
			break;
		}
	}

	fprintf(stderr, "exit:Decode thread \n");

}



void MyRepeater::Connect(bool isMaster, const char *baseip, const char *masterip)
{
	proto = new CProtocol(isMaster);
	if (isMaster)
	{

		my_baseinfo->p_file->GetMap(&base_masterMap);//get map
		proto->SetMap(base_masterMap);//set map

		if (base_masterMap.size() > 0)rtp_session_create(base_masterMap);

		proto->SetCallBackFunc(MyRepeater::MasterOnData);
		bool result = proto->Connect(masterip, ""); //master
		
	}
	else
	{
		proto->SetCallBackFunc(MyRepeater::SlaveOnData);
		bool result = proto->Connect(masterip, baseip); //slave
	}
}


void MyRepeater::MasterOnData(int command, ResponeData data)
{
	if (pThis == NULL)exit(-1);
	pThis->MasterOnDataFunc(command, data);

}

void MyRepeater::MasterOnDataFunc(int command, ResponeData data)
{
	//static int slave_channel_applied_flag = 0;
	static master_state_t status = WAIT_SLAVE_GET_CHANNEL;

	switch (command)
	{
	case SLAVEMAP:
		fprintf(stderr, "RepeaterRecvmap\n");
		//fprintf(stderr, "write map into file...\n");

		my_baseinfo->p_file->SaveMap(data.slavemap);

		//动态创建rtp_session
		rtp_session_create(data.slavemap);

		//map_mutex->Lock();//lock-flag
		//devicemap = data.slavemap;
		//map_mutex->Unlock();//unlock-flag

		fprintf(stderr, "save map-file is finished\n");

		break;
	case GETCHANNELSTATUS:

		if (timeout_flag){
			status = WAIT_SLAVE_GET_CHANNEL;
			timeout_flag = 0;
		}

		switch (status)
		{
		case WAIT_SLAVE_GET_CHANNEL:

			fprintf(stderr, "RepeaterRecvGetChannelStatus\n");

			flag_mutex->Lock();//lock-flag

			proto->ReplyChannelStatus(channel_busy_flag, data.getChannelIp.c_str());//Tell slave current channel status
			if (channel_busy_flag){
				fprintf(stderr, "Slave:%s apply for channel\n", data.getChannelIp.c_str());
				fprintf(stderr, "Sorry, The current channel is busy...\n");
				status = WAIT_SLAVE_GET_CHANNEL;
			}
			else{

				//my_alsa->do_pause(RESUME);//RESUME Record and Playback
				usleep(5000);//5ms
				fprintf(stderr, "Slave:%s will send RTP data..\n", data.getChannelIp.c_str());
				proto->BeginRecordVoice();//Notify all machines to receive RTP data

				Set_RTPRecv_Event();

				Set_Playback_Event();

				my_gpio_app->ptt_onoff(ON);//enable ptt
				channel_busy_flag = 1;//set busy

				status = WAIT_SLAVE_RELEASE_CHANNEL;

			}
			flag_mutex->Unlock();//unlock-flag


			break;

		default:

			break;

		}
		break;

	case RELEASECHANNELSTATUS:

		switch (status)
		{
		case WAIT_SLAVE_GET_CHANNEL:
			break;
		case WAIT_SLAVE_RELEASE_CHANNEL:

			if (0 == channel_busy_flag){

				status = WAIT_SLAVE_GET_CHANNEL;
				break;
			}
			fprintf(stderr, "RepeaterRecvReleaseChannel\n");
			fprintf(stderr, "Slave:%s has end RTP data..\n", data.releaseChannelIp.c_str());

			flag_mutex->Lock();//lock-flag
			proto->EndRecorderVoice();//Notify all slave, slave voice data ends
			Reset_RTPRecv_Event();
			Reset_Playback_Event();
			my_gpio_app->ptt_onoff(OFF);//disable ptt
			channel_busy_flag = 0;//set free
			flag_mutex->Unlock();//unlock-flag

			audio_codec_err_counter = 0;//Return to normal

			status = WAIT_SLAVE_GET_CHANNEL;

			break;

		default:

			break;

		}
		break;

	default:

		fprintf(stderr, "!!!Should not happen cmd: %d\n", command);

		break;

	}

}


void  MyRepeater::SlaveOnData(int command, ResponeData data)
{

	if (pThis == NULL)exit(-1);
	pThis->SlaveOnDataFunc(command, data);

}
void MyRepeater::SlaveOnDataFunc(int command, ResponeData data)
{
	char tmp[50];
	bzero(tmp, 50);

	switch (command)
	{
	case SLAVEMAP:
		fprintf(stderr, "RepeaterRecvmap\n");
		//fprintf(stderr, "write map into file...\n");

		my_baseinfo->p_file->SaveMap(data.slavemap);

		//动态创建rtp_session
		rtp_session_create(data.slavemap);
		//map_mutex->Lock();//lock-flag
		//devicemap = data.slavemap;
		//map_mutex->Unlock();//unlock-flag

		fprintf(stderr, "save map-file is finished\n");
		break;
	case LOCALSETCHANNELSTATUS:
	case SETCHANNELSTATUS:

		//fprintf(stderr, "RepeaterRecvSetChannelStatus\n");
		//fprintf(stderr, "data.status ：%d\n", data.status);
		//fprintf(stderr, "command：%d\n", command);

		if (data.status){//busy
			fprintf(stderr, "channel is busy...\n");
			fprintf(stderr, "ignore local audio data...\n");

		}
		else{//free

			if (slave_busy_flag)break;
			if (command == LOCALSETCHANNELSTATUS){

				//if (CD_Trigger == 0)break;//cd has released
				if (my_gpio_app->get_cd_current_value() == HIGH_LEVEL)break;//cd has released
				if (Mulcast_Trigger_flag == 1){
					fprintf(stderr, "Slave RTP is Recv...\n");
					break;
				}//salve处于正在接收状态，CD触发，主机无响应，本地转发响应，则应该忽略
				fprintf(stderr, "Master is not work!!!...\n");
				stop_send_rtp_flag = 1;
				slave_busy_flag = 1;
			}

			flag_mutex->Lock();//lock-flag
			channel_applied_flag = 1;

			Set_RecordAndTimePoll_Event();

			Set_Playback_Event();

			my_gpio_app->ptt_onoff(ON);//enable ptt

			gettimeofday(&start, NULL);

			flag_mutex->Unlock();//unlock-flag

		}


		break;

	case BEGINRECORDERVOICE:

		if (channel_applied_flag)break;
		if (slave_busy_flag)break;

		fprintf(stderr, "Remote Voice begin\n");

		flag_mutex->Lock();//lock-flag

		Set_RTPRecv_Event();

		Set_Playback_Event();

		my_gpio_app->ptt_onoff(ON);//enable ptt

		flag_mutex->Unlock();//unlock-flag

		fprintf(stderr, "Some one will send RTP data..\n");


		break;

	case ENDRECORDERVOICE:

		if (channel_applied_flag)
		{
			channel_applied_flag = 0;
			break;
		}
		if (slave_busy_flag)break;
		if (!playback_start_flag)break;

		fprintf(stderr, "Remote Voice end\n");

		flag_mutex->Lock();//lock-flag

		Reset_RTPRecv_Event();

		Reset_Playback_Event();

		my_gpio_app->ptt_onoff(OFF);//disable ptt

		flag_mutex->Unlock();//unlock-flag

		audio_codec_err_counter = 0;//Return to normal

		break;

	default:

		fprintf(stderr, "!!!Should not happen cmd: %d\n", command);

		break;

	}

}

void MyRepeater::repeater_task_start()
{

	int err = 0;

	////pthread_attr_setschedpolicy(&attr1, SCHED_RR);
	////param.sched_priority = 15;
	////pthread_attr_setschedparam(&attr1, &param);
	////pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话

	////audio_record_pthread
	//err = pthread_create(&id_record, NULL, RecordThread, this);
	//if (err != 0){
	//	fprintf(stderr, "audio_record_pthread  create fail...\n");
	//}

	////param.sched_priority = 14;
	////pthread_attr_setschedpolicy(&attr2, SCHED_RR);
	////pthread_attr_setschedparam(&attr2, &param);
	////pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话
	//////audio_playback_pthread
	//err = pthread_create(&id_playback, NULL, PlaybackThread, this);
	//if (err != 0){
	//	fprintf(stderr, "audio_playback_pthread  create fail...\n");
	//}


	////mulcastport_poll_pthread
	//err = pthread_create(&id_rtppoll, NULL, MulcastPortPollThread, this);
	//if (err != 0){
	//	fprintf(stderr, "mulcastport_poll_pthread  create fail...\n");
	//}

	////rtp_send_pthread
	//err = pthread_create(&id_rtpsend, NULL, RTPsendThread, this);
	//if (err != 0){
	//	fprintf(stderr, "rtp_send_pthread  create fail...\n");
	//}

	//param.sched_priority = 16;
	//pthread_attr_setschedpolicy(&attr3, SCHED_RR);
	//pthread_attr_setschedparam(&attr3, &param);
	//pthread_attr_setinheritsched(&attr3, PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话
	////time_interrupt_thread
	//err = pthread_create(&id_time, &attr3, TimePollThread, this);
	//if (err != 0){
	//	fprintf(stderr, "TimePollThread  create fail...\n");
	//}

	////endoe_thread
	//err = pthread_create(&id_encode, NULL, EncodeThread, this);
	//if (err != 0){
	//	fprintf(stderr, "EncodeThread  create fail...\n");
	//}

	////decode_thread
	//err = pthread_create(&id_decode, NULL, DecodeThread, this);
	//if (err != 0){
	//	fprintf(stderr, "DecodeThread  create fail...\n");
	//}

	//param.sched_priority = 17;
	//pthread_attr_setschedpolicy(&attr4, SCHED_RR);
	//pthread_attr_setschedparam(&attr4, &param);
	//pthread_attr_setinheritsched(&attr4, PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话
	////CD_poll_thread
	//err = pthread_create(&id_CDpoll, &attr4, CDPollThread, this);
	//if (err != 0){
	//	fprintf(stderr, "CD_poll_thread  create fail...\n");
	//}

	cd_poll_thread_p = new MyCreateThread(CDPollThread, this);
	if (cd_poll_thread_p == NULL)
	{
		fprintf(stderr, "CD_poll_thread  create fail...\n");
	}

	record_thread_p = new MyCreateThread(RecordThread, this);
	if (record_thread_p == NULL)
	{
		fprintf(stderr, "record_thread_p  create fail...\n");
	}

	playback_thread_p = new MyCreateThread(PlaybackThread, this);
	if (playback_thread_p == NULL)
	{
		fprintf(stderr, "playback_thread_p  create fail...\n");
	}

	rtp_poll_thread_p = new MyCreateThread(MulcastPortPollThread, this);
	if (rtp_poll_thread_p == NULL)
	{
		fprintf(stderr, "rtp_poll_thread_p  create fail...\n");
	}

	rtp_send_thread_p = new MyCreateThread(RTPsendThread, this);
	if (rtp_send_thread_p == NULL)
	{
		fprintf(stderr, "rtp_send_thread_p  create fail...\n");
	}

	timer_thread_p = new MyCreateThread(TimePollThread, this);
	if (timer_thread_p == NULL)
	{
		fprintf(stderr, "timer_thread_p  create fail...\n");
	}

	encode_thread_p = new MyCreateThread(EncodeThread, this);
	if (encode_thread_p == NULL)
	{
		fprintf(stderr, "encode_thread_p  create fail...\n");
	}

	decode_thread_p = new MyCreateThread(DecodeThread, this);
	if (decode_thread_p == NULL)
	{
		fprintf(stderr, "decode_thread_p  create fail...\n");
	}



	fprintf(stderr, "start to pthread...\n");
	fprintf(stderr, "\r\n$/***********************************/&\r\n");
	//sleep(1);
	//led_thread
	//err = pthread_create(&id_led, NULL, LedIndicatorThread, this);
	//if (err != 0){
	//	fprintf(stderr, "LedIndicatorThread  create fail...\n");
	//}

}




//void* /* DWORD WINAPI*/ MyRepeater::LedIndicatorThread(void*/*LPVOID*/ p)
//{
//	MyRepeater *arg = (MyRepeater*)p;
//	if (arg != NULL)
//	{
//		arg->LedIndicatorThreadFunc();
//	}
//	return (void*)0;
//
//}


void MyRepeater::LedIndicatorThreadFunc()
{

	my_baseinfo->led_control();
	//char tmp[50];
	//static unsigned char flag;
	//static unsigned char i = 1;

	//bzero(tmp, 50);

	////while (1)
	//{
	//	flag = i&0x01;
	//	sprintf(tmp,"echo %d > /sys/class/leds/d19/brightness",flag);
	//	system(tmp);
	//	usleep(30000);
	//	sprintf(tmp, "echo 00 > /sys/class/leds/d19/brightness", flag);
	//	system(tmp);
	//	usleep(32000);
	//	sprintf(tmp, "echo %d > /sys/class/leds/d19/brightness", flag);
	//	system(tmp);
	//	usleep(27000);
	//	sprintf(tmp, "echo 00 > /sys/class/leds/d19/brightness", flag);
	//	system(tmp);

	//	//usleep(200000);
	//	i++;

	//	if (i == 100)i = 1;

	//}
	//


}


void*  /*DWORD WINAPI*/ MyRepeater::CDPollThread(void */*LPVOID*/ p)
{
	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->CDPollThreadFunc();
	}
	return (void*)0;
}
void MyRepeater::CDPollThreadFunc()
{
	int len = 0;
	int rc = 0;
	int CD_fd;
	int nfds = 1;
	char buf[10];
	volatile static unsigned int temp = 0;

	char tmp[50];
	bzero(tmp, 50);


	fdset = (struct pollfd*)malloc(sizeof(struct pollfd));
	memset((void*)fdset, 0, sizeof(fdset));

	//设置描述符的就绪事件
	CD_fd = my_gpio_app->get_cd_fd();//Get CD_port file descriptor
	fdset->fd = CD_fd;
	fdset->events = POLLPRI;

	bzero(buf, 10);
	//pthread_detach(pthread_self());
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	fprintf(stderr, "CD poll thread is running...\n");
	while (!m_PleaseStopRepeater)
	{

		//pthread_testcancel();
		rc = poll(fdset, nfds, POLL_TIMEOUT);
		//pthread_testcancel();
		if (rc < 0){
			fprintf(stderr, "poll() failed!\n");
			break;
		}
		if (rc == 0){

			LedIndicatorThreadFunc();

			//fprintf(stderr,".");
		}
		if (fdset->revents & POLLPRI){//The POLLPRI event happened.

			rc = lseek(CD_fd, 0, SEEK_SET);
			if (rc == -1)fprintf(stderr, "lseek err\n");

			len = read(CD_fd, buf, 10);
			if (len == -1){
				fprintf(stderr, "read failed\n");
				break;
			}
			buf[1] = '\0';
			//fprintf(stderr,"\nGPIO: CD interrupt occurred\n");
			//fprintf(stderr,"read len: %d, value :%s\n", len, buf);
			//rising or falling

			if ('1' == buf[0]){//rising trriger
				setTimer(0, 15000);//delay 15ms
				temp = my_gpio_app->get_cd_current_value();//debounce CD_pin
				if (temp == HIGH_LEVEL){
					if (0 == CD_Trigger_flag)continue;
					fprintf(stderr, "!!!CD_release...\n");

					//flag_mutex->Lock();//lock-flag
					flag_mutex->Lock();

					if (basedevice_ID == true){
						proto->EndRecorderVoice();//Notify all slave, host voice data ends
						channel_busy_flag = 0;//set channel free
					}
					else{
						slave_busy_flag = 0;//slave return to free
						channel_applied_flag = 0;
						proto->ReleaseChannelStatus(0);//Notify the host, the voice is over, the slave is ready to release the channel
					}

					Reset_RecordAndTimePoll_Event();

					Reset_Playback_Event();

					my_gpio_app->ptt_onoff(OFF);//disable ptt

					flag_mutex->Unlock();
					//flag_mutex->Unlock();//unlock-flag
					audio_codec_err_counter = 0;//Return to normal

				}

			}
			else if ('0' == buf[0]){//falling trriger
				setTimer(0, 15000);//delay:15ms
				temp = my_gpio_app->get_cd_current_value();//debounce CD_pin
				if (temp == LOW_LEVEL){
					if (1 == CD_Trigger_flag)continue;
					if (basedevice_ID == true){//master

						flag_mutex->Lock();//lock-flag
						//Check channel status
						if (channel_busy_flag == 0){
							fprintf(stderr, "!!!Master CD_Trigger...\n");

							gettimeofday(&start, NULL);

							proto->BeginRecordVoice();//Notify all machines to receive RTP data
							channel_busy_flag = 1;//set channel busy

							Set_RecordAndTimePoll_Event();

							Set_Playback_Event();

							my_gpio_app->ptt_onoff(ON);//enable ptt


						}
						else{// rtp is running 
							//Ignore interrupt
							fprintf(stderr, "channel is busy...\n");
							fprintf(stderr, "rtp-recv is running...\n");
						}
						flag_mutex->Unlock();//unlock-flag
					}
					else//slave
					{
						if (playback_start_flag)continue;
						fprintf(stderr, "!!!Slave CD_Trigger...\n");
						proto->GetStatus();
						usleep(10000);//sleep 10ms
					}

				}
			}
		}
		bzero(buf, 10);

	}

	my_gpio_app->close_cd_fd();
	free(fdset);
	fdset = NULL;
	fprintf(stderr, "exit:CD_poll_thread \n");

}

void*  /*DWORD WINAPI*/ MyRepeater::RecordThread(void */*LPVOID*/ p)
{
	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->RecordThreadFunc();
	}
	return (void*)0;
}
void MyRepeater::RecordThreadFunc()
{
	int err;
	static unsigned long record_count = 0;
	int delay_count = 0;
	bool temp = false;
	int size = 0;

	size = my_alsa->get_record_period_size();

	if (NULL == (capture_buffer = (char *)malloc(size))){
		fprintf(stderr, "capture_buffer malloc error..\n");
		exit(1);
	}
	bzero(capture_buffer, size);

	//pthread_detach(pthread_self());
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	//pthread_cleanup_push(pthread_mutex_unlock, (void*)&CD_cond_mutex);
	fprintf(stderr, "record pthread is running...\n");
	while (1){

		//fprintf(stderr,"record go\n");
		//wait for CD_trigger_cond to wakeup the function:record audio
		Wait_Record_Event();
		if (m_PleaseStopRepeater)break;

		my_alsa->get_record_buf(capture_buffer);

		//pthread_testcancel();
		temp = m_PlayBackQueue.PushToQueue((char *)capture_buffer, size);
		if (temp == false)
		{
			fprintf(stderr, "m_PlayBackQueue  PushToQueue full...\n");
		}
		//pthread_testcancel();

		if (stop_send_rtp_flag == 0){

			//temp = m_RtpSendQueue.PushToQueue((char *)capture_buffer, size);
			temp = m_EncodeQueue.PushToQueue((char *)capture_buffer, size);
			if (temp == false)
			{
				fprintf(stderr, "m_EncodeQueue PushToQueue full...\n");
			}
		}
		setTimer(0, 15000);//20ms
		//usleep(20000);//20ms
		//fprintf(stderr, "record_run\n");

	}

	//pthread_cleanup_pop(0);
	/*  close PCM handle */
	free(capture_buffer);
	capture_buffer = NULL;

	fprintf(stderr, "exit:record_pthread \n");

}

void*/* DWORD WINAPI*/ MyRepeater::EncodeThread(void*/*LPVOID*/ p)
{

	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->EncodeThreadFunc();
	}
	return (void*)0;

}
void MyRepeater::EncodeThreadFunc()
{

	int temp = 0;
	int waitTime = 0xFFFF;
	struct  timeval enc_start;
	struct  timeval enc_end;
	int size = 0;
	int nbyte = 0;
	short buff[160];
	char encode_buff[100];
	//pthread_detach(pthread_self());
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	fprintf(stderr, "EncodeThread is running...\n");
	while (!m_PleaseStopRepeater)
	{
		/*pthread_testcancel();
		temp = m_EncodeQueue.TakeFromQueue(buff, size);
		pthread_testcancel();*/
		temp = m_EncodeQueue.TakeFromQueue(buff, size);
		if (temp == 0){

			//gettimeofday(&enc_start, NULL);
			nbyte = my_speex->encode_audio((short *)buff, (char *)encode_buff);//压缩
			//gettimeofday(&enc_end, NULL);
			m_RtpSendQueue.PushToQueue(encode_buff, nbyte);//推送到发送队列
			bzero(buff, sizeof(buff));
			bzero(encode_buff, sizeof(encode_buff));
			usleep(300);//1ms
			//fprintf(stderr, "Encode Time is :%ld s,%ld us\n", (enc_end.tv_sec - enc_start.tv_sec), (enc_end.tv_usec - enc_start.tv_usec));
		}
		else if (temp > 0){//Queue empty
			usleep(15000);//15ms
			//continue;
		}
		else{

			fprintf(stderr, "m_EncodeQueue.TakeFromQueue err : %d\n", temp);//nerver happened
			break;
		}
	}
	fprintf(stderr, "exit:EncodeThread\n");

}




void*  /*DWORD WINAPI*/ MyRepeater::PlaybackThread(void */*LPVOID*/ p)
{
	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->PlaybackThreadFunc();
	}
	return (void*)0;
}
void MyRepeater::PlaybackThreadFunc()
{

	int play_counter = 0;
	static int err = 0;
	int temp = 0;
	int length = 0;
	int size = 0;
	int s_counter = 0;
	char speex_buf[20];

	size = my_alsa->get_playback_period_size();
	if (NULL == (playback_buffer = (char *)malloc(size))){
		fprintf(stderr, "capture_buffer malloc error..\n");
		exit(1);
	}
	bzero(playback_buffer, size);

	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	//pthread_detach(pthread_self());
	fprintf(stderr, "playback pthead is running...\n");

	for (;;){

			Wait_Playback_Event();
			if (m_PleaseStopRepeater)break;
			/*pthread_testcancel();
			temp = m_PlayBackQueue.TakeFromQueue((char *)playback_buffer, length);
			pthread_testcancel();*/
	
			temp = m_PlayBackQueue.TakeFromQueue((char *)playback_buffer, length);
		
			if (temp == 0){
				err = my_alsa->send_buf_playback(playback_buffer);
				if (err == -EPIPE){
					audio_codec_err_counter++;
				}
				
				bzero(playback_buffer, length);
				play_counter++;
				if (play_counter > 139){
					play_counter = 0;
					s_counter++;
					fprintf(stderr, " local-playback 139p, tip:%d\n", s_counter);
				}
				//usleep(20000);//20ms
			}
			else if (temp > 0){//Queue empty
				//setTimer(0, 20000);//20ms
				//usleep(20000);//20ms
				//fprintf(stderr, "play Queue is empty\n");
				//my_alsa->send_buf_playback(playback_buffer);;
				//continue;		
			}
			else{

				fprintf(stderr, "m_PlayBackQueue.TakeFromQueue err : %d\n", temp);//nerver happened
				break;
			}
			setTimer(0, 1000);//1ms

	}

	free(playback_buffer);
	playback_buffer = NULL;
	fprintf(stderr, "exit:playback_thread \n");
	//pthread_exit(NULL);

}

void*  /*DWORD WINAPI*/ MyRepeater::RTPsendThread(void */*LPVOID*/ p)
{
	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->RTPsendThreadFunc();
	}
	return (void*)0;
}

void MyRepeater::RTPsendThreadFunc()
{

	int status = 0;
	int length = 0;
	int temp = 0;
	int send_counter = 0;
	int trans_counter = 0;
	int s_counter = 0;
	int err = 0;
	int waitTime = 0xFFFF;

	int size = 0;
	std::map<std::string, Myrtp *>::iterator it;
	uint32_t destip;

	size = my_alsa->get_playback_period_size();

	Myrtp *pSess = new Myrtp();

	char package_send_buffer[20];
	bzero(package_send_buffer, 20);

	//pthread_detach(pthread_self());
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	fprintf(stderr, "Rtp send pthread is running...\n");

	while (!m_PleaseStopRepeater){

	/*	pthread_mutex_lock(&send_rtp_cond_mutex);
		while (rtp_can_send == 0){
			fprintf(stderr, "rtp send pthread is ready...\n");
			pthread_cond_wait(&send_rtp_cond, &send_rtp_cond_mutex);
			fprintf(stderr, "rtp send pthread is running...\n");
		}
		pthread_mutex_unlock(&send_rtp_cond_mutex);
		if ((my_gpio_app->get_cd_current_value() == 1) || (CD_Trigger ==0)){
			rtp_can_send = 0;
			continue;
		}*/
		//fprintf(stderr, "rtp-send_run\n");
		//temp = m_RtpSendQueue.TakeFromQueue((char *)package_send_buffer, length);// 非阻塞模式
		//pthread_testcancel();
		//temp = m_RtpSendQueue.TakeFromQueueForSpeex((char *)package_send_buffer, length);// 非阻塞模式
		//pthread_testcancel();

		temp = m_RtpSendQueue.TakeFromQueue((char *)package_send_buffer, length);// 非阻塞模式
		if (temp == 0){

			if ((my_gpio_app->get_cd_current_value() == 1) || (CD_Trigger_flag == 0)){

				if (m_RtpSendQueue.QueueIsEmpty() == false)m_RtpSendQueue.ClearQueue();
				usleep(200000);//200ms
				continue;

			}

			map_mutex->Lock();//lock-flag
			for (it = sessionmap.begin(); it != sessionmap.end(); it++)
			{
				string key = it->first;

				pSess = it->second;

				//status = pSess->SendPacket(package_send_buffer, 320, 0, false, 160);
				status = pSess->SendPacket(package_send_buffer, 20, 0, false, 160);
				checkerror(status);
				send_counter++;
				if (send_counter > 139){
					send_counter = 0;
					s_counter++;
					fprintf(stderr, " send-RTP 139p to %s, tip:%d\n", key.c_str() , s_counter);
				}
			}
			map_mutex->Unlock();//unlock-flag
			usleep(20);//1ms

		}
		else{//Queue empty

			usleep(20000);//20ms
			//fprintf(stderr, "rtp-send_run\n");
			//fprintf(stderr, "RTP Send is empty\n");
			//continue;
		}
	}


	for (it = sessionmap.begin(); it != sessionmap.end(); it++)
	{
		pSess = it->second;

		pSess->BYEDestroy(RTPTime(1, 0), 0, 0);

	}
	if (pSess != NULL)
	{
		delete pSess;
		pSess = NULL;
	}
		

	fprintf(stderr, "exit:rtp send pthread\n");


}

void*  /*DWORD WINAPI*/ MyRepeater::MulcastPortPollThread(void */*LPVOID*/ p)
{
	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->MulcastPortPollThreadFunc();
	}
	return (void*)0;
}

void MyRepeater::MulcastPortPollThreadFunc()
{
	int status = 0;
	//int current_ssrc = 0;
	/*int p_recv = 0;
	int recv_counter = 0;*/
	RTPTime delay(0.001);
	//pthread_detach(pthread_self());
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	//pthread_cleanup_push(pthread_mutex_unlock, (void *)&mulcast_poll_cond);
	fprintf(stderr, "mulcastport poll pthread is running...\n");

	while (1){

			Wait_RTPRecv_Event();
			if (m_PleaseStopRepeater)break;

			gettimeofday(&end, NULL);
			if (audio_codec_err_counter > 30){
				//pthread_cancel(id_playback);
				fprintf(stderr, "Sound card failure, ready to restart the gateway\n");
			}
			if ((end.tv_sec - start.tv_sec) > 60){

				fprintf(stderr, "remote rtp timeout!!!\r\n");
				timeout_flag = 1;
				flag_mutex->Lock();//lock-flag

					Reset_RTPRecv_Event();
					Reset_Playback_Event();
					my_gpio_app->ptt_onoff(OFF);//disable ptt
					if (basedevice_ID == true)channel_busy_flag = 0;//set free

				flag_mutex->Unlock();//unlock-flag

				audio_codec_err_counter = 0;//Return to normal

			}

			//if (EXIT_flag)break;
			my_recvrtp->BeginDataAccess();

			// check incoming packets  
			if (my_recvrtp->GotoFirstSourceWithData())
			{
				do
				{
					RTPPacket *pack;
					RTPSourceData *srcdat;

					srcdat = my_recvrtp->GetCurrentSourceInfo();
					//current_ssrc = srcdat->GetSSRC();
					//fprintf(stderr,"recv-okay3.1..\n");
					while ((pack = my_recvrtp->GetNextPacket()) != NULL)
					{
						// You can examine the data here
						ProcessRTPPacket(*srcdat, *pack);
						my_recvrtp->DeletePacket(pack);

					}

				} while (my_recvrtp->GotoNextSourceWithData());
			}
			my_recvrtp->EndDataAccess();
			status = my_recvrtp->Poll();
			checkerror(status);
			//pthread_testcancel();
			//usleep(1000);//1ms
			RTPTime::Wait(delay);
			//pthread_testcancel();

	}

	//pthread_cleanup_pop(0);
	my_recvrtp->BYEDestroy(RTPTime(1, 0), 0, 0);
	fprintf(stderr, "exit: mulcastport poll pthread\n");

}

void*  /*DWORD WINAPI*/ MyRepeater::TimePollThread(void */*LPVOID*/ p)
{
	MyRepeater *arg = (MyRepeater*)p;
	if (arg != NULL)
	{
		arg->TimePollThreadFunc();
	}
	return (void*)0;
}

void MyRepeater::TimePollThreadFunc()
{
	/*pthread_detach(pthread_self());*/
	char tmp[50];
	bzero(tmp, 50);

	int cd_level = HIGH_LEVEL;//default HIGH
	/*pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);*/
	fprintf(stderr, "timer poll pthread is running...\n");
	while (1)
	{
		Wait_TimePoll_Event();
		if (m_PleaseStopRepeater)break;

		gettimeofday(&end, NULL);
		cd_level = my_gpio_app->get_cd_current_value();//get CD_pin

		setTimer(0, 13000);
		//pthread_testcancel();
		//usleep(20000);//20ms
		//pthread_testcancel();
		
		if (audio_codec_err_counter > 30){

			//pthread_cancel(id_playback);
			fprintf(stderr, "Sound card failure, ready to restart the gateway\n");

		}

		if (((end.tv_sec - start.tv_sec) > 55) || (cd_level == HIGH_LEVEL)){

			if (cd_level == LOW_LEVEL){
				fprintf(stderr, "end_time:%ld , %ld ", end.tv_sec, end.tv_usec);
				timeout_flag = 1;
			}
			else
			{	
				if (CD_Trigger_flag == 0)continue;
				fprintf(stderr, "!!!timer-NOTE:user has released the CD\n ");

			}

			flag_mutex->Lock();//lock-flag
			if (basedevice_ID == true){
				proto->EndRecorderVoice();//Notify all slave, host voice data ends
				channel_busy_flag = 0;//set channel free
			}
			else{
				proto->ReleaseChannelStatus(0);//Notify the host, the voice is over, the slave is ready to release the channel
				slave_busy_flag = 0;//slave return to free
				channel_applied_flag = 0;
			}

			Reset_RecordAndTimePoll_Event();

			Reset_Playback_Event();

			my_gpio_app->ptt_onoff(OFF);//disable ptt
			flag_mutex->Unlock();//unlock-flag

			audio_codec_err_counter = 0;//Return to normal

		}

	}
	fprintf(stderr, "exit:timer poll pthread\n");

}


void MyRepeater::rtp_session_create(std::map<std::string, std::string> slavemap)
{
	int size = 0;
	int err = 0;
	std::map<std::string, std::string>::iterator it;
	map <std::string, Myrtp *>::iterator itSession;
	static uint16_t base_port = 10000;

	size = slavemap.size();
	fprintf(stderr,"slavemap.size() :%d\n", size);

	map_mutex->Lock();//lock-flag

	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{

		string key = it->first;
		//sessionmap.insert(map <std::string, Myrtp *>::value_type(it->first, pRtp));
		itSession = sessionmap.find(key);
		if (itSession != sessionmap.end())
		{
			/*find success*/
			fprintf(stderr,"IPaddress is have\n");
			continue;
		}
		else
		{
			/*find fail*/
			err = strcmp(baseip_str.c_str(), key.c_str());
			if (err != 0){//不发给自己

				//fprintf(stderr,"base_port: %d\n", base_port);
				Myrtp *pRtp = new Myrtp();
				//fprintf(stderr,"key:%s\n", key.c_str());
				pRtp->init(base_port, key, baseip_str);

				sessionmap[key] = pRtp;
				fprintf(stderr,"new session create okay!!!\n");
				base_port = base_port + 20;
			}
			
		}
		

	}
	if (basedevice_ID == false)//slave
	{
		itSession = sessionmap.find(masterip_str);
		if (itSession != sessionmap.end())
		{
			/*find success*/
			fprintf(stderr,"Master IPaddress is have\n");
		
		}
		else
		{
			Myrtp *pRtp = new Myrtp();
			pRtp->init(4000, masterip_str, baseip_str);
			sessionmap[masterip_str] = pRtp;
			fprintf(stderr,"Slave connect master session create okay!!!\n");
		}
	
	}


	if (slavemap.size() < sessionmap.size()){//Someone dropped

		map <std::string, std::string>::iterator temp;
		for (itSession = sessionmap.begin(); itSession != sessionmap.end(); itSession++)
		{
			string session_key = itSession->first;

			err = strcmp(masterip_str.c_str(), session_key.c_str());
			if (err == 0)continue;

			temp = slavemap.find(session_key);
			if (temp == slavemap.end())
			{
				itSession->second->BYEDestroy(RTPTime(1, 20), 0, 0);//1s,20ms
				delete itSession->second;
				itSession->second = NULL;
				sessionmap.erase(session_key);
				fprintf(stderr, "%s has dropped...\r\n", session_key.c_str());

			}


		}
	}


	map_mutex->Unlock();//unlock-flag


}


void MyRepeater::Set_RecordAndTimePoll_Event()
{

	//pthread_mutex_lock(&CD_cond_mutex);
	//CD_Trigger = 1;
	//pthread_cond_broadcast(&CD_trigger_cond);//active record-pthread and TimePoll
	//pthread_mutex_unlock(&CD_cond_mutex);
	CD_Trigger_flag = 1;
	CD_trigger_cond->CondTrigger(true);
	my_alsa->record_start();

}
void MyRepeater::Reset_RecordAndTimePoll_Event()
{

	//CD_Trigger = 0;//record_pthread and timepoll_phtread enter waitting for the CD_trigger_cond
	CD_Trigger_flag = 0;
	stop_send_rtp_flag = 0;
	CD_trigger_cond->Clear_Trigger_Flag();
	my_alsa->record_prepare();
}
void MyRepeater::Wait_Record_Event()
{
	CD_trigger_cond->CondWait(0);

	//pthread_mutex_lock(&CD_cond_mutex);
	//while (CD_Trigger == 0){
	//	fprintf(stderr, "record  is ready\n");
	//	stop_send_rtp_flag = 0;
	//	my_alsa->record_prepare();
	//	pthread_cond_wait(&CD_trigger_cond, &CD_cond_mutex);
	//	fprintf(stderr, "record  is running\n");
	//	my_alsa->record_start();
	//}
	//pthread_mutex_unlock(&CD_cond_mutex);


}

void MyRepeater::Wait_TimePoll_Event()
{
	CD_trigger_cond->CondWait(0);
	//pthread_mutex_lock(&CD_cond_mutex);
	//while (CD_Trigger == 0){
	//	fprintf(stderr, "Timekeeper   is ready\n");
	//	pthread_cond_wait(&CD_trigger_cond, &CD_cond_mutex);
	//	fprintf(stderr, "Timekeeper   is running\n");
	//}
	//pthread_mutex_unlock(&CD_cond_mutex);

}



void MyRepeater::Set_Playback_Event()
{
	playback_start_flag = 1;
	playback_cond->CondTrigger(false);
	my_alsa->play_start();
	//pthread_mutex_lock(&playback_start_flag_mutex);
	//playback_start_flag = 1;
	//pthread_cond_signal(&playback_cond);//active playback-pthread			
	//pthread_mutex_unlock(&playback_start_flag_mutex);


}
void MyRepeater::Reset_Playback_Event()
{
	playback_start_flag = 0;
	playback_cond->Clear_Trigger_Flag();
	my_alsa->play_prepare();
}
void MyRepeater::Wait_Playback_Event()
{

	playback_cond->CondWait(0);
/*
	pthread_mutex_lock(&playback_start_flag_mutex);
	while (playback_start_flag == 0){
		fprintf(stderr, "playback  is ready\n");
		my_alsa->play_prepare();
		pthread_cond_wait(&playback_cond, &playback_start_flag_mutex);
		fprintf(stderr, "playback  is running\n");
		my_alsa->play_start();
	}
	pthread_mutex_unlock(&playback_start_flag_mutex);*/

}



void MyRepeater::Set_RTPRecv_Event()
{
	//pthread_mutex_lock(&poll_cond_mutex);//lock-flag
	//Mulcast_Trigger_flag = 1;
	//pthread_cond_signal(&mulcast_poll_cond);//active mulcast_poll_pthread
	//pthread_mutex_unlock(&poll_cond_mutex);//lock-flag

	Mulcast_Trigger_flag = 1;
	mulcast_poll_cond->CondTrigger(false);
	gettimeofday(&start, NULL);//开启计时

}
void MyRepeater::Reset_RTPRecv_Event()
{
	//Mulcast_Trigger_flag = 0;
	Mulcast_Trigger_flag = 0;
	mulcast_poll_cond->Clear_Trigger_Flag();

}

void MyRepeater::Wait_RTPRecv_Event()
{

	mulcast_poll_cond->CondWait(0);
	//pthread_mutex_lock(&poll_cond_mutex);
	//while (Mulcast_Trigger_flag == 0){
	//	fprintf(stderr, "mulcastport poll  is ready\n");
	//	pthread_cond_wait(&mulcast_poll_cond, &poll_cond_mutex);
	//	fprintf(stderr, "mulcastport poll  is running\n");
	//	gettimeofday(&start, NULL);
	//}
	//pthread_mutex_unlock(&poll_cond_mutex);

}





