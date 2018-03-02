////////////////////////////
// Author: Allan Gilmore  //
// date: 7/13/17		  //
////////////////////////////

// Description: This program runs the satellite system with the GPIO to time the
//				the execution time of each task.


#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"
#include "task_functions.h"


void main() {
	// Thruster control globals
	unsigned int thrusterCommand = 0;

	// Power Management globals
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

	FILE* switchPin1;
	FILE* switchPin2;
	FILE* switchPin3;
	FILE* switchPin4;
	FILE* switchPin5;

	FILE* pinMode1;
	FILE* pinMode2;
	FILE* pinMode3;
	FILE* pinMode4;
	FILE* pinMode5;

	FILE* pinVal1;
	FILE* pinVal2;
	FILE* pinVal3;
	FILE* pinVal4;
	FILE* pinVal5;

	//task 1
	switchPin1 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin1, 0, SEEK_SET);
	fprintf(switchPin1, "%d", 67);
	fflush(switchPin1);

	pinMode1 = fopen("/sys/class/gpio/gpio67/direction", "w");
	fseek(pinMode1, 0, SEEK_SET);
	fprintf(pinMode1, "out");
	fflush(pinMode1);

	pinVal1 = fopen("/sys/class/gpio/gpio67/value", "w");
	fseek(pinVal1, 0, SEEK_SET);
	fprintf(pinVal1, "%d", 0);
	fflush(pinVal1);

	// task 2
	switchPin2 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin2, 0, SEEK_SET);
	fprintf(switchPin2, "%d", 68);
	fflush(switchPin2);

	pinMode2 = fopen("/sys/class/gpio/gpio68/direction", "w");
	fseek(pinMode2, 0, SEEK_SET);
	fprintf(pinMode2, "out");
	fflush(pinMode2);

	pinVal2 = fopen("/sys/class/gpio/gpio68/value", "w");
	fseek(pinVal2, 0, SEEK_SET);
	fprintf(pinVal2, "%d", 0);
	fflush(pinVal2);

	// task 3
	switchPin3 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin3, 0, SEEK_SET);
	fprintf(switchPin3, "%d", 44);
	fflush(switchPin3);

	pinMode3 = fopen("/sys/class/gpio/gpio44/direction", "w");
	fseek(pinMode3, 0, SEEK_SET);
	fprintf(pinMode3, "out");
	fflush(pinMode3);

	pinVal3 = fopen("/sys/class/gpio/gpio44/value", "w");
	fseek(pinVal3, 0, SEEK_SET);
	fprintf(pinVal3, "%d", 0);
	fflush(pinVal3);

	// task 4
	switchPin4 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin4, 0, SEEK_SET);
	fprintf(switchPin4, "%d", 26);
	fflush(switchPin4);

	pinMode4 = fopen("/sys/class/gpio/gpio26/direction", "w");
	fseek(pinMode4, 0, SEEK_SET);
	fprintf(pinMode4, "out");
	fflush(pinMode4);

	pinVal4 = fopen("/sys/class/gpio/gpio26/value", "w");
	fseek(pinVal4, 0, SEEK_SET);
	fprintf(pinVal4, "%d", 0);
	fflush(pinVal4);

		// task 5
	switchPin5 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin5, 0, SEEK_SET);
	fprintf(switchPin5, "%d", 46);
	fflush(switchPin5);

	pinMode5 = fopen("/sys/class/gpio/gpio46/direction", "w");
	fseek(pinMode5, 0, SEEK_SET);
	fprintf(pinMode5, "out");
	fflush(pinMode5);

	pinVal5 = fopen("/sys/class/gpio/gpio46/value", "w");
	fseek(pinVal5, 0, SEEK_SET);
	fprintf(pinVal5, "%d", 0);
	fflush(pinVal5);


	// Define data structs for tasks;
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

	while(1) {
		clock_t start = clock();
		
		pinVal1 = fopen("/sys/class/gpio/gpio67/value", "w");
		fseek(pinVal1, 0, SEEK_SET);
		fprintf(pinVal1, "%d", 1);
		fflush(pinVal1);

		taskQ[0].myTask(taskQ[0].taskDataPtr);

		fseek(pinVal1, 0, SEEK_SET);
		fprintf(pinVal1, "%d", 0);
		fflush(pinVal1);


		fseek(pinVal2, 0, SEEK_SET);
		fprintf(pinVal2, "%d", 1);
		fflush(pinVal2);

		taskQ[4].myTask(taskQ[4].taskDataPtr);
		
		fseek(pinVal2, 0, SEEK_SET);
		fprintf(pinVal2, "%d", 0);
		fflush(pinVal2);

		fseek(pinVal3, 0, SEEK_SET);
		fprintf(pinVal3, "%d", 1);
		fflush(pinVal3);

		taskQ[2].myTask(taskQ[2].taskDataPtr);
		
		fseek(pinVal3, 0, SEEK_SET);
		fprintf(pinVal3, "%d", 0);
		fflush(pinVal3);

		fseek(pinVal4, 0, SEEK_SET);
		fprintf(pinVal4, "%d", 1);
		fflush(pinVal4);

		taskQ[3].myTask(taskQ[3].taskDataPtr);
		
		fseek(pinVal4, 0, SEEK_SET);
		fprintf(pinVal4, "%d", 0);
		fflush(pinVal4);

		fseek(pinVal5, 0, SEEK_SET);
		fprintf(pinVal5, "%d", 1);
		fflush(pinVal5);

		taskQ[4].myTask(taskQ[4].taskDataPtr);
		
		fseek(pinVal5, 0, SEEK_SET);
		fprintf(pinVal5, "%d", 0);
		fflush(pinVal5);


		clock_t currentTime = clock();
		while((currentTime - start) < 0.5 * CLOCKS_PER_SEC) {
			currentTime = clock();
		}
		
		

	}










}


