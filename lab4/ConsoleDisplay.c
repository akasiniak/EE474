////////////////////////////
// Author: Allan Gilmore  //
// date: 7/11/17		  //
////////////////////////////

// Description: This program takes the status and annucations data from the
//				satellite system and displays the data on the on-board console.
//				Reads a gpio pin and displays the status information when the
//				pin is high and annucations when the pin is low.
//				Updates on a 5 second interval.
// Reference: http://derekmolloy.ie/gpios-on-the-beaglebone-black-using-device-tree-overlays/
//	- used as reference for working with GPIO pins.

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

#define INPUTPIN 60		// GPIO pin number

void consoleDisplay(void* CDD) {
	consoleDisplayData* data = (consoleDisplayData*) CDD;

	// Check if 5 seconds has passed
	if (*(data->timer) % 50 == 0) {

		FILE* switchPin;
		FILE* pinMode;
		int switchValue;

		// initalize GPIO pin
		switchPin = fopen("/sys/class/gpio/export", "w");
		fseek(switchPin, 0, SEEK_SET);
		fprintf(switchPin, "%d", INPUTPIN);
		fflush(switchPin);

		// Set pin as input
		pinMode = fopen("/sys/class/gpio/gpio60/direction", "w");
		fseek(pinMode, 0, SEEK_SET);
		fprintf(pinMode, "in");
		fflush(pinMode);

		// read value of pin
		switchValue = open("/sys/class/gpio/gpio60/value", O_RDONLY);
		char val[2];
		read(switchValue, val, 2);


		// open console window connection
		int display = open("/dev/pts/1", O_WRONLY);

		if (val[0] > '0') {
			dprintf(display, "Status:\n");
			if (*(data->solarPanelState)) {
				dprintf(display, "Solar Panel State: active\n");
			} else {
				dprintf(display, "Solar Panel State: inactive\n");
			}
			dprintf(display, "Battery Level: %f\n", *(data->batteryLevelPtr)/1000000.0);
			dprintf(display, "Fuel Level: %hu\n", *(data->fuelLevel));
			dprintf(display, "Power Consumption: %hu\n", *(data->powerConsumption));
			dprintf(display, "batteryTemp1: %f\n", ((32.0 * *(data->batteryTemperature1))/100.0 + 33.0));
			dprintf(display, "batteryTemp2: %f\n", ((32.0 * *(data->batteryTemperature2))/100.0 + 33.0));
			/*
			if (data->transportDistance->current != NULL) {
				dprintf(display, "transportDistance: %i\n\n", *(data->transportDistance->current));
			}
			*/
			
		} else {
			dprintf(display, "Annucations:\n");
			if (*(data->fuelLow)) {
				dprintf(display, "Fuel Low: TRUE\n");
			} else {
				dprintf(display, "Fuel Low: FALSE\n");
			}

			if (*(data->batteryLow)) {
				dprintf(display, "Battery Low: TRUE\n\n");
			} else {
				dprintf(display, "Battery Low: FALSE\n\n");
			}
			if (*(data->batteryOverTemperature)) {
				dprintf(display, "Battery Over Temperature: TRUE\n\n");
			} else {
				dprintf(display, "Battery Over Temperature: FALSE\n\n");
			}
		}
		fclose(switchPin);
		fclose(pinMode);
		close(switchValue);
	}

}
