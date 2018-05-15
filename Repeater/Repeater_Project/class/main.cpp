/*
* main.c
*
* Created: 2016/12/20
* Author: EDWARDS
* Modify: 2018/05/15
*/
#include "MyRepeater.h"

int exit_flag = 0;

void signal_handler(int signo)
{
	fprintf(stderr, "\nHave caught sig N.O. %d\n", signo);

	// Reinstantiate signal handler
	signal(signo, signal_handler);
	{
		exit_flag = sig_num;
	}

}

int main(void)
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);

	MyRepeater *my_repeater = new MyRepeater;

	my_repeater->Start();
	for (;;)
	{
		if (exit_flag != 0)
		{
			delete my_repeater;
		}
		else
		{
			usleep(200 * 1000);
		}

	}

	return 0;
	
}


