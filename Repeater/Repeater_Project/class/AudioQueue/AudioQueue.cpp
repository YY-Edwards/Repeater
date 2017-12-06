/*
 * AudioQueue.c
 *
 * Created: 2016/12/19
 * Author: EDWARDS
 */ 

#include "AudioQueue.h"

AudioQueue::AudioQueue()
{
	
	pthread_mutex_init(&m_hLocker, NULL);
	int err = 0 ;
	for (int i = 0; i < 20; i++){
		memset(&fifobuff[i][0], 0x00, 320);
	}
	fifo_counter = 0;

	//fprintf(stderr,"init queue mutex, sem...\n");
	
}
AudioQueue::~AudioQueue()
{
	//fprintf(stderr, "delete AudioQueue err1\n");
	//std::list<char *>::iterator i = m_list.begin();
	//m_list.erase(i);
	pthread_mutex_destroy(&m_hLocker);

	//fprintf(stderr,"delete class AudioQueue\n");
	
}

void AudioQueue::ClearQueue()
{
	int err = 0;
	pthread_mutex_lock(&m_hLocker);//等待互斥句柄触发
	m_list.clear();
	pthread_mutex_unlock(&m_hLocker);//等待互斥句柄触发
	//fprintf(stderr, "clear Queue okay\n");

}


bool  AudioQueue::QueueIsEmpty()
{
	return(m_list.empty());
}
bool AudioQueue::PushToQueue(void *packet, int len)
{
	int err = 0;
	static int counter = 0;

	memcpy(&fifobuff[fifo_counter][0], packet, len);
	pthread_mutex_lock(&m_hLocker);//等待互斥句柄触发
	m_list.push_back(&fifobuff[fifo_counter][0]);//将一个数据包地址插入链表尾
	pthread_mutex_unlock(&m_hLocker);//触发互斥句柄
	fifo_counter++;
	if (fifo_counter > 19){
		fifo_counter = 0;
		//fprintf(stderr, "fifo_counter is over\n");
	}
	return true;

}

unsigned int AudioQueue::TakeFromQueue(void *packet, int& len)
{

	char* sBuffer;

	if (!m_list.empty())
	{
			sBuffer = m_list.front();//返回链表第一个数据包地址给sbuffer

			pthread_mutex_lock(&m_hLocker);//等待互斥句柄触发
			m_list.pop_front();//并删除链表第一个数据包地址
			pthread_mutex_unlock(&m_hLocker);//触发互斥句柄

			memcpy(packet, sBuffer, 320);
			len = 320;

	}
	else
	{
		return -1;

	}
	return 0;


}

unsigned int AudioQueue::TakeFromQueueForSpeex(void *packet, int& len)
{
	char* sBuffer;


	if (!m_list.empty())
	{
		sBuffer = m_list.front();//返回链表第一个数据包地址给sbuffer

		pthread_mutex_lock(&m_hLocker);//等待互斥句柄触发
		m_list.pop_front();//并删除链表第一个数据包地址
		pthread_mutex_unlock(&m_hLocker);//触发互斥句柄

		memcpy(packet, sBuffer, 20);
		len = 20;

	}
	else
	{
		return -1;

	}
	return 0;


}



