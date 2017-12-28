
//#include "stdafx.h"
#include<stdio.h>
#include"CMaster.h"
//#include <pthread.h>



CMaster::CMaster()
{
	isRecvStatus = true;
	socketOpen = false;
	isAlive = false;
	memset(recvBuf,0,BUFLENGTH);
	getStatusIp = "";
	//memset(sendBuf, 0, BUFLENGTH);
	//m_mapLocker = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&m_mapLocker, NULL);
	pthread_mutex_init(&m_onDataLocker, NULL);
	pthread_mutex_init(&m_sendLocker, NULL);
	pthread_mutex_init(&m_aliveLocker, NULL);
}

CMaster::~CMaster()
{
	pthread_mutex_destroy(&m_mapLocker);
	pthread_mutex_destroy(&m_onDataLocker);
	pthread_mutex_destroy(&m_sendLocker);
	pthread_mutex_destroy(&m_aliveLocker);
	syslog(LOG_LOCAL7 | LOG_DEBUG, "delete class: CMaster\n");// fprintf(stderr, "delete class CMaster\n");

}
bool CMaster::Connect(const char* masterIp)
{
	this->masterIp = masterIp;
	syslog(LOG_LOCAL7 | LOG_DEBUG, "connect master\n");//fprintf(stderr,"connect master\n");
	return InitSocket();
}
bool CMaster::InitSocket()
{

	if (socketOpen)
	{
		CloseSocket(sockfd);
	}
	//WSADATA wsda;
	//int ret1 = WSAStartup(MAKEWORD(1,1),&wsda);
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
	int ret = bind(sockfd, (struct sockaddr *)&myAddr, sizeof(myAddr));
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
bool CMaster::CloseSocket(int sockfd)
{
	if (socketOpen)
	{
		close(sockfd);
	}
	return true;
}
void CMaster::CreateRecvThread()
{
	pthread_t id, aliveId;
	int  ret = pthread_create(&id, NULL, RecvThread, this);
	int   aliveRet = pthread_create(&aliveId, NULL, MonitorAliveThread, this);
	//CreateThread(NULL, 0, RecvThread, this, THREAD_PRIORITY_NORMAL, NULL);
	
	if (ret != 0) {
		fprintf(stderr,"Create master main thread error!\n");
	}
	else
	{
		//fprintf(stderr,"Create master main thread sucess!\n");
	}
	if (aliveRet != 0) {
		fprintf(stderr,"Create monitor alive thread error!\n");
	}
	else
	{
		//fprintf(stderr,"Create monitor alive thread sucess!\n");
	}
	pthread_detach(id);
	pthread_detach(aliveId);
}
void*  /*DWORD WINAPI*/ CMaster::RecvThread(void */*LPVOID*/ p)
{
	CMaster *pMaster = (CMaster*)p;
	if (pMaster != NULL)
	{
		pMaster->RecvThreadFunc();
	}
	return (void*)0;
}
void*  /*DWORD WINAPI*/ CMaster::MonitorAliveThread(void */*LPVOID*/ p)
{
	CMaster *pMaster = (CMaster*)p;
	if (pMaster != NULL)
	{
		pMaster->MonitorAliveThreadFunc();
	}
	return (void*)0;
}
void CMaster::MonitorAliveThreadFunc()
{
	while (isRecvStatus)
	{
		pthread_mutex_lock(&m_aliveLocker);
		isAlive = false;
		pthread_mutex_unlock(&m_aliveLocker);
		sleep(60);

		pthread_mutex_lock(&m_aliveLocker);
		if (!isAlive)
		{
			pthread_mutex_lock(&m_mapLocker);
			slavemap.clear();
			if (myCallBackFunc != NULL)
			{
				ResponeData r = { slavemap, "", "", INVALIDCHANNEL };
				onData(myCallBackFunc, SLAVEMAP, r);
			}
			pthread_mutex_unlock(&m_mapLocker);
		}
		pthread_mutex_unlock(&m_aliveLocker);
	}
}
void CMaster::RecvThreadFunc()
{
	while (isRecvStatus)
	{
		//int  len = sizeof(struct sockaddr_in);
		socklen_t  len = sizeof(struct sockaddr_in);
		memset(recvBuf,0,BUFLENGTH);
		int ret = recvfrom(sockfd, recvBuf, BUFLENGTH, 0, (struct sockaddr *)&rmtAddr, &len);
		if (-1 != ret)
		{
		
			std::string rmtIp = inet_ntoa(rmtAddr.sin_addr);
			switch (static_cast<char>(recvBuf[0]))
			{
			case static_cast<char>(RegisterOpcode) :
				//fprintf(stderr,"recvRegister \n");
				syslog(LOG_LOCAL7 | LOG_NOTICE, "recvRegister \n");
				Sendmap2Repeater(rmtIp);
				Sendmap2Slave();
				break;
			case static_cast<char>(AliveOpcode):
				//fprintf(stderr,"recvAlive:%s\n", rmtIp.c_str());
				syslog(LOG_LOCAL7 | LOG_NOTICE, "recvAlive:%s\n", rmtIp.c_str());
				pthread_mutex_lock(&m_aliveLocker);
				isAlive = true;
				pthread_mutex_unlock(&m_aliveLocker);
				SendAlive2Slave(rmtIp);
				Updatemap(rmtIp);
				break;
			case static_cast<char>(GetChannelStatusOpcode):
				//fprintf(stderr,"recvGetChannelStatus\n");
				syslog(LOG_LOCAL7 | LOG_NOTICE, "recvGetChannelStatus\n");
				if (myCallBackFunc != NULL)
				{
					//fprintf(stderr,"myCallBackFunc\n");
					if ("" == getStatusIp)
					{
						getStatusIp = rmtIp;
					}
					ResponeData r = {slavemap, rmtIp,"",GETCHANNEL,-1 };
					onData(myCallBackFunc, GETCHANNELSTATUS, r);
				}
				break;
			case static_cast<char>(ReleaseChannelOpcode):
				//fprintf(stderr,"recvReleaseChannel\n");
				syslog(LOG_LOCAL7 | LOG_NOTICE, "recvReleaseChannel\n");
				if (myCallBackFunc != NULL)
				{
					ResponeData r = {slavemap,"",rmtIp,RELEASECHANNNEL,-1 };
					onData(myCallBackFunc, RELEASECHANNELSTATUS, r);
					//SlaveEndRecorderVoice(rmtIp);
				}
				break;
			default:
				break;
			}
		}
		usleep(5000);
	}
}

void CMaster::SendAlive2Slave(std::string slaveIp)
{
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = static_cast<char>(AliveOpcode);
	int ip[4] = { 0 };
	sscanf(slaveIp.c_str(), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	sendBuf[1] = ip[0];
	sendBuf[2] = ip[1];
	sendBuf[3] = ip[2];
	sendBuf[4] = ip[3];
	//fprintf(stderr, "sendAlive2Slave:%s\n", slaveIp.c_str());
	syslog(LOG_LOCAL7 | LOG_NOTICE, "sendAlive2Slave:%s\n", slaveIp.c_str());
	Send2Slave(sendBuf, LENGTH, slaveIp);
}

int CMaster::Send2Slave(char *pSendBuf,int length,std::string ip)
{
	pthread_mutex_lock(&m_sendLocker);
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	strcpy(sendBuf,pSendBuf);
	//bzero(&rmtAddr, sizeof(rmtAddr));
	rmtAddr.sin_family = AF_INET;
	rmtAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	rmtAddr.sin_port = htons(UDPPORT);
	int len = sendto(sockfd, sendBuf, length, 0, (struct sockaddr *)&rmtAddr, sizeof(rmtAddr));
	//fprintf(stderr,"sendToLength:%d\n",len);
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
void  CMaster::onData(void(*func)(int, ResponeData), int command, ResponeData data)
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
void CMaster::SetCallBackFunc(void(*callBackFunc)(int, ResponeData))
{
	myCallBackFunc = callBackFunc;
}
void CMaster::SetStatus(int flag,std::string ip) /*flag: 0x00 信道空闲   0x01 信道忙碌*/
{
	
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = static_cast<char>(SetChannelStatusOpcode);
	//sscanf(ip.c_str(), "%c.%c.%c.%c", &sendBuf[1], &sendBuf[2], &sendBuf[3], &sendBuf[4]);
	int temp[4] = { 0 };
	sscanf(ip.c_str(), "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
	sendBuf[1] = temp[0];
	sendBuf[2] = temp[1];
	sendBuf[3] = temp[2];
	sendBuf[4] = temp[3];
	sendBuf[5] = static_cast<char>(flag);
	Send2Slave(sendBuf,LENGTH+1,ip);
	//Send2Slave(sendBuf, LENGTH, getStatusIp);
	getStatusIp = "";
	/*if (STATUSFREE == flag)
	{
		SlaveBeginRecordVoice(ip);
	}*/
	
}
char* CMaster::stringSplit()
{
	//char sendBuf[BUFLENGTH];
	//sscanf(strIp.c_str(),"%c.%c.%c.%c",&sendBuf[1],&sendBuf[2],&sendBuf[3],&sendBuf[4]);
	return 0;
}
void CMaster::Sendmap2Repeater(std::string rmtIp)
{
	bool isHave = false;
	std::map<std::string, std::string>::iterator it;
	pthread_mutex_lock(&m_mapLocker);
	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{
		if (rmtIp == it->first)
		{
			isHave = true;
			time_t t = time(0);
			//tm timeinfo;
			char tmp[64];
			//localtime_s(&timeinfo, &t);
			strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
			slavemap[rmtIp] = tmp;
			break;
		}
		else
		{
			isHave = false;
		}
	}
	if (false == isHave)
	{	time_t t = time(0);
		//tm timeinfo;
		char tmp[64];
		//localtime_s(&timeinfo,&t);
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
	
		slavemap[rmtIp] = tmp;
	}
	if (myCallBackFunc != NULL)
	{
		//fprintf(stderr,"sendMap2Repeater\n");
		syslog(LOG_LOCAL7 | LOG_NOTICE, "sendMap2Repeater\n");
		ResponeData r = { slavemap,"","",INVALIDCHANNEL };
		onData(myCallBackFunc, SLAVEMAP, r);
		
	}
	pthread_mutex_unlock(&m_mapLocker);
}
void CMaster::Updatemap(std::string rmtIp)
{
	pthread_mutex_lock(&m_mapLocker);
	std::map<std::string, std::string>::iterator it;
	bool isHave = false;
	time_t t = time(0);
	localtime(&t);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
	
	for (it = slavemap.begin(); it != slavemap.end();)
	{
		if (rmtIp == it->first)
		{
			isHave = true;
		}
		time_t tm_time;
		tm tm;
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
		strptime((it->second).c_str(), "%Y-%m-%d %H:%M:%S", &tm); 
		//fprintf(stderr,"%s,%s\n", (it->first).c_str(), (it->second).c_str());
		syslog(LOG_LOCAL7 | LOG_NOTICE, "%s,%s\n", (it->first).c_str(), (it->second).c_str());
		tm_time = mktime(&tm);
		if ((t - tm_time>60) || (t - tm_time < 0))            //超时60s则发送map,时间bug.
		{
			
			 //fprintf(stderr,"updateMap2:%s\n", (it->first).c_str());
			syslog(LOG_LOCAL7 | LOG_NOTICE, "updateMap2:%s\n", (it->first).c_str());
			 slavemap.erase(it++);
			 pthread_mutex_unlock(&m_mapLocker);
			 Sendmap2Slave();
			 pthread_mutex_lock(&m_mapLocker);
			 if (myCallBackFunc != NULL)
			 {
				 ResponeData r = { slavemap,"","",INVALIDCHANNEL };
				 onData(myCallBackFunc, SLAVEMAP, r);
			 }
			
		}
		else if(rmtIp == it->first)
		{
			strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
			slavemap[it->first] = tmp;
			it++;

			/*if (myCallBackFunc != NULL)
			{
				ResponeData r = { slavemap, "", "", INVALIDCHANNEL };
				onData(myCallBackFunc, SLAVEMAP, r);
			}*/

		}
		else
		{
			it++;
		}
		
	}
	if (!isHave || 0 == slavemap.size())
	{
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
		slavemap[rmtIp] = tmp;
		if (myCallBackFunc != NULL)
		{
			ResponeData r = { slavemap,"","",INVALIDCHANNEL };
			onData(myCallBackFunc, SLAVEMAP, r);
		}
		pthread_mutex_unlock(&m_mapLocker);
		Sendmap2Slave();
		pthread_mutex_lock(&m_mapLocker);



	}
	pthread_mutex_unlock(&m_mapLocker);
	
}
time_t CMaster::convert_string_to_time_t(const std::string & time_string)
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
void CMaster::ReleaseChannelStatus(std::string ip)
{
	char sendBuf[BUFLENGTH];
	memset(sendBuf,0,BUFLENGTH);
	sendBuf[0] = static_cast<char>(ReleaseChannelOpcode);
	//sscanf(ip.c_str(), "%c.%c.%c.%c", &sendBuf[1], &sendBuf[2], &sendBuf[3], &sendBuf[4]);
	int temp[4] = { 0 };
	sscanf(ip.c_str(), "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
	sendBuf[1] = temp[0];
	sendBuf[2] = temp[1];
	sendBuf[3] = temp[2];
	sendBuf[4] = temp[3];
	memcpy(&sendBuf[1],temp,4);
	sendBuf[5] = static_cast<char>(RELEASECHANNNEL);
	Send2Slave(sendBuf,6,ip);
}
void CMaster::Sendmap2Slave()
{
	char sendBuf[BUFLENGTH];
	memset(sendBuf,0,BUFLENGTH);
	int i = 2;
	sendBuf[0] = static_cast<char>(mapOpcode);
	sendBuf[1] = 0;
	std::map<std::string, std::string>::iterator it;
	pthread_mutex_lock(&m_mapLocker);
	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{
		sendBuf[1]++;
		int ip[4] = { 0 };
		sscanf((it->first).c_str(), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
		sendBuf[i] = ip[0];
		sendBuf[i + 1] = ip[1];
		sendBuf[i + 2] = ip[2];
		sendBuf[i + 3] = ip[3];
		i += 4;
	}
	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{
		Send2Slave(sendBuf,i, (it->first).c_str());
		//fprintf(stderr,"sendMap2:%s\n",(it->first).c_str());
		syslog(LOG_LOCAL7 | LOG_NOTICE, "sendMap2:%s\n", (it->first).c_str());
	}
	
	pthread_mutex_unlock(&m_mapLocker);
	
}
void CMaster::DisConnect()
{
	isRecvStatus = false;
	if (socketOpen)
	{
		CloseSocket(sockfd);
	}

}
void CMaster::MasterBeginRecordVoice()
{
	pthread_mutex_lock(&m_mapLocker);
	char sendBuf[BUFLENGTH];
	memset(sendBuf,0,BUFLENGTH);
	sendBuf[0] = BeginRecorderVoiceOpcode;
	std::map<std::string, std::string>::iterator it;
	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{
		//sscanf((it->first).c_str(), "%x.%x.%x.%x", &sendBuf[2], &sendBuf[3], &sendBuf[4], &sendBuf[5]);
		int ip[4] = { 0 };
		sscanf((it->first).c_str(), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
		sendBuf[2] = ip[0];
		sendBuf[3] = ip[1];
		sendBuf[4] = ip[2];
		sendBuf[5] = ip[3];
		Send2Slave(sendBuf,6,it->first);
	}
	pthread_mutex_unlock(&m_mapLocker);
}
void CMaster::MasterEndRecorderVoice()
{
	pthread_mutex_lock(&m_mapLocker);
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = EndRecorderVoiceOpcode;
	std::map<std::string, std::string>::iterator it;
	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{
		//sscanf((it->first).c_str(), "%x.%x.%x.%x", &sendBuf[2], &sendBuf[3], &sendBuf[4], &sendBuf[5]);
		int ip[4] = { 0 };
		sscanf((it->first).c_str(), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
		sendBuf[2] = ip[0];
		sendBuf[3] = ip[1];
		sendBuf[4] = ip[2];
		sendBuf[5] = ip[3];
		Send2Slave(sendBuf, 6, it->first);
	}
	pthread_mutex_unlock(&m_mapLocker);
}
void CMaster::SlaveBeginRecordVoice(std::string ip)
{
	pthread_mutex_lock(&m_mapLocker);
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = BeginRecorderVoiceOpcode;
	std::map<std::string, std::string>::iterator it;
	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{
		if (it->first != ip)
		{
			//sscanf((it->first).c_str(), "%x.%x.%x.%x", &sendBuf[2], &sendBuf[3], &sendBuf[4], &sendBuf[5]);
			int temp[4] = { 0 };
			sscanf((it->first).c_str(), "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
			sendBuf[2] = temp[0];
			sendBuf[3] = temp[1];
			sendBuf[4] = temp[2];
			sendBuf[5] = temp[3];

			Send2Slave(sendBuf, 6, it->first);
		}	
	}
	pthread_mutex_unlock(&m_mapLocker);
}
void CMaster::SlaveEndRecorderVoice(std::string ip)
{
	pthread_mutex_lock(&m_mapLocker);
	char sendBuf[BUFLENGTH];
	memset(sendBuf, 0, BUFLENGTH);
	sendBuf[0] = EndRecorderVoiceOpcode;
	std::map<std::string, std::string>::iterator it;
	for (it = slavemap.begin(); it != slavemap.end(); it++)
	{
		if (it->first != ip)
		{
			//sscanf((it->first).c_str(), "%x.%x.%x.%x", &sendBuf[2], &sendBuf[3], &sendBuf[4], &sendBuf[5]);
			int temp[4] = { 0 };
			sscanf((it->first).c_str(), "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
			sendBuf[2] = temp[0];
			sendBuf[3] = temp[1];
			sendBuf[4] = temp[2];
			sendBuf[5] = temp[3];
			Send2Slave(sendBuf, 6, it->first);
		}
		
	}
	pthread_mutex_unlock(&m_mapLocker);
}



void  CMaster::setmap(std::map<std::string, std::string>  masterMap)
{
	//fprintf(stderr, "set map okay\n");
	pthread_mutex_lock(&m_mapLocker);
	slavemap = masterMap;
	pthread_mutex_unlock(&m_mapLocker);

	//fprintf(stderr, "slavemap size is : %d\n", slavemap.size());
}