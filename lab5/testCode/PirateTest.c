#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "../kernel_util.h"
#include <time.h>
#include <stdlib.h>

void PirateManagement(void* PMD);
void PirateDetection(void* PDD);


int main(){
  //initialize A/D
  FILE *ain;
  ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  fseek(ain, 0, SEEK_SET);
  fprintf(ain, "cape-bone-iio");
  fflush(ain);
  fclose(ain);

  //PirateDetection, PirateManagement globals
  bool detected = FALSE;
  bool firePhasor = FALSE;
  bool fireTorpedo = FALSE;
  double pirateDistance = 10000000;
  bool phasorEnable = FALSE;
  bool torpedoEnable = FALSE;

  pirateDetectionData PDD;
  PDD.pirateDistancePtr = &pirateDistance;
  PDD.torpedoEnablePtr = &torpedoEnable;
  PDD.phasorEnablePtr = & phasorEnable;
  PDD.firePhasorPtr = &firePhasor;
  PDD.fireTorpedoPtr = &fireTorpedo;
  PDD.detected = &detected;

  pirateDetectionData PMD;
  PMD.pirateDistancePtr = &pirateDistance;
  PMD.torpedoEnablePtr = &torpedoEnable;
  PMD.phasorEnablePtr = & phasorEnable;
  PMD.firePhasorPtr = &firePhasor;
  PMD.fireTorpedoPtr = &fireTorpedo;

  fflush(stdout);

  while (1) {
    bool firePhasor = rand()%2;
    bool fireTorpedo = rand()%2;
    PirateDetection((void*)&PDD);
    PirateManagement((void*)&PMD);
    sleep(1);
  }
  return 0;
}
