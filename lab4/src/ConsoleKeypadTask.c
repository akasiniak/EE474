////////////////////////////
// Author: Allan Gilmore  //
// date: 7/25/17		  //
////////////////////////////

// Description: This program takes the input from two buttons and sets whether the speed of the
//				a motor driver should be increased or decreased by %5.
// Reference: http://derekmolloy.ie/gpios-on-the-beaglebone-black-using-device-tree-overlays/
//	- used as reference for working with GPIO pins.

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "kernel_util.h"

#define INCREASEPIN 67		// GPIO pin number
#define DECREASEPIN 68		// GPIO pin nummber

void ConsoleKeypadTask(void* CKD) {
	keyboardConsoleData* data = (keyboardConsoleData*) CKD;
	// run every 2 seconds
	if (*(data->timer) % 4 == 0) {

		int incSig;
		int decSig;

		FILE* incGpio = fopen("/sys/class/gpio/export", "w");
		fseek(incGpio, 0, INCREASEPIN);
		fprintf(incGpio, "%d", INCREASEPIN);
		fflush(incGpio);

		// Set pin as input
		FILE* pinModeInc = fopen("/sys/class/gpio/gpio67/direction", "w");
		fseek(pinModeInc, 0, SEEK_SET);
		fprintf(pinModeInc, "in");
		fflush(pinModeInc);

		// read value of pin
		incSig = open("/sys/class/gpio/gpio67/value", "O_RDONLY");
		char incVal[2];
		read(incSig, incVal, 2);


		FILE* decGpio = fopen("/sys/class/gpio/export", "w");
		fseek(decGpio, 0, DECREASEPIN);
		fprintf(decGpio, "%d", DECREASEPIN);
		fflush(decGpio);

		// Set pin as input
		FILE* pinModeDec = fopen("/sys/class/gpio/gpio68/direction", "w");
		fseek(pinModeDec, 0, SEEK_SET);
		fprintf(pinModeDec, "in");
		fflush(pinModeDec);

		// read value of pin
		decSig = open("/sys/class/gpio/gpio68/value", "O_RDONLY");
		char decVal[2];
		printf(decVal);
		read(decSig, decVal, 2);

		// set the speed to increase
		if ('0' == incVal[0]) {
			*(data->driveMotorSpeedInc) = TRUE;
			printf("SPEED INCREASE!!!\n");
			fflush(stdout);
		}
		// set the speed to decrease
		if ('0' == decVal[0]) {
			*(data->driveMotorSpeedDec) = TRUE;
			printf("SPEED DECREASE!!!\n");
			fflush(stdout);
		}
		// close files
		fclose(incGpio);
		fclose(pinModeInc);
		fclose(decGpio);
		fclose(pinModeDec);
	}
}




