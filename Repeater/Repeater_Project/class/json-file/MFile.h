#ifndef MFILE_H
#define MFILE_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <map>
#include <string>
#include <sstream>
#include <list>

#define DeviceType (unsigned char)0x00//master or peer 
#define PeerDevice (unsigned char)0x00 
#define MasterDevice (unsigned char)0x01

#define DeviceIP (unsigned char)0x01
#define DeviceMac (unsigned char)0x02
#define MasterIP (unsigned char)0x03

#define GatewayIP (unsigned char)0x04
#define Subnetmask (unsigned char)0x05
#define SwitchIP (unsigned char)0x06


#define DeviceMode (unsigned char)0x10//dan ge hai shi liang ge 
#define SignleMode (unsigned char)0x00 
#define DoubleMode (unsigned char)0x01// if this,devicenumber is send,else is recive

#define DeviceInterfaceNumber (unsigned char)0x11 

#define Interface_A 0x00 
#define Interface_B 0x01

#define CONFIG_PATH "/opt/config"
#define MAP_PATH "/opt/map"

typedef struct
{
	char ip[32];
	char time[32];
}PEER_T;

union m_ip_info
{
	unsigned char strIP[4];
	int intIP;
};



class MWorks;

class MFile
{
public:
	MFile();
	~MFile();

	int GetConfig(int type, char *pValue);
	int SaveMap(std::map<std::string,std::string> mapS);
	int GetMap(std::map<std::string, std::string> *masterMap);
	
	int Start();
private:
	std::map<int, std::string> m_config;
	pthread_mutex_t m_configMutex;
	std::map<int, PEER_T*> m_map;
	pthread_mutex_t m_mapMutex;
	MWorks *m_pWorks;
	pthread_t m_readMapThrdId;
	bool m_bReadMapThrdRun;
	int m_configFd;
	int m_mapFd;
	bool m_bMapChange;

	void lockConfig();
	void unlockConfig();
	void lockMap();
	void unlockMap();
	int saveConfig();
	int readConfig();
	int readMap();
	static void *readMapFun(void* args);
	int AnalysisConfig(char *p,int length);
	//int AnalysisMap(char *p, int length);
	int AnalysisMap(char *p, int length, std::map <std::string, std::string> *baseMap);
	int checkMapChange(std::map<int, PEER_T*> &peerMap);
	int CompressionConfig(char *p);
	void split(std::string str, char split, std::list<std::string> &rlt);
	/*update config info,but isChange is no use*/
	void updateConfig(std::string key, std::string value,bool &isChange);
	//void updateMap(std::string ipInfo, std::string timeInfo, std::map<int, PEER_T*> &peerMap);
	void updateMap(std::string ipInfo, std::string timeInfo, std::map <std::string, std::string> *baseMap);
	void setInterface(std::string value1,std::string value2);
	void clearMap();
};


#endif