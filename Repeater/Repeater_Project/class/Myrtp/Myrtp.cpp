/*
* Myrtp.c
*
* Created: 2016/12/19
* Author: EDWARDS
*/
#include "Myrtp.h"



Myrtp::Myrtp()
{
	
}

Myrtp::~Myrtp()
{

}
void Myrtp::init(uint16_t portbase, string destip_str, string baseip_str)
{

	int status = 0;
	uint32_t destip =0;
	uint32_t baseip;
	uint32_t ssrc = 0;

	baseip = inet_addr(baseip_str.c_str());
	baseip = ntohl(baseip);
	ssrc = (baseip & 0x0000FFFF);//取低八位


	destip = inet_addr(destip_str.c_str());
	destip = ntohl(destip);//将32位网络字节序转换成主机字节序  


	// Now, we'll create a RTP session, set the destination, send some
	// packets and poll for incoming data.

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be sending 10 samples each second, so we'll
	// put the timestamp unit to (1.0/10.0)
	sessparams.SetOwnTimestampUnit(1.0 / 8000.0);

	sessparams.SetAcceptOwnPackets(false);
	sessparams.SetUsePredefinedSSRC(true);//set SSRC for rtp-send	
	sessparams.SetPredefinedSSRC(ssrc);

	transparams.SetPortbase(portbase);
	status = Create(sessparams, &transparams);
	//fprintf(stderr,"status :%d\n", status);
	checkerror(status);

	if (destip != 0){

		RTPIPv4Address addr(destip, 8000);

		status = AddDestination(addr);
		checkerror(status);
	}



}

void Myrtp::checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "RTP_ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		//while (1);
		exit(-1);
	}
}
