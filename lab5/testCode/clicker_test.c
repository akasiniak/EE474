#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include "kernel_util.h"
//#include "task_functions.h"


void main() {

	FILE* switchPin;
	FILE* pinMode;
	FILE* switchValue;

	// initalize GPIO pin
	switchPin = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin, 0, SEEK_SET);
	fprintf(switchPin, "%d", 20);
	fflush(switchPin);
	fclose(switchPin);

	// Set pin as input
	pinMode = fopen("/sys/class/gpio/gpio20/direction", "w");
	fseek(pinMode, 0, SEEK_SET);
	fprintf(pinMode, "out");
	fflush(pinMode);
	fclose(pinMode);
	while (1) {

		switchValue = fopen("/sys/class/gpio/gpio20/value", "w");
		fseek(switchValue, 0, SEEK_SET);
		fprintf(switchValue, "1");
		fflush(switchValue);
		usleep(100000);

		fseek(switchValue, 0, SEEK_SET);
		fprintf(switchValue, "0");
		fflush(switchValue);
		usleep(100000);
	}

}