
#include "contiki.h"
#include "sys/process.h"
#include <stdio.h> /* For printf() */

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(hello_world_process, ev, data)
{
	PROCESS_BEGIN();
	serial_shell_init();
	shell_rime_ping_init();
	printf("Hello, world\n");
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
	printf("event received : %lu \n",(unsigned long)data);

	PROCESS_END();
}
