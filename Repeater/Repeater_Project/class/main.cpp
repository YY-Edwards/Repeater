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

	delete my_repeater;

	exit(0);

}

int main(void)
{

	fprintf(stderr,"\n/***********************V1.0.4.3******************************/\n");

	fprintf(stderr,"\n/****************Repeater main() is running*******************/\n");

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);

	my_repeater->Start();

	delete my_repeater;

	return 0;
	
}


