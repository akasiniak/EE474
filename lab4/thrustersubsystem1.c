//Haobo Zhang
//EE 474
//ThrusterSubSystem is used to control the direction, magnitude and duration of thruster. It also updates the fuelLevel accordingly.

#include "kernel_util.h"
#include <stdio.h>

#define FUELRATE 25
#define PWMPERIOD 500000


void ThrusterSubsystem(void* tsd){
    static unsigned short duration;
    
    //static int seconds;

    thrusterSubsystemData* tsdPtr = (thrusterSubsystemData*) tsd;


    if(*(tsdPtr->timer) % 2 == 0) {
      if(duration == 0){
        thruster
      }

    }


    if(*(tsdPtr->timer) % 10 == 0) {

        unsigned short left = 0;
        unsigned short right = 0;
        unsigned short up = 0;
        unsigned short down = 0;

        unsigned int thrusterCommand = *(tsdPtr->thrusterCommand);
        unsigned short fuelLevel = *(tsdPtr->fuelLevel);
        //Bit mask the detail information of thruster command
        unsigned short thruster = thrusterCommand & 0x000F;

        left = thruster & 0x0001;
        right = (thruster & 0x0002) >> 1;
        up = (thruster & 0x0004) >> 2;
        down = (thruster & 0x0008) >> 3;
        unsigned short magnitude = (thrusterCommand & 0x00F0) >> 4;
        unsigned short duration = (thrusterCommand & 0xFF00) >> 8;

        //set duty cycle according to magnitude
        FILE* duty = fopen("/sys/devices/ocp.3/pwm_test_P9_14.17/duty", "w");
        fseek(duty,0,SEEK_SET);
        fprintf(duty,"%d", (int)(magnitude/15.0 * PWMPERIOD));
    	  fflush(duty);
        fclose(duty);
        //Updates the fuelLevel
        fuelLevel = fuelLevel - FUELRATE * (magnitude / (double)15) * (duration / (double)255);
        *(tsdPtr->fuelLevel) = fuelLevel;
        //Print out the thruster information and fuelLevel
        printf("Thruster Left: %hu\n", left);
        printf("Thruster Right: %hu\n", right);
        printf("Thruster Up: %hu\n", up);
        printf("Thruster Down: %hu\n", down);
        printf("Magnitude is: %hx\n", magnitude);
        printf("Duration is: %hx\n", duration);
        printf("FuelLevel is: %hu\n", fuelLevel);
        printf("ThrusterCommand is: %x\n\n", thrusterCommand);
    }

}
