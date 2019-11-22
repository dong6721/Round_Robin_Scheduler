#include "Thread.h"
#include <signal.h>
Thread* CThread;	//실행중 쓰레드
void enqueueReadyQ(Thread* target);

void enqueueWaitQ(Thread* target);

Thread* dequeueReadyQ(thread_t tid);

Thread* dequeueWaitQ(thread_t tid);

Thread* getThread(thread_t tid);

void thread_wait_handler(int signo);

void* wrapperFunc(void* arg);

void __thread_wakeup(Thread* pTh);
