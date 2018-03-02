#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "kernel_util.h"
#include "task_functions.h"
#include <string.h>
#include <stdlib.h>
int pastBatteryData[16];
int pastTemperatureData[16];
unsigned int temperatureMeasurements = 3;
unsigned int batteryMeasurements = 2;

void CommandTask(void* CTD);
int main(){
  commandTaskData CTD;
  void* d = &CTD;
  char rcmd[7] = {'T', '8', '4', '2', '6', '8', '6'};
  char mess[100] = {'\0'};
  bool sysStart = FALSE;
  bool sysStop = FALSE;
  unsigned int imageData[16] = {0};
  dbuff imageBuffer;
  imageBuffer.data = imageData;
  imageBuffer.size = 16;
  imageBuffer.current = imageData + 1;
  dbuff transportDistance;
  transportDistance.size = 8;
  unsigned int transportBuff[8];
  transportDistance.data = transportBuff;
  transportDistance.current = transportBuff;
  transportDistance.current = transportBuff + 2;
  int j;
  for(j = 0; j < 16; j++){
    imageData[j] = j * 9;
  }
  for(j = 0; j < 8; j++){
    transportBuff[j] = 2 * j;
  }
  for(j = 0; j < 16; j++){
    pastTemperatureData[j] = j * 5;
  }
  for(j = 0; j < 16; j++){
    pastBatteryData[j] = j * 8;
  }
  unsigned int tcmd = 0b0000000000000000;
  unsigned int batteryLevel = 15;
  unsigned int batteryTemp1 = 96;
  unsigned int batteryTemp2 = 76;
  bool enableDis = FALSE;
  bool phasorFire = FALSE;
  bool torpedoFire = FALSE;
  char vehiclecmd = 'P';
  CTD.rawCommand = rcmd;
  CTD.message = mess;
  CTD.startSys = &sysStart;
  CTD.stopSys = &sysStop;
  CTD.thrusterCommand = &tcmd;
  CTD.batteryLevePtr = &batteryLevel;
  CTD.batteryTemperature1 = &batteryTemp1;
  CTD.batteryTemperature2 = &batteryTemp2;
  CTD.displayEn = &enableDis;
  CTD.firePhasorPtr = &phasorFire;
  CTD.fireTorpedoPtr = &torpedoFire;
  CTD.vehicleCommand = &vehiclecmd;
  CTD.transportDistanceData = &transportDistance;
  CTD.imageDataPtr = &imageBuffer;
  printf("raw command %c\n", rcmd[0]);
  CommandTask(d);
  printf("message: %s\n", mess);
  return 0;
}
void CommandTask(void* CTD) {
	commandTaskData* data = (commandTaskData*) CTD;
	printf("Command Task Entered\n");
	fflush(stdout);
	char command = data->rawCommand[0];
  int i;
	if ('S' == command) {
		*(data->startSys) = TRUE;
    *(data->stopSys) = FALSE;
		printf("S command received\n");
		strcpy(data->message, "A S\0");
	} else if ('P' == command) {
		*(data->stopSys) = TRUE;
    *(data->startSys) = FALSE;
		printf("P command received\n");
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
    printf("\n\nReceived Thruster Command = %i\n\n", *(data->thrusterCommand));
    fflush(stdout);
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
      batteryMeasurementsString[0] = batteryMeasurements + '0';
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
		printf("M command received\n");
	} else if ('H' == command){
		*(data->firePhasorPtr) = TRUE;
	} else if ('O' == command){
		*(data->fireTorpedoPtr) = TRUE;
	} else if ('V' == command){
		*(data->vehicleCommand) = data->rawCommand[1];
	} else {
		// return an error
		strcpy(data->message, "E\0");
	}
	fflush(stdout);
	data->rawCommand[0] = '\0';
	//delete_task(data->commandTaskPtr);
  printf("delete command task\n");
}
