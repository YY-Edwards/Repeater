/*
 * Monitor_Interface.h
 *
 * Created: 2016/12/16
 * Author: EDWARDS
 */ 
#ifndef Monitor_Interface_h_
#define Monitor_Interface_h_ 
#include <string.h>
#include <string>

#include<stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include "MFile.h"
#include <syslog.h>


class Monitor_Interface 
{	

public:

	Monitor_Interface();
	~Monitor_Interface();
	MFile *p_file;

	bool write_map_file();

	bool get_repeater_is_master(){ return isMaster; }
	uint8_t get_userport(){ return userport; }
	std::string get_base_ip(){ return ((std::string)base_ip_str); }
	std::string get_master_ip(){ return master_ip_str; }//isMaster


protected:

	//bool channel_busy_flag;

private:

	
	//Read configuration file from Monitor(json)
	//Config ID; baseip; MAC; (masterip);
	void config_hw_repeater();

	uint8_t config_hw_repeater_flag;

	//0:portA; 1:portB; 2:portA-RX, portB-TX; 3:portA-TX, portB-RX;
	uint8_t userport;//e.g:AR-BT

	//uint8_t ID;//isMaster or isSlave
	bool isMaster;
	//uint32_t base_ip;//e.g:192.168.2.113
	//std::string base_ip_str;
	char base_ip_str[32];

	//uint32_t master_ip;//e.g:192.168.2.113
	std::string master_ip_str;

	char MAC[32];//e.g:00:01:23:55:00:01

	char gateway_ip[32];//e.g:192.168.2.1
	char subnetmask[32];//e.g:255.255.255.0
	char switch_ip[32];//e.g:192.168.1.1

	void read_config_file();
	
	
};




#endif
