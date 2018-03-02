#include <fcntl.h> 
#include <string.h> 
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "../kernel_util.h"
#include "../task_functions.h"

void SatelliteComs(void* scd);



int main() {


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

	unsigned int batteryTemperature1 = 10;
	unsigned int batteryTemperature2 = 20;



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
	command = '\0';
	response = '\0';

	// Global Timer
	timer = 0;

	char rawCommand[100];
	rawCommand[0] = '\0';
	char message[100];
	message[0] = '\0';

	bool displayEn = TRUE;

	unsigned int imageData[16] = {0};
	dbuff imageBuffer;
	imageBuffer.data = imageData;
	imageBuffer.size = 16;
	imageBuffer.current = imageData;

	unsigned int transportData[8] = {0};
	dbuff transportBuffer;
	transportBuffer.data = transportData;
	transportBuffer.size = 8;
	transportBuffer.current = transportData;

	bool startSys = FALSE;
	bool stopSys = FALSE;

	bool firePhasor = FALSE;
	bool fireTorpedo = FALSE;

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
	SCD.rawCommand = rawCommand;
	SCD.message = message;
	SCD.displayEn = &displayEn;
	SCD.imageDataPtr = &imageBuffer;
	SCD.batteryTemperature1 = &batteryTemperature1;
	SCD.batteryTemperature2 = &batteryTemperature2;
	SCD.transportDistance = &transportBuffer;

	commandTaskData CTD;
	CTD.rawCommand = rawCommand;
	CTD.message = message;
	CTD.startSys = &startSys;
	CTD.stopSys = &stopSys;
	CTD.thrusterCommand = &thrusterCommand;
	CTD.transportDistanceData = &transportBuffer;
	CTD.imageDataPtr = &imageBuffer;
	CTD.displayEn = &displayEn;
	CTD.command = &command;
	CTD.response = &response;
	CTD.firePhasorPtr = &firePhasor;
	CTD.fireTorpedoPtr = &fireTorpedo;
	CTD.timer = &timer;


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
	SCDTask.priority = 1;

	SCD.commandTaskPtr = &CTDTask;
	CTD.commandTaskPtr = &CTDTask;

	taskQ = &SCDTask;
	suspendedQ = &CTDTask;

	TCB* currentTask;

	while (1) {
		currentTask = taskQ;
		while (currentTask != NULL) {
			currentTask->myTask(currentTask->taskDataPtr);
			currentTask = currentTask->next;
		}		
		printf("rawCommand: %s\n", rawCommand);
		printf("message: %s\n", message);
		printf("thrusterCommand: %i\n", thrusterCommand);
		sleep(1);
	}

	return 0;
}





