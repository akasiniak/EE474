#include "kernel_util.h"
#include <stdio.h>

void batteryTemperature(void* PSD){
  powerSubsystemData* data = (powerSubsystemData*) PSD;

  static unsigned short measurements;
  unsigned int pastTemperatureData[16];
  float percentDifference;
  //start taking measurements
  FILE *aval;
  int value;
  aval = fopen("/sys/devices/ocp.3/helper.15/AIN2", "r");
  fseek(aval,0,SEEK_SET);
  fscanf(aval,"%d",&value);
  fclose(aval);
  *(data->batteryTemperature1) = value;
  pastTemperatureData[measurements] = 10 * value;
  measurements++;
  usleep(500);
  FILE *aval;
  aval = fopen("/sys/devices/ocp.3/helper.15/AIN3", "r");
  fseek(aval,0,SEEK_SET);
  fscanf(aval,"%d",&value);
  fclose(aval);
  *(data->batteryTemperature2) = value;
  pastTemperatureData[measurements] = 10 * value;
  measurements++;
  percentDifference = ((float)*(data->batteryTemperature1) - (float)*(data->batteryTemperature2)) / (float)*(data->batteryTemperature1);
  percentDifference = percentDifference * 100;
  if(percentDifference > 20){
    *(data->batteryOverTemperature) = TRUE;
  }
  if(measurements == 16){
    measurements = 0;
  }
  if(!*(data->solarPanelState)){
    remove_task(*(data->BTTempPtr));
  }
}
