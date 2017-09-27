#include "MFile.h"
//#include "MWorks.h"
#include <string.h>

MFile::MFile()
{
	pthread_mutex_init(&m_configMutex, NULL);
	pthread_mutex_init(&m_mapMutex, NULL);
	m_pWorks = NULL;
	m_bReadMapThrdRun = false;
	m_configFd = -1;
	m_mapFd = -1;
	m_bMapChange = false;
}

MFile::~MFile()
{

	void *tret;
	m_bReadMapThrdRun = false;
	//if (pthread_join(m_readMapThrdId, &tret) != 0)
	//{
	//	fprintf(stderr,"Join readMapFun error!\n");
	//}
	//fprintf(stderr,"readMapFun exit code: %d.\n", *((int *)tret));
	lockMap();
	clearMap();
	unlockMap();

	if (m_mapFd != -1)
	{
		close(m_mapFd);
	}
	if (m_configFd != -1)
	{
		close(m_configFd);
	}

	pthread_mutex_destroy(&m_configMutex);
	pthread_mutex_destroy(&m_mapMutex);
	fprintf(stderr,"delete class MFile\n");
}

//int MFile::SaveConfig(int type, char *pValue)
//{
//	int rlt = 0;
//	lockConfig();
//	bool isNeedSave = true;
//	std::map<int, std::string>::iterator it = m_config.find(type);
//	if (it != m_config.end())
//	{
//		if (0 == (strcmp(pValue, it->second.c_str())))
//		{
//			isNeedSave = false;
//		}
//	}
//
//	if (isNeedSave)
//	{
//		m_config[type] = pValue;
//		rlt = saveConfig();
//	}
//	unlockConfig();
//	return rlt;
//}

int MFile::GetConfig(int type, char *pValue)
{
	int rlt = 0;
	lockConfig();
	std::map<int, std::string>::iterator it = m_config.find(type);
	if (it != m_config.end())
	{
		strcpy(pValue, it->second.c_str());
	}
	else
	{
		rlt = 1;
	}
	unlockConfig();
	return rlt;
}

int MFile::GetMap(std::map<std::string, std::string> *masterMap)
{

	int rlt = 0;
	char mapBuf[1024] = { 0 };
	int nmRead = 0;

	lseek(m_mapFd, 0, SEEK_SET);
	nmRead = read(m_mapFd, mapBuf, 1024);
	AnalysisMap(mapBuf, nmRead, masterMap);

	/*test code*/
	//fprintf(stderr, "base_map size is : %d\n", (*masterMap).size());
	//std::map<std::string, std::string>::iterator it = (*masterMap).begin();
	//fprintf(stderr,"read map:\n");

	//fprintf(stderr, "map: 1\n");
	//for (; it != (*masterMap).end(); it++)
	//{
	//	std::string key = it->first;
	//	std::string value = it->second;
	//	fprintf(stderr,"ip:%s\n", it->second->ip);
	//	fprintf(stderr, "ip:%s\n", key.c_str());
	//	fprintf(stderr, "time:%s\n", value.c_str());

	//}

	//fprintf(stderr, "map: 1.1\n");
	return 0;


}



void MFile::lockConfig()
{
	pthread_mutex_lock(&m_configMutex);
}

void MFile::unlockConfig()
{
	pthread_mutex_unlock(&m_configMutex);
}

void MFile::lockMap()
{
	pthread_mutex_lock(&m_mapMutex);
}

void MFile::unlockMap()
{
	pthread_mutex_unlock(&m_mapMutex);
}

//int MFile::GetMap(PEER_T peer[MAX_PEER_SIZE], int &count)
//{
//	int rlt = 0;
//	count = 0;
//	int index = 0;
//	//fprintf(stderr,"tag2.2.1.1\n");
//	std::map<int, PEER_T*>::iterator i = m_map.begin();
//	lockMap();
//	for (i = m_map.begin(); i != m_map.end(); i++, index++)
//	{
//		if (index > (MAX_PEER_SIZE - 1))
//		{
//			break;
//		}
//		//fprintf(stderr,"tag2.2.1.2\n");
//		count++;
//		memcpy(&peer[index], i->second, sizeof(PEER_T));
//		//fprintf(stderr,"tag2.2.1.3\n");
//	}
//	unlockMap();
//	//fprintf(stderr,"tag2.2.1.4\n");
//	return rlt;
//}

int MFile::saveConfig()
{
	int rlt = 0;
	char buf[1024] = { 0 };
	/* 清空文件 */
	ftruncate(m_configFd, 0);
	/* 重新设置文件偏移量 */
	lseek(m_configFd, 0, SEEK_SET);
	/*按特定格式压缩*/
	CompressionConfig(buf);

	fprintf(stderr,"save config:\n");
	std::map<int, std::string>::iterator itTemp = m_config.begin();
	for (; itTemp != m_config.end(); itTemp++)
	{
		//fprintf(stderr,"tag4\n");
		fprintf(stderr,"DeviceType:0x%02x,Value:%s\n", itTemp->first, itTemp->second.c_str());
	}

	/*写入文件*/
	write(m_configFd, buf, strlen(buf));
	return rlt;
}

int MFile::readMap()
{
	//int rlt = 0;
	//char mapBuf[1024] = { 0 };
	//int nmRead = 0;
	//while (m_bReadMapThrdRun)
	//{
	//	/*读文件判断IP是否发生改变*/
	//	/*test config write*/
	//	//SaveConfig(DeviceIP, "192.168.2.139");
	//	//fprintf(stderr,"start read map\n");
	//	lseek(m_mapFd, 0, SEEK_SET);
	//	nmRead = read(m_mapFd, mapBuf, 1024);
	//	AnalysisMap(mapBuf, nmRead);
	//	/*发生改变则广播给PC*/
	//	if (m_bMapChange)
	//	{
	//		fprintf(stderr,"map change:\n");
	//		lockMap();
	//		std::map<int, PEER_T*>::iterator it = m_map.begin();
	//		for (; it != m_map.end(); it++)
	//		{
	//			fprintf(stderr,"ip:%s,last time:%s\n", it->second->ip, it->second->time);
	//		}
	//		unlockMap();
	//		m_pWorks->AddBroadCastCmd(SystemMap);
	//		m_bMapChange = false;
	//	}
	//	usleep(INTERVAL_READ_MAP);
	//}
	//return rlt;
}

//int MFile::setWorkPointer(MWorks* value)
//{
//	m_pWorks = value;
//	return 0;
//}

int MFile::Start()
{
	int rlt = 0;

	m_configFd = open(CONFIG_PATH, O_RDONLY);
	if (m_configFd == -1)
	{
		fprintf(stderr,"config open error\n");
		return 1;
	}
	//m_configFd = fd;

	m_mapFd = open(MAP_PATH, O_RDWR | O_CREAT); 
	if (m_mapFd == -1)
	{
		fprintf(stderr,"map open error\n");
		return 1;
	}

	/*读取配置到缓存*/
	readConfig();
	///*开启定时读取map表的线程*/
	//m_bReadMapThrdRun = true;
	//if (0 != pthread_create(&m_readMapThrdId, NULL, readMapFun, this))
	//{
	//	fprintf(stderr,"Create readMapFun error!\n");
	//}
	//fprintf(stderr,"1.\n");
	return m_configFd;
}

int MFile::readConfig()
{
	/*从文件读取当前配置,若没有就创建*/
	char configBuf[1024] = { 0 };
	int ncRead = 0;
	ncRead = read(m_configFd, configBuf, 1024);
	AnalysisConfig(configBuf, ncRead);
	return 0;
}

void * MFile::readMapFun(void* args)
{
	MFile *p = (MFile*)args;
	if (p)
	{
		fprintf(stderr,"MFile::readMapFun run\n");
		p->readMap();
	}
	return ((void *)0); // 自动退出线程
}

int MFile::AnalysisConfig(char *p, int length)
{
	std::list<std::string> rlt1, rlt2;

	split(p, '{', rlt1);
	while (rlt1.size() > 0)
	{
		split(rlt1.front(), '}', rlt2);
		//fprintf(stderr,"rlt1.size:%d %s\n", rlt1.size(), rlt1.front().c_str());
		rlt1.pop_front();
	}
	while (rlt2.size() > 0)
	{
		split(rlt2.front(), ',', rlt1);
		//fprintf(stderr,"rlt2.size:%d %s\n", rlt2.size(), rlt2.front().c_str());
		rlt2.pop_front();
	}

	while (rlt1.size() > 0)
	{
		split(rlt1.front(), ':', rlt2);
		//fprintf(stderr,"rlt1.size:%d %s\n", rlt1.size(), rlt1.front().c_str());
		rlt1.pop_front();
	}
	bool isChange = false;
	lockConfig();
	//fprintf(stderr,"tag1\n");
	while (rlt2.size() > 1)
	{
		//fprintf(stderr,"tag2\n");
		std::string key = rlt2.front();
		rlt2.pop_front();
		std::string value = rlt2.front();
		rlt2.pop_front();
		//fprintf(stderr,"key:%s,value:%s\n", key.c_str(), value.c_str());
		updateConfig(key, value, isChange);
	}
	fprintf(stderr,"read config:\n");
	std::map<int, std::string>::iterator itTemp = m_config.begin();
	for (; itTemp != m_config.end(); itTemp++)
	{
		//fprintf(stderr,"tag4\n");
		//fprintf(stderr,"DeviceType:0x%02x,Value:%s\n", itTemp->first, itTemp->second.c_str());
	}
	//fprintf(stderr,"tag5\n");
	unlockConfig();

	//fprintf(stderr,"tag6\n");
	return 0;
}

//int MFile::AnalysisMap(char *p, int length)
int MFile::AnalysisMap(char *p, int length, std::map <std::string, std::string> *base_Map)
{
	m_bMapChange = false;

	std::list<std::string> rlt1, rlt2;
	split(p, '[', rlt1);
	while (rlt1.size() > 0)
	{
		split(rlt1.front().c_str(), ']', rlt2);
		rlt1.pop_front();
	}
	while (rlt2.size() > 0)
	{
		split(rlt2.front().c_str(), '{', rlt1);
		rlt2.pop_front();
	}
	while (rlt1.size() > 0)
	{
		split(rlt1.front().c_str(), '}', rlt2);
		rlt1.pop_front();
	}
	while (rlt2.size() > 0)
	{
		split(rlt2.front().c_str(), ',', rlt1);
		rlt2.pop_front();
	}
	//while (rlt1.size() > 0)
	//{
	//	split(rlt1.front().c_str(), ':', rlt2);
	//	rlt1.pop_front();
	//}


	std::map<int, PEER_T*> peerMap;
	while (rlt1.size() > 1)
	{
		std::string ipInfo = rlt1.front();
		rlt1.pop_front();
		std::string timeInfo = rlt1.front();
		rlt1.pop_front();
		//fprintf(stderr,"key:%s,value:%s\n", ipInfo.c_str(), timeInfo.c_str());
		//updateMap(ipInfo, timeInfo, peerMap);
		updateMap(ipInfo, timeInfo, base_Map);
	}

	/*test code*/
	//std::map<int, PEER_T*>::iterator it = peerMap.begin();
	//fprintf(stderr,"read map:\n");
	//for (; it != peerMap.end();it++)
	//{
	//	fprintf(stderr,"ip:%s\n", it->second->ip);
	//}

	//lockMap();
	/*检查map是否存在改变*/
	//checkMapChange(peerMap);
	//unlockMap();

	return 0;
}

int MFile::CompressionConfig(char *p)
{
	std::string isMaster = "";
	std::string port = "";
	//lockConfig();
	if ("0" == m_config[DeviceType])
	{
		isMaster = "false";
	}
	else
	{
		isMaster = "true";
	}

	if (m_config[DeviceMode] == "1")
	{
		if (m_config[DeviceInterfaceNumber] == "1")
		{
			port = "AR-BT";
		}
		else
		{
			port = "AT-BR";
		}
	}
	else
	{
		if (m_config[DeviceInterfaceNumber] == "1")
		{
			port = "AR-AT";
		}
		else
		{
			port = "BT-BR";
		}
	}

	sprintf(p, "{\"isMaster\":%s,\"ip\":\"%s\",\"mac\":\"%s\",\"masterIp\":\"%s\",\"port\":\"%s\"}",
		isMaster.c_str(),
		m_config[DeviceIP].c_str(),
		m_config[DeviceMac].c_str(),
		m_config[MasterIP].c_str(),
		port.c_str());
	//unlockConfig();
	return 0;
}

void MFile::split(std::string str, char split, std::list<std::string> &rlt)
{
	std::string value = str;
	std::stringstream ss(str);
	std::string subStr;
	while (std::getline(ss, subStr, split))
	{
		int length = subStr.length();
		if (length != 0 && subStr != "\n")
		{
			rlt.push_back(subStr);
		}
	}
}

void MFile::updateConfig(std::string key, std::string value, bool &isChange)
{
	char tempValue[128] = { 0 };
	isChange = false;
	std::list<std::string> temp;
	//fprintf(stderr,"\"isMaster\"\n");
	//fprintf(stderr,"tag2.1\n");
	if ("\"isMaster\"" == key)
	{
		//fprintf(stderr,"tag2.2\n");
		if ("false" == value)
		{
			//fprintf(stderr,"tag2.3\n");
			//m_config[DeviceType] = PeerDevice;
			sprintf(tempValue, "%d", PeerDevice);
		}
		else
		{
			//fprintf(stderr,"tag2.4\n");
			//m_config[DeviceType] = MasterDevice;
			sprintf(tempValue, "%d", MasterDevice);
		}
		m_config[DeviceType] = tempValue;
		//fprintf(stderr,"tag2.5\n");
	}
	else if ("\"ip\"" == key)
	{
		temp.clear();
		split(value, '"', temp);
		m_config[DeviceIP] = temp.front();
		//fprintf(stderr,"tag2.6\n");
	}
	else if ("\"gatewayAddress\"" == key)
	{
		temp.clear();
		split(value, '"', temp);
		m_config[GatewayIP] = temp.front();
	}
	else if ("\"subnetMask\"" == key)
	{
		temp.clear();
		split(value, '"', temp);
		m_config[Subnetmask] = temp.front();
	}
	else if ("\"switchAddress\"" == key)
	{
		temp.clear();
		split(value, '"', temp);
		m_config[SwitchIP] = temp.front();
	}
	else if ("\"mac\"" == key)
	{
		temp.clear();
		split(value, '"', temp);
		m_config[DeviceMac] = temp.front();
		//fprintf(stderr,"tag2.7\n");
	}
	else if ("\"masterIp\"" == key)
	{
		temp.clear();
		split(value, '"', temp);
		m_config[MasterIP] = temp.front();
		//fprintf(stderr,"tag2.8\n");
	}
	//else if ("\"port\"" == key)
	//{
	//	temp.clear();
	//	split(value, '"', temp);
	//	std::string valueInfo = temp.front();
	//	temp.clear();
	//	split(valueInfo, '-', temp);
	//	std::string port1 = temp.front();
	//	temp.pop_front();
	//	std::string port2 = temp.front();
	//	temp.pop_front();
	//	//fprintf(stderr,"tag2.9\n");
	//	setInterface(port1, port2);
	//	//fprintf(stderr,"tag2.10\n");
	//}
}

void MFile::setInterface(std::string value1, std::string value2)
{
	char tempValue[128] = { 0 };
	/*A发A收*/
	if ((value1 == "AR" && value2 == "AT") || (value1 == "AT" && value2 == "AR"))
	{
		//fprintf(stderr,"tag2.9.1\n");
		sprintf(tempValue, "%d", SignleMode);
		m_config[DeviceMode] = tempValue;
		sprintf(tempValue, "%d", Interface_A);
		m_config[DeviceInterfaceNumber] = tempValue;
	}
	else if ((value1 == "BR" && value2 == "BT") || (value1 == "BT" && value2 == "BR"))
	{
		//fprintf(stderr,"tag2.9.2\n");
		sprintf(tempValue, "%d", SignleMode);
		m_config[DeviceMode] = tempValue;
		sprintf(tempValue, "%d", Interface_B);
		m_config[DeviceInterfaceNumber] = tempValue;
	}
	else if ((value1 == "AR" && value2 == "BT") || (value1 == "BT" && value2 == "AR"))
	{
		//fprintf(stderr,"tag2.9.3\n");
		sprintf(tempValue, "%d", DoubleMode);
		m_config[DeviceMode] = tempValue;
		sprintf(tempValue, "%d", Interface_B);
		m_config[DeviceInterfaceNumber] = tempValue;
	}
	else if ((value1 == "BR" && value2 == "AT") || (value1 == "AT" && value2 == "BR"))
	{
		//fprintf(stderr,"tag2.9.4\n");
		sprintf(tempValue, "%d", DoubleMode);
		m_config[DeviceMode] = tempValue;
		sprintf(tempValue, "%d", Interface_A);
		m_config[DeviceInterfaceNumber] = tempValue;
	}
}

//void MFile::updateMap(std::string ipInfo, std::string timeInfo, std::map<int, PEER_T*> &peerMap)
void MFile:: updateMap(std::string ipInfo, std::string timeInfo, std::map <std::string, std::string> *baseMap)
{
	PEER_T *p = new PEER_T;

	//std::map <std::string, std::string> Map = *baseMap;

	int ip[4] = { 0 };
	int time[6] = { 0 };
	sscanf(ipInfo.c_str(), "\"ip\":\"%d.%d.%d.%d\"",
		&ip[0],
		&ip[1],
		&ip[2],
		&ip[3]);
	sscanf(timeInfo.c_str(), "\"time\":\"%d-%d-%d %d:%d:%d\"",
		&time[0],
		&time[1],
		&time[2],
		&time[3],
		&time[4],
		&time[5]);
	m_ip_info ipinfo = { 0 };
	ipinfo.strIP[0] = ip[0];
	ipinfo.strIP[1] = ip[1];
	ipinfo.strIP[2] = ip[2];
	ipinfo.strIP[3] = ip[3];
	sprintf(p->ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	sprintf(p->time, "%04d-%02d-%02d %02d:%02d:%02d", time[0], time[1], time[2], time[3], time[4], time[5]);

	
	(*baseMap)[p->ip] = p->time;
	//Map[p->ip] = p->time;

	//std::map<int, PEER_T*>::iterator it = m_map.find(ipinfo.intIP);
	//peerMap[ipinfo.intIP] = p;
}

int MFile::checkMapChange(std::map<int, PEER_T*> &peerMap)
{
	std::map<int, PEER_T*>::iterator dst, src;
	if (peerMap.size() != m_map.size())
	{
		m_bMapChange = true;
	}
	else
	{
		for (dst = peerMap.begin(); dst != peerMap.end(); dst++)
		{
			src = m_map.find(dst->first);
			if (src == m_map.end())
			{
				m_bMapChange = true;
				break;
			}
		}
	}

	if (m_bMapChange)
	{
		/*清空map*/
		clearMap();
		/*赋值*/
		for (dst = peerMap.begin(); dst != peerMap.end(); dst++)
		{
			m_map[dst->first] = dst->second;
		}
	}
	else
	{
		while (peerMap.size() > 0)
		{
			std::map<int, PEER_T*>::iterator it = peerMap.begin();
			if (it->second)
			{
				delete it->second;
				it->second = NULL;
			}
			peerMap.erase(it);
		}
	}
	return 0;
}

int MFile::SaveMap(std::map<std::string, std::string> mapS)
{
	std::map<std::string, std::string>::iterator it = mapS.begin();
	std::string json = "";
	int num = mapS.size();
	char buf[1024] = { 0 };
	if (num == 0){
		/* 清空文件 */
		ftruncate(m_mapFd, 0);
		/* 重新设置文件偏移量 */
		lseek(m_mapFd, 0, SEEK_SET);

		fprintf(stderr, "reset map okay\n");
	
	}

	for (; it != mapS.end(); it++)
	{
		std::string peerinfo = "";
		//std::string ip = it->first;
		//std::string time = it->second;
		sprintf(buf, "{\"ip\":\"%s\",\"time\":\"%s\"}", it->first.c_str(), it->second.c_str());
		//peerinfo = "{" + "\"ip\":" + "\"" + ip + "\"," + "\"time\":" + "\"" + time + "\"}";
		peerinfo = buf;
		if (num != 1)
		{
			//peerinfo += ",";
			sprintf(buf, "%s,", peerinfo.c_str());
			peerinfo = buf;
		}
		num--;
		json += peerinfo;
	}
	if ("" != json)
	{
		//json = "[" + json + "]";
		sprintf(buf, "[%s]", json.c_str());
		json = buf;
		/* 清空文件 */
		ftruncate(m_mapFd, 0);
		/* 重新设置文件偏移量 */
		lseek(m_mapFd, 0, SEEK_SET);
		/*写入文件*/
		write(m_mapFd, json.c_str(), json.length());

	}
	fprintf(stderr,"mapJson:%s\n", json.c_str());
	return 0;
}



void MFile::clearMap()
{
	while (m_map.size() > 0)
	{
		std::map<int, PEER_T*>::iterator it = m_map.begin();
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
		m_map.erase(it);
	}
}
