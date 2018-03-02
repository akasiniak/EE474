#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "kernel_util.h"
#include "task_functions.h"
void powerSubsystem(void* PSD);
int main(){
  powerSubsystemData PSD;
  void* p = &PSD;
  int i;
  bool state = TRUE;
  bool deploy = FALSE;
  bool retract = FALSE;
  unsigned short consumption = 0;
  unsigned short generation = 0;
  unsigned short t = 0;
  PSD.solarPanelState = &state;
  PSD.solarPanelDeploy = &deploy;
  PSD.solarPanelRetract = &retract;
  unsigned int batteryData[16];
  PSD.batteryLevelPtr = batteryData;
  PSD.powerConsumption = &consumption;
  PSD.powerGeneration = &generation;
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
    printf("\n");
    for(i = 0; i < 100000000; i++);
  }
  return 0;
}

//Alex Kasiniak
//E E 474
//This is the power subsystem. It updates the power consumption, power generation, solar panel state, and battery level variables.
//The class has an input of the PSD data struct, and no outputs, and updates the variables according to the requirements in the spec.
#include "kernel_util.h"

void powerSubsystem(void* PSD) {
	powerSubsystemData* data = (powerSubsystemData*) PSD;

	if (*(data->timer) % 10 == 0) {
		static unsigned short call = 0;
		static unsigned short measurements = 0;
    static int* startOfBuffer;
    if(call == 0){
      startOfBuffer = data->batteryLevelPtr;
    }
		//0 is an even numbered call, 1 is an odd numbered call
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
		startOfBuffer[measurements] = value * 20191;
		batteryLevelPercentage = startOfBuffer[measurements]/360000.0;
		data->batteryLevelPtr = &startOfBuffer[measurements];
		measurements++;
		if(measurements == 16){
			measurements = 0;
		}

		*(data->powerGeneration) += *(data->solarPanelState) * (2 * (1-(call%2)) + 1 * (call%2) * (batteryLevelPercentage <= 50));
		fflush(stdout);
		if(batteryLevelPercentage > 95) {
			*(data->powerGeneration) = 0;
			*(data->solarPanelRetract) = TRUE;
			//add_task(&SPDTask);
		}
		if(batteryLevelPercentage <= 10){
			*(data->solarPanelDeploy) = TRUE;
			//add_task(&SPDTask);
		}
    call++;
  }
}
