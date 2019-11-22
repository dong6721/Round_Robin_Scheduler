#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"

#include <stdio.h>
#include <stdlib.h>

void Init(void)
{
	//Create ready queue and waiting queue
	ReadyQHead = ReadyQTail = NULL;
	WaitQHead = WaitQTail=NULL;
	
}


