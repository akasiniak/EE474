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
#include "kernel_util.h"
#include "task_functions.h"

#define PWMPERIOD 500000
/*
TCB* taskQ;
TCB* suspendedQ;

TCB PSDTask;
TCB SPDTask;
TCB KCDTask;
TCB TSDTask;
TCB SCDTask;
TCB VCDTask;
TCB CDDTask;
TCB WADTask;
*/


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

	// Vehicle Comm globals
	command = NULL;
	response = NULL;

	// Global Timer
	timer = 0;

	unsigned int batteryData;
	batteryLevelPtr = &batteryData;

	//initialize A/D
	FILE *ain;
	ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(ain, 0, SEEK_SET);
	fprintf(ain, "cape-bone-iio");
	fflush(ain);

	// Initialize LEDs
	FILE* LEDHandler = NULL;

	char *LEDPath0 = "/sys/class/leds/beaglebone:green:usr0/brightness";
	char *LEDPath1 = "/sys/class/leds/beaglebone:green:usr1/brightness";
	char *LEDPath2 = "/sys/class/leds/beaglebone:green:usr2/brightness";
	char *LEDPath3 = "/sys/class/leds/beaglebone:green:usr3/brightness";

	if ((LEDHandler = fopen(LEDPath0, "r+"))) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}
	if ((LEDHandler = fopen(LEDPath1, "r+"))) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}
	if ((LEDHandler = fopen(LEDPath2, "r+"))) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}
	if ((LEDHandler = fopen(LEDPath3, "r+"))) {
		fwrite("0", 1, 1, LEDHandler);
		fclose(LEDHandler);
	}

	//initialize PWM
	FILE *pwm, *period, *run;

	//enable pins
	pwm = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(pwm,0,SEEK_SET);
	fprintf(pwm, "am33xx_pwm");
	fflush(pwm);

	fprintf(pwm, "bone_pwm_P9_16");
	fflush(pwm);

  fprintf(pwm, "bone_pwm_P9_14");
  fflush(pwm);

	//set period
	period = fopen("/sys/devices/ocp.3/pwm_test_P9_16.16/period", "w");
	fseek(period,0,SEEK_SET);
	fprintf(period,"%d", PWMPERIOD);
	fflush(period);

	//enable signal for solarpanel
	run = fopen("/sys/devices/ocp.3/pwm_test_P9_16.16/run", "w");
	fseek(run,0,SEEK_SET);
	fprintf(run,"%d",1);
	fflush(run);

	//set period
	period = fopen("/sys/devices/ocp.3/pwm_test_P9_14.17/period", "w");
	fseek(period,0,SEEK_SET);
	fprintf(period,"%d", PWMPERIOD);
	fflush(period);

	//enable signal for thruster
	run = fopen("/sys/devices/ocp.3/pwm_test_P9_14.17/run", "w");
	fseek(run,0,SEEK_SET);
	fprintf(run,"%d",1);
	fflush(run);


	fclose(pwm);
	fclose(period);
	fclose(run);

	// Creat FIFO
	char * myfifo0 = "/tmp/myfifo0";
	mkfifo(myfifo0, 0666);
	fd0 = open(myfifo0, O_RDWR);

	// Define data structs for tasks
	powerSubsystemData PSD;
	PSD.solarPanelState = &solarPanelState;
	PSD.solarPanelDeploy = &solarPanelDeploy;
	PSD.solarPanelRetract = &solarPanelRetract;
	PSD.batteryLevelPtr = batteryLevelPtr;
	PSD.powerConsumption = &powerConsumption;
	PSD.powerGeneration = &powerGeneration;
	PSD.timer = &timer;

	solarPanelControlData SPD;
	SPD.solarPanelState = &solarPanelState;
	SPD.solarPanelDeploy = &solarPanelDeploy;
	SPD.solarPanelRetract = &solarPanelRetract;
	SPD.driveMotorSpeedInc = &driveMotorSpeedInc;
	SPD.driveMotorSpeedDec = &driveMotorSpeedDec;
	SPD.timer = &timer;

	keyboardConsoleData KCD;
	KCD.driveMotorSpeedInc = &driveMotorSpeedInc;
	KCD.driveMotorSpeedDec = &driveMotorSpeedDec;
	KCD.timer = &timer;

	thrusterSubsystemData TSD;
	TSD.thrusterCommand = &thrusterCommand;
	TSD.fuelLevel = &fuelLevel;
	TSD.timer = &timer;

	satelliteComsData SCD;
	SCD.fuelLow = &fuelLow;
	SCD.batteryLow = &batteryLow;
	SCD.solarPanelState = &solarPanelState;
	SCD.batteryLevelPtr = batteryLevelPtr;
	SCD.fuelLevel = &fuelLevel;
	SCD.powerConsumption = &powerConsumption;
	SCD.powerGeneration = &powerGeneration;
	SCD.thrusterCommand = &thrusterCommand;
	SCD.timer = &timer;
 	SCD.pipe = &fd0;
  	SCD.command = &command;
  	SCD.response = &response;

	vehicleCommsData VCD;
	VCD.command = &command;
	VCD.response = &response;
	VCD.timer = &timer;
 	VCD.pipe = &fd0;

	consoleDisplayData CDD;
	CDD.fuelLow = &fuelLow;
	CDD.batteryLow = &batteryLow;
	CDD.solarPanelState = &solarPanelState;
	CDD.batteryLevelPtr = batteryLevelPtr;
	CDD.fuelLevel = &fuelLevel;
	CDD.powerConsumption = &powerConsumption;
	CDD.powerGeneration = &powerGeneration;
	CDD.timer = &timer;

	warningAlarmData WAD;
	WAD.fuelLow = &fuelLow;
	WAD.batteryLow = &batteryLow;
	WAD.batteryLevelPtr = batteryLevelPtr;
	WAD.fuelLevel = &fuelLevel;
	WAD.timer = &timer;

	// Initialize Task Control Blocks for each task and insert into task queue

	//TCB* taskQ = NULL;
	//TCB* suspendedQ = NULL;

	TCB PSDTask; //Check
	PSDTask.taskDataPtr = (void*) &PSD;
	PSDTask.myTask = powerSubsystem;
	PSDTask.prev = NULL;
	PSDTask.next = NULL;

	TCB SPDTask; //Check
	SPDTask.taskDataPtr = (void*) &SPD;
	SPDTask.myTask = solarPanelControl;
	SPDTask.prev = NULL;
	SPDTask.next = NULL;

	TCB KCDTask;	//Check
	KCDTask.taskDataPtr = (void*) &KCD;
	KCDTask.myTask = ConsoleKeypadTask;
	KCDTask.prev = NULL;
	KCDTask.next = NULL;

	TCB TSDTask;	//Check
	TSDTask.taskDataPtr = (void*) &TSD;
	TSDTask.myTask =  ThrusterSubsystem;
	TSDTask.prev = NULL;
	TSDTask.next = NULL;

	TCB SCDTask;
	SCDTask.taskDataPtr = (void*) &SCD;
	SCDTask.myTask = SatelliteComs;
	SCDTask.prev = NULL;
	SCDTask.next = NULL;

	TCB VCDTask;
	VCDTask.taskDataPtr = (void*) &VCD;
	VCDTask.myTask = VehicleComms;
	VCDTask.prev = NULL;
	VCDTask.next = NULL;

	TCB CDDTask;	//Check
	CDDTask.taskDataPtr = (void*) &CDD;
	CDDTask.myTask = consoleDisplay;
	CDDTask.prev = NULL;
	CDDTask.next = NULL;

	TCB WADTask;	//Check
	WADTask.taskDataPtr = (void*) &WAD;
	WADTask.myTask = warningAlarm;
	WADTask.prev = NULL;
	WADTask.next = NULL;

	PSD.SPDTaskPtr = &SPDTask;
	SPD.KCDTaskPtr = &KCDTask;
	SPD.SPDTaskPtr = &SPDTask;

	taskQ = &PSDTask;

	TCB* taskEnd = taskQ;
	taskEnd->next = &TSDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	taskEnd->next = &SCDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	taskEnd->next = &VCDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	taskEnd->next = &CDDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	taskEnd->next = &WADTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	suspendedQ = &SPDTask;
	suspendedQ->next = &KCDTask;
	suspendedQ->next->prev = suspendedQ;

	FILE* timePin = fopen("/sys/class/gpio/export", "w");
	fseek(timePin, 0, SEEK_SET);
	fprintf(timePin, "%d", 66);
	fflush(timePin);

	// Set pin as input
	FILE* pinModeInc = fopen("/sys/class/gpio/gpio66/direction", "w");
	fseek(pinModeInc, 0, SEEK_SET);
	fprintf(pinModeInc, "out");
	fflush(pinModeInc);

	FILE* pinVal = fopen("/sys/class/gpio/gpio66/value", "w");
	fseek(pinVal, 0, SEEK_SET);
	fprintf(pinVal, "0");
	fflush(pinVal);

	// Scheduler
	while(1) {
		clock_t start = clock();

		//pinVal = fopen("/sys/class/gpio/gpio66/value", "w");
		fseek(pinVal, 0, SEEK_SET);
		fprintf(pinVal, "1");
		fflush(pinVal);

		VCDTask.myTask(VCDTask.taskDataPtr);
		//sleep(1);

		//FILE* pinVal = fopen("/sys/class/gpio/gpio66/value", "w");
		fseek(pinVal, 0, SEEK_SET);
		fprintf(pinVal, "0");
		fflush(pinVal);

		sleep(1);

		clock_t currentTime = clock();

		// Check that half a second has passed
		while((currentTime - start) < 0.5 * CLOCKS_PER_SEC) {
			currentTime = clock();
		}

			}
	return 0;
}
