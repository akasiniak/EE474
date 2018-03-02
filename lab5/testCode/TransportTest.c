#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "../kernel_util.h"
#include <time.h>

void TransportDistance();

int main(){
	fflush(stdout);

	while (1) {
		TransportDistance();
		sleep(1);
	}
	return 0;
}