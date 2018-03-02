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
void PirateDetection(void* PDD);
int main(){
  pirateDetectionData PDD;
  void* d = &PDD;
  double distance = 0.0;
  bool torpedoEn = FALSE;
  bool phasorEn = FALSE;
  bool phasorFire = FALSE;
  bool torpedoFire = FALSE;
  bool det = FALSE;
  PDD.pirateDistancePtr = &distance;
  PDD.torpedoEnablePtr = &torpedoEn;
  PDD.phasorEnablePtr = &phasorEn;
  PDD.firePhasorPtr = &phasorFire;
  PDD.fireTorpedoPtr = &torpedoFire;
  PDD.detected = &det;
  PirateDetection(d);
  printf("distance: %f\n", distance);
  printf("det: %d\n", det);
}

void PirateDetection(void* PDD){
  pirateDetectionData* data = (pirateDetectionData*) PDD;

  int voltage;

  //Take A/D input from 0 to 1.8V
/*FILE *aval;
  aval = fopen("/sys/devices/ocp.3/helper.15/AIN2", "r");
  fseek(aval,0,SEEK_SET);
  fscanf(aval,"%d", &voltage);
  fclose(aval);
  //1.8V -- 0m  0V -- 180m or larger*/
  voltage = 800;
  *(data->pirateDistancePtr) = (-10)*(voltage/(double)100) + 180;

  if(*(data->pirateDistancePtr) <= 100){
    *(data->detected) = TRUE;
    //add_task(data->PMDTaskPtr);
  } else {
    *(data->detected) = FALSE;
    //delete_task(data->PMDTaskPtr);
    *(data->phasorEnablePtr) = FALSE;
    *(data->torpedoEnablePtr) = FALSE;
    *(data->firePhasorPtr) = FALSE;
    *(data->fireTorpedoPtr) = FALSE;
  }
}
