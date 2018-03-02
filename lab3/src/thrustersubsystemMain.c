#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

void ThrusterSubsystem(void* tsd);
int randomInteger(int low, int high);


int main() {
	// Thruster control globals
	randomInteger(0, 65535);
	unsigned int thrusterCommand = (unsigned int)randomInteger(0, 65535);

	// Power Management globals
	unsigned short batteryLevelPtr = 100;
	unsigned short fuelLevel = 100;
	unsigned short powerConsumption = 0;
	unsigned short powerGeneration = 0;

	// Solar Panel Control globals
	bool solarPanelState = FALSE;

	// Warning Alarm globals
	bool fuelLow = FALSE;
	bool batteryLow = FALSE;

  unsigned short timer = 0;

  //initialize PWM
	FILE *pwm, *run *period;

	//enable pins
	pwm = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(pwm,0,SEEK_SET);
	fprintf(pwm, "am33xx_pwm");
	fflush(pwm);

  fprintf(pwm, "bone_pwm_P9_14");
  fflush(pwm);

  //enable signal for thruster
	run = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/run", "w");
	fseek(run,0,SEEK_SET);
	fprintf(run,"%d",1);
	fflush(run);

	//set period according to duration
	period = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w");
	fseek(period,0,SEEK_SET);
	fprintf(period,"%d", 500000);
	fflush(period);


	thrusterSubsystemData tsd;
  tsd.thrusterCommand = &thrusterCommand;
	tsd.fuelLevel = &fuelLevel;
  tsd.timer = &timer;
	while (1) {
    unsigned int thrusterCommand = (unsigned int)randomInteger(0, 65535);
    tsd.thrusterCommand = &thrusterCommand;
		ThrusterSubsystem((void*) &tsd);
		sleep(1);
	}
	return 0;
}
