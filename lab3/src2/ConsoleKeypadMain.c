#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

void ConsoleKeypadTask(void* CKD);

int main() {
	bool driveMotorSpeedInc = FALSE;
	bool driveMotorSpeedDec = FALSE;
	unsigned int timer = 0;

	keyboardConsoleData KCD;
	KCD.driveMotorSpeedInc = &driveMotorSpeedInc;
	KCD.driveMotorSpeedDec = &driveMotorSpeedDec;
	KCD.timer = &timer;

	while (1) {
		ConsoleKeypadTask((void*) &KCD);
		sleep(1);
		printf("increase: %i\n", driveMotorSpeedInc);
		printf("decrease: %i\n\n", driveMotorSpeedDec);
		//timer++;
	}
	return 0;
}