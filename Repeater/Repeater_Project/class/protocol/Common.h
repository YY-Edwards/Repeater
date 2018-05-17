#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <map>
#include <semaphore.h>  
#include "syninterface.h"
#include "socketwrap.h"
#define                    UDPPORT                              9000
#define                    BUFLENGTH                            1024
#define                    REGISTER                             0
#define                    SLAVEMAP                             1
#define                    ALIVE                                2
#define                    GETCHANNELSTATUS                     3
#define                    SETCHANNELSTATUS                     4
#define                    RELEASECHANNELSTATUS                 5
#define                    BEGINRECORDERVOICE                   6
#define                    ENDRECORDERVOICE                     7
#define                    LOCALSETCHANNELSTATUS                8
#define                    RegisterOpcode                      0x80
#define                    mapOpcode                           0x81
#define                    AliveOpcode                         0x82
#define                    GetChannelStatusOpcode              0x83
#define                    SetChannelStatusOpcode              0x84
#define                    ReleaseChannelOpcode                0x85
#define                    BeginRecorderVoiceOpcode            0x86
#define                    EndRecorderVoiceOpcode              0x87
#define                    LENGTH                              0x05


#define                    INVALIDCHANNEL                       0X00
#define                    GETCHANNEL                           0X01
#define                    RELEASECHANNNEL                      0X02
#define                    SETCHANNEL                           0X03
#define                    BEGINVOICE                           0X04
#define                    ENDVOICE                             0X05

#define                    STATUSFREE                           0X00
struct ResponeData
{
	std::map <std::string, std::string>  slavemap;
	std::string getChannelIp;
	std::string releaseChannelIp;
	int channel;              /*0X01:getchannelStatus    0x02  releaseChannel   0X03:setChannelStatus*/
	int status ;               /*0x00  信道空闲   0x01信道忙*/
};

extern pthread_mutex_t m_onDataLocker;
#endif // !COMMON_H
