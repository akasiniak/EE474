#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

void PirateDetection(void* PDD){
  pirateDetectionData* data = (pirateDetectionData*) PDD;

  int voltage;

  //Take A/D input from 0 to 1.8V
  FILE *aval;
  aval = fopen("/sys/devices/ocp.3/helper.15/AIN4", "r");
  fseek(aval,0,SEEK_SET);
  fscanf(aval,"%d", &voltage);
  fclose(aval);
  //1.8V -- 0m  0V -- 180m or larger
  *(data->pirateDistancePtr) = (-10)*(voltage/(double)100) + 180;
  printf("\n\nPirate Distance: %i\n\n", *(data->pirateDistancePtr));
  if(*(data->pirateDistancePtr) <= 100){
    *(data->detected) = TRUE;
    add_task(data->PMDTaskPtr);
  } else {
    *(data->detected) = FALSE;
    delete_task(data->PMDTaskPtr);
    *(data->phasorEnablePtr) = FALSE;
    *(data->torpedoEnablePtr) = FALSE;
    *(data->firePhasorPtr) = FALSE;
    *(data->fireTorpedoPtr) = FALSE;
  }
}
