#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../kernel_util.h"

void consoleDisplay(void* CDD);

int main() {
		// Thruster control globals
	unsigned int thrusterCommand = 0;

	//test
	// Power Management globals
	unsigned int batteryLevel = 100;
	unsigned short fuelLevel = 100;
	unsigned short powerConsumption = 0;
	unsigned short powerGeneration = 0;

	unsigned int batteryTemperature1 = 10;
	unsigned int batteryTemperature2 = 15;

	// Solar Panel Control globals
	bool solarPanelState = FALSE;

	// Warning Alarm globals
	bool fuelLow = FALSE;
	bool batteryLow = FALSE;

	unsigned short timer = 0;

	consoleDisplayData CDD;
	CDD.fuelLow = &fuelLow;
	CDD.batteryLow = &batteryLow;
	CDD.solarPanelState = &solarPanelState;
	CDD.batteryLevelPtr = &batteryLevel;
	CDD.fuelLevel = &fuelLevel;
	CDD.powerConsumption = &powerConsumption;
	CDD.batteryTemperature1 = &batteryTemperature1;
	CDD.batteryTemperature2 = &batteryTemperature2;
	CDD.timer = &timer;
	while (1) {
		consoleDisplay((void*) &CDD);
		sleep(1);
		powerConsumption++;

	}
	return 0;
}
