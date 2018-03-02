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
void PirateManagement(void* PMD);
int main(){
  pirateManagementData PMD;
  void* d = &PMD;
  double distance = 100.0;
  bool phasorFire = FALSE;
  bool torpedoFire = FALSE;
  bool torpedoEn = FALSE;
  bool phasorEn = FALSE;
  PMD.pirateDistancePtr = &distance;
  PMD.firePhasorPtr = &phasorFire;
  PMD.fireTorpedoPtr = &torpedoFire;
  PMD.torpedoEnablePtr = &torpedoEn;
  PMD.phasorEnablePtr = &phasorEn;
  int i;
  for(i = 0; i < 110; i+=10){
    distance = i;
    if(i % 20){
      torpedoFire = TRUE;
      phasorFire = FALSE;
    } else {
      torpedoFire = TRUE;
      phasorFire = TRUE;
    }
    printf("phasorFire pre: %d\n", phasorFire);
    printf("torpedoFire pre: %d\n", torpedoFire);
    PirateManagement(d);
    printf("distance: %f\n", distance);
    printf("phasorFire: %d\n", phasorFire);
    printf("torpedoFire: %d\n", torpedoFire);
    printf("torpedoEnable: %d\n", torpedoEn);
    printf("phasorEnable: %d\n", phasorEn);
    printf("\n");
  }
}

void PirateManagement (void* PMD){

  pirateManagementData* data = (pirateManagementData*) PMD;


  if(*(data->pirateDistancePtr) <= 30){
    *(data->phasorEnablePtr) = TRUE;
  } else {
    *(data->phasorEnablePtr) = FALSE;
  }

  if(*(data->pirateDistancePtr) <= 5){
    *(data->torpedoEnablePtr) = TRUE;
  } else {
    *(data->torpedoEnablePtr) = FALSE;
  }

  if(*(data->phasorEnablePtr) == TRUE && *(data->firePhasorPtr) == TRUE){
    printf("Phasor Fired!\n");
    *(data->firePhasorPtr) = FALSE;
  }
  if(*(data->torpedoEnablePtr) == TRUE && *(data->fireTorpedoPtr) == TRUE){
    printf("Torpedo Fired!\n");
    *(data->fireTorpedoPtr) = FALSE;
  }
}
