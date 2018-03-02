//Haobo Zhang
//EE 474
//This implements the task vehicleComms. It takes in a void pointer and returns nothing. It is used to process the command and send the response.

#include <unistd.h> /* Symbolic Constants */
#include <sys/types.h> /* Primitive System Data Types */
#include <sys/stat.h>
#include <errno.h> /* Errors */
#include <stdio.h> /* Input/Output */
#include <stdlib.h> /* General Utilities */
#include <pthread.h> /* POSIX Threads */
#include <string.h> /* String handling */
#include <semaphore.h> /* Semaphore */
#include <fcntl.h>

#include "kernel_util.h"

#define BUF_SIZE 5
#define VALID_COMMANDS "FBLRDH"

int fd0;

struct sateData {
    pthread_t vehicle;
    char* command;
    char* response;
    TCB* optfftPtr;
};



void* satellite_func (void* sd);
void* vehicle_func (void* arg);

void VehicleComms(void* vcd){
    
    vehicleCommsData* vcdPtr = (vehicleCommsData*) vcd;
    if (*(vcdPtr->timer) % 50 == 0) {

        char* myfifo0 = "/tmp/myfifo0";
        mkfifo(myfifo0, 0666);
        fd0 = open(myfifo0, O_RDWR);

        pthread_t satellite;
        pthread_t vehicle;

        int threadError;

        threadError = pthread_create(&vehicle, NULL, vehicle_func, NULL);
        if (threadError) {
            printf("ERROR CREATING THREAD: vehicle\n");
            fflush(stdout);
            exit(-1);
        }

        struct sateData sd;
        sd.vehicle = vehicle;
        sd.command = vcdPtr->command;
        sd.response = vcdPtr->response;
        sd.optfftPtr = vcdPtr->optfftPtr;

        threadError = pthread_create(&satellite, NULL, satellite_func, (void*) &sd);
        if (threadError) {
            printf("ERROR CREATING THREAD: satellite\n");
            fflush(stdout);
            exit(-1);
        }

        

        pthread_join(satellite, NULL);
        pthread_join(vehicle, NULL);
    }
}

void* satellite_func (void* sd) {

    struct sateData* data = (struct sateData*) sd;
    char buf[BUF_SIZE];
    buf[0] = *data->command;
    buf[1] = '\0';
    write(fd0, buf, BUF_SIZE);
    printf("Sent: %s\n", buf);
    fflush(stdout);
    char recievedBuf[BUF_SIZE];
    pthread_join(data->vehicle, NULL);
    read(fd0, recievedBuf, BUF_SIZE);
    *data->response = recievedBuf[0];
    if (*(data->response)) {
        //add_task(data->optfftPtr);
    }
    printf("Recieved: %s\n", recievedBuf);
    pthread_exit(NULL);
}

void* vehicle_func (void* arg) {
    char buf[BUF_SIZE];
    read(fd0, buf, BUF_SIZE);
    
    if ('T' == buf[0]) {
        write(fd0, "K", BUF_SIZE);
    } else if ('D' == buf[0]) {
        write(fd0, "C", BUF_SIZE);
    } else if ('S' == buf[0]) {
        write(fd0, "W", BUF_SIZE);
    } else if ('I' == buf[0]) {
        write(fd0, "P", BUF_SIZE);
    } else if ('\0' != buf[0] && strchr(VALID_COMMANDS, buf[0])) {
        char buf2[BUF_SIZE] = "A ";
        strncat(buf2, buf, BUF_SIZE);
        write(fd0, buf2, BUF_SIZE);
    } else {
        write(fd0, "E", BUF_SIZE);
    }
    pthread_exit(NULL);
}


