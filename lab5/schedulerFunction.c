////////////////////////////
// Author: Allan Gilmore  //
// date: 7/26/17		  //
////////////////////////////

// Description: Definitions of the Scheduler functions from adding and removing
//				tasks from the task queue.

#include "kernel_util.h"
#include <stdio.h>

// Add task to the end of the task queue. If the task is already in the task queue
// do nothing.
void add_task(TCB* newTask) {

	if (newTask == NULL) {
		return;
	}

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

	// add task to the task queue
	queuePtr = taskQ;
	if (taskQ == NULL) {
		taskQ = newTask;
		taskQ->prev = NULL;
		taskQ->next = NULL;
		return;
	} else if (taskQ->priority > newTask->priority) {
		newTask->next = taskQ;
		taskQ->prev = newTask;
		taskQ = newTask;
		taskQ->prev = NULL;
		return;
	}

	while (queuePtr->next != NULL) {
		if (queuePtr->next->priority > newTask->priority) {
			newTask->next = queuePtr->next;
			newTask->next->prev = newTask;
			queuePtr->next = newTask;
			newTask->prev = queuePtr;
			return;
		}
		queuePtr = queuePtr->next;
	}
	if (taskQ->priority > newTask->priority) {
		newTask->next = taskQ;
		taskQ->prev = newTask;
		taskQ = newTask;
		taskQ->prev = NULL;
	} else {
		newTask->next = NULL;
		queuePtr->next = newTask;
		newTask->prev = queuePtr;
	}
}

// remove task from the task queue. If the task is not in the task queue, do nothing.
void delete_task(TCB* task) {

	if (task == NULL) {
		return;
	}
	TCB* queuePtr = taskQ;
	while (queuePtr != NULL) {
		if (queuePtr == task) {
			if (queuePtr->prev == NULL) {
				taskQ = taskQ->next;
				if (taskQ != NULL) {
					taskQ->prev = NULL;
				}
			} else if (queuePtr->next == NULL) {
				queuePtr->prev->next = NULL;
				queuePtr->prev = NULL;
			} else {
				queuePtr->next->prev = queuePtr->prev;
				queuePtr->prev->next = queuePtr->next;
			}
			queuePtr = suspendedQ;
			if (queuePtr == NULL) {
				suspendedQ = task;
				task->prev = NULL;
				task->next = NULL;
				return;
			}
			while (queuePtr->next != NULL) {
				queuePtr = queuePtr->next;
			}
			queuePtr->next = task;
			task->prev = queuePtr;
			task->next = NULL;
			return;
		} else {
			queuePtr = queuePtr->next;
		}
	}
}



