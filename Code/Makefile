a.out: TestMain.o Init.o MyCode.o Scheduler.o Thread.o TestCase1.o TestCase2.o TestCase3.o
	gcc TestMain.o Init.o MyCode.o Scheduler.o Thread.o TestCase1.o TestCase2.o TestCase3.o -pthread
TestMain.o: TestMain.c TestCase1.h TestCase2.h TestCase3.h Init.h Scheduler.h Thread.h
	gcc -c TestMain.c
Init.o: Init.c Init.h Scheduler.h Thread.h
	gcc -c Init.c
MyCode.o: MyCode.c MyCode.h Thread.h
	gcc -c MyCode.c -pthread
Scheduler.o: Scheduler.c Init.h Thread.h Scheduler.h MyCode.h
	gcc -c Scheduler.c -pthread
Thread.o: Thread.c Thread.h Init.h Scheduler.h MyCode.h
	gcc -c Thread.c -pthread
TestCase1.o: TestCase1.c TestCase1.h
	gcc -c TestCase1.c
TestCase2.o: TestCase2.c TestCase2.h
	gcc -c TestCase2.c
TestCase3.o: TestCase3.c TestCase3.h
	gcc -c TestCase3.c
clean:
	rm -f a.out TestMain.o Init.o MyCode.o Scheduler.o Thread.o TestCase1.o TestCase2.o TestCase3.o
