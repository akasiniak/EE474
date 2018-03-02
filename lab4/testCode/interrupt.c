#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#define MYSIG = 5;
void get (void* aNumber);
void increment (void* aNumber);
void display (void* aNumber);
void myHandler(int aSig);
void testHandler(int aSig);
// Declare a TCB structure
typedef struct {
  void* taskDataPtr;
  void (*taskPtr)(void*);
}
TCB;
int main(void) {
  signal(SIGUSR1, myHandler);
  signal(SIGUSR2, testHandler);
  int i=0;
  int data;
  int* aPtr = &data;
  TCB* queue[3];
// input task
// computation task
// output task
// interrupt / signal handler
// associate the signal with its handler
// queue index
// declare a shared data // point to it
// declare queue as an array of pointers to TCBs
// Declare some TCBs
  TCB inTask;
  TCB compTask;
  TCB outTask;
  TCB* aTCBPtr;
// Initialize the TCBs
  inTask.taskDataPtr = (void*)&data;
  inTask.taskPtr = get;
  compTask.taskDataPtr = (void*)&data;
  compTask.taskPtr = increment;
  outTask.taskDataPtr = (void*)&data;
  outTask.taskPtr = display;
// Initialize the task queue
  queue[0] = &inTask;
  queue[1] = &compTask;
  queue[2] = &outTask;
  while(1) {
    aTCBPtr = queue[i];
    aTCBPtr->taskPtr( (aTCBPtr->taskDataPtr) );
    i = (i+1)%3;
    usleep(30);
  }
  return EXIT_SUCCESS;
}

void get (void* aNumber) {
  printf ("Enter a number: 0..9 ");
  *(int*) aNumber = getchar();
  getchar();
  *(int*) aNumber -= '0';
  return;
}

void increment (void* aNumber) {
  int* aPtr = (int*) aNumber;
  (*aPtr)++;
  return;
}
// perform input operation
// discard cr
// convert to decimal from ascii
// perform computation
// perform output operation
void display (void* aNumber) {
  int testHandlerEntered = 0;
  printf ("The result is: %d\n", *(int*)aNumber); //
  raise(SIGINT);
  raise(SIGUSR1);
  raise(SIGUSR2);
  if(testHandlerEntered){
    printf("testHandlerEntered");
  }
  return;
}
void myHandler(int aSig) {
  printf("in the handler\n");
  return;
}

void testHandler(int aSig) {
  testHandlerEntered = 1;
  return;
}
