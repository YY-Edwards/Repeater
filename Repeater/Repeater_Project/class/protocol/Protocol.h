#ifndef PROTOCOL_H
#include "CMaster.h"
#include "CSlave.h"
#include <syslog.h>
class CProtocol
{
public:
	CProtocol(bool isMaster);
	~CProtocol();
	/*
	���ӽӿ�
	*/
	bool Connect( const char * masterIp,const char* slaveIp);
	/*
	�ص��ӿ�
	���ûص�����
	*/
	void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));
	/*
	�ͷ��ŵ�
	*/
	void ReleaseChannelStatus(const char* ip);             /* slave ip*/
	/*
	��ȡ�ŵ���æ״̬
	*/
	void GetStatus();
	/*
	���ŵ���æ״̬
	*/
	void ReplyChannelStatus(int flag, const char* ip);
	/*
	�ͷ���Դ
	*/
	void DisConnect();
	/*
	׼����ʼ¼��
	*/
	void BeginRecordVoice();
	/*
	����¼��
	*/
	void EndRecorderVoice();

	/*
	���ӱ����ļ��л�ȡ��map���͵�master
	*/
	void SetMap(std::map<std::string, std::string> masterMap);

	/*��ȡmap*/
	//std::map<std::string, std::string> GetMap();

private:
	bool isMaster;
	CMaster * pMaster;
	CSlave  * pSlave;

};
#endif // !PROTOCOL_H

