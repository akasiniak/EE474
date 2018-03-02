#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

//  specify three tasks

void get (void* aNumber);							//  input task
void increment (void* aNumber);						//  computation task
void display (void* aNumber);						//  output task

//  specify the interrupt service routines
void isr0(void);
void isr1(void);

typedef void(*isr)(void);

void myHandler(int aSig);							//  specify the signal handler

typedef struct 										//  Declare a TCB structure
{
	void* taskDataPtr;
	void (*taskPtr)(void*);
}
TCB;

int isrNum = 0;										//  isr selecter .. two used
void (*isrPtr)();									//  isr pointer
isr isrArray[2];									//  queue of isr handlers

// time declarations

clock_t start_t, end_t, total_t;
int cps = CLOCKS_PER_SEC;
double total = 0.0;
double ftotal = 0.0;



int main(void)
{

	struct tm*	tp;									// this is a time struc

	//  need to identify this???
	char*		p;

	//  time is traditionally an unsigned long
	//  time_t is ansi c return type from time
	//  can be any arithmetic type
	time_t		now = time(NULL);

	// declare the signal and associate a handler with the signal

	// the first specifies a ^c signal the second a user defined one

	//signal(SIGINT, myHandler);

	signal(SIGUSR1, myHandler);

	int i=0;										//  queue index
	int data;										//  declare a shared data
	int* aPtr = &data;								//  point to it

	TCB* queue[3];									//  declare queue as array of pointers to TCBs

/*
 *  starting the program
 */

	//  Declare some TCBs
	TCB inTask;
	TCB compTask;
	TCB outTask;
	TCB* aTCBPtr;

	// 	Initialize the isr queue
    	isrArray[0]= isr0;
    	isrArray[1]= isr1;




	//  Initialize the TCBs
	inTask.taskDataPtr = (void*)&data;
	inTask.taskPtr = get;

	compTask.taskDataPtr = (void*)&data;
	compTask.taskPtr = increment;

	outTask.taskDataPtr = (void*)&data;
	outTask.taskPtr = display;

	// 	Initialize the task queue
	queue[0] = &inTask;
	queue[1] = &compTask;
	queue[2] = &outTask;

	// schedule and dispatch the tasks
	while(1)
	{
		//  select a task
		aTCBPtr = queue[i];

		//  run the task
		aTCBPtr->taskPtr( (aTCBPtr->taskDataPtr) );

		/*
		 *  track the time system time
		 */

		now = time(NULL);									//  get current time
		tp = localtime(&now);								//  identify this

		printf("i is %d  time is %d\n", i, tp-> tm_sec);	//  print current second
            i = (i+1)%3;

        	usleep(30);										//  delay 30 ms
	}

	tp = localtime(&now);									//  get current time

	printf("%d \n", tp-> tm_hour);							//  print current hour
	printf("%d \n", tp-> tm_sec);							//  print current second

	return EXIT_SUCCESS;
}

//  get task
void get (void* aNumber)									//  perform input operation
{
	start_t = clock();										//  get and print initial time
	printf("Starting get, start_t = %ld\n", start_t);

	printf ("Enter a number: 0..9 ");						//  prompt user for data
	*(int*) aNumber  = getchar();							//  read data
	getchar();												//  discard cr
	*(int*) aNumber -= '0';  								//  convert to decimal from ascii
	sleep(5);												//  delay

	end_t = clock();										//  get and print end time
	printf("End of get, end_t = %ld\n", end_t);

	total = end_t - start_t;								//  compute and print total time
	printf("total is %f \n", (double)total);
	printf(" the cps is: %d \n", CLOCKS_PER_SEC);

	ftotal = (double) (total / cps);

	printf("ftotal is: %f \n", ftotal);

	return;
}

//  increment task
void increment (void* aNumber)								//  perform computation
{
	int* aPtr = (int*) aNumber;								//  increment shared data
	(*aPtr)++;

	isrNum = 1;												//  specify call to isr 1
	raise(SIGUSR1);											//  raise a signal

	return;
}






//  display task
void display (void* aNumber)								// perform output operation
{
	printf ("The result is: %d\n", *(int*)aNumber);
	//  the arg is a system defined ^c signal
	// raise(SIGINT);

	//  the arg is a user defined signal
	//  raise a signal

	isrNum = 0;												//  specify call to isr 0
	raise(SIGUSR1);											//  raise a signal

	return;
}

//  signal handler
void myHandler(int aSig)
{
	printf("in the handler isrnum %d\n", isrNum);

	switch (isrNum)
	{
		case 0:
		{
			// signal / interrupt 0 raised
			// handle it
			isrArray[0]();
			break;
		}

		case 1:
		{
			// signal / interrupt 1 raised
			// handle it
			isrArray[1]();
			break;
		}
	}

	//  will exit the program
	//exit(1);


	//  return to where signal raised
	return;
}

//  handle signal / interrupt 0
void isr0(void)
{
    printf("in isr0 %d\n", isrNum);
    return;
}

//  handle signal / interrupt 1
void isr1(void)
{
    printf("in isr1 %d\n", isrNum);
    return;
}
	
