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
	连接接口
	*/
	bool Connect( const char * masterIp,const char* slaveIp);
	/*
	回调接口
	设置回调函数
	*/
	void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));
	/*
	释放信道
	*/
	void ReleaseChannelStatus(const char* ip);             /* slave ip*/
	/*
	获取信道闲忙状态
	*/
	void GetStatus();
	/*
	答复信道闲忙状态
	*/
	void ReplyChannelStatus(int flag, const char* ip);
	/*
	释放资源
	*/
	void DisConnect();
	/*
	准备开始录音
	*/
	void BeginRecordVoice();
	/*
	结束录音
	*/
	void EndRecorderVoice();

	/*
	将从本地文件中获取的map发送到master
	*/
	void SetMap(std::map<std::string, std::string> masterMap);

	/*获取map*/
	//std::map<std::string, std::string> GetMap();

private:
	bool isMaster;
	CMaster * pMaster;
	CSlave  * pSlave;

};
#endif // !PROTOCOL_H

