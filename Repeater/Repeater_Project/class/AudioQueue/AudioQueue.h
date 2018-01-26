/*
 * AudioQueue.h
 *
 * Created: 2016/12/16
 * Author: EDWARDS
 */ 

#ifndef AudioQueue_h_
#define AudioQueue_h_
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h> 
#include <list>
#include <stdint.h>
#include <sys/time.h>

class RepeaterAudioPacket
{
	
public:

	RepeaterAudioPacket() : len(0){}
	RepeaterAudioPacket(char *packet, int len)
	{
		//memcpy_s(buffer, sizeof(buffer), packet, len);
		memcpy(buffer, packet, len);
		this->len = len;
	}
public:

	char buffer[512];
	int  len;
	
};

typedef RepeaterAudioPacket*	P_RepeaterAudioBuffer;


#define FIFODEEP  40
#pragma pack(push, 1)
typedef struct{
	char		data[320];
	uint8_t		len;

}fifoqueue_t;

#pragma pack(pop)


//时间a的值增加ms毫秒 
void timeraddMS(struct timeval *a, unsigned int ms);

class AudioQueue 
{
	
	public:
		AudioQueue();
		~AudioQueue();

	public:
		bool  			PushToQueue(void *packet, int len);
		 int 			TakeFromQueue(void *packet, int& len, int waitTime);
		 //int 			TakeFromQueueForSpeex(void *packet, int& len, int waitTime);
		//unsigned int 	TakeFromQueue(void *packet, int& len);
		//unsigned int 	TakeFromQueueForSpeex(void *packet, int& len);
		void			ClearQueue();
		bool 			QueueIsEmpty();

	private:
		sem_t								m_hSemaphore;
		pthread_mutex_t						m_hLocker;//init mutex
		//std::list<P_RepeaterAudioBuffer>  	m_list;
		//std::list<char *>  	m_list;
		std::list<fifoqueue_t *>  	m_list;
		fifoqueue_t fifobuff[FIFODEEP];
		//char fifobuff[20][320];
		unsigned int fifo_counter;

	
};



#endif