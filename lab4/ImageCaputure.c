#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "kernel_util.h"


signed int optfft(signed int real[256], signed int imag[256]);

void ImageCapture(void* ICD) {

  imageCaptureData* data = (imageCaptureData*) ICD;
  int imageDataRawReal[256] = {0};
  int imageDataRawImag[256] = {0};
  clock_t begin, end;
  int FS;


  FILE *aval;
  begin = clock();

  for(int i = 0; i < 256; i++){
    aval = fopen("/sys/devices/ocp.3/helper.15/AIN1", "r");
    // enable the ADC ports
    fseek(aval,0,SEEK_SET);
    int element;
    fscanf(aval,"%d", &element);
    //*(data->imageDataRawPtr + i) = element;
    printf("Voltage: %u\n", element);
    //Shift 0.9V to have both positive and negative value
    imageDataRawReal[i] = (int)((element - 900)/(double)30);
    printf("raw: %d\n", imageDataRawReal[i]);
    //Control the sampling frequency
    //usleep(1000);
    fclose(aval);
  }
  end = clock();
  float totalTime = (float)(end-begin)/CLOCKS_PER_SEC;
  FS = (int)(1/(totalTime/256));
  printf("sampling freq: %u\n", FS);
  signed int peakfreq_index = optfft(imageDataRawReal, imageDataRawImag);
  printf("peak: %u\n", peakfreq_index);
  unsigned int signalFreq = (unsigned int)(FS * peakfreq_index / (double)256);
  //Add data to the imageDataPtr
  addBuff(&signalFreq, data -> imageDataPtr);
  //test
  printf("Recent Frequency: %u\n", signalFreq);
  printf("Recent Frequency: %u\n\n", *(data->imageDataPtr->current));
}

void addBuff(unsigned int* dataAppend, dbuff* dbuffPtr) {
  if(dbuffPtr->current - dbuffPtr->data == dbuffPtr->size){
     dbuffPtr->current = dbuffPtr->data;
  }else {
    dbuffPtr->current = (dbuffPtr->current) + 1;
  }
  *(dbuffPtr->current) = *dataAppend;
}
