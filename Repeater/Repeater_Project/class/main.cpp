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
	signal(signo, signal_handler);
	// Do not do the trick with ignoring SIGCHLD, cause not all OSes (e.g. QNX)
	// reap zombies if SIGCHLD is ignored. On QNX, for example, waitpid()
	// fails if SIGCHLD is ignored, making system() non-functional.
	if (signo == SIGCHLD) {
		do {} while (waitpid(-1, &signo, WNOHANG) > 0);
	}
	else// Reinstantiate signal handler
	{
		exit_flag = signo;
	}

}

int main(void)
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGCHLD, signal_handler);

	fprintf(stderr, "\n/***********************V2.1.0.1******************************/\n");
	fprintf(stderr, "\n/****************Repeater main() is running*******************/\n");
	//The 4-byte APP Firmware Version number uses a Major Number to track the major changes,
	// Minor Number to track minor changes and Product ID Number to differentiate the product line.
	/*Product ID Number:
	0x0001   DRS
	0x0002
	0x0003	...
	...		...



	*/


	MyRepeater *my_repeater = new MyRepeater;

	my_repeater->Start();
	for (;;)
	{
		if (exit_flag != 0)
		{
			my_repeater->Stop();
			break;
		}
		else
		{
			usleep(200 * 1000);
		}

	}

	if (my_repeater != NULL)delete my_repeater;

	return 0;
	
}


