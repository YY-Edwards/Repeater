/*
 * Myrtp.h
 *
 * Created: 2016/12/16
 * Author: EDWARDS
 */ 
#ifndef Myrtp_h_
#define Myrtp_h_ 


#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rtpsourcedata.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>

using namespace jrtplib;
using namespace std;

/*
linux rtp multicast note:

Use the "ifconfig" command to see whether the network card supports multicast
	:UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1


key_words:
	rp_filters:
		net.ipv4.conf.default.rp_filter = 0
		net.ipv4.conf.all.rp_filter = 0	

	add route:

		route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0 
		route add default gw 192.168.2.1 netmask 255.255.255.0 dev eth0
		

*/

class Myrtp :public RTPSession
{

	public:
		
		Myrtp();
		~Myrtp();

		void init(uint16_t portbase,string destip_str, string baseip_str);
		
	private:
	
		void checkerror(int rtperr);

		
};


#endif

