
//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Protocol.h"
CProtocol::CProtocol(bool isMaster)
{
	this->isMaster = isMaster;
	if (isMaster)
	{
		pMaster = new CMaster();
	}
	else
	{
		pSlave = new CSlave();
	}

}
CProtocol::~CProtocol()
{
	if (isMaster && pMaster != NULL)
	{
		delete pMaster;
	}
	if (!isMaster && pSlave != NULL)
	{
		delete pSlave;
	}
	syslog(LOG_LOCAL7 | LOG_DEBUG, "delete class: CProtocol\n"); //fprintf(stderr, "delete class CProtocol\n");
}
bool CProtocol::Connect( const char * masterIp,const char* slaveIp)
{
	if (isMaster && pMaster != NULL)
	{
		return pMaster->Connect(masterIp);
	}
	else if(pSlave!=NULL)
	{
		return pSlave->Connect(masterIp,slaveIp);
	}
	return false;
		 
}
void CProtocol::SetCallBackFunc(void(*callBackFunc)(int, ResponeData))
{
	if (isMaster && pMaster != NULL)
	{
		pMaster->SetCallBackFunc(callBackFunc);
	}
	else if (pSlave != NULL)
	{
		pSlave->SetCallBackFunc(callBackFunc);
	}
}
void CProtocol::ReleaseChannelStatus(const char* ip)
{
	if (isMaster && pMaster != NULL)
	{
		pMaster->ReleaseChannelStatus(ip);
	}
	else if (pSlave != NULL)
	{
		pSlave->ReleaseChannelStatus();
	}
}
void CProtocol::ReplyChannelStatus(int flag, const char* ip)
{
	if (isMaster && pMaster != NULL)
	{
		pMaster->SetStatus(flag,ip);
	}
	else if (pSlave != NULL)
	{

	}
}
void CProtocol::GetStatus()
{
	if (isMaster && pMaster != NULL)
	{
		
	}
	else if (pSlave != NULL)
	{
		pSlave->GetStatus();
	}
}
void CProtocol::DisConnect()
{
	if (isMaster && pMaster != NULL)
	{
		pMaster -> DisConnect();
	}
	else if (pSlave != NULL)
	{
		pSlave->DisConnect();
	}
}
void CProtocol::BeginRecordVoice()
{
	if (isMaster && pMaster != NULL)
	{
		pMaster->MasterBeginRecordVoice();
	}
}
void CProtocol::EndRecorderVoice()
{
	if (isMaster && pMaster != NULL)
	{
		pMaster->MasterEndRecorderVoice();
	}
}

void  CProtocol::SetMap(std::map<std::string, std::string> masterMap)
{

	if (isMaster && pMaster != NULL)
	{
		return pMaster->setmap(masterMap);
	}
}
//std::map<std::string, std::string> CProtocol::GetMap()
//{
//
//	if (isMaster && pMaster != NULL)
//	{
//		return pMaster->getmap();
//	}
//	else if (pSlave != NULL)
//	{
//		return pSlave->getmap();
//	}
//
//
//}



