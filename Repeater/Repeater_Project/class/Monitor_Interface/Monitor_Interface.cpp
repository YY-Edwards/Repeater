/*
* Monitor_Interface.c
*
* Created: 2016/12/19
* Author: EDWARDS
*/

#include "Monitor_Interface.h"
#include "MFile.h"


//slave:00:01:03:02:aa:0c
Monitor_Interface::Monitor_Interface()
				:isMaster(false)
				, master_ip_str("192.168.2.131")
				, userport(1)
				, config_hw_repeater_flag(0)
				, p_file(NULL)
{
	p_file = new MFile;
	strcpy(base_ip_str,"192.168.2.133");
	strcpy(MAC,"00:01:03:02:aa:0c");

	strcpy(gateway_ip, "192.168.2.1");
	strcpy(subnetmask, "20.255.11.0");
	strcpy(switch_ip, "19.18.0.1");

	config_hw_repeater();

}

Monitor_Interface::~Monitor_Interface()
{
	if (p_file != NULL)delete p_file;
	fprintf(stderr,"delete class Monitor_Interface\n");


}



int Monitor_Interface::pox_system(const char *cmd_line)
{
	//调整的原因：调用system函数时，其中会调用fork()函数，并用子进程去执行命令，父进程等待回收子进程。
	//而初始化的时候，将SIGCHLD信号注册为回调方式处理退出的子线程，此时可能出现进入回调回收了子线程，
	//然后再回到system里的父线程，可能因此不能回收子线程，而出现错误。而命令在子线程里有可能是执行成功的。
	int ret = 0;
	//fprintf(stderr, "pox_system:%s", cmd_line);
	sighandler_t old_handler;
	old_handler = signal(SIGCHLD, SIG_DFL);//返回上一次的行为
	ret = system(cmd_line);
	//（1） - 1 != status
	//（2）WIFEXITED(status)为真
	//（3）0 == WEXITSTATUS(status)
	if (ret == -1)
	{
		fprintf(stderr, "system error:%d\n", errno);
	}
	else
	{
		//fprintf(stderr, "exit ret value:0x%x\n", ret);
		if (WIFEXITED(ret))
		{
			if (0 == WEXITSTATUS(ret))
			{
				//fprintf(stderr, "run shell script successfully.\n");
			}
			else
			{
				fprintf(stderr, "run shell script fail, script exit code: %d\n", WEXITSTATUS(ret));
			}
		}
		else
		{
			fprintf(stderr, "exit status = [%d]\n", WEXITSTATUS(ret));
		}
	}

	signal(SIGCHLD, old_handler);//恢复之前的行为

}


void Monitor_Interface::config_hw_repeater()
{
	char tmp[100];
	bzero(tmp, 100);

	//fprintf(stderr,"Read config file...\n");
	read_config_file();
	fprintf(stderr,"config Reapeater hw...\n");

	sprintf(tmp,"ifconfig eth0 hw ether %s", MAC);//config MAC
	pox_system(tmp);
	bzero(tmp, 100);

	sprintf(tmp,"ifconfig eth0 %s", base_ip_str);//config eth0 IP
	pox_system(tmp);
	bzero(tmp, 100);


	sprintf(tmp, "ifconfig");//print out the information
	pox_system(tmp);
	bzero(tmp, 100);

	sprintf(tmp, "route add default gw %s netmask %s dev eth0", gateway_ip, subnetmask);//config default gateway
	pox_system(tmp);
	bzero(tmp, 100);

	//add multicast route
	/*sprintf(tmp, "route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0 ");
	pox_system(tmp);
	bzero(tmp, 100);*/

	//Recording sound using ALSA from Line IN
	sprintf(tmp, "amixer -c 0 cset numid=7 1");
	pox_system(tmp);
	bzero(tmp, 100);

	//capture volume:95%
	sprintf(tmp, "amixer -c 0 cset numid=2 14");
	pox_system(tmp);
	bzero(tmp, 100);

	//playback volume:95%
	sprintf(tmp, "amixer -c 0 cset numid=4 180");
	pox_system(tmp);
	bzero(tmp, 100);


}


void Monitor_Interface::led_control()
{

	char tmp[50];
	static unsigned char flag;
	static unsigned char i = 1;

	bzero(tmp, 50);

	//while (1)
	{
		flag = i & 0x01;
		sprintf(tmp, "echo %d > /sys/class/leds/d19/brightness", flag);
		pox_system(tmp);
		usleep(30000);
		sprintf(tmp, "echo 00 > /sys/class/leds/d19/brightness", flag);
		pox_system(tmp);
		usleep(32000);
		sprintf(tmp, "echo %d > /sys/class/leds/d19/brightness", flag);
		pox_system(tmp);
		usleep(27000);
		sprintf(tmp, "echo 00 > /sys/class/leds/d19/brightness", flag);
		pox_system(tmp);

		//usleep(200000);
		i++;

		if (i == 100)i = 1;

	}

}

void Monitor_Interface::read_config_file()
{
	
	if (config_hw_repeater_flag == 0)
		config_hw_repeater_flag = 1;

	int err = 0;
	char data[128];
	bzero(data, sizeof(data));
	int fp;

	//fprintf(stderr, "config-file open successfully \n");
	//fprintf(stderr,"1...\n");
	fp = p_file->Start();//file_fp

/****Read configuration information and assign*****/
	//get ID
	//fprintf(stderr,"2...\n");
	err = p_file->GetConfig(DeviceType, data);
	if (err == 0){
		if (strcmp(data, "0") == 0){
			isMaster = false;
		}
		else
			isMaster = true;
	}
	else 
		fprintf(stderr,"p_file->GetConfig:DeviceType err...\n");

	bzero(data, sizeof(data));

	//get userport
	//fprintf(stderr,"3...\n");
	//err = p_file->GetConfig(DeviceMode, data);
	////fprintf(stderr,"DeviceMode :%s\n", data);
	//int temp = 0;
	//if (strcmp(data, "0") == 0){
	//	temp = 0;
	//}
	//else
	//	temp = 1;

	////fprintf(stderr,"temp :%d\n", temp);

	//if (err == 0){
	//	switch (temp)
	//	{
	//		case SignleMode:
	//				bzero(data, sizeof(data));
	//				err = p_file->GetConfig(DeviceInterfaceNumber, data);
	//				if (strcmp(data, "0") == 0){
	//					userport = 0;
	//				}
	//				else{//Interface_B
	//					userport = 1;
	//				}

	//			break;

	//		case DoubleMode:
	//				bzero(data, sizeof(data));
	//				err = p_file->GetConfig(DeviceInterfaceNumber, data);
	//				if (strcmp(data, "1") == 0){
	//					//portB - TX, portA - RX;
	//					userport = 2;
	//				}
	//				else{
	//					userport = 3;
	//				}

	//			break;

	//		default:
	//			break;
	//	}
	//	
	//}
	//else
	//	fprintf(stderr,"p_file->GetConfig:DeviceMode  err...\n");

	//bzero(data, sizeof(data));


	//get base-ip
	//fprintf(stderr,"4...\n");
	err = p_file->GetConfig(DeviceIP, data);
	//fprintf(stderr,"DeviceIP ; %s\n", data);
	if (err == 0){
		//fprintf(stderr,"base-ip: %s\n", data);
		//base_ip_str = data;
		strcpy(base_ip_str, data);
		//memcpy(base_ip_str, data, strlen(data));
		//memcpy(base_ip_str, data, strlen(data));
	}
	else
		fprintf(stderr,"p_file->GetConfig:DeviceIP  err...\n");

	bzero(data, sizeof(data));

	//get master-ip
	//fprintf(stderr,"5...\n");
	err = p_file->GetConfig(MasterIP, data);
	//fprintf(stderr,"MasterIP ; %s\n", data);
	if (err == 0){
		master_ip_str = data;
	}
	else
		fprintf(stderr,"p_file->GetConfig:MasterIP  err...\n");

	bzero(data, sizeof(data));

	//get MAC
	//fprintf(stderr,"6...\n");
	err = p_file->GetConfig(DeviceMac, data);
	//fprintf(stderr,"DeviceMac ; %s\n", data);
	if (err == 0){
		strcpy(MAC, data);

		/*build mac*/
		int tempMac[6] = { 0 };
		sscanf(MAC,"%x-%x-%x-%x-%x-%x",&tempMac[0]
			, &tempMac[1]
			, &tempMac[2]
			, &tempMac[3]
			, &tempMac[4]
			, &tempMac[5]);
		sprintf(MAC,"%02x:%02x:%02x:%02x:%02x:%02x",tempMac[0]
			, tempMac[1]
			, tempMac[2]
			, tempMac[3]
			, tempMac[4]
			, tempMac[5]);
	}
	else
		fprintf(stderr,"p_file->GetConfig:DeviceMac  err...\n");

	bzero(data, sizeof(data));


	//get IPv4 gateway-ip
	err = p_file->GetConfig(GatewayIP, data);
	if (err == 0){
		strcpy(gateway_ip, data);
	}
	else
		fprintf(stderr, "p_file->GetConfig:GatewayIP  err...\n");

	bzero(data, sizeof(data));

	//get IPv4 subnet-mask
	err = p_file->GetConfig(Subnetmask, data);
	if (err == 0){
		strcpy(subnetmask, data);
	}
	else
		fprintf(stderr, "p_file->GetConfig:Subnetmask  err...\n");

	bzero(data, sizeof(data));


	//get switch-ip
	err = p_file->GetConfig(SwitchIP, data);
	if (err == 0){
		strcpy(switch_ip, data);
	}
	else
		fprintf(stderr, "p_file->GetConfig:SwitchIP  err...\n");

	bzero(data, sizeof(data));



	//fprintf(stderr,"7...\n");
	close(fp);//close config-file
	//fprintf(stderr,"8...\n");
}

bool Monitor_Interface::write_map_file()
{

	if (config_hw_repeater_flag != 1){

		fprintf(stderr,"repeater is not configed");
		exit(-1);
	}
	FILE *fp = fopen("/opt/map", "wb+");
	if (fp == NULL)
		exit(-1);

	//fprintf(stderr, "map file open successfully \n");
	fseek(fp, 0, SEEK_SET);

	//err = fwrite(buffer, 1, 320, fp);

	fclose(fp);


}
