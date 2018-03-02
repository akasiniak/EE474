
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "kernel_util.h"

#define INCREASEPIN 67		// GPIO pin number
#define DECREASEPIN 68

void ConsoleKeypadTask(void* CKD) {
	keyboardConsoleData* data = (keyboardConsoleData*) CKD;

	printf("keyboardConsole Called\n");
	fflush(stdout);
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


		if ('0' == incVal[0]) {
			*(data->driveMotorSpeedInc) = TRUE;
			printf("SPEED INCREASE!!!\n");
			fflush(stdout);
		}
		if ('0' == decVal[0]) {
			*(data->driveMotorSpeedDec) = TRUE;
			printf("SPEED DECREASE!!!\n");
			fflush(stdout);
		}

		fclose(incGpio);
		fclose(pinModeInc);
		fclose(decGpio);
		fclose(pinModeDec);
	}
}




