//Haobo Zhang
//EE 474
//This implements the task vehicleComms. It takes in a void pointer and returns nothing. It is used to process the command from satellite and send back the response through FIFO.

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "kernel_util.h"
#define MAX_BUF 1024

void VehicleComms(void* vcd){
	vehicleCommsData* vcdPtr = (vehicleCommsData*) vcd;
	char buf[MAX_BUF];

  int fd0 = *(vcdPtr -> pipe);
  read(fd0, buf, MAX_BUF);

  char response[MAX_BUF];

  strcpy(response, "A ");
  strcat(response, buf);

  int size = sizeof("A A");
  write(fd0, response, size);
  return;
}
