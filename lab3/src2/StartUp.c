
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "kernel_util.h"
#include "task_functions.h"

#define PWMPERIOD 500000

TCB* taskQ;
TCB* suspendedQ;

/*
TCB PSDTask;
TCB SPDTask;
TCB KCDTask;
TCB TSDTask;
TCB SCDTask;
TCB VCDTask;
TCB CDDTask;
TCB WADTask;
*/

void StartUp(void* SUD) {
	startUpData* data = (startUpData*) SUD;

	*(data->thrusterCommand) = 0;

	// Power and Fuel globals
	//unsigned short batteryLevel = 100;
	*(data->fuelLevel) = 100;
	*(data->powerConsumption) = 0;
	*(data->powerGeneration) = 0;

	// Solar Panel Control globals
	*(data->solarPanelState) = FALSE;
	*(data->solarPanelDeploy) = FALSE;
	*(data->solarPanelRetract) = FALSE;

	*(data->driveMotorSpeedInc) = FALSE;
	*(data->driveMotorSpeedDec) = FALSE;

	// Warning Alarm globals
	*(data->fuelLow) = FALSE;
	*(data->batteryLow) = FALSE;

	// Vehicle Comm globals
	*(data->command) = NULL;
	*(data->response) = NULL;

	// Global Timer
	*(data->timer) = 0;

	unsigned int batteryData[16];
	data->batteryLevelPtr = batteryData;

	//initialize A/D
	FILE *ain;
	ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(ain, 0, SEEK_SET);
	fprintf(ain, "cape-bone-iio");
	fflush(ain);
	fclose(ain);

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
	*(data->fd0) = open(myfifo0, O_RDWR);

	// Define data structs for tasks
	powerSubsystemData PSD;
	PSD.solarPanelState = data->solarPanelState;
	PSD.solarPanelDeploy = data->solarPanelDeploy;
	PSD.solarPanelRetract = data->solarPanelRetract;
	PSD.batteryLevelPtr = data->batteryLevelPtr;
	PSD.powerConsumption = data->powerConsumption;
	PSD.powerGeneration = data->powerGeneration;
	PSD.timer = data->timer;

	solarPanelControlData SPD;
	SPD.solarPanelState = data->solarPanelState;
	SPD.solarPanelDeploy = data->solarPanelDeploy;
	SPD.solarPanelRetract = data->solarPanelRetract;
	SPD.driveMotorSpeedInc = data->driveMotorSpeedInc;
	SPD.driveMotorSpeedDec = data->driveMotorSpeedDec;
	SPD.timer = data->timer;

	keyboardConsoleData KCD;
	KCD.driveMotorSpeedInc = data->driveMotorSpeedInc;
	KCD.driveMotorSpeedDec = data->driveMotorSpeedDec;
	KCD.timer = data->timer;

	thrusterSubsystemData TSD;
	TSD.thrusterCommand = data->thrusterCommand;
	TSD.fuelLevel = data->fuelLevel;
	TSD.timer = data->timer;

	satelliteComsData SCD;
	SCD.fuelLow = data->fuelLow;
	SCD.batteryLow = data->batteryLow;
	SCD.solarPanelState = data->solarPanelState;
	SCD.batteryLevelPtr = data->batteryLevelPtr;
	SCD.fuelLevel = data->fuelLevel;
	SCD.powerConsumption = data->powerConsumption;
	SCD.powerGeneration = data->powerGeneration;
	SCD.thrusterCommand = data->thrusterCommand;
	SCD.timer = data->timer;
 	SCD.pipe = data->fd0;
  	SCD.command = data->command;
  	SCD.response = data->response;

	vehicleCommsData VCD;
	VCD.command = data->command;
	VCD.response = data->response;
	VCD.timer = data->timer;
 	VCD.pipe = data->fd0;

	consoleDisplayData CDD;
	CDD.fuelLow = data->fuelLow;
	CDD.batteryLow = data->batteryLow;
	CDD.solarPanelState = data->solarPanelState;
	CDD.batteryLevelPtr = data->batteryLevelPtr;
	CDD.fuelLevel = data->fuelLevel;
	CDD.powerConsumption = data->powerConsumption;
	CDD.powerGeneration = data->powerGeneration;
	CDD.timer = data->timer;

	warningAlarmData WAD;
	WAD.fuelLow = data->fuelLow;
	WAD.batteryLow = data->batteryLow;
	WAD.batteryLevelPtr = data->batteryLevelPtr;
	WAD.fuelLevel = data->fuelLevel;
	WAD.timer = data->timer;

	// Initialize Task Control Blocks for each task and insert into task queue

	//TCB* taskQ = NULL;
	//TCB* suspendedQ = NULL;

	//TCB PSDTask;
	PSDTask.taskDataPtr = (void*) &PSD;
	PSDTask.myTask = powerSubsystem;
	PSDTask.prev = NULL;
	PSDTask.next = NULL;

	TCB SPDTask;
	SPDTask.taskDataPtr = (void*) &SPD;
	SPDTask.myTask = SolarPanelControl;
	SPDTask.prev = NULL;
	SPDTask.next = NULL;

	TCB KCDTask;
	KCDTask.taskDataPtr = (void*) &KCD;
	KCDTask.myTask = ConsoleKeypad;
	KCDTask.prev = NULL;
	KCDTask.next = NULL;

	TCB TSDTask;
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

	TCB CDDTask;
	CDDTask.taskDataPtr = (void*) &CDD;
	CDDTask.myTask = consoleDisplay;
	CDDTask.prev = NULL;
	CDDTask.next = NULL;

	TCB WADTask;
	WADTask.taskDataPtr = (void*) &WAD;
	WADTask.myTask = warningAlarm;
	WADTask.prev = NULL;
	WADTask.next = NULL;

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
}
