#include "kernel_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern unsigned int pastBatteryData[16];
extern unsigned int pastTemperatureData[16];
extern unsigned short temperatureMeasurements;
extern unsigned short voltageMeasurements;

void CommandTask(void* CTD) {
	commandTaskData* data = (commandTaskData*) CTD;
	printf("Command Task Entered\n");
	fflush(stdout);
	char command = data->rawCommand[0];
  int i;
	if ('S' == command) {
		*(data->startSys) = TRUE;
		strcpy(data->message, "A S\0");
	} else if ('P' == command) {
		*(data->stopSys) = TRUE;
		*(data->startSys) = FALSE;
		strcpy(data->message, "A P\0");
	} else if ('D' == command) {
		// toggle display
		if (*(data->displayEn)) {
			*(data->displayEn) = FALSE;
		} else {
			*(data->displayEn) = TRUE;
		}
		strcpy(data->message, "A D\0");
	} else if ('T' == command) {
		// thruster command
	    char* rawThrusterCommand = data->rawCommand + 1;
	    *(data->thrusterCommand) = atoi(rawThrusterCommand);
			strcpy(data->message, "A T\0");
	} else if ('M' == command) {
		if(data->rawCommand[1] == 'D'){
		char temperatureMeasurementsString[3];
		temperatureMeasurementsString[0] = temperatureMeasurements + '0';
		temperatureMeasurementsString[1] = ' ';
		temperatureMeasurementsString[2] = '\0';
		strcpy(data->message, temperatureMeasurementsString);
     	for(i = 0; i < 16; i++){
			char batteryTemperatureString[5];
			snprintf(batteryTemperatureString, 4, "%d", pastTemperatureData[i]);
			strcat(data->message, batteryTemperatureString);
			strcat(data->message, " ");
	    }
	} else if(data->rawCommand[1] == 'B'){
      char batteryMeasurementsString[3];
      batteryMeasurementsString[0] = voltageMeasurements + '0';
      batteryMeasurementsString[1] = ' ';
      batteryMeasurementsString[2] = '\0';
      strcpy(data->message, batteryMeasurementsString);
      for(i = 0; i < 16; i++){
        char batteryLevelString[5];
        snprintf(batteryLevelString, 4, "%d", pastBatteryData[i]);
        strcat(data->message, batteryLevelString);
        strcat(data->message, " ");
      }
		} else if(data->rawCommand[1] == 'T'){
			char transportMeasurementsString[3];
      transportMeasurementsString[0] = data->transportDistanceData->current - data->transportDistanceData->data + '0';
      transportMeasurementsString[1] = ' ';
      transportMeasurementsString[2] = '\0';
      strcpy(data->message, transportMeasurementsString);
      for(i = 0; i < 8; i++){
        char transportDistanceString[5];
        snprintf(transportDistanceString, 4, "%d", data->transportDistanceData->data[i]);
        strcat(data->message, transportDistanceString);
        strcat(data->message, " ");
      }
		} else if(data->rawCommand[1] == 'I'){
      char imageMeasurementsString[3];
      imageMeasurementsString[0] = data->imageDataPtr->current - data->imageDataPtr->data + '0';
      imageMeasurementsString[1] = ' ';
      imageMeasurementsString[2] = '\0';
      strcpy(data->message, imageMeasurementsString);
      for(i = 0; i < 16; i++){
        char imageCaptureString[5];
        snprintf(imageCaptureString, 4, "%d", data->imageDataPtr->data[i]);
        strcat(data->message, imageCaptureString);
        strcat(data->message, " ");
      }
    } else{
      strcpy(data->message, "E\0");
    }
	} else if ('H' == command){
		*(data->firePhasorPtr) = TRUE;
		strcpy(data->message, "H\0");
	} else if ('O' == command){
		*(data->fireTorpedoPtr) = TRUE;
		strcpy(data->message, "O\0");
	} else if ('V' == command){
		*(data->command) = data->rawCommand[1];
		strcpy(data->message, "V\0");
	} else {
		// return an error
		strcpy(data->message, "E\0");
	}
	fflush(stdout);
	data->rawCommand[0] = '\0';
	delete_task(data->commandTaskPtr);
}
