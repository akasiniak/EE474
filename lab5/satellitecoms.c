//Haobo Zhang, Allan Gilmore
//EE 474
//This implements the task satellitecomms. It takes in a void pointer and returns nothing. It is used to display the information on console.

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

#define MAX_COMMAND 100

int randomInteger(int low, int high);

unsigned int pastBatteryData[16] = {0};
unsigned int pastTemperatureData[16] = {0};
unsigned short temperatureMeasurements = 2;
unsigned short voltageMeasurements = 5;

void SatelliteComs(void* scd) {
  satelliteComsData* scdPtr = (satelliteComsData*) scd;

  if (*(scdPtr->timer) % 50 == 0) {
    //Generate a random number for thruster command
    //*(scdPtr->thrusterCommand) = (unsigned int)randomInteger(0, 65535);

    bool fuelLow = *(scdPtr->fuelLow);
    bool batteryLow = *(scdPtr->batteryLow);
    bool solarPanelState = *(scdPtr->solarPanelState);
    unsigned short batteryLevel = *(scdPtr->batteryLevelPtr);
    unsigned short fuelLevel = *(scdPtr->fuelLevel);
    unsigned short powerConsumption = *(scdPtr->powerConsumption);
    unsigned short powerGeneration = *(scdPtr->powerGeneration);


    // For web interface
    FILE* inputCommand = fopen("/var/lib/cloud9/Satellite_Earth_Station/commandIn", "r");
    char remoteCommand[MAX_COMMAND];
    if (fread(&remoteCommand, sizeof(char), MAX_COMMAND, inputCommand)) {
        strncpy(scdPtr->rawCommand, remoteCommand, MAX_COMMAND);
        add_task(scdPtr->commandTaskPtr);
    }
    freopen("/var/lib/cloud9/Satellite_Earth_Station/commandIn", "w", inputCommand);
    fclose (inputCommand);

    //Display the information on terminal 2
    dbuff* imageDataPtr = scdPtr->imageDataPtr;


    FILE* outputData = fopen("/var/lib/cloud9/Satellite_Earth_Station/commandOut", "w");
    if (scdPtr->displayEn) {
        if (fuelLow == TRUE){
          fprintf(outputData, "Fuel Low: TRUE\n");
        } else {
          fprintf(outputData, "Fuel Low: FALSE\n");
        }

        if (batteryLow == TRUE){
          fprintf(outputData, "Battery Low: TRUE\n");
        } else {
          fprintf(outputData, "Battery Low: FALSE\n");
        }

        if (solarPanelState == TRUE) {
          fprintf(outputData, "Solar Panel State: active\n");
        } else {
          fprintf(outputData, "Solar Panel State: inactive\n");
        }

        fprintf(outputData, "Battery Level: %hu\n", batteryLevel);
        fprintf(outputData, "Fuel Level: %hx\n", fuelLevel);
        fprintf(outputData, "Power Consumption: %hu\n", powerConsumption);
        fprintf(outputData, "Power Generation: %hu\n", powerGeneration);
        fprintf(outputData, "Image Data: %u\n", *(imageDataPtr->current));
        fprintf(outputData, "Transport Distance: %u\n", *(scdPtr->transportDistance->current));
        fprintf(outputData, "Battery Temperature 1: %u\n", *(scdPtr->batteryTemperature1));
        fprintf(outputData, "Battery Temperature 2: %u\n", *(scdPtr->batteryTemperature1));
        fflush(outputData);
        printf(scdPtr->message);
        if (scdPtr->message[0] != '\0') {
            fprintf(outputData, "Response: %s\n", scdPtr->message);
            fflush(outputData);
        }
    }
    fflush(outputData);
    fclose(outputData);

    scdPtr->message[0] = '\0';


  }
}
