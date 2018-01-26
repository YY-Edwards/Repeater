/*
 * AudioQueue.c
 *
 * Created: 2016/12/19
 * Author: EDWARDS
 */ 

#include "AudioQueue.h"



//ʱ��a��ֵ����ms���� 
void timeraddMS(struct timeval *a, unsigned int ms)
{
	a->tv_usec += ms * 1000;
	if (a->tv_usec >= 1000000)
	{
		a->tv_sec += a->tv_usec / 1000000;
		a->tv_usec %= 1000000;
	}
}

AudioQueue::AudioQueue()
{
	
	pthread_mutex_init(&m_hLocker, NULL);
	sem_init(&m_hSemaphore, 0, 0);
	for (int i = 0; i < FIFODEEP; i++){
		memset(&(fifobuff[i].data), 0x00, 512);
		memset(&(fifobuff[i].len), 0x00, sizeof(uint8_t));
		//memset(&fifobuff[i][0], 0x00, 512);
	}
	//int err = 0 ;
	//for (int i = 0; i < 20; i++){
	//	memset(&fifobuff[i][0], 0x00, 320);
	//}
	fifo_counter = 0;

	//fprintf(stderr,"init queue mutex, sem...\n");
	
}
AudioQueue::~AudioQueue()
{
	//fprintf(stderr, "delete AudioQueue err1\n");
	//std::list<char *>::iterator i = m_list.begin();
	//m_list.erase(i);
	pthread_mutex_destroy(&m_hLocker);
	sem_destroy(&m_hSemaphore);

	fprintf(stderr,"delete class AudioQueue\n");
	
}

void AudioQueue::ClearQueue()
{
	int err = 0;
	pthread_mutex_lock(&m_hLocker);//�ȴ�����������
	m_list.clear();
	pthread_mutex_unlock(&m_hLocker);//�ȴ�����������
	fprintf(stderr, "clear Queue okay\n");

}


bool  AudioQueue::QueueIsEmpty()
{
	return(m_list.empty());
}
bool AudioQueue::PushToQueue(void *packet, int len)
{
	int err = 0;
	if (m_list.size() >= FIFODEEP)return false;//fifo full

	pthread_mutex_lock(&m_hLocker);//�ȴ�����������

	memcpy(&fifobuff[fifo_counter].data, packet, len);
	fifobuff[fifo_counter].len = len;
	m_list.push_back(&fifobuff[fifo_counter]);//��һ���ṹ�����ݵ�ַ��������β
	fifo_counter++;
	if (fifo_counter > 19){
		fifo_counter = 0;
	}

	pthread_mutex_unlock(&m_hLocker);//����������

	sem_post(&m_hSemaphore);

	//memcpy(&fifobuff[fifo_counter][0], packet, len);
	//pthread_mutex_lock(&m_hLocker);//�ȴ�����������
	//m_list.push_back(&fifobuff[fifo_counter][0]);//��һ�����ݰ���ַ��������β
	//pthread_mutex_unlock(&m_hLocker);//����������
	//fifo_counter++;
	//if (fifo_counter > 19){
	//	fifo_counter = 0;
	//	//fprintf(stderr, "fifo_counter is over\n");
	//}
	return true;

}

int AudioQueue::TakeFromQueue(void *packet, int& len, int waitTime)
{
	fifoqueue_t *sBuffer = NULL;
	int ret =0;
	struct timeval now;
	struct timespec outtime;

	gettimeofday(&now, NULL);
	timeraddMS(&now, waitTime);//ms����
	outtime.tv_sec = now.tv_sec;
	outtime.tv_nsec = now.tv_usec * 1000;
	while ((ret = sem_timedwait(&m_hSemaphore, &outtime) == -1) && errno == EINTR)
		continue;

	if (ret < 0)
	{
		if (errno == ETIMEDOUT)
		{
			ret = 1;
			//timeout
		}
		else
		{
			ret = -1;
			//failed
		}
		return ret;
	}
	else
	{

		if (!m_list.empty())
		{
			pthread_mutex_lock(&m_hLocker);//�ȴ�����������

			sBuffer = m_list.front();//���������һ���ṹ�����ݰ���ַ��sbuffer
			m_list.pop_front();//��ɾ�������һ�����ݰ���ַ
			memcpy(packet, sBuffer->data, sBuffer->len);
			len = sBuffer->len;

			pthread_mutex_unlock(&m_hLocker);//����������
			return ret;
		}
		else
		{
			return -2;//no happen but must check

		}
	}

}


//unsigned int AudioQueue::TakeFromQueue(void *packet, int& len)
//{
//
//	char* sBuffer;
//
//	if (!m_list.empty())
//	{
//			sBuffer = m_list.front();//���������һ�����ݰ���ַ��sbuffer
//
//			pthread_mutex_lock(&m_hLocker);//�ȴ�����������
//			m_list.pop_front();//��ɾ�������һ�����ݰ���ַ
//			pthread_mutex_unlock(&m_hLocker);//����������
//
//			memcpy(packet, sBuffer, 320);
//			len = 320;
//
//	}
//	else
//	{
//		return -1;
//
//	}
//	return 0;
//
//
//}
//
//unsigned int AudioQueue::TakeFromQueueForSpeex(void *packet, int& len)
//{
//	char* sBuffer;
//
//
//	if (!m_list.empty())
//	{
//		sBuffer = m_list.front();//���������һ�����ݰ���ַ��sbuffer
//
//		pthread_mutex_lock(&m_hLocker);//�ȴ�����������
//		m_list.pop_front();//��ɾ�������һ�����ݰ���ַ
//		pthread_mutex_unlock(&m_hLocker);//����������
//
//		memcpy(packet, sBuffer, 20);
//		len = 20;
//
//	}
//	else
//	{
//		return -1;
//
//	}
//	return 0;
//
//
//}



