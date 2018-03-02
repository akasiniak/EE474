//Haobo Zhang
//EE 474
//This implements the task satellitecomms. It takes in a void pointer and returns nothing. It is used to display the information on console.

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "kernel_util.h"
#define MAX_BUF 1024


int randomInteger(int low, int high);

void SatelliteComs(void* scd, rw coms){

  satelliteComsData* scdPtr = (satelliteComsData*) scd;

  if (0 == *(scdPtr->timer)) {
    int fd2;
    fd2 = open("/dev/pts/0", O_WRONLY);
    int fd0 = *(scdPtr -> pipe);


    switch(coms){
      case wrt:{

        bool fuelLow = *(scdPtr->fuelLow);
        bool batteryLow = *(scdPtr->batteryLow);
        bool solarPanelState = *(scdPtr->solarPanelState);
        unsigned short batteryLevelPtr = *(scdPtr->batteryLevelPtr);
        unsigned short fuelLevel = *(scdPtr->fuelLevel);
        unsigned short powerConsumption = *(scdPtr->powerConsumption);
        unsigned short powerGeneration = *(scdPtr->powerGeneration);


    //Display the information on terminal 2
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


        float scaledBatteryLevel = batteryLevelPtr/3600000.0;
        dprintf(fd2, "Battery Level: %f\n", batteryLevelPtr);
        dprintf(fd2, "Fuel Level: %hx\n", fuelLevel);
        dprintf(fd2, "Power Consumption: %hu\n", powerConsumption);
        dprintf(fd2, "Power Generation: %hu\n\n", powerGeneration);

        unsigned int numberForCommand = (unsigned int)randomInteger(0, 65535);
        char commands[6] = {'F', 'B', 'L', 'R', 'D', 'H'};
        char vehicleCommand = commands[numberForCommand%6];

        int size = sizeof("A");
        write(fd0, &vehicleCommand, size);
        dprintf(fd2, "Command: %c\n", vehicleCommand);
        break;
      }
      case rd:
      {
        char buf[MAX_BUF];
        read(fd0, buf, MAX_BUF);
        dprintf(fd2, "Response: %s\n\n", buf);
        break;
      }

    }

  }
  return;
}
