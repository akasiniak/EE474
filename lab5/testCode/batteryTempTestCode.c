#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "kernel_util.h"
#include "task_functions.h"
void batteryTemperature(void* PSD);
int main(){
  powerSubsystemData PSD;
  void* d = &PSD;
  int i;
  bool state = FALSE;
  bool deploy = FALSE;
  bool retract = FALSE;
  unsigned short consumption = 0;
  unsigned short generation = 0;
  unsigned int batteryData = 0;
  unsigned short t = 0;
  unsigned int batt1 = 0;
  unsigned int batt2 = 0;
  bool overTemp = FALSE;
  PSD.solarPanelState = &state;
  PSD.solarPanelDeploy = &deploy;
  PSD.solarPanelRetract = &retract;
  PSD.batteryLevelPtr = &batteryData;
  PSD.powerConsumption = &consumption;
  PSD.powerGeneration = &generation;
  PSD.timer = &t;
  PSD.batteryTemperature1 = &batt1;
  PSD.batteryTemperature2 = &batt2;
  PSD.batteryOverTemperature = &overTemp;
  FILE *ain;
  ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  fseek(ain, 0, SEEK_SET);
  fprintf(ain, "cape-bone-iio");
  fflush(ain);
  while(1){
    batteryTemperature(d);
    float battTemp1 = ((float)batt1 * 32.0)/100.0 + 33.0;
    float battTemp2 = ((float)batt2 * 32.0)/100.0 + 33.0;
    printf("battery 1 temperature: %f\n", battTemp1);
    printf("battery 2 temperature: %f\n", battTemp2);
    printf("batter over temperature: %d\n\n", overTemp);
    sleep(1);
  }
}

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
  printf("batteryTemp 1 in function : %d\n", *(data->batteryTemperature1));
  pastTemperatureData[measurements] = 10 * value;
  measurements++;
  usleep(500);
  aval = fopen("/sys/devices/ocp.3/helper.15/AIN3", "r");
  fseek(aval,0,SEEK_SET);
  fscanf(aval,"%d",&value);
  fclose(aval);
  *(data->batteryTemperature2) = value;
  printf("batteryTemp 2 in function : %d\n", *(data->batteryTemperature2));
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
    //remove_task(*(data->BTTempPtr));
    printf("remove\n");
  }
}
