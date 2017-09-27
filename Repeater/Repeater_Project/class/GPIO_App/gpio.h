/*
 * gpio.h
 *
 * Created: 2016/12/19
 * Author: EDWARDS
 */ 
#ifndef gpio_h_
#define gpio_h_

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define GPIO_A_PTT 62 //LCD_DOTCLK(BANK1_PIN30):1*32+30=62
#define GPIO_A_CD 56 //LCD_RD_E(BANK1_PIN24)：1*32+24 = 56
#define GPIO_A_IO1 126 //LCD_RESET
#define GPIO_A_IO2 61 //LCD_HSYNC
#define GPIO_A_IO3 59 //LCD_CS
#define GPIO_A_IO4 62 //LCD_ENABLE

#define GPIO_RELAY 45 //LCD_D13

//#define LINE_IN_AUD_SEL0 43 //LCD_D11
//#define LINE_IN_AUD_SEL1 44 //LCD_D12
//#define LINE_IN_AUD_SEL2 45 //LCD_D13
 
#define GPIO_B_PTT 37 //LCD_D05
//#define GPIO_B_CD  38 //LCD_D06
#define GPIO_B_IO1 39 //LCD_D07
#define GPIO_B_IO2 40 //LCD_D08
#define GPIO_B_IO3 41 //LCD_D09
#define GPIO_B_IO4 42 //LCD_D10
//#define LINE_OUT_AUD_SEL0 46 //LCD_D14
//#define LINE_OUT_AUD_SEL1 47 //LCD_D15
//#define LINE_OUT_AUD_SEL2 48 //LCD_D16

 


#define MAX_BUF 60
#define POLL_TIMEOUT (200) /* 200 milliseconds */
#define OUT 1
#define IN 0
#define HIGH_LEVEL 1
#define LOW_LEVEL  0


int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio ,int out_flag);
int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_get_value(unsigned int gpio, unsigned int *value);
int gpio_set_edge(unsigned int gpio ,char *edge);
int gpio_fd_open(unsigned int gpio);
int gpio_fd_close(int fd) ;


#endif

