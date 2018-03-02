////////////////////////////
// Author: Allan Gilmore  //
// date: 7/11/17		  //
////////////////////////////

// Description: This program outlines the kernal and scheduler for a emmbedded
//				Satillite system.

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../kernel_util.h"
#include "../task_functions.h"


void one(void* data) {
	printf("One\n");
	fflush(stdout);
}
void two(void* data) {
	printf("Two\n");
	fflush(stdout);
}
void three(void* data) {
	printf("Three\n");
	fflush(stdout);
}
void four(void* data) {
	printf("Four\n");
	fflush(stdout);
}
void five(void* data) {
	printf("Five\n");
	fflush(stdout);
}
void six(void* data) {
	printf("Six\n");
	fflush(stdout);
}

int main() {
	// Thruster control globals
	unsigned int thrusterCommand;

	// Power and Fuel globals
	//unsigned short batteryLevel = 100;
	unsigned short fuelLevel;
	unsigned short powerConsumption;
	unsigned short powerGeneration;

	// Solar Panel Control globals
	bool solarPanelState;
	bool solarPanelDeploy;
	bool solarPanelRetract;

	bool driveMotorSpeedInc;
	bool driveMotorSpeedDec;

	// Warning Alarm globals
	bool fuelLow;
	bool batteryLow;

	// Vehicle Comm globals
	char command;
	char response;

	// Global Timer
	unsigned short timer;

	unsigned int* batteryLevelPtr;

	int pipe;

	// Creat FIFO
	int fd0;

	thrusterCommand = 0;

	// Power and Fuel globals
	//unsigned short batteryLevel = 100;
	fuelLevel = 100;
	powerConsumption = 0;
	powerGeneration = 0;

	// Solar Panel Control globals
	solarPanelState = FALSE;
	solarPanelDeploy = FALSE;
	solarPanelRetract = FALSE;

	driveMotorSpeedInc = FALSE;
	driveMotorSpeedDec = FALSE;

	// Warning Alarm globals
	fuelLow = FALSE;
	batteryLow = FALSE;
/*
	// Vehicle Comm globals
	command = NULL;
	response = NULL;
*/
	// Global Timer
	timer = 0;

	unsigned int batteryData;
	batteryLevelPtr = &batteryData;

	/*
	TCB PSDTask;
	PSDTask.taskDataPtr = (void*) &PSD;
	PSDTask.myTask = powerSubsystem;
	PSDTask.prev = NULL;
	PSDTask.next = NULL;
	*/

	TCB One;
	One.myTask = one;
	One.taskDataPtr = NULL;
	One.prev = NULL;
	One.next = NULL;
	One.priority = 2;

	TCB Two;
	Two.myTask = two;
	Two.taskDataPtr = NULL;
	Two.prev = NULL;
	Two.next = NULL;
	Two.priority = 1;
	
	TCB Three;
	Three.myTask = three;
	Three.taskDataPtr = NULL;
	Three.prev = NULL;
	Three.next = NULL;
	Three.priority = 1;
	
	TCB Four;
	Four.myTask = four;
	Four.taskDataPtr = NULL;
	Four.prev = NULL;
	Four.next = NULL;
	Four.priority = 1;
	
	TCB Five;
	Five.myTask = five;
	Five.taskDataPtr = NULL;
	Five.prev = NULL;
	Five.next = NULL;
	Five.priority = 1;
	
	TCB Six;
	Six.myTask = six;
	Six.taskDataPtr = NULL;
	Six.prev = NULL;
	Six.next = NULL;
	Six.priority = 1;

	taskQ = NULL;
	suspendedQ = NULL;

	suspendedQ = &One;
	suspendedQ->next = &Two;
	Two.prev = &One;
	Two.next = &Three;
	Three.prev = &Two;

	// Scheduler
	//while(1) {
		add_task(&One);
		TCB* currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
		add_task(&Two);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
		add_task(&Three);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}

		/*
		delete_task(&One);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
		*/
		
		delete_task(&Two);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
		/*
		delete_task(&Three);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
	*/
		add_task(&One);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
		add_task(&Two);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
		add_task(&Three);
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(NULL);
			currentTask = currentTask->next;
		}
	//}
	return 0;
}
