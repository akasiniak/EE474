////////////////////////////
// Author: Allan Gilmore  //
iifejio// date: 7/11/17		  //
////////////////////////////

// Description: This program outlines the kernal and scheduler for a emmbedded
//				Satillite system.

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include "kernel_util.h"
#include "task_functions.h"

#define MAX_COMMAND 100

void isr0(void);
void isr1(void);
void isr2(void);

typedef void(*isr)(void);

void mySignalHandler(int aSig);

TCB TDDTask;
bool transportIncomming = FALSE;

#define PWMPERIOD 500000
int isrNum = 0; //keeps track of which isr we should be calling
int motorDriveReset = 0; //becomes true when the solar panel is being overdriven
void (*isrPtr)();
isr isrArray[3];
// Test change

bool startSys = FALSE;
bool stopSys = TRUE;

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

	thrusterCommand = 0;


	dbuff transportDistance;
	transportDistance.size = 8;
	unsigned int transportBuff[8] ={0};
	transportDistance.data = transportBuff;
	transportDistance.current = transportBuff;

	// ImageCapture globals
	unsigned int imageData[16] = {0};
	dbuff imageBuffer;
	imageBuffer.data = imageData;
	imageBuffer.size = 16;
	imageBuffer.current = imageData;

	unsigned int imageDataRaw[256] = {0};

	//batteryTemperature globals
	unsigned int batteryTemperature1 = 0;
	unsigned int batteryTemperature2 = 0;
	bool batteryOverTemperature = FALSE;

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
	command = '\0';
	response = '\0';

	//PirateDetection, PirateManagement globals
	bool detected = FALSE;
	bool firePhasor = FALSE;
	bool fireTorpedo = FALSE;
	double pirateDistance = 10000000;
	bool phasorEnable = FALSE;
	bool torpedoEnable = FALSE;

	// Global Timer
	timer = 0;

	unsigned int batteryData;
	batteryLevelPtr = &batteryData;


	char rawCommand[MAX_COMMAND];
	char message[MAX_COMMAND];

	bool displayEn = TRUE;


	signal(SIGUSR1, mySignalHandler);
	isrArray[0] = isr0;
	isrArray[1] = isr1;
	isrArray[2] = isr2;

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

	//enable PWM pins
	pwm = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(pwm,0,SEEK_SET);
	fprintf(pwm, "am33xx_pwm");
	fflush(pwm);

	fprintf(pwm, "bone_pwm_P9_16");
	fflush(pwm);

	fprintf(pwm, "bone_pwm_P9_14");
	fflush(pwm);

	fclose(pwm);

	//set period
	period = fopen("/sys/devices/ocp.3/pwm_test_P9_16.16/period", "w");
	fseek(period,0,SEEK_SET);
	fprintf(period,"%d", PWMPERIOD);
	fflush(period);
	fclose(period);

	//enable signal for solarpanel
	run = fopen("/sys/devices/ocp.3/pwm_test_P9_16.16/run", "w");
	fseek(run,0,SEEK_SET);
	fprintf(run,"%d",1);
	fflush(run);
	fclose(run);

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


	fclose(period);
	fclose(run);

	FILE* transportPin = fopen("/sys/class/gpio/export", "w");
	fseek(transportPin, 0, SEEK_SET);
	fprintf(transportPin, "%d", 115);
	fflush(transportPin);
	fclose(transportPin);

	// Set pin as input
	FILE* transportMode = fopen("/sys/class/gpio/gpio115/direction", "w");
	fseek(transportMode, 0, SEEK_SET);
	fprintf(transportMode, "in");
	fflush(transportMode);
	fclose(transportMode);

	int transportInput = open("/sys/class/gpio/gpio115/value", O_RDONLY);
	/*
	// Creat FIFO
	char * myfifo0 = "/tmp/myfifo0";
	mkfifo(myfifo0, 0666);
	fd0 = open(myfifo0, O_RDWR);
	*/

	// Define data structs for tasks
	powerSubsystemData PSD;
	PSD.solarPanelState = &solarPanelState;
	PSD.solarPanelDeploy = &solarPanelDeploy;
	PSD.solarPanelRetract = &solarPanelRetract;
	PSD.batteryLevelPtr = batteryLevelPtr;
	PSD.powerConsumption = &powerConsumption;
	PSD.powerGeneration = &powerGeneration;
	PSD.batteryTemperature1 = &batteryTemperature1;
	PSD.batteryTemperature2 = &batteryTemperature2;
	PSD.batteryOverTemperature = &batteryOverTemperature;
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
  	SCD.command = &command;
  	SCD.response = &response;
  	SCD.imageDataPtr = &imageBuffer;
  	SCD.rawCommand = rawCommand;
  	SCD.message = message;
  	SCD.displayEn = &displayEn;
  	SCD.batteryTemperature1 = &batteryTemperature1;
  	SCD.batteryTemperature2 = &batteryTemperature2;
  	SCD.transportDistance = &transportDistance;

	commandTaskData CTD;
	CTD.rawCommand = rawCommand;
	CTD.message = message;
	CTD.startSys = &startSys;
	CTD.stopSys = &stopSys;
	CTD.thrusterCommand = &thrusterCommand;
	CTD.transportDistanceData = &transportDistance;
	CTD.imageDataPtr = &imageBuffer;
	CTD.displayEn = &displayEn;
	CTD.command = &command;
	CTD.response = &response;
	CTD.firePhasorPtr = &firePhasor;
	CTD.fireTorpedoPtr = &fireTorpedo;
	CTD.timer = &timer;

	vehicleCommsData VCD;
	VCD.command = &command;
	VCD.response = &response;
	VCD.timer = &timer;
 	//VCD.pipe = &fd0;

	consoleDisplayData CDD;
	CDD.fuelLow = &fuelLow;
	CDD.batteryLow = &batteryLow;
	CDD.solarPanelState = &solarPanelState;
	CDD.batteryLevelPtr = batteryLevelPtr;
	CDD.fuelLevel = &fuelLevel;
	CDD.powerConsumption = &powerConsumption;
	CDD.powerGeneration = &powerGeneration;
	CDD.batteryTemperature1 = &batteryTemperature1;
	CDD.batteryTemperature2 = &batteryTemperature2;
	CDD.batteryOverTemperature = &batteryOverTemperature;
	CDD.transportDistance = &transportDistance;
	CDD.timer = &timer;

	warningAlarmData WAD;
	WAD.fuelLow = &fuelLow;
	WAD.batteryLow = &batteryLow;
	WAD.batteryLevelPtr = batteryLevelPtr;
	WAD.fuelLevel = &fuelLevel;
	WAD.batteryOverTemperature = &batteryOverTemperature;
	WAD.timer = &timer;

	transportDistanceData TDD;
	TDD.transportDistance = &transportDistance;
	TDD.timer = &timer;

	imageCaptureData ICD;
	ICD.imageDataRawPtr = imageDataRaw;
	ICD.imageDataPtr = &imageBuffer;
	ICD.timer = &timer;

	pirateDetectionData PDD;
	PDD.pirateDistancePtr = &pirateDistance;
	PDD.torpedoEnablePtr = &torpedoEnable;
	PDD.phasorEnablePtr = & phasorEnable;
	PDD.firePhasorPtr = &firePhasor;
	PDD.fireTorpedoPtr = &fireTorpedo;
	PDD.detected = &detected;

	pirateManagementData PMD;
	PMD.pirateDistancePtr = &pirateDistance;
	PMD.torpedoEnablePtr = &torpedoEnable;
	PMD.phasorEnablePtr = & phasorEnable;
	PMD.firePhasorPtr = &firePhasor;
	PMD.fireTorpedoPtr = &fireTorpedo;

	// Initialize Task Control Blocks for each task and insert into task queue


	TCB PSDTask;
	PSDTask.taskDataPtr = (void*) &PSD;
	PSDTask.myTask = powerSubsystem;
	PSDTask.prev = NULL;
	PSDTask.next = NULL;
	PSDTask.priority = 1;

	TCB SPDTask;
	SPDTask.taskDataPtr = (void*) &SPD;
	SPDTask.myTask = solarPanelControl;
	SPDTask.prev = NULL;
	SPDTask.next = NULL;
	SPDTask.priority = 1;

	TCB KCDTask;
	KCDTask.taskDataPtr = (void*) &KCD;
	KCDTask.myTask = ConsoleKeypadTask;
	KCDTask.prev = NULL;
	KCDTask.next = NULL;
	KCDTask.priority = 1;

	TCB TSDTask;
	TSDTask.taskDataPtr = (void*) &TSD;
	TSDTask.myTask =  ThrusterSubsystem;
	TSDTask.prev = NULL;
	TSDTask.next = NULL;
	TSDTask.priority = 1;

	TCB SCDTask;
	SCDTask.taskDataPtr = (void*) &SCD;
	SCDTask.myTask = SatelliteComs;
	SCDTask.prev = NULL;
	SCDTask.next = NULL;
	SCDTask.priority = 1;

	TCB CTDTask;
	CTDTask.taskDataPtr = (void*) &CTD;
	CTDTask.myTask = CommandTask;
	CTDTask.prev = NULL;
	CTDTask.next = NULL;
	CTDTask.priority = 1;

	TCB VCDTask;
	VCDTask.taskDataPtr = (void*) &VCD;
	VCDTask.myTask = VehicleComms;
	VCDTask.prev = NULL;
	VCDTask.next = NULL;
	VCDTask.priority = 1;

	TCB CDDTask;
	CDDTask.taskDataPtr = (void*) &CDD;
	CDDTask.myTask = consoleDisplay;
	CDDTask.prev = NULL;
	CDDTask.next = NULL;
	CDDTask.priority = 1;

	TCB WADTask;
	WADTask.taskDataPtr = (void*) &WAD;
	WADTask.myTask = warningAlarm;
	WADTask.prev = NULL;
	WADTask.next = NULL;
	WADTask.priority = 1;


	TDDTask.taskDataPtr = (void*) &TDD;
	TDDTask.myTask = TransportDistance;
	TDDTask.prev = NULL;
	TDDTask.next = NULL;
	TDDTask.priority = 1;

	TCB ICDTask;
	ICDTask.taskDataPtr = (void*) &ICD;
	ICDTask.myTask = ImageCapture;
	ICDTask.prev = NULL;
	ICDTask.next = NULL;
	ICDTask.priority = 1;

	TCB PDDTask;
	PDDTask.taskDataPtr = (void*) &PDD;
	PDDTask.myTask = PirateDetection;
	PDDTask.prev = NULL;
	PDDTask.next = NULL;
	PDDTask.priority = 1;

	TCB PMDTask;
	PMDTask.taskDataPtr = (void*) &PMD;
	PMDTask.myTask = PirateManagement;
	PMDTask.prev = NULL;
	PMDTask.next = NULL;
	PMDTask.priority = 1;

	PSD.SPDTaskPtr = &SPDTask;
	SPD.KCDTaskPtr = &KCDTask;
	SPD.SPDTaskPtr = &SPDTask;
	TDD.TDDTaskPtr = &TDDTask;
	PDD.PMDTaskPtr = &PMDTask;
	VCD.optfftPtr = &ICDTask;
	SCD.commandTaskPtr = &CTDTask;
	CTD.commandTaskPtr = &CTDTask;

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

	taskEnd->next = &ICDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	taskEnd->next = &PDDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	suspendedQ = &SPDTask;

	taskEnd = suspendedQ;
	taskEnd->next = &KCDTask;
	taskEnd->next->prev = suspendedQ;
	taskEnd = taskEnd->next;

	taskEnd->next = &TDDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	taskEnd->next = &PMDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;

	taskEnd->next = &CTDTask;
	taskEnd->next->prev = taskEnd;
	taskEnd = taskEnd->next;





	//char readInput

	sleep(1);
	char interruptVal = '0';
	// Scheduler
	while(1) {
		clock_t start = clock();
		// execute all tasks in the task queue
		TCB* currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(currentTask->taskDataPtr);
			currentTask = currentTask->next;
		}
		fflush(stdout);
		clock_t currentTime = clock();

		// Check that half a second has passed
		while((currentTime - start) < 0.05 * CLOCKS_PER_SEC) {
			if (transportIncomming == FALSE) {
				lseek(transportInput, 0, SEEK_SET);
				read(transportInput, &interruptVal, 1);
				if (interruptVal == '1') {
					isrNum = 2;
					raise(SIGUSR1);
				}
			}
			if(startSys && stopSys){
				isrNum = 0;
				raise(SIGUSR1);
				stopSys = FALSE;
				//add_task();
			} else if (stopSys) {
				//remove_task();
			}
			currentTime = clock();
		}
		//printf("timer: %u", timer);
		//fflush(stdout);
		timer++;
	}
	return 0;
}

void mySignalHandler(int aSig){
	//if(startSys){
		switch(isrNum){
			case 0:{
				isrArray[0]();
				break;
			}
			case 1:{
				isrArray[1]();
				break;
			}
			case 2:{
				isrArray[2]();
				break;
			}
		}
	//}
	//exit(1);
	return;
}

//Power Subsystem interrupt
void isr0(void){
	usleep(600);
	return;
}

//Solar Panel Control interrupt
void isr1(void){
	motorDriveReset = 1;
	return;
}

void isr2(void) {
	printf("\n\ntransport Called!\n\n");
	fflush(stdout);
	transportIncomming = TRUE;
	add_task(&TDDTask);
	return;
}
