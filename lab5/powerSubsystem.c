//Alex Kasiniak
//E E 474
//This is the power subsystem. It updates the power consumption, power generation, solar panel state, and battery level variables.
//The class has an input of the PSD data struct, and no outputs, and updates the variables according to the requirements in the spec.
#include "kernel_util.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_t batteryTemp;
void* batteryTemperature(void* PSD);

unsigned int pastBatteryData[16];
unsigned int pastTemperatureData[16];
static unsigned short voltageMeasurements = 0;
static unsigned short temperatureMeasurements = 0;

void powerSubsystem(void* PSD) {
	powerSubsystemData* data = (powerSubsystemData*) PSD;

  static bool batteryTempCreated = FALSE;
  if (*(data->timer) % 50 == 0) {
		static unsigned short call = 0;
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
		pastBatteryData[voltageMeasurements] = *(data->batteryLevelPtr);
		voltageMeasurements++;
		if(voltageMeasurements == 16){
			voltageMeasurements = 0;
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
			pthread_create(&batteryTemp, NULL, batteryTemperature, PSD);
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
	static short read = -2;
	unsigned int pastGreaterTemperature;
	unsigned int greaterTemperature;
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
	  pastTemperatureData[temperatureMeasurements] = value;
	  temperatureMeasurements++;
	  aval = fopen("/sys/devices/ocp.3/helper.15/AIN3", "r");
	  fseek(aval,0,SEEK_SET);
	  fscanf(aval,"%d",&value);
	  fclose(aval);
	  *(data->batteryTemperature2) = value;
	  pastTemperatureData[temperatureMeasurements] = value;
	  temperatureMeasurements++;
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
	  if(temperatureMeasurements == 16){
	    temperatureMeasurements = 0;
	  }
		if(read == 16){
			read = 0;
		}
		usleep(500);
	}
}
