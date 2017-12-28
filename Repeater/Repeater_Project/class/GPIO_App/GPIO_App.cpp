/*
* GPIO_App.c
*
* Created: 2016/12/19
* Author: EDWARDS
*/
#include "GPIO_App.h"
#include <string>
//#include <sys/time.h>  
//#include <sys/select.h>  
//#include <time.h> 


//extern int pause_flag;

GPIO_App::GPIO_App()
	: gpio_config_flag(0)
	, userport(0)
	, gpio_cd_pin(GPIO_A_CD)
	, gpio_ptt_pin(GPIO_A_PTT)
{
	
}

GPIO_App::~GPIO_App()
{

//Unexport GPIO
	gpio_unexport(GPIO_A_PTT);
	gpio_unexport(GPIO_A_CD);
	gpio_unexport(GPIO_RELAY);

	gpio_unexport(GPIO_B_PTT);
	//gpio_unexport(GPIO_B_CD);


	/*gpio_export(GPIO_A_IO1);
	gpio_export(GPIO_A_IO2);
	gpio_export(GPIO_A_IO3);
	gpio_export(GPIO_A_IO4);*/
	/*gpio_unexport(LINE_IN_AUD_SEL0);
	gpio_unexport(LINE_IN_AUD_SEL1);
	gpio_unexport(LINE_IN_AUD_SEL2);

	gpio_unexport(LINE_OUT_AUD_SEL0);
	gpio_unexport(LINE_OUT_AUD_SEL1);
	gpio_unexport(LINE_OUT_AUD_SEL2);*/

	//fprintf(stderr,"delete class GPIO_App\n");
	syslog(LOG_LOCAL7 | LOG_DEBUG, "delete class: GPIO_App\n");

}

//void GPIO_App::setTimer(int seconds, int mseconds)
//{
//	//int counter = 0;
//
//	struct timeval temp;
//
//	temp.tv_sec = seconds;
//
//	temp.tv_usec = mseconds;
//
//	select(0, NULL, NULL, NULL, &temp);
//
//	//counter++;
//
//	//fprintf(stderr,"timer: %d\n", counter);
//
//	return;
//
//
//}

void GPIO_App::config_GPIO(uint8_t port)
{
	//fprintf(stderr,"config GPIO...\n");
	//syslog(LOG_LOCAL7 | LOG_DEBUG, "config GPIO...\n");
	if (gpio_config_flag == 0)
		gpio_config_flag = 1;

	std::string p = "both";
	char edge_mode[8] = { 0 };
	strcpy(edge_mode, p.c_str());
	//fprintf(stderr, "edge_mode : %s\n", edge_mode);

	//fprintf(stderr, "config port A \n");
	gpio_cd_pin = GPIO_A_CD;
	gpio_ptt_pin = GPIO_A_PTT;
	//Export GPIO
	gpio_export(gpio_cd_pin);
	gpio_export(gpio_ptt_pin);
	gpio_export(GPIO_RELAY);

	gpio_export(GPIO_B_PTT);//µ¼³öB_PTT¶Ë¿Ú
	/*gpio_export(GPIO_A_IO1);
	gpio_export(GPIO_A_IO2);
	gpio_export(GPIO_A_IO3);
	gpio_export(GPIO_A_IO4);*/

	//Set direction
	gpio_set_dir(GPIO_RELAY, OUT);
	gpio_set_dir(gpio_ptt_pin, OUT);
	gpio_set_dir(gpio_cd_pin, IN);

	gpio_set_dir(GPIO_B_PTT, OUT);


	//Set edge
	gpio_set_edge(gpio_cd_pin, edge_mode);

	//Set value
	gpio_set_value(gpio_ptt_pin, HIGH_LEVEL);//default out high level
	gpio_set_value(GPIO_RELAY, HIGH_LEVEL);//default out high level

	gpio_set_value(GPIO_B_PTT, HIGH_LEVEL);//default out high level

			//fprintf(stderr,"config port A \n");
		
			//	gpio_export(LINE_IN_AUD_SEL0);
			//	gpio_export(LINE_IN_AUD_SEL1);
			//	gpio_export(LINE_IN_AUD_SEL2);

			//	gpio_export(LINE_OUT_AUD_SEL0);
			//	gpio_export(LINE_OUT_AUD_SEL1);
			//	gpio_export(LINE_OUT_AUD_SEL2);


			////Set direction

			//	gpio_set_dir(LINE_IN_AUD_SEL0, OUT);
			//	gpio_set_dir(LINE_IN_AUD_SEL1, OUT);
			//	gpio_set_dir(LINE_IN_AUD_SEL2, OUT);

			//	gpio_set_dir(LINE_OUT_AUD_SEL0, OUT);
			//	gpio_set_dir(LINE_OUT_AUD_SEL1, OUT);
			//	gpio_set_dir(LINE_OUT_AUD_SEL2, OUT);

			////Set value
			//	
			//	//Select channel:1(portA:lineIN)
			//	gpio_set_value(LINE_IN_AUD_SEL0, HIGH_LEVEL);//1
			//	gpio_set_value(LINE_IN_AUD_SEL1, LOW_LEVEL);//0
			//	gpio_set_value(LINE_IN_AUD_SEL2, LOW_LEVEL);//0

			//	//Select channel:1(portA:lineOUT)
			//	gpio_set_value(LINE_OUT_AUD_SEL0, HIGH_LEVEL);//1
			//	gpio_set_value(LINE_OUT_AUD_SEL1, LOW_LEVEL);//0
			//	gpio_set_value(LINE_OUT_AUD_SEL2, LOW_LEVEL);//0


	
	//switch (port){

	//	case 0://portA

	//		fprintf(stderr,"config port A \n");
	//		gpio_cd_pin = GPIO_A_CD;
	//		gpio_ptt_pin = GPIO_A_PTT;
	//		//Export GPIO
	//			gpio_export(GPIO_A_PTT);
	//			gpio_export(GPIO_A_CD);
	//			/*gpio_export(GPIO_A_IO1);
	//			gpio_export(GPIO_A_IO2);
	//			gpio_export(GPIO_A_IO3);
	//			gpio_export(GPIO_A_IO4);*/
	//			gpio_export(LINE_IN_AUD_SEL0);
	//			gpio_export(LINE_IN_AUD_SEL1);
	//			gpio_export(LINE_IN_AUD_SEL2);

	//			gpio_export(LINE_OUT_AUD_SEL0);
	//			gpio_export(LINE_OUT_AUD_SEL1);
	//			gpio_export(LINE_OUT_AUD_SEL2);


	//		//Set direction
	//			gpio_set_dir(GPIO_A_PTT, OUT);
	//			gpio_set_dir(GPIO_A_CD, IN);
	//			gpio_set_dir(LINE_IN_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL2, OUT);

	//			gpio_set_dir(LINE_OUT_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL2, OUT);


	//		//Set edge
	//			gpio_set_edge(GPIO_A_CD, edge_mode);
	//			//CD_fd = gpio_fd_open(GPIO_A_CD);

	//		//Set value
	//			gpio_set_value(GPIO_A_PTT, HIGH_LEVEL);//default out high level
	//			
	//			//Select channel:1(portA:lineIN)
	//			gpio_set_value(LINE_IN_AUD_SEL0, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_IN_AUD_SEL1, LOW_LEVEL);//0
	//			gpio_set_value(LINE_IN_AUD_SEL2, LOW_LEVEL);//0
	//			fprintf(stderr, "Aport_okay\n");

	//			//Select channel:1(portA:lineOUT)
	//			gpio_set_value(LINE_OUT_AUD_SEL0, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_OUT_AUD_SEL1, LOW_LEVEL);//0
	//			gpio_set_value(LINE_OUT_AUD_SEL2, LOW_LEVEL);//0

	//		break;
	//	case 1://portB

	//		fprintf(stderr,"config port B \n");

	//		gpio_cd_pin = GPIO_B_CD;
	//		gpio_ptt_pin = GPIO_B_PTT;
	//		//Export GPIO
	//			gpio_export(GPIO_B_PTT);
	//			gpio_export(GPIO_B_CD);
	//			/*gpio_export(GPIO_A_IO1);
	//			gpio_export(GPIO_A_IO2);
	//			gpio_export(GPIO_A_IO3);
	//			gpio_export(GPIO_A_IO4);*/
	//			gpio_export(LINE_IN_AUD_SEL0);
	//			gpio_export(LINE_IN_AUD_SEL1);
	//			gpio_export(LINE_IN_AUD_SEL2);

	//			gpio_export(LINE_OUT_AUD_SEL0);
	//			gpio_export(LINE_OUT_AUD_SEL1);
	//			gpio_export(LINE_OUT_AUD_SEL2);


	//		//Set direction
	//			gpio_set_dir(GPIO_B_PTT, OUT);
	//			gpio_set_dir(GPIO_B_CD, IN);
	//			gpio_set_dir(LINE_IN_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL2, OUT);

	//			gpio_set_dir(LINE_OUT_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL2, OUT);


	//		//Set edge
	//			gpio_set_edge(GPIO_B_CD, edge_mode);
	//			//CD_fd = gpio_fd_open(GPIO_B_CD);

	//		//Set value
	//			gpio_set_value(GPIO_B_PTT, HIGH_LEVEL);//default out high level

	//			//Select channel:2(portB:lineIN)
	//			gpio_set_value(LINE_IN_AUD_SEL0, LOW_LEVEL);//0
	//			gpio_set_value(LINE_IN_AUD_SEL1, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_IN_AUD_SEL2, LOW_LEVEL);//0

	//			//Select channel:2(portA:lineOUT)
	//			gpio_set_value(LINE_OUT_AUD_SEL0, LOW_LEVEL);//0
	//			gpio_set_value(LINE_OUT_AUD_SEL1, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_OUT_AUD_SEL2, LOW_LEVEL);//0


	//		break;
	//	case 2://portA-RX, portB-TX

	//		fprintf(stderr,"config port A-RX, B-TX\n");
	//		gpio_cd_pin = GPIO_A_CD;
	//		gpio_ptt_pin = GPIO_B_PTT;
	//		//Export GPIO;
	//			gpio_export(GPIO_A_CD);
	//			gpio_export(GPIO_B_PTT);

	//			gpio_export(LINE_IN_AUD_SEL0);
	//			gpio_export(LINE_IN_AUD_SEL1);
	//			gpio_export(LINE_IN_AUD_SEL2);

	//			gpio_export(LINE_OUT_AUD_SEL0);
	//			gpio_export(LINE_OUT_AUD_SEL1);
	//			gpio_export(LINE_OUT_AUD_SEL2);


	//		//Set direction
	//			gpio_set_dir(GPIO_B_PTT, OUT);
	//			gpio_set_dir(GPIO_A_CD, IN);
	//			gpio_set_dir(LINE_IN_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL2, OUT);

	//			gpio_set_dir(LINE_OUT_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL2, OUT);


	//		//Set edge
	//			gpio_set_edge(GPIO_A_CD, edge_mode);
	//			//CD_fd = gpio_fd_open(GPIO_A_CD);

	//		//Set value
	//			gpio_set_value(GPIO_B_PTT, HIGH_LEVEL);//default out high level:1

	//			//Select channel:1(portA:lineIN)
	//			gpio_set_value(LINE_IN_AUD_SEL0, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_IN_AUD_SEL1, LOW_LEVEL);//0
	//			gpio_set_value(LINE_IN_AUD_SEL2, LOW_LEVEL);//0

	//			//Select channel:2(portB:lineOUT)
	//			gpio_set_value(LINE_OUT_AUD_SEL0, LOW_LEVEL);//0
	//			gpio_set_value(LINE_OUT_AUD_SEL1, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_OUT_AUD_SEL2, LOW_LEVEL);//0



	//		break;
	//	case 3://portA-TX, portB-RX
	//		fprintf(stderr,"config port B-RX, A-TX\n");
	//		gpio_cd_pin = GPIO_B_CD;
	//		gpio_ptt_pin = GPIO_A_PTT;
	//		//Export GPIO;
	//			gpio_export(GPIO_B_CD);
	//			gpio_export(GPIO_A_PTT);

	//			gpio_export(LINE_IN_AUD_SEL0);
	//			gpio_export(LINE_IN_AUD_SEL1);
	//			gpio_export(LINE_IN_AUD_SEL2);

	//			gpio_export(LINE_OUT_AUD_SEL0);
	//			gpio_export(LINE_OUT_AUD_SEL1);
	//			gpio_export(LINE_OUT_AUD_SEL2);


	//		//Set direction
	//			gpio_set_dir(GPIO_A_PTT, OUT);
	//			gpio_set_dir(GPIO_B_CD, IN);
	//			gpio_set_dir(LINE_IN_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_IN_AUD_SEL2, OUT);

	//			gpio_set_dir(LINE_OUT_AUD_SEL0, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL1, OUT);
	//			gpio_set_dir(LINE_OUT_AUD_SEL2, OUT);


	//		//Set edge
	//			gpio_set_edge(GPIO_B_CD, edge_mode);
	//			//CD_fd = gpio_fd_open(GPIO_B_CD);

	//		//Set value
	//			gpio_set_value(GPIO_A_PTT, HIGH_LEVEL);//default out high level:1

	//			//Select channel:2(portB:lineIN)
	//			gpio_set_value(LINE_IN_AUD_SEL0, LOW_LEVEL);//0
	//			gpio_set_value(LINE_IN_AUD_SEL1, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_IN_AUD_SEL2, LOW_LEVEL);//0

	//			//Select channel:1(portA:lineOUT)
	//			gpio_set_value(LINE_OUT_AUD_SEL0, HIGH_LEVEL);//1
	//			gpio_set_value(LINE_OUT_AUD_SEL1, LOW_LEVEL);//0
	//			gpio_set_value(LINE_OUT_AUD_SEL2, LOW_LEVEL);//0

	//		break;

	//	default:
	//		break;

	//}

}

uint8_t GPIO_App::port_b_ptt_onoff(uint8_t action)
{
	if (gpio_config_flag != 1){
		//fprintf(stderr, "gpio is not configed!!!\n");
		syslog(LOG_LOCAL7 | LOG_DEBUG, "gpio is not configed!!!\n");
		exit(-1);
	}


	switch (action){

	case 0://PTT-off

		//fprintf(stderr, "B_ptt  disabled...\n");
		gpio_set_value(GPIO_B_PTT, HIGH_LEVEL);//disable B_ptt out 

		break;

	case 1://PTT-on

		gpio_set_value(GPIO_B_PTT, LOW_LEVEL);//enable B_ptt out
		//fprintf(stderr, "B_ptt  enable...\n");

		break;

	default:
		break;


	}

	

}



uint8_t GPIO_App::ptt_onoff(uint8_t action)
{
	
	if (gpio_config_flag != 1){
		//fprintf(stderr,"gpio is not configed!!!\n");
		syslog(LOG_LOCAL7 | LOG_DEBUG, "gpio is not configed!!!\n");
		exit(-1);
	}
		

	//fprintf(stderr,"ptt  enable...\n");
	//0-OFF; 1-ON


	switch (action){

	case 0://PTT-off

		//fprintf(stderr,"ptt  disabled...\n");
		syslog(LOG_LOCAL7 | LOG_DEBUG, "ptt  disabled...\n");
		gpio_set_value(gpio_ptt_pin, HIGH_LEVEL);//disable ptt out 

		break;

	case 1://PTT-on

		gpio_set_value(gpio_ptt_pin, LOW_LEVEL);//enable ptt out
		//fprintf(stderr,"ptt  enable...\n");
		syslog(LOG_LOCAL7 | LOG_DEBUG, "ptt  enable...\n");

		break;

	default:
		break;


	}



}


uint8_t GPIO_App::get_cd_current_value()
{
	
	if (gpio_config_flag != 1){
		//fprintf(stderr,"gpio is not configed!!!\n");
		syslog(LOG_LOCAL7 | LOG_DEBUG, "gpio is not configed!!!\n");
		exit(-1);
	}

	//fprintf(stderr,"get gpio cd value...\n");
	unsigned int value = 0;
	
	gpio_get_value(gpio_cd_pin, &value);//get value for CD  

	return(value);

}

