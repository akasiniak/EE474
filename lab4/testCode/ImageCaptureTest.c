#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "../kernel_util.h"
#include <time.h>

void ImageCapture(void* ICD);

int main(){

	//initialize A/D
	FILE *ain;
	ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(ain, 0, SEEK_SET);
	fprintf(ain, "cape-bone-iio");
	fflush(ain);
	fclose(ain);

  // Image Caputure globals
	int imageDataRaw[256] = {0};
	unsigned int imageData[16] = {0};
	dbuff dybuff;
	dybuff.data = imageData;
	dybuff.size = 16;
	dybuff.current = imageData;

	imageCaptureData ICD;
	ICD.imageDataRawPtr = imageDataRaw;
	ICD.imageDataPtr = &dybuff;

	fflush(stdout);

	while (1) {
		ImageCapture((void*)&ICD);
		sleep(1);
	}
	return 0;
}
