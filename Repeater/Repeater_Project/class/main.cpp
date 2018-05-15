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
		exit_flag = signo;
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
			my_repeater->Stop();
		}
		else
		{
			usleep(200 * 1000);
		}

	}

	if (my_repeater != NULL)delete my_repeater;

	return 0;
	
}


