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
	//fprintf(stderr, "\nHave caught sig N.O. %d\n", signo);
	syslog(LOG_LOCAL7 | LOG_INFO, "\nHave caught sig N.O. %d\n", signo);
	syslog(LOG_LOCAL7 | LOG_DEBUG, "\nHave caught sig N.O. %d\n", signo);

	signal(signo, SIG_DFL);

	delete my_repeater;

	exit(0);

}

int main(void)
{

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);

	syslog(LOG_LOCAL7 | LOG_DEBUG, "APP_Version:1.5.0.1 \n");
	syslog(LOG_LOCAL7 | LOG_DEBUG, "Repeater main() is running \n");
	syslog(LOG_NOTICE, "yoyo-info\n");
	syslog(LOG_DEBUG, "yoyo-debug\n");
	syslog(LOG_NOTICE | LOG_DEBUG, "yoyo-debug+info\n");
	//The 4-byte APP Firmware Version number uses a Major Number to track the major changes,
	// Minor Number to track minor changes and Product ID Number to differentiate the product line.
	/*Product ID Number:
	0x0001   DRS
	0x0002   
	0x0003	...
	...		...

	*/


	my_repeater->Start();

	delete my_repeater;

	return 0;
	
}


