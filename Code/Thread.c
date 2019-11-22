#include "Thread.h"
#include "Init.h"
#include "Scheduler.h"
#include "MyCode.h"
#include <stdlib.h>		//malloc
#include <stdio.h>

//thread function
int 	thread_create(thread_t *thread, thread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
	int returnValue;
	//Wrapperfunction 
	WrapperArg* wrapperArg=(WrapperArg*)malloc(sizeof(WrapperArg));
	wrapperArg->funcPtr = (*start_routine);
	wrapperArg->funcArg = arg;
	//signal handler set
	sigset_t sigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask,SIGUSR2);
	pthread_sigmask(SIG_BLOCK,&sigmask,NULL);

	if(returnValue=pthread_create(thread,attr,wrapperFunc,wrapperArg)==0)
	{
		//Allocate and init TCB
		Thread* child= (Thread*)malloc(sizeof(Thread));
		//child's TCB setting
		child->status=1;
		child->tid=*thread;
		child->bRunnable=0;
		child->pExitCode=NULL;
		child->parentTid=thread_self();
		pthread_mutex_init(&(child->readyMutex),NULL);
		//TCB move to ready Queue
		enqueueReadyQ(child);
		//send SIGUSR1 to child
		pthread_kill(child->tid,SIGUSR2);
		return returnValue;
	}
	else
	{
		perror("thread create error!");
		exit(1);
	}
}

int 	thread_join(thread_t thread, void **retval)
{
	//get this thread's TCB get
	Thread* children=getThread(thread);
	Thread* self;
	if((self=getThread(thread_self()))==NULL)
		self=CThread;
	//set thread status to sleep
	self->status=2;
	children->parentTid=thread_self();
	//remove this thread's TCB from ready queue and insert waiting queue
	CThread=NULL;
	enqueueWaitQ(self);
	//call thread wait handler()
	if(children->pExitCode==NULL)		//not exit child thread
	{
		self->status=3;
		self->bRunnable=0;
		thread_wait_handler(0);
	}
	//put pExitCode into retval
	*retval=children->pExitCode;
	//remove child's TCB from ready queue; deallocate child's TCB
	dequeueReadyQ(thread);
	free(children);
	return 0;
}

int 	thread_suspend(thread_t tid)		//from ready queue to wait queue
{
	//dequeue from Ready queue
	Thread* target=dequeueReadyQ(tid);
	if(target==NULL)
		return -1;
	//enqueue to waiting Queue	
	enqueueWaitQ(target);
	//TCB set
	target->status=2;
	return 0;
}


int	thread_resume(thread_t tid)		//from wait queue to ready queue
{
	//dequeue from Wait queue
	Thread* target=dequeueWaitQ(tid);
	if(target==NULL)
		return -1;
	//enqueue to Ready Queue
	enqueueReadyQ(target);
	//TCB set
	target->status=1;
	target->bRunnable=0;
	return 0;
}

int thread_exit(void* retval)
{
	Thread* self=CThread;			//running Thread
	Thread* parent=getThread(self->parentTid);
	if(parent==NULL)			//parent already deleted
	{
		self->pExitCode=retval;
		return 0;
	}
	else if(parent->status==3){		//parent is joining
		thread_resume(parent->tid);
	}
	self->pExitCode=retval;
	return 0;
}

thread_t	thread_self()
{
	return pthread_self();
}
