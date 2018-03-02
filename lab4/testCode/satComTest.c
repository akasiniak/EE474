#include <fcntl.h> 
#include <string.h> 
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "../kernel_util.h"

void SatelliteComs(void* scd);

void main() {


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



	while (1) {
		SatelliteComs((void*) &SCD);
		printf("command: %c\n", command);
		printf("response: %c\n", respon
		sleep(1);
	}
}





