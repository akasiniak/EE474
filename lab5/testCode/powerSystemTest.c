#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "kernel_util.h"
#include "task_functions.h"
#include <pthread.h>
void powerSubsystem(void* PSD);
void* batteryTemperature(void* PSD);
int main(){
  powerSubsystemData PSD;
  void* p = &PSD;
  int i;
  bool state = TRUE;
  bool deploy = FALSE;
  bool retract = FALSE;
  unsigned short consumption = 0;
  unsigned short generation = 0;
  bool bot = FALSE;
  unsigned int batt1 = 0;
  unsigned int batt2 = 0;
  unsigned short t = 0;
  PSD.solarPanelState = &state;
  PSD.solarPanelDeploy = &deploy;
  PSD.solarPanelRetract = &retract;
  unsigned int batteryData[16];
  PSD.batteryLevelPtr = batteryData;
  PSD.powerConsumption = &consumption;
  PSD.powerGeneration = &generation;
  PSD.batteryOverTemperature = &bot;
  PSD.batteryTemperature1 = &batt1;
  PSD.batteryTemperature2 = &batt2;
  PSD.timer = &t;
  //initialize A/D
  FILE *ain;
  ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  fseek(ain, 0, SEEK_SET);
  fprintf(ain, "cape-bone-iio");
  fflush(ain);
  while(1){
    powerSubsystem(p);
    printf("solar panel state: %d\n", state);
    printf("solar panel deploy: %d\n", deploy);
    printf("solar panel retract: %d\n", retract);
    printf("battery level: %d\n", *(PSD.batteryLevelPtr));
    printf("scaled battery level %f\n", (*(PSD.batteryLevelPtr)/1000000.0));
    printf("power consumption: %hu\n", consumption);
    printf("power generation; %d\n", generation);
    printf("batteryTemeperature 1: %f\n", (((float)batt1 * 32.0)/100.0 + 32.0));
    printf("batteryTemeperature 2: %f\n", ((batt2 * 32.0)/100.0 + 32.0));
    printf("battery over temperature: %d\n", bot);
    printf("\n");
    for(i = 0; i < 100000000; i++);
  }
  return 0;
}

//Alex Kasiniak
//E E 474
//This is the power subsystem. It updates the power consumption, power generation, solar panel state, and battery level variables.
//The class has an input of the PSD data struct, and no outputs, and updates the variables according to the requirements in the spec.

pthread_t batteryTemp;

void powerSubsystem(void* PSD) {
	powerSubsystemData* data = (powerSubsystemData*) PSD;

  static bool batteryTempCreated = FALSE;
  int threadError;
  if (*(data->timer) % 10 == 0) {
		static unsigned short call = 0;
		static unsigned short measurements = 0;
    unsigned int pastBatteryData[16];
		float batteryLevelPercentage;
		static int consumptionDecrement = 0;

		//updating power consumption
		if((int)*(data->powerConsumption) < 5 && consumptionDecrement) {
			consumptionDecrement = 0;
		} else if((int)*(data->powerConsumption) > 10) {
			consumptionDecrement = 1;
		}

		*(data->powerConsumption) += (-3 * (call%2) + 2) * (1 - 2 * consumptionDecrement);

		//start taking measurements
		FILE *aval;
		int value;
		aval = fopen("/sys/devices/ocp.3/helper.15/AIN0", "r");
    fseek(aval,0,SEEK_SET);
    fscanf(aval,"%d",&value);
    fclose(aval);
		*(data->batteryLevelPtr) = value * 20191;
		batteryLevelPercentage = *(data->batteryLevelPtr)/360000.0;
		pastBatteryData[measurements] = *(data->batteryLevelPtr);
		measurements++;
		if(measurements == 16){
			measurements = 0;
		}

		*(data->powerGeneration) += *(data->solarPanelState) * (2 * (1-(call%2)) + 1 * (call%2) * (batteryLevelPercentage <= 50));
		fflush(stdout);
		if(batteryLevelPercentage > 95) {
			*(data->powerGeneration) = 0;
			*(data->solarPanelRetract) = TRUE;
			add_task(data->SPDTaskPtr);
		}
		if(batteryLevelPercentage <= 10){
			*(data->solarPanelDeploy) = TRUE;
			add_task(data->SPDTaskPtr);
		}
    call++;
  }
	if(*(data->solarPanelState)){
		if(!batteryTempCreated){
			threadError = pthread_create(&batteryTemp, NULL, batteryTemperature, PSD);
			batteryTempCreated = TRUE;
		}
	} else {
		if(batteryTempCreated){
			pthread_cancel(batteryTemp);
			batteryTempCreated = FALSE;
		}
	}
}

void* batteryTemperature(void* PSD){
  powerSubsystemData* data = (powerSubsystemData*) PSD;

  static unsigned short measurements;
	static short read = -2;
	unsigned int pastGreaterTemperature;
	unsigned int greaterTemperature;
  unsigned int pastTemperatureData[16];
  float percentDifference = 0;
  //start taking measurements
  while(1){
		FILE *aval;
	  int value;
	  aval = fopen("/sys/devices/ocp.3/helper.15/AIN2", "r");
	  fseek(aval,0,SEEK_SET);
	  fscanf(aval,"%d",&value);
	  fclose(aval);
	  *(data->batteryTemperature1) = value;
	  pastTemperatureData[measurements] = value;
	  measurements++;
	  aval = fopen("/sys/devices/ocp.3/helper.15/AIN3", "r");
	  fseek(aval,0,SEEK_SET);
	  fscanf(aval,"%d",&value);
	  fclose(aval);
	  *(data->batteryTemperature2) = value;
	  pastTemperatureData[measurements] = value;
	  measurements++;
		if(!(read == -2)){
			if(pastTemperatureData[read] > pastTemperatureData[read + 1]){
				pastGreaterTemperature = pastTemperatureData[read];
			} else {
				 pastGreaterTemperature = pastTemperatureData[read + 1];
			}
			if(*(data->batteryTemperature1) > *(data->batteryTemperature2)){
				greaterTemperature = *(data->batteryTemperature1);
			} else {
				 greaterTemperature = *(data->batteryTemperature2);
			}
			percentDifference = ((float)greaterTemperature - (float)pastGreaterTemperature) / (float)pastGreaterTemperature;
		}
    read += 2;
	  percentDifference = percentDifference * 100;
	  if(percentDifference > 20){
	    *(data->batteryOverTemperature) = TRUE;
	  }
	  if(measurements == 16){
	    measurements = 0;
	  }
		if(read == 16){
			read = 0;
		}
		usleep(500);
	}
}
