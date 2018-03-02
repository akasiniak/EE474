//Haobo Zhang
//EE 474
//This implements the task satellitecomms. It takes in a void pointer and returns nothing. It is used to display the information on console.

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

int randomInteger(int low, int high);

void SatelliteComs(void* scd) {
  satelliteComsData* scdPtr = (satelliteComsData*) scd;

  if (*(scdPtr->timer) % 50 == 0) {
    //Generate a random number for thruster command
    *(scdPtr->thrusterCommand) = (unsigned int)randomInteger(0, 65535);

    bool fuelLow = *(scdPtr->fuelLow);
    bool batteryLow = *(scdPtr->batteryLow);
    bool solarPanelState = *(scdPtr->solarPanelState);
    unsigned short batteryLevel = *(scdPtr->batteryLevelPtr);
    unsigned short fuelLevel = *(scdPtr->fuelLevel);
    unsigned short powerConsumption = *(scdPtr->powerConsumption);
    unsigned short powerGeneration = *(scdPtr->powerGeneration);

    FILE* display = fopen("commandFile", "rw");

    char command = '\0';
    fseek(display, 0, SEEK_SET);
    command = fgetc(display);
    if (command >= 'A' && command <= 'Z') {
        *(scdPtr->command) = command;
    } else {
        *(scdPtr->command) = '\0';
    }
    fclose(display);

    
    //Display the information on terminal 2 
    //dbuff* imageDataPtr = *(scd->imageDataPtr);

    //Display the information on terminal 2
    int fd2;
    fd2 = open("/dev/pts/2", O_WRONLY);
    if(fuelLow == TRUE){
      dprintf(fd2, "Fuel Low: TRUE\n");
    }else{
      dprintf(fd2, "Fuel Low: FALSE\n");
    }

    if(batteryLow == TRUE){
      dprintf(fd2, "Battery Low: TRUE\n");
    }else{
      dprintf(fd2, "Battery Low: FALSE\n");
    }

    if (solarPanelState == TRUE) {
      dprintf(fd2, "Solar Panel State: active\n");
    } else {
      dprintf(fd2, "Solar Panel State: inactive\n");
    }


    dprintf(fd2, "Battery Level: %hu\n", batteryLevel);
    dprintf(fd2, "Fuel Level: %hx\n", fuelLevel);
    dprintf(fd2, "Power Consumption: %hu\n", powerConsumption);
    dprintf(fd2, "Power Generation: %hu\n\n", powerGeneration);
    //dprintf(fd2, "Recent Frequency: %u\n", *(imageDataPtr->current));
  }
}
