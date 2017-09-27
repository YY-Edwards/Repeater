/*
 * gpio.cpp
 *
 * Created: 2016/12/19
 * Author: EDWARDS
 */ 
#include<stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include <poll.h>
#include "gpio.h"

/** 
 * brief: export the GPIO to user space
 * @Param: gpio: the GPIO number
 */
int gpio_export(unsigned int gpio)
{
	int fd ,len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export" ,O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
	len = snprintf(buf ,sizeof(buf) ,"%d" ,gpio);
	write(fd ,buf ,len);
	close(fd);
	return 0;
}

/** 
 * brief: unexport the GPIO to user space
 * @Param: gpio: the GPIO number
 */  
int gpio_unexport(unsigned int gpio)
{
	int fd ,len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport" ,O_WRONLY);
	if (fd < 0) {
		perror("gpio/unexport");
		return fd;
	}
	len = snprintf(buf ,sizeof(buf) ,"%d" ,gpio);
	write(fd ,buf ,len);
	close(fd);
	return 0;
}

/** 
 * brief: configure GPIO for input or output
 * @Param: gpio: the GPIO number
 * @Param: out_flag: the flag of output or input.It's value can be 1 or 0.  
 */ 
int gpio_set_dir(unsigned int gpio ,int out_flag)
{
	int fd ,len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
	fd = open(buf ,O_WRONLY);
	if (fd < 0) {
		perror(buf);
		return fd;
	}
	if (out_flag) 
		write(fd ,"out" ,4);
	else 
		write(fd ,"in" ,3);
	close(fd);
	return 0;
}

/** 
 * brief: Set the value of GPIO 
 * @Param: gpio: the GPIO number
 * @Param: value: the value of GPIO. Supports values of 0 and 1. 
 */  
int gpio_set_value(unsigned int gpio, unsigned int value)  
{  
    int fd, len;  
    char buf[MAX_BUF];  
   
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);      
    fd = open(buf, O_WRONLY);
	if (fd < 0) {  
        perror("gpio/set-value");  
        return fd;  
    }  
   
    if (value)  
        write(fd, "1", 2);  
    else  
        write(fd, "0", 2);  
   
    close(fd);  
    return 0;  
}

/** 
 * brief: get the value of GPIO
 * @Param: gpio: the GPIO number
 * @Param: value: pointer to the value of GPIO
 */
int gpio_get_value(unsigned int gpio, unsigned int *value)
{
	int fd, len;
	char ch;
	char buf[MAX_BUF];

	len = snprintf(buf ,sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value" ,gpio); 
	fd = open(buf ,O_RDONLY);
	if (fd < 0) {
		perror("gpio_get_value");
		return fd;
	}
	read(fd ,&ch ,1);
	if (ch == '1') 
		*value = 1;
	else if(ch == '0') 
			*value = 0;
	close(fd);
	return 0;
}

/** 
 * brief: set the edge that trigger interrupt
 * @Param: gpio: the GPIO number
 * @Param: edge:  edge that trigger interrupt
 */
int gpio_set_edge(unsigned int gpio ,char *edge)
{
	int fd ,len;
	char buf[MAX_BUF];

	len = snprintf(buf ,sizeof(buf) ,SYSFS_GPIO_DIR "/gpio%d/edge" ,gpio);
	fd = open(buf ,O_WRONLY);
	if (fd < 0) {
		perror("gpio_set_edge");
		return fd;
	}
	write(fd ,edge ,strlen(edge) + 1);
	close(fd);
	return 0;
}

/** 
 * brief: open gpio device and return the file descriptor
 * @Param: gpio: the GPIO number
 */  
int gpio_fd_open(unsigned int gpio)  
{  
    int fd, len;  
    char buf[MAX_BUF];  
  
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);  
   
    fd = open(buf, O_RDONLY | O_NONBLOCK );  
    if (fd < 0) {  
        perror("gpio/fd_open");  
    }  
    return fd;  
}  
  
/** 
 * brief: close gpio device 
 * @Param: fd: the file descriptor of gpio
 */
int gpio_fd_close(int fd)  
{  
    return close(fd);  
}  
  
// /**
 // * @brief: main function 
 // * @Param: argc: number of parameters
 // * @Param: argv: parameters list
 // */  
// int main(int argc, char **argv)  
// {  
    // struct pollfd *fdset;  
    // int nfds = 1;  
    // int gpio_fd, timeout, rc;  
    // char *buf[MAX_BUF];  
    // unsigned int gpio;  
    // int len;
    // char *cmd;
    // unsigned int value;
      
  	// fdset = (struct pollfd*)malloc(sizeof(struct pollfd));  
  
    // if (argc < 3) {  
        // fprintf(stderr,"Usage: %s <gpio-pin> <direction> [value]\n\n", argv[0]);        
        // exit(-1);  
    // }
    // cmd = argv[2];
    // gpio = atoi(argv[1]);
	// gpio_export(gpio);
	
    // if (strcmp(cmd, "interrupt") == 0) {
    	// fprintf(stderr,"\n****************************GPIO interrupt*****************************\n");
		  
		// gpio_set_dir(gpio, IN);  
		// gpio_set_edge(gpio, "rising");  
		// gpio_fd = gpio_fd_open(gpio);
		
		// /* GPIO_LED configure */
		// //gpio_export(GPIO_LED);  
		// //gpio_set_dir(GPIO_LED, OUT);
		
		// timeout = POLL_TIMEOUT;  
	   
		// while (1) {  
		    // memset((void*)fdset, 0, sizeof(fdset)); 
		    // fdset->fd = gpio_fd;  
		    // fdset->events = POLLPRI;  
	  
		    // rc = poll(fdset, nfds, timeout);        
	  
		    // if (rc < 0) {  
		        // fprintf(stderr,"\npoll() failed!\n");  
		        // return -1;  
		    // }  
		    
		    // if (rc == 0) {  
		        // fprintf(stderr,".");
		        // /* LED off */
		        // //gpio_set_value(GPIO_LED, 1);  
		    // }  
		          
		    // if (fdset->revents & POLLPRI) {  
		        // len = read(fdset->fd, buf, MAX_BUF);  
		        // fprintf(stderr,"\nGPIO %d interrupt occurred\n", gpio);
		        // /* when GPIO interrupt occurred, LED turn on */
		        // //gpio_set_value(GPIO_LED, 0);  
		    // }
		    // fflush(stdout);   
		// } 
		
		// gpio_fd_close(gpio_fd); 
		   	
    // } else if (strcmp(cmd, "out") == 0) {
    	
		// gpio_set_dir(gpio, OUT);
			
  		// if (argc = 4) {
  			// gpio_set_value(gpio, atoi(argv[3]));
  			// fprintf(stderr,"gpio%d is set to %d\n", gpio, atoi(argv[3]));	
  		// }
  	// } else if (strcmp(cmd, "in") == 0) {
  		
  		// gpio_set_dir(gpio, IN);
  		// fprintf(stderr,"\n");
  		
  		// while (1) {
  			// gpio_get_value(gpio, &value);
  			// fprintf(stderr,"\rvalue:%d", value);
  		// }
  		
  	// } else if (strcmp(cmd, "unexport") == 0) {
  		// gpio_unexport(gpio);	
  	// }
  	
  	// else {
  		// fprintf(stderr,"Usage: %s <gpio-pin> <direction> [value]\n\n", argv[0]);        
        // exit(-1);	
  	// }  	  	
     
    // return 0;  
// } 






















