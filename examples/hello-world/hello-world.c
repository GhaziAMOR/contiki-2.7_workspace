
#include "contiki.h"
#include "sys/process.h"
#include <stdio.h> /* For printf() */
#include "shell.h"
#include "sys/etimer.h"
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
static struct etimer et ;
PROCESS_THREAD(hello_world_process, ev, data)
{

	char * ipAddress = "rime-ping 1.0";

	PROCESS_BEGIN();

	serial_shell_init();
	shell_ping_init();
	/*etimer_set(&et,CLOCK_SECOND*50);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	process_post(&shell_process,PROCESS_EVENT_CONTINUE,ipAddress);
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);*/
	//printf("event received : %lu \n",(unsigned long)data);

	PROCESS_END();
}
