////////////////////////////
// Author: Allan Gilmore  //
// date: 7/11/17		  //
////////////////////////////

// Description: This program outlines the kernal and scheduler for a emmbedded
//				Satillite system.

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"
#include "task_functions.h"


void main() {
	// Thruster control globals
	unsigned int thrusterCommand = 0;

	// Power and Fuel globals
	unsigned short batteryLevel = 100;
	unsigned short fuelLevel = 100;
	unsigned short powerConsumption = 0;
	unsigned short powerGeneration = 0;

	// Solar Panel Control globals
	bool solarPanelState = FALSE;

	// Warning Alarm globals
	bool fuelLow = FALSE;
	bool batteryLow = FALSE;

	// Global Timer
	unsigned short timer = 0;

	// Initialize LEDs
	FILE* LEDHandler = NULL;

	char *LEDPath0 = "/sys/class/leds/beaglebone:green:usr0/brightness";
	char *LEDPath1 = "/sys/class/leds/beaglebone:green:usr1/brightness";
	char *LEDPath2 = "/sys/class/leds/beaglebone:green:usr2/brightness";
	char *LEDPath3 = "/sys/class/leds/beaglebone:green:usr3/brightness";

	if (LEDHandler = fopen(LEDPath0, "r+")) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}
	if (LEDHandler = fopen(LEDPath1, "r+")) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}
	if (LEDHandler = fopen(LEDPath2, "r+")) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}
	if (LEDHandler = fopen(LEDPath3, "r+")) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}

	// Define data structs for tasks
	powerSubsystemData PSD;
	PSD.solarPanelState = &solarPanelState;
	PSD.batteryLevel = &batteryLevel;
	PSD.powerConsumption = &powerConsumption;
	PSD.powerGeneration = &powerGeneration;
	PSD.timer = &timer;

	thrusterSubsystemData TSD;
	TSD.thrusterCommand = &thrusterCommand;
	TSD.fuelLevel = &fuelLevel;
	TSD.timer = &timer;

	satelliteComsData SCD;
	SCD.fuelLow = &fuelLow;
	SCD.batteryLow = &batteryLow;
	SCD.solarPanelState = &solarPanelState;
	SCD.batteryLevel = &batteryLevel;
	SCD.fuelLevel = &fuelLevel;
	SCD.powerConsumption = &powerConsumption;
	SCD.powerGeneration = &powerGeneration;
	SCD.thrusterCommand = &thrusterCommand;
	SCD.timer = &timer;

	consoleDisplayData CDD;
	CDD.fuelLow = &fuelLow;
	CDD.batteryLow = &batteryLow;
	CDD.solarPanelState = &solarPanelState;
	CDD.batteryLevel = &batteryLevel;
	CDD.fuelLevel = &fuelLevel;
	CDD.powerConsumption = &powerConsumption;
	CDD.powerGeneration = &powerGeneration;
	CDD.timer = &timer;

	warningAlarmData WAD;
	WAD.fuelLow = &fuelLow;
	WAD.batteryLow = &batteryLow;
	WAD.batteryLevel = &batteryLevel;
	WAD.fuelLevel = &fuelLevel;
	WAD.timer = &timer;

	// Initialize Task Control Blocks for each task and insert into task queue
	TCB taskQ[6];
	TCB PSDTask;
	PSDTask.taskDataPtr = (void*) &PSD;
	PSDTask.myTask = powerSubsystem;
	taskQ[0] = PSDTask;

	TCB TSDTask;
	TSDTask.taskDataPtr = (void*) &TSD;
	TSDTask.myTask =  ThrusterSubsystem;
	taskQ[1] = TSDTask;

	TCB SCDTask;
	SCDTask.taskDataPtr = (void*) &SCD;
	SCDTask.myTask = SatelliteComs;
	taskQ[2] = SCDTask;

	TCB CDDTask;
	CDDTask.taskDataPtr = (void*) &CDD;
	CDDTask.myTask = consoleDisplay;
	taskQ[3] = CDDTask;

	TCB WADTask;
	WADTask.taskDataPtr = (void*) &WAD;
	WADTask.myTask = warningAlarm;
	taskQ[4] = WADTask;

	// Scheduler
	while(1) {
		clock_t start = clock();
		int i;
		for (i = 0; i < 5; i++) {
			taskQ[i].myTask(taskQ[i].taskDataPtr);
		}
		time_t t_start = time(NULL);
		clock_t currentTime = clock();

		// Check that half a second has passed
		while((currentTime - start) < 0.5 * CLOCKS_PER_SEC) {
			currentTime = clock();
		}

		timer++;
		timer = timer % 10;
	}
}


