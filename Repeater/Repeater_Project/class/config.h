#ifndef _GLOBAL_CONFIG_H
#define _GLOBAL_CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>  
#include <unistd.h>
#include <signal.h> 
#include <poll.h>
#include <signal.h> 
#include <stdint.h>
#include <string.h>
#include <errno.h>



#define SELECT_TIMEOUT							1       //selectµÄtimeout seconds

#ifdef WIN32
typedef  CRITICAL_SECTION GOCRITICAL_SECTION;
typedef  HANDLE GOMUTEX_T;
typedef  HANDLE GOSEM_T;
typedef  HANDLE GOCOND_T;
typedef  HANDLE GOTHREAD_T;
typedef  SOCKET HSocket;
//#define GOTHREADCREATE(x, y, z, q, w, e) _beginthreadex((x), (y), (z), (q), (w), (e))


#else
//#define GOTHREADCREATE(x, y, z, q, w, e) pthread_create((x), (y), (z), (q))
typedef  int GOTHREAD_T;
typedef  pthread_mutex_t	GOCRITICAL_SECTION;
typedef  pthread_mutex_t	GOMUTEX_T;
typedef  sem_t				GOSEM_T;
typedef  pthread_cond_t		GOCOND_T;
typedef int HSocket;
#define SOCKET_ERROR  (-1)  
#define INVALID_SOCKET  0  

#endif


#endif // !_GLOBAL_CONFIG_H
