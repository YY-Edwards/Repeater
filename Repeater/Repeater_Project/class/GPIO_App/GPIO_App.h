/*
 * GPIO_App.h
 *
 * Created: 2016/12/16
 * Author: EDWARDS
 */ 
#ifndef GPIO_App_h_
#define GPIO_App_h_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gpio.h"//gpio interface
#include <syslog.h>

//static struct pollfd *fdset;

class GPIO_App 
{

public:
	GPIO_App();
	~GPIO_App();

	void config_GPIO(uint8_t port);//config :gpio_cd_portnumb;, gpio_ptt_portnumb;

	uint8_t port_b_ptt_onoff(uint8_t action);

	uint8_t ptt_onoff(uint8_t action);
	uint8_t get_cd_current_value();

	uint16_t get_cd_fd(){ return gpio_fd_open(gpio_cd_pin); }////CD_fd = gpio_fd_open(GPIO_B_CD);userport£»
	uint16_t close_cd_fd(){ return gpio_fd_close(gpio_cd_pin); }

	uint8_t restart_device_system(){ return gpio_set_value(GPIO_RELAY, HIGH_LEVEL); }//Restart hardware system


private:

	uint8_t userport;

	uint8_t gpio_cd_pin;
	uint8_t gpio_ptt_pin;

	uint8_t gpio_config_flag;
	
	
};




#endif