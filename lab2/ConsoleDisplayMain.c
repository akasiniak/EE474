#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

void ConsoleDisplay(void* CDD);

int main() {
		// Thruster control globals
	unsigned int thrusterCommand = 0;

	//test
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

	unsigned short timer = 0;

	consoleDisplayData CDD;
	CDD.fuelLow = &fuelLow;
	CDD.batteryLow = &batteryLow;
	CDD.solarPanelState = &solarPanelState;
	CDD.batteryLevel = &batteryLevel;
	CDD.fuelLevel = &fuelLevel;
	CDD.powerConsumption = &powerConsumption;
	CDD.powerGeneration = &powerGeneration;
	CDD.timer = &timer;
	while (1) {
		ConsoleDisplay((void*) &CDD);
		sleep(1);
		timer += 5;
		timer = timer % 10;
	}
	return 0;
}
