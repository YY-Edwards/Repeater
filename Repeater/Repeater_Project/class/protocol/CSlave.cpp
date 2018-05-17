//#include "stdafx.h"
#include "CSlave.h"
CSlave::CSlave()
{
	isRecvStatus = true;
	isSendAlive = false;
	isRecvedmap = false;
	memset(recvBuf, 0, BUFLENGTH);
	//m_mapLocker = PTHREAD_MUTEX_INITIALIZER;
	//m_sendLocker = PTHREAD_MUTEX_INITIALIZER;

	m_mapLocker = new Mutex("m_mapLocker");
	m_onDataLocker = new Mutex("m_onDataLocker");
	m_sendLocker = new Mutex("m_sendLocker");
	m_statusLocker = new Mutex("m_statusLocker");
	lastRecvAliveTimeLocker = new Mutex("lastRecvAliveTimeLocker");

	/*pthread_mutex_init(&m_mapLocker, NULL);
	pthread_mutex_init(&m_sendLocker, NULL);
	pthread_mutex_init(&m_statusLocker, NULL);
	pthread_mutex_init(&m_onDataLocker, NULL);
	pthread_mutex_init(&lastRecvAliveTimeLocker, NULL);*/

	alive_pthread_p = NULL;
	udp_recv_pthread_p = NULL;
	monitor_pthread_p = NULL;

	//sem_init(&sem, 0, 0);
	sem = new MySynSem();
	isGetStatus = false;

	memset(lastRecvAliveTime, 0, 64);
}

CSlave::~CSlave()
{

	if (sem != NULL)
	{
		sem->SemPost();
		delete sem;
		sem = NULL;
	}


	if (m_mapLocker != NULL)
	{
		delete m_mapLocker;
		m_mapLocker = NULL;
	}

	if (m_onDataLocker != NULL)
	{
		delete m_onDataLocker;
		m_onDataLocker = NULL;
	}

	if (m_sendLocker != NULL)
	{
		delete m_sendLocker;
		m_sendLocker = NULL;
	}

	if (m_statusLocker != NULL)
	{
		delete m_statusLocker;
		m_statusLocker = NULL;
	}

	if (lastRecvAliveTimeLocker != NULL)
	{
		delete lastRecvAliveTimeLocker;
		lastRecvAliveTimeLocker = NULL;
	}



	/*pthread_mutex_destroy(&lastRecvAliveTimeLocker);
	pthread_mutex_destroy(&m_onDataLocker);
	pthread_mutex_destroy(&m_statusLocker);
	pthread_mutex_destroy(&m_sendLocker);
	pthread_mutex_destroy(&m_mapLocker);*/

	//sem_destroy(&sem);
	//pthread_cancel(id);
	//pthread_cancel(aliveId);
	//pthread_cancel(monitorId);

	fprintf(stderr, "delete class CSlave\n");
}
bool CSlave::Connect(const char* masterIp, const char* slaveIp)
{
	isRecvStatus = true;
	this->masterIp = masterIp;
	this->slaveIp = slaveIp;
	fprintf(stderr,"connect slave\n");
	return InitSocket();
}
void CSlave::SetCallBackFunc(void(*callBackFunc)(int, ResponeData))
{
	myCallBackFunc = callBackFunc;
}
void CSlave::GetStatus()
{
	
	char sendBuf[BUFLENGTH];
	memset(sendBuf,0,BUFLENGTH);
	//fprintf(stderr,"sem_post \n");
	m_statusLocker->Lock();
	isGetStatus = true;
	m_statusLocker->Unlock();
	sendBuf[0] = static_cast<char>(GetChannelStatusOpcode);
	sendBuf[3] = static_cast<char>(GETCHANNEL);
	Send2Master(sendBuf,LENGTH-2);

	sem->SemPost():
	//sem_post(&sem);

}
void CSlave::ReleaseChannelStatus()
{
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = static_cast<char>(ReleaseChannelOpcode);
	stringSplit(slaveIp);
	sendBuf[5] = static_cast<char>(RELEASECHANNNEL);
	Send2Master(sendBuf,LENGTH+1);
}
bool CSlave::InitSocket()
{
	if (socketOpen)
	{
		CloseSocket(sockfd);
	}
	//WSADATA wsda;
	//int ret1 = WSAStartup(MAKEWORD(1, 1), &wsda);
	//bool bReuseraddr = false;
	//setsockopt(sockfd, SOL_SOCKET, SO_DONTLINGER, (const char*)&bReuseraddr, sizeof(bool));/*windows*/
	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		CloseSocket(sockfd);
		return false;
	}
	bzero(&myAddr, sizeof(myAddr));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(UDPPORT);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret;
	 ret = bind(sockfd, (struct sockaddr *)&myAddr, sizeof(myAddr));
	if (ret < 0)
	{
		CloseSocket(sockfd);
		return false;
	}
	socketOpen = true;
	//fprintf(stderr,"InitSocketSucess\n");
	CreateRecvThread();


	return true;
}
bool CSlave::CloseSocket(int sockfd)
{
	if (socketOpen)
	{
		//close(sockfd);
	}
	return true;
}
void CSlave::CreateRecvThread()
{

	udp_recv_pthread_p = new MyCreateThread(RecvThread, this);
	alive_pthread_p = new MyCreateThread(SendAliveThread, this);
	monitor_pthread_p = new MyCreateThread(MonitorStatusThread, this);
	if (udp_recv_pthread_p == NULL || alive_pthread_p == NULL || monitor_pthread_p == NULL)
	{
		fprintf(stderr, "Create slave main thread error!\n");
	}
	//fprintf(stderr,"createThread\n");
	/*pthread_t id ,aliveId,monitorId;*/
	//int  ret,aliveRet, monitorRet;
	//ret = pthread_create(&id, NULL, RecvThread, this);
	//aliveRet = pthread_create(&aliveId, NULL, SendAliveThread, this);
	//monitorRet = pthread_create(&monitorId, NULL, MonitorStatusThread, this);
	////CreateThread(NULL,0,RecvThread,this,THREAD_PRIORITY_NORMAL,NULL);
	////CreateThread(NULL, 0, SendAliveThread, this, THREAD_PRIORITY_NORMAL, NULL);
	//if (ret != 0) {
	//	fprintf(stderr,"Create slave recv thread error!\n");
	//}
	//else
	//{
	//	//fprintf(stderr,"Create slave recv thread sucess!\n");
	//}
	//if (aliveRet != 0) {
	//	fprintf(stderr,"Create slave alive thread error!\n");
	//}
	//else
	//{
	//	//fprintf(stderr,"Create slave alive thread sucess!\n");
	//}
	//if (monitorRet != 0) {
	//	fprintf(stderr,"Create monitor thread error!\n");
	//}
	//else
	//{
	//	//fprintf(stderr,"Create monitor thread sucess!\n");
	//}
	//pthread_detach(id);
	//pthread_detach(aliveId);
	//pthread_detach(monitorId);
}
void* /*DWORD WINAPI */CSlave::RecvThread(void*/*LPVOID*/ p)
{
	CSlave *pSlave = (CSlave*)p;
	if (pSlave != NULL)
	{
		pSlave->RecvThreadFunc();
	}
	return (void*)0;
}
void*/*DWORD WINAPI */CSlave::SendAliveThread(void*/*LPVOID*/ p)
{
	CSlave *pSlave = (CSlave*)p;
	if (pSlave != NULL)
	{
		pSlave->SendAliveThreadFunc();
	}
	return (void*)0;
}
void*/*DWORD WINAPI */CSlave::MonitorStatusThread(void*/*LPVOID*/ p)
{
	CSlave *pSlave = (CSlave*)p;
	if (pSlave != NULL)
	{
		pSlave->MonitorStatusThreadFunc();
	}
	return (void*)0;
}
void CSlave::RecvThreadFunc()
{
	int temp = 0; 

	/*pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);*/
	fprintf(stderr, "slave RecvThreadFunc is running\n");

	while (isRecvStatus)
	{
		socklen_t  len = sizeof(struct sockaddr_in);
		//int  len = sizeof(struct sockaddr_in);
		bzero(recvBuf, sizeof(recvBuf));
		//pthread_testcancel();
		int ret = recvfrom(sockfd, recvBuf, BUFLENGTH, 0, (struct sockaddr *)&rmtAddr, &len);
		//pthread_testcancel();
		time_t t = time(0);

		if (-1 != ret)
		{
			std::string  strIp = inet_ntoa(rmtAddr.sin_addr);
			int mapCount = 0;
			switch (static_cast<char>(recvBuf[0]))
			{
			case static_cast<char>(mapOpcode) :
				fprintf(stderr,"recvmap\n");
				isRecvedmap = true;
				isSendAlive = true;   //开启心跳线程
				mapCount = recvBuf[1];
				Sendmap2Repeater(mapCount);
				break;
			case static_cast<char>(AliveOpcode):
				//fprintf(stderr,"recvAlive:%s\n", inet_ntoa(rmtAddr.sin_addr));
					fprintf(stderr, "recvAlive:%s\n", inet_ntoa(rmtAddr.sin_addr));
					localtime(&t);
					//lastRecvAliveTimeLocker->Lock();
					strftime(lastRecvAliveTime, sizeof(lastRecvAliveTime), "%Y-%m-%d %H:%M:%S", localtime(&t));
					//pthread_mutex_unlock(&lastRecvAliveTimeLocker);
				break;
			case static_cast<char>(SetChannelStatusOpcode):
				fprintf(stderr,"recvSetChannelStatus\n");
				m_statusLocker->Lock();
				isGetStatus = false;
				m_statusLocker->Unlock();
				if (myCallBackFunc != NULL)
				{
					temp = (int)recvBuf[5];

					//fprintf(stderr,"recvBuf[5] is : %d\n", recvBuf[5]);
					//fprintf(stderr,"temp : %d\n", temp);

					ResponeData r = { slavemap, "", "", SETCHANNEL, temp};
					onData(myCallBackFunc,	SETCHANNELSTATUS, r);
				}
				break;
			case static_cast<char>(ReleaseChannelOpcode):
				/*fprintf(stderr,"recvReleaseChannel\n");
				if (myCallBackFunc != NULL)
				{
					ResponeData r = { slavemap,"",strIp,RELEASECHANNNEL };
					onData(myCallBackFunc, RELEASECHANNELSTATUS, r);
				}*/
				break;
			case static_cast<char>(BeginRecorderVoiceOpcode) :
				if (myCallBackFunc != NULL)
				{
					ResponeData r = { slavemap,"","",BEGINVOICE ,1 };
					onData(myCallBackFunc, BEGINRECORDERVOICE, r);
				}
				break;
			case static_cast<char>(EndRecorderVoiceOpcode) :
				if (myCallBackFunc != NULL)
				{
					ResponeData r = { slavemap,"","",ENDVOICE ,0 };
					onData(myCallBackFunc, ENDRECORDERVOICE, r);
				}
				break;
			default:
				break;
			}
		}
		usleep(5000);
	}
	fprintf(stderr, "exit:slave RecvThreadFunc\n");
}
void CSlave::SendAliveThreadFunc()
{
	/*pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);*/
	fprintf(stderr, "slave SendAliveThreadFunc is running\n");
	while (isRecvStatus)
	{
		if (!isRecvedmap)
		{
			SendRegister2Master();
		}
		if (isSendAlive)
		{
			SendAlive2Master();
		}
		//pthread_testcancel();
		sleep(20);/*20s更新一次心跳*/
		//pthread_testcancel();
	}

	fprintf(stderr, "exit:slave SendAliveThreadFunc\n");
}

//void CSlave::MonitorStatusThreadFunc()
//{
//
//	while (isRecvStatus)
//	{
//		//fprintf(stderr,"sem_wait\n");
//
//		sem_wait(&sem);
//
//		//usleep(60000);//60ms
//		////sleep(1);
//		//fprintf(stderr,"sleep \n");
//		////delay(250);   //延时1s
//
//		/*if (isGetStatus == true)
//		{*/
//		time_t t = time(0);
//		localtime(&t);
//		char tmp[64];
//		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
//		time_t tm_time;
//		tm tm;
//		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
//		strptime(lastRecvAliveTime, "%Y-%m-%d %H:%M:%S", &tm);
//		tm_time = mktime(&tm);
//
//		if (t - tm_time > 20)   // getChannel的时间和recvAlive的时间差大于20s ，说明master掉线，语音直接本地转发
//		{
//			m_statusLocker->Lock();
//			if (isGetStatus == true)
//			{
//				if (myCallBackFunc != NULL)
//				{
//					ResponeData r = { slavemap, "", "", SETCHANNEL, STATUSFREE };
//					onData(myCallBackFunc, LOCALSETCHANNELSTATUS, r);
//				}
//			}
//			m_statusLocker->Unlock();
//
//		}
//		else
//		{
//			fprintf(stderr, "t - tm_time <= 20\r\n");
//		
//		}
//		//else
//		//{
//		//	usleep(50000);//50ms
//		//	m_statusLocker->Lock();
//		//	if (isGetStatus == true)
//		//	{
//		//		if (myCallBackFunc != NULL)
//		//		{
//		//			ResponeData r = { slavemap, "", "", SETCHANNEL, STATUSFREE };
//		//			onData(myCallBackFunc, LOCALSETCHANNELSTATUS, r);
//		//		}
//		//	}
//		//	m_statusLocker->Unlock();
//		//}
//
//		//}
//		usleep(30000);
//	}
//}

void CSlave::MonitorStatusThreadFunc()
{
	
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	fprintf(stderr, "slave MonitorStatusThreadFunc is running\n");
	while (isRecvStatus)
	{
		//fprintf(stderr,"sem_wait\n");
		sem->SemWait(0);
		//sem_wait(&sem);
		//usleep(30000);//30ms
		//pthread_testcancel();
		usleep(400000);//400ms
		//pthread_testcancel();
		//sleep(1);
		//fprintf(stderr,"sleep \n");
		//delay(250);   //延时1s
		if (isGetStatus == true)
		{
			if (myCallBackFunc != NULL)
			{
				ResponeData r = { slavemap,"","",SETCHANNEL ,STATUSFREE };
				onData(myCallBackFunc, LOCALSETCHANNELSTATUS, r);
			}
		}

	}

	fprintf(stderr, "exit:slave MonitorStatusThreadFunc\n");

}
int CSlave::Send2Master(char* pSendBuf ,int length)
{
	m_sendLocker->Lock();
	char sendBuf[BUFLENGTH];
	memset(sendBuf,0,BUFLENGTH);
	strcpy(sendBuf,pSendBuf);
	bzero(&rmtAddr, sizeof(rmtAddr));
	rmtAddr.sin_family = AF_INET;
	rmtAddr.sin_addr.s_addr = inet_addr(masterIp);
	rmtAddr.sin_port = htons(UDPPORT);
	int len = sendto(sockfd, sendBuf, length, 0, (struct sockaddr *)&rmtAddr, sizeof(rmtAddr));
	if (len <= 0)
	{
		//fprintf(stderr,"sendFailure\n");
	}
	else
	{
		//fprintf(stderr,"sendSucess\n");
	}
	m_sendLocker->Unlock();
	return len;
	
}
int CSlave::Send2Slave(int length,std::string ip)
{
	m_sendLocker->Lock();
	char sendBuf[BUFLENGTH];
	//bzero(&rmtAddr, sizeof(rmtAddr));
	rmtAddr.sin_family = AF_INET;
	rmtAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	rmtAddr.sin_port = htons(UDPPORT);
	int len = sendto(sockfd, sendBuf, length, 0, (struct sockaddr *)&rmtAddr, sizeof(rmtAddr));
	if (len <= 0)
	{
		//fprintf(stderr,"sendFailure\n");
	}
	else
	{
		//fprintf(stderr,"sendSucess\n");
	}
	m_sendLocker->Unlock();
	return len;
	
}
void CSlave::stringSplit(std::string strTemp)
{
	//sscanf(strTemp.c_str(), "%c.%c.%c.%c", &sendBuf[1], &sendBuf[2], &sendBuf[3], &sendBuf[4]);
}
void CSlave::Sendmap2Repeater(int mapCount)
{
	/*当收到map时，先清空以前的map表，再重新将数据写入map*/
	std::string strIp;
	m_mapLocker->Lock();
	slavemap.clear();
	for (int i = 0; i < mapCount; i++)
	{
		
		char  temp[20];
		sprintf(temp,  "%d.%d.%d.%d", recvBuf[i*4 + 2] & 0xff000000 >> 24, recvBuf[i * 4 + 3]&0x00ff0000 >> 16, recvBuf[i * 4 + 4]&0x0000ff00 >> 8, recvBuf[i * 4 + 5] & 0x000000ff);
		strIp = temp;
		time_t t = time(0);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
		slavemap[strIp] = tmp;
		fprintf(stderr,"slave :%s\n",strIp.c_str());
	}
	m_mapLocker->Unlock();
	if (myCallBackFunc != NULL)
	{
		ResponeData r = { slavemap,"","",INVALIDCHANNEL };
		onData(myCallBackFunc, SLAVEMAP, r);
	}
}

time_t CSlave::convert_string_to_time_t(const std::string & time_string)
{

	struct tm tm1;
	time_t time1;
	sscanf(time_string.c_str(), "%d/%d/%d %d:%d:%d",
		&(tm1.tm_year),
		&(tm1.tm_mon),
		&(tm1.tm_mday),
		&(tm1.tm_hour),
		&(tm1.tm_min),
		&(tm1.tm_sec));
	tm1.tm_year -= 1900;
	tm1.tm_mon--;
	tm1.tm_isdst = -1;
	time1 = mktime(&tm1);
	return time1;
}


void CSlave::SendRegister2Master()
{
	char sendBuf[BUFLENGTH];
	memset(sendBuf,0,BUFLENGTH);
	isRecvedmap = false;
	sendBuf[0] = static_cast<char>(RegisterOpcode);
	int ip[4] = { 0 };
	sscanf(slaveIp, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	sendBuf[1] = ip[0];
	sendBuf[2] = ip[1];
	sendBuf[3] = ip[2];
	sendBuf[4] = ip[3];
	//sscanf(slaveIp, "%c.%c.%c.%c", &sendBuf[1], &sendBuf[2], &sendBuf[3], &sendBuf[4]);
	Send2Master(sendBuf,LENGTH);
	fprintf(stderr,"sendRegister2Master\n");
}
void CSlave::SendAlive2Master()
{
	m_mapLocker->Lock();
	std::map<std::string, std::string>::iterator it;
	/*
		给master发送alive
	*/
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = static_cast<char>(AliveOpcode);
	stringSplit(masterIp);
	Send2Master(sendBuf,LENGTH);
	fprintf(stderr,"sendAlive2Master\n");
	/*
		给每个slave 发送alive，自己除外 
	*/
	for (it = slavemap.begin(); it != slavemap.end(); it++)   
	{
		if (slaveIp != it->first)
		{
			sendBuf[0] = static_cast<char>(ALIVE);
			stringSplit(it->first);
			Send2Slave(LENGTH, it->first);
			fprintf(stderr,"sendAlive2:%s\n",(it->first).c_str());
		}
	}
	m_mapLocker->Unlock();
}
void CSlave::DisConnect()
{
	isRecvStatus = false;
	isSendAlive = false;
	isRecvedmap = true;

	if (udp_recv_pthread_p != NULL)
	{
		delete udp_recv_pthread_p;
		udp_recv_pthread_p = NULL;

	}
	if (alive_pthread_p != NULL)
	{
		delete alive_pthread_p;
		alive_pthread_p = NULL;
	}

	if (sem != NULL)
	{
		sem->SemPost();
	}

	if (monitor_pthread_p != NULL)
	{
		delete monitor_pthread_p;
		monitor_pthread_p = NULL;

	}
	delete sem;
	sem = NULL;

	if (socketOpen)
	{
		CloseSocket(sockfd);
	}
}
void  CSlave::onData(void(*func)(int, ResponeData), int command, ResponeData data)
{
	m_onDataLocker->Lock();
	try
	{
		func(command, data);
	}
	catch (double)
	{

	}
	m_onDataLocker->Unlock();
}