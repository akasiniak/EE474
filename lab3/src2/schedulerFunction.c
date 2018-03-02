
#include "kernel_util.h"
#include <stdio.h>

void add_task(TCB* newTask) {
	printf("add_task called\n");
	fflush(stdout);
	TCB* queuePtr = suspendedQ;

	// look for and delete task in the suspended queue
	while (queuePtr != newTask) {
		if (queuePtr == NULL) {
			return;
		} else {
			queuePtr = queuePtr->next;
		}
	}
	if (queuePtr->prev == NULL) {
		suspendedQ = suspendedQ->next;
		if (suspendedQ != NULL) {
			suspendedQ->prev = NULL;
		}
	} else {
		queuePtr->prev->next = queuePtr->next;
	}
	//printf("suspendedQ passed\n");
	//fflush(stdout);
	// add task to the task queue
	queuePtr = taskQ;
	if (taskQ == NULL) {
		taskQ = newTask;
		return;
	}
	while (queuePtr->next != NULL) {
		queuePtr = queuePtr->next;
	}
	queuePtr->next = newTask;
	queuePtr->next->next = NULL;
	queuePtr->next->prev = queuePtr;

	//printf("add_task exits");
	//fflush(stdout);
}

void delete_task(TCB* task) {
	TCB* queuePtr = taskQ;
	while (queuePtr != NULL) {
		if (queuePtr == task) {
			if (queuePtr->prev == NULL) {
				taskQ = taskQ->next;
			} else {
				queuePtr->prev->next = queuePtr->next;
			}
			queuePtr->next = NULL;
			queuePtr = suspendedQ;
			if (queuePtr == NULL) {
				suspendedQ = task;
				task->prev = NULL;
				return;
			}
			while (queuePtr->next != NULL) {
				queuePtr = queuePtr->next;
			}
			queuePtr->next = task;
			queuePtr->next->prev = queuePtr;
			task->next = NULL;
			return;
		} else {
			queuePtr = queuePtr->next;
		}
	}
}