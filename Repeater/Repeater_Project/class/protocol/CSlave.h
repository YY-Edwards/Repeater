#ifndef CSLAVE_H
#define  CSLAVE_H
#include "Common.h"
class CSlave
{
public:
	CSlave();
	~CSlave();
	bool Connect(const char* masterIp,const char* slaveIp);
	void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));
	void GetStatus();
	void ReleaseChannelStatus();
	void DisConnect();
	//std::map<std::string, std::string> getmap(){ return slavemap; }
	//std::map<std::string, std::string> slavemap;
private:
	void(*myCallBackFunc)(int, ResponeData);
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);
	bool InitSocket();
	bool CloseSocket(int sockfd);
	void SendRegister2Master();
	void CreateRecvThread();
	static  void* /*DWORD WINAPI*/ RecvThread(void*/*LPVOID*/ p);
	static void* /*DWORD  WINAPI*/ SendAliveThread(void* /*LPVOID*/ p);
	static void* /*DWORD  WINAPI*/ MonitorStatusThread(void* /*LPVOID*/ p);
	void RecvThreadFunc();
	void SendAliveThreadFunc();
	void MonitorStatusThreadFunc();
	int Send2Master(char* pSendBuf,int length);
	int Send2Slave(int length,std::string ip);
	void stringSplit(std::string strTemp);
	void Sendmap2Repeater(int mapCount);
	time_t convert_string_to_time_t(const std::string & time_string);
	void SendAlive2Master();

	
	bool set_thread_exit_flag;

	/*
	设置线程退出标志
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }

	
	int sockfd;
	struct sockaddr_in rmtAddr;
	struct sockaddr_in myAddr;
	bool  socketOpen;
	const char* masterIp;
	const char* slaveIp;
	bool isRecvStatus;
	bool isSendAlive;
	char recvBuf[BUFLENGTH];
	std::map<std::string, std::string> slavemap;
	pthread_mutex_t m_mapLocker;
	pthread_mutex_t m_sendLocker;
	pthread_mutex_t m_statusLocker;
	pthread_mutex_t m_onDataLocker;


	pthread_mutex_t lastRecvAliveTimeLocker;

	//std::string strIp;
	//char sendBuf[BUFLENGTH];
	char  lastRecvAliveTime[64];
	bool isRecvedmap;
	sem_t sem;
	bool isGetStatus;

	transresult_t rt;

	pthread_t id;
	pthread_t aliveId;
	pthread_t monitorId;

};


#endif