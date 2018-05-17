#ifndef MASTER_H
#define MASTER_H
#include "Common.h"

class CMaster
{
public:
	CMaster();
	~CMaster();
	bool Connect(const char* masterIp);
	void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));
	void SetStatus(int flag, std::string ip);
	void ReleaseChannelStatus(std::string ip);
	void DisConnect();
	void MasterBeginRecordVoice();
	void MasterEndRecorderVoice();

	void  setmap(std::map<std::string, std::string>  masterMap);
	//std::map<std::string, std::string> getmap(){ return slavemap; }
	//std::map<std::string, std::string> slavemap;
private:
	void(*myCallBackFunc)(int, ResponeData);
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);
	bool InitSocket();
	bool CloseSocket(int sockfd);
	void CreateRecvThread();
	static void*/* DWORD WINAPI*/ RecvThread(void*/*LPVOID*/ p);
	static void* /*DWORD  WINAPI*/ MonitorAliveThread(void* /*LPVOID*/ p);
	void RecvThreadFunc();
	void MonitorAliveThreadFunc();
	char * stringSplit();
	void Sendmap2Repeater(std::string rmtIp);
	void Updatemap(std::string rmtIp);
	void Sendmap2Slave();
	void SendAlive2Slave(std::string slaveIp);
	int Send2Slave(char * pSendBuf, int length, std::string ip);
	time_t convert_string_to_time_t(const std::string & time_string);
	void SlaveBeginRecordVoice(std::string ip);
	void SlaveEndRecorderVoice(std::string ip);
	
	int sockfd;
	//SOCKET sockfd;
	struct sockaddr_in rmtAddr;
	struct sockaddr_in myAddr;
	bool  socketOpen;
	const char* masterIp;
	bool isRecvStatus;
	char recvBuf[BUFLENGTH];
	std::map<std::string, std::string> slavemap;


	Mutex *m_mapLocker;
	Mutex *m_onDataLocker;
	Mutex *m_sendLocker;
	Mutex *m_aliveLocker;

	/*pthread_mutex_t m_mapLocker;
	pthread_mutex_t m_onDataLocker;
	pthread_mutex_t m_sendLocker;
	pthread_mutex_t m_aliveLocker;*/
	//std::string strIp;
	//char sendBuf[BUFLENGTH];
	std::string getStatusIp;
	bool isAlive;
	

	MyCreateThread * alive_pthread_p;
	MyCreateThread * udp_recv_pthread_p;

	/*pthread_t id;
	pthread_t aliveId;*/
	
};


#endif 