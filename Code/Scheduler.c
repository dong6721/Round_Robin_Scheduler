#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include "MyCode.h"
#include <unistd.h>
int	RunScheduler( void )
{
	CThread=NULL;
	Thread* NewThread;
	while(1)
	{
		sleep(TIMESLICE);
		if(ReadyQHead==NULL)
		{
			continue;
		}
		NewThread=ReadyQHead;
		__ContextSwitch(CThread,NewThread);
	}
}


void	__ContextSwitch(Thread* pCurThread, Thread* pNewThread)
{
	//stop run current Thread
	if(pCurThread!=NULL)				//not first run
	{
		pCurThread->bRunnable=0;
		pthread_kill(pCurThread->tid,SIGUSR1);
		//go to ready queue Tail
		enqueueReadyQ(pCurThread);
	}
	//New Thread dequeue from Ready Queue
	dequeueReadyQ(pNewThread->tid);
	CThread=pNewThread;
	//run NewThread
	__thread_wakeup(pNewThread);
}

