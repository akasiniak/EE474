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

	// Vehichle Comms globals
	char command;
	char response;

  unsigned short timer = 0;



	char * myfifo0 = "/tmp/myfifo0";
	mkfifo(myfifo0, 0666);
	int fd0 = open(myfifo0, O_RDWR);

	vehicleCommsData vcd;
	vcd.command = &command;
	vcd.response = &response;
  vcd.timer = &timer;
	vcd.pipe = &fd0;

	satelliteComsData scd;
	scd.fuelLow = &fuelLow;
	scd.batteryLow = &batteryLow;
	scd.solarPanelState = &solarPanelState;
	scd.batteryLevelPtr = &batteryLevelPtr;
	scd.fuelLevel = &fuelLevel;
	scd.powerConsumption = &powerConsumption;
	scd.powerGeneration = &powerGeneration;
	scd.thrusterCommand = &thrusterCommand;
  scd.timer = &timer;
	scd.pipe = &fd0;

 	while(1){
		SatelliteComs((void*) &scd, wrt);
		sleep(1);
		VehicleComms((void*) &vcd);
    sleep(1);
    SatelliteComs((void*) &scd, rd);
    sleep(1);
	}
	close(fd0);
	/* remove the FIFO */ //unlink(myfifo0);
	return 0;
}
