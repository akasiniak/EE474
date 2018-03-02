#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "kernel_util.h"

#define INPUTPIN 115

extern bool transportIncomming;

void TransportDistance(void* tdd) {
	transportDistanceData* data = (transportDistanceData*) tdd;
	if (*(data->timer) % 50 == 0) {

	FILE* switchPin;
	FILE* pinMode;
	int switchValue;

	// initalize GPIO pin
	switchPin = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin, 0, SEEK_SET);
	fprintf(switchPin, "%d", INPUTPIN);
	fflush(switchPin);
	fclose(switchPin);

	// Set pin as input
	pinMode = fopen("/sys/class/gpio/gpio115/direction", "w");
	fseek(pinMode, 0, SEEK_SET);
	fprintf(pinMode, "in");
	fflush(pinMode);
	fclose(pinMode);

	// read value of pin
	switchValue = open("/sys/class/gpio/gpio115/value", O_RDONLY);
	char val[2];
	char prev = '\0';
	int count = 0;
	int i;

	for (i = 0; i < 40; i++) {
		lseek(switchValue, 0, SEEK_SET);
		read(switchValue, val, 2);
		
		if (prev == '1' && val[0] == '0') {
			count++;
		}
		prev = val[0];
		
		usleep(483);
		
	}

	if (count == 0) {
		transportIncomming = FALSE;
		printf("Transport Deleted\n");
		delete_task(data->TDDTaskPtr);
	}

	unsigned int distance = -100 * count + 2000;
	//void addBuff(unsigned int* dataAppend, dbuff* debuff);
	//if ((double)distance > *(data->transportDistance->current) * 1.1 || 
	//		(double)distance < *(data->transportDistance->current) * 0.9) {
		addBuff(&distance, data->transportDistance);
		printf("distance: %d\n\n",distance);
	//}
	}


}


