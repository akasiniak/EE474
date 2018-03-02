//Alex Kasiniak
//E E 474
//This is the power subsystem. It updates the power consumption, power generation, solar panel state, and battery level variables.
//The class has an input of the PSD data struct, and no outputs, and updates the variables according to the requirements in the spec.
#include "kernel_util.h"
#include <stdio.h>

void powerSubsystem(void* PSD) {
	powerSubsystemData* data = (powerSubsystemData*) PSD;

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
}
