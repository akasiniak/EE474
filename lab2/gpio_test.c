#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"
#include "task_functions.h"


void main() {

	FILE* switchPin1;
	FILE* switchPin2;
	FILE* switchPin3;
	FILE* switchPin4;
	FILE* switchPin5;

	FILE* pinMode1;
	FILE* pinMode2;
	FILE* pinMode3;
	FILE* pinMode4;
	FILE* pinMode5;

	FILE* pinVal1;
	FILE* pinVal2;
	FILE* pinVal3;
	FILE* pinVal4;
	FILE* pinVal5;

	//task 1
	switchPin1 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin1, 0, SEEK_SET);
	fprintf(switchPin1, "%d", 67);
	fflush(switchPin1);

	pinMode1 = fopen("/sys/class/gpio/gpio67/direction", "w");
	fseek(pinMode1, 0, SEEK_SET);
	fprintf(pinMode1, "out");
	fflush(pinMode1);

	pinVal1 = fopen("/sys/class/gpio/gpio67/value", "w");
	fseek(pinVal1, 0, SEEK_SET);
	fprintf(pinVal1, "%d", 1);
	fflush(pinVal1);

	// task 2
	switchPin2 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin2, 0, SEEK_SET);
	fprintf(switchPin2, "%d", 68);
	fflush(switchPin2);

	pinMode2 = fopen("/sys/class/gpio/gpio68/direction", "w");
	fseek(pinMode2, 0, SEEK_SET);
	fprintf(pinMode2, "out");
	fflush(pinMode2);

	pinVal2 = fopen("/sys/class/gpio/gpio68/value", "w");
	fseek(pinVal2, 0, SEEK_SET);
	fprintf(pinVal2, "%d", 0);
	fflush(pinVal2);

	// task 3
	switchPin3 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin3, 0, SEEK_SET);
	fprintf(switchPin3, "%d", 44);
	fflush(switchPin3);

	pinMode3 = fopen("/sys/class/gpio/gpio44/direction", "w");
	fseek(pinMode3, 0, SEEK_SET);
	fprintf(pinMode3, "out");
	fflush(pinMode3);

	pinVal3 = fopen("/sys/class/gpio/gpio44/value", "w");
	fseek(pinVal3, 0, SEEK_SET);
	fprintf(pinVal3, "%d", 1);
	fflush(pinVal3);

	// task 4
	switchPin4 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin4, 0, SEEK_SET);
	fprintf(switchPin4, "%d", 26);
	fflush(switchPin4);

	pinMode4 = fopen("/sys/class/gpio/gpio26/direction", "w");
	fseek(pinMode4, 0, SEEK_SET);
	fprintf(pinMode4, "out");
	fflush(pinMode4);

	pinVal4 = fopen("/sys/class/gpio/gpio26/value", "w");
	fseek(pinVal4, 0, SEEK_SET);
	fprintf(pinVal4, "%d", 0);
	fflush(pinVal4);

		// task 5
	switchPin5 = fopen("/sys/class/gpio/export", "w");
	fseek(switchPin5, 0, SEEK_SET);
	fprintf(switchPin5, "%d", 46);
	fflush(switchPin5);

	pinMode5 = fopen("/sys/class/gpio/gpio46/direction", "w");
	fseek(pinMode5, 0, SEEK_SET);
	fprintf(pinMode5, "out");
	fflush(pinMode5);

	pinVal5 = fopen("/sys/class/gpio/gpio46/value", "w");
	fseek(pinVal5, 0, SEEK_SET);
	fprintf(pinVal5, "%d", 1);
	fflush(pinVal5);


	int count = 0;

	while(1) {
		count++;
	}










}


