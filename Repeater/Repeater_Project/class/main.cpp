/*
* main.c
*
* Created: 2016/12/20
* Author: EDWARDS
*/
#include "MyRepeater.h"
MyRepeater *my_repeater = new MyRepeater;

void signal_handler(int signo)
{
	fprintf(stderr, "\nHave caught sig N.O. %d\n", signo);

	signal(signo, SIG_DFL);
	if (my_repeater != NULL){

		delete my_repeater;
		my_repeater = NULL;
	}

	exit(0);

}

int main(void)
{

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);

	my_repeater->Start();

	while (1)
	{	
		sleep(10);
	}

	if (my_repeater != NULL){

		delete my_repeater;
		my_repeater = NULL;
	}

	return 0;
	
}


