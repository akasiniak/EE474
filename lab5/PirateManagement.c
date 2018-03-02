#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kernel_util.h"

void PirateManagement (void* PMD){

  pirateManagementData* data = (pirateManagementData*) PMD;


  if(*(data->pirateDistancePtr) <= 30){
    *(data->phasorEnablePtr) = TRUE;
  }

  if(*(data->pirateDistancePtr) <= 5){
    *(data->torpedoEnablePtr) = TRUE;
  }

  if(*(data->phasorEnablePtr) == TRUE && *(data->firePhasorPtr) == TRUE){
    printf("Phasor Fired!\n");
    *(data->firePhasorPtr) = FALSE;
  }
  if(*(data->torpedoEnablePtr) == TRUE && *(data->fireTorpedoPtr) == TRUE){
    printf("Torpedo Fired!\n");
    *(data->fireTorpedoPtr) = FALSE;
  }
  *(data->phasorEnablePtr) = FALSE;
  *(data->torpedoEnablePtr) = FALSE;
}
