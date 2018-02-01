/*
* main.c
*
* Created: 2016/12/20
* Author: EDWARDS
*/
#include "MyRepeater.h"

int exit_flag = 0;

void signal_handler(int signo)
{
	signal(signo, signal_handler);
	fprintf(stderr, "\nHave caught sig N.O. %d\n", signo);
	signal(signo, SIG_DFL);

	exit_flag = signo;

	//if (my_repeater != NULL){
	//	delete my_repeater;
	//	my_repeater = NULL;
	//}
	//exit(0);

}

int main(void)
{

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);

	MyRepeater *my_repeater = new MyRepeater;

	my_repeater->Start();

	while (exit_flag == 0)
	{	
		usleep(500*1000);
	}

	if (my_repeater != NULL){

		my_repeater->Stop();

		delete my_repeater;
		my_repeater = NULL;
	}

	return 0;
	
}


