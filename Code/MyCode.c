#include "MyCode.h"
#include "Thread.h"
#include "Init.h"
#include "Scheduler.h"
#include <stdio.h>
//input ready and wait queue
void enqueueReadyQ(Thread* target)
{
	if(ReadyQTail==NULL)
	{
		ReadyQTail=ReadyQHead=target;
		target->pNext=NULL;
		target->pPrev=NULL;
	}
	else
	{
		ReadyQTail->pNext=target;
		target->pPrev=ReadyQTail;
		ReadyQTail=target;
		target->pNext=NULL;
	}
}
void enqueueWaitQ(Thread* target)
{
	if(WaitQTail==NULL)
	{
		WaitQTail=WaitQHead=target;
		target->pNext=NULL;
		target->pPrev=NULL;
	}
	else
	{
		WaitQTail->pNext=target;
		target->pPrev=WaitQTail;
		WaitQTail=target;
		target->pNext=NULL;
	}
}
//dequeue ready and wait queue
Thread* dequeueReadyQ(thread_t tid)
{
	Thread* target=getThread(tid);
	if(target==NULL)
	{
		perror("get Thread error!(ready)");
		return NULL;
	}
	if(ReadyQHead==target&&target==ReadyQTail)
	{
		ReadyQHead=NULL;
		ReadyQTail=NULL;
	}
	else if(ReadyQHead==target)				//when target is head
	{
		Thread* next=(target->pNext);
		next->pPrev=NULL;
		ReadyQHead=next;
	}
	else if(target->pNext!=NULL)			//not in the last queue
	{
		Thread* next=target->pNext;
		Thread* prev=target->pPrev;
		next->pPrev=prev;
		prev->pNext=next;
	}
	else if(ReadyQTail==target)			//when target is tail
	{
		Thread* prev=target->pPrev;
		prev->pNext=NULL;
		ReadyQTail=prev;
	}
	target->pPrev=NULL;
	target->pNext=NULL;
	return target;
}
Thread* dequeueWaitQ(thread_t tid)
{
	Thread* target=getThread(tid);
	if(target==NULL)
	{
		perror("get Thread error!");
		return NULL;
	}
	if(WaitQHead==target&&target==WaitQTail)
	{
		WaitQHead=NULL;
		WaitQTail=NULL;
	}
	else if(WaitQHead==target)				//when target is head
	{
		Thread* next=target->pNext;
		next->pPrev=NULL;
		WaitQHead=next;
	}
	else if(target->pNext!=NULL)			//not in the last queue
	{
		Thread* next=target->pNext;
		Thread* prev=target->pPrev;
		next->pPrev=prev;
		prev->pNext=next;
	}
	else if(WaitQTail==target)			//when target is tail
	{
		Thread* prev=target->pPrev;
		prev->pNext=NULL;
		ReadyQTail=prev;
	}
	target->pPrev=NULL;
	target->pNext=NULL;
	return target;
}
//get Thread function
Thread* getThread(thread_t tid)
{
	Thread* target=ReadyQHead;
	while(target!=NULL)
	{
		if(target->tid==tid)
		{
			return target;
		}
		target=target->pNext;
	}
	target=WaitQHead;
	while(target!=NULL)
	{
		if(target->tid==tid)
		{
			return target;
		}
		target=target->pNext;
	}
	return NULL;
}

//Thread_wait_handler
void thread_wait_handler(int signo)
{
	Thread* pTh;
	//__getThread()는 tid로 linked list 의 TCB를 찾아서 반환한다.
	pTh=getThread(thread_self());
	pthread_mutex_lock(&(pTh->readyMutex));
	while(pTh->bRunnable==0)
		pthread_cond_wait(&(pTh->readyCond), &(pTh->readyMutex));
	pthread_mutex_unlock(&(pTh->readyMutex));
}

//child wrapper function
void* wrapperFunc(void* arg)
{
	void* ret;
	WrapperArg* pArg =(WrapperArg*)arg;
	sigset_t set;
	int retsig;
	//child sleeps until TCB is initialized
	sigemptyset(&set);
	sigaddset(&set,SIGUSR2);
	sigwait(&set, &retsig);//SIGUSR1 come, wakeup and thread_waithandler call
	//child is ready to run
	signal(SIGUSR1,(void *)thread_wait_handler);
	thread_wait_handler(0);

	ret = (pArg->funcPtr)(pArg->funcArg);
	return ret;
}
//Thread wake up
void __thread_wakeup(Thread* pTh)
{
	pTh->bRunnable = 1;
	pthread_mutex_lock(&(pTh->readyMutex));
	pthread_cond_signal(&(pTh->readyCond));
	pthread_mutex_unlock(&(pTh->readyMutex));
}
