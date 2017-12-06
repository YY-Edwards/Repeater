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
	pthread_mutex_init(&m_mapLocker, NULL);
	pthread_mutex_init(&m_sendLocker, NULL);
	pthread_mutex_init(&m_statusLocker, NULL);
	pthread_mutex_init(&m_onDataLocker, NULL);
	pthread_mutex_init(&lastRecvAliveTimeLocker, NULL);

	sem_init(&sem, 0, 0);
	isGetStatus = false;

	memset(lastRecvAliveTime, 0, 64);
}

CSlave::~CSlave()
{

	pthread_mutex_destroy(&lastRecvAliveTimeLocker);
	pthread_mutex_destroy(&m_onDataLocker);
	pthread_mutex_destroy(&m_statusLocker);
	pthread_mutex_destroy(&m_sendLocker);
	pthread_mutex_destroy(&m_mapLocker);

	sem_destroy(&sem);
	//fprintf(stderr, "delete class CSlave\n");
	syslog(LOG_LOCAL7 | LOG_DEBUG, "delete class: CSlave\n");
}
bool CSlave::Connect(const char* masterIp, const char* slaveIp)
{
	isRecvStatus = true;
	this->masterIp = masterIp;
	this->slaveIp = slaveIp;
	//fprintf(stderr,"connect slave\n");
	syslog(LOG_LOCAL7 | LOG_DEBUG, "connect slave\n");
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
	pthread_mutex_lock(&m_statusLocker);
	isGetStatus = true;
	pthread_mutex_unlock(&m_statusLocker);
	sendBuf[0] = static_cast<char>(GetChannelStatusOpcode);
	sendBuf[3] = static_cast<char>(GETCHANNEL);
	Send2Master(sendBuf,LENGTH-2);

	sem_post(&sem);

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
	//fprintf(stderr,"createThread\n");
	pthread_t id ,aliveId,monitorId;
	int  ret,aliveRet, monitorRet;
	ret = pthread_create(&id, NULL, RecvThread, this);
	aliveRet = pthread_create(&aliveId, NULL, SendAliveThread, this);
	monitorRet = pthread_create(&monitorId, NULL, MonitorStatusThread, this);
	//CreateThread(NULL,0,RecvThread,this,THREAD_PRIORITY_NORMAL,NULL);
	//CreateThread(NULL, 0, SendAliveThread, this, THREAD_PRIORITY_NORMAL, NULL);
	if (ret != 0) {
		fprintf(stderr,"Create slave recv thread error!\n");
	}
	else
	{
		//fprintf(stderr,"Create slave recv thread sucess!\n");
	}
	if (aliveRet != 0) {
		fprintf(stderr,"Create slave alive thread error!\n");
	}
	else
	{
		//fprintf(stderr,"Create slave alive thread sucess!\n");
	}
	if (monitorRet != 0) {
		fprintf(stderr,"Create monitor thread error!\n");
	}
	else
	{
		//fprintf(stderr,"Create monitor thread sucess!\n");
	}
	pthread_detach(id);
	pthread_detach(aliveId);
	pthread_detach(monitorId);
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

	while (isRecvStatus)
	{
		socklen_t  len = sizeof(struct sockaddr_in);
		//int  len = sizeof(struct sockaddr_in);
		bzero(recvBuf, sizeof(recvBuf));
		int ret = recvfrom(sockfd, recvBuf, BUFLENGTH, 0, (struct sockaddr *)&rmtAddr, &len);
		time_t t = time(0);

		if (-1 != ret)
		{
			std::string  strIp = inet_ntoa(rmtAddr.sin_addr);
			int mapCount = 0;
			switch (static_cast<char>(recvBuf[0]))
			{
			case static_cast<char>(mapOpcode) :
				//fprintf(stderr,"recvmap\n");
				syslog(LOG_LOCAL7 | LOG_DEBUG | LOG_INFO, "recvmap\n");
				isRecvedmap = true;
				isSendAlive = true;   //开启心跳线程
				mapCount = recvBuf[1];
				Sendmap2Repeater(mapCount);
				break;
			case static_cast<char>(AliveOpcode):
				//fprintf(stderr,"recvAlive:%s\n", inet_ntoa(rmtAddr.sin_addr));
					//fprintf(stderr, "recvAlive:%s\n", inet_ntoa(rmtAddr.sin_addr));
					syslog(LOG_LOCAL7 | LOG_DEBUG | LOG_INFO, "recvAlive:%s\n", inet_ntoa(rmtAddr.sin_addr));
					localtime(&t);
					pthread_mutex_lock(&lastRecvAliveTimeLocker);
					strftime(lastRecvAliveTime, sizeof(lastRecvAliveTime), "%Y-%m-%d %H:%M:%S", localtime(&t));
					pthread_mutex_unlock(&lastRecvAliveTimeLocker);
				break;
			case static_cast<char>(SetChannelStatusOpcode):
				//fprintf(stderr,"recvSetChannelStatus\n");
				syslog(LOG_LOCAL7 | LOG_DEBUG | LOG_INFO, "recvSetChannelStatus\n");
				pthread_mutex_lock(&m_statusLocker);
				isGetStatus = false;
				pthread_mutex_unlock(&m_statusLocker);
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
}
void CSlave::SendAliveThreadFunc()
{
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
		
		sleep(20);/*20s更新一次心跳*/
	}
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
//			pthread_mutex_lock(&m_statusLocker);
//			if (isGetStatus == true)
//			{
//				if (myCallBackFunc != NULL)
//				{
//					ResponeData r = { slavemap, "", "", SETCHANNEL, STATUSFREE };
//					onData(myCallBackFunc, LOCALSETCHANNELSTATUS, r);
//				}
//			}
//			pthread_mutex_unlock(&m_statusLocker);
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
//		//	pthread_mutex_lock(&m_statusLocker);
//		//	if (isGetStatus == true)
//		//	{
//		//		if (myCallBackFunc != NULL)
//		//		{
//		//			ResponeData r = { slavemap, "", "", SETCHANNEL, STATUSFREE };
//		//			onData(myCallBackFunc, LOCALSETCHANNELSTATUS, r);
//		//		}
//		//	}
//		//	pthread_mutex_unlock(&m_statusLocker);
//		//}
//
//		//}
//		usleep(30000);
//	}
//}

void CSlave::MonitorStatusThreadFunc()
{
	
	while (isRecvStatus)
	{
		//fprintf(stderr,"sem_wait\n");
		sem_wait(&sem);
		//usleep(30000);//30ms
		usleep(400000);//400ms
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
}
int CSlave::Send2Master(char* pSendBuf ,int length)
{
	pthread_mutex_lock(&m_sendLocker);
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
	pthread_mutex_unlock(&m_sendLocker);
	return len;
	
}
int CSlave::Send2Slave(int length,std::string ip)
{
	pthread_mutex_lock(&m_sendLocker);
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
	pthread_mutex_unlock(&m_sendLocker);
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
	pthread_mutex_lock(&m_mapLocker);
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
		//fprintf(stderr,"slave :%s\n",strIp.c_str());
		syslog(LOG_LOCAL7 | LOG_DEBUG | LOG_INFO, "slave :%s\n", strIp.c_str());
	}
	pthread_mutex_unlock(&m_mapLocker);
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
	//fprintf(stderr,"sendRegister2Master\n");
	syslog(LOG_LOCAL7 | LOG_DEBUG | LOG_INFO, "sendRegister2Master\n");
}
void CSlave::SendAlive2Master()
{
	pthread_mutex_lock(&m_mapLocker);
	std::map<std::string, std::string>::iterator it;
	/*
		给master发送alive
	*/
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = static_cast<char>(AliveOpcode);
	stringSplit(masterIp);
	Send2Master(sendBuf,LENGTH);
	//fprintf(stderr,"sendAlive2Master\n");
	syslog(LOG_LOCAL7 | LOG_DEBUG | LOG_INFO, "sendAlive2Master\n");
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
			//fprintf(stderr,"sendAlive2:%s\n",(it->first).c_str());
			syslog(LOG_LOCAL7 | LOG_DEBUG | LOG_INFO, "sendAlive2:%s\n", (it->first).c_str());
		}
	}
	pthread_mutex_unlock(&m_mapLocker);
}
void CSlave::DisConnect()
{
	isRecvStatus = false;
	isSendAlive = false;
	isRecvedmap = true;
	if (socketOpen)
	{
		CloseSocket(sockfd);
	}
}
void  CSlave::onData(void(*func)(int, ResponeData), int command, ResponeData data)
{
	pthread_mutex_lock(&m_onDataLocker);
	try
	{
		func(command, data);
	}
	catch (double)
	{

	}
	pthread_mutex_unlock(&m_onDataLocker);
}