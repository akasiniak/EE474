#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "kernel_util.h"

void SatelliteComs(void* scd, rw coms);
void VehicleComms(void* vcd);

int main(){
	fflush(stdout);
	/*
	// Thruster control globals
	unsigned int thrusterCommand = 0;

	// Power Management globals
	unsigned int batteryLevelPtr = 100;
	unsigned short fuelLevel = 100;
	unsigned short powerConsumption = 0;
	unsigned short powerGeneration = 0;

	// Solar Panel Control globals
	bool solarPanelState = FALSE;

	// Warning Alarm globals
	bool fuelLow = FALSE;
	bool batteryLow = FALSE;
	*/
	// Vehichle Comms globals
	char command = '\0';
	char response;

	//unsigned short timer = 0;


/*
	char * myfifo0 = "/tmp/myfifo0";
	mkfifo(myfifo0, 0666);
	int fd0 = open(myfifo0, O_RDWR);
*/
	vehicleCommsData vcd;
	vcd.command = &command;
	vcd.response = &response;
	//vcd.timer = &timer;
	//vcd.pipe = &fd0;
	while (1) {
		printf("Waiting for Command: ");
		scanf("%c", &command);
		VehicleComms((void*) &vcd);
		while(getchar() != '\n');
	}

	//close(fd0);
	/* remove the FIFO */ //unlink(myfifo0);
	//return 0;
}
