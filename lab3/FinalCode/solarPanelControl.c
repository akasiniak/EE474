//Alex Kasiniak
//E E 474
//This is the solar panel control subsystem. It is responsible for maintaining the status of
//the solar panel. The program has one input: an SPD data struct and no outputs
#include "kernel_util.h"
#include <stdio.h>
#define DEFAULTMOTORSPEED 50;

void solarPanelControl(void* SPD) {
  solarPanelControlData* data = (solarPanelControlData*) SPD;
  fflush(stdout);

  static int solarPanelProgress = 0; //progress to fully retracted or deployed

  if(*(data->timer) % 10 == 0) {
    static unsigned short motorDrive = 0; //duty cycle of PWM
    static int defaultSpeed = 0;

    FILE* defaultControl = fopen("/sys/class/gpio/export", "w");
    fseek(defaultControl, 0, 49);
    fprintf(defaultControl, "%d", 49);
    fflush(defaultControl);

    // Set pin as input
    defaultControl = fopen("/sys/class/gpio/gpio49/direction", "w");
    fseek(defaultControl, 0, SEEK_SET);
    fprintf(defaultControl, "in");
    fflush(defaultControl);

    // read value of pin
    defaultControl = fopen("/sys/class/gpio/gpio49/value", "r");
    fseek(defaultControl,0,SEEK_SET);
    fscanf(defaultControl,"%d",&defaultSpeed);
    fflush(defaultControl);

    fclose(defaultControl);

    if(defaultSpeed){
      motorDrive = DEFAULTMOTORSPEED;
      delete_task(data->KCDTaskPtr);
    } else {
      add_task(data->KCDTaskPtr);
    }
    //add motor drive if deploying, subtract motor drive if retracting
    solarPanelProgress += motorDrive * *(data->solarPanelDeploy) - motorDrive * *(data->solarPanelRetract);

    if(*(data->driveMotorSpeedInc)){ //increment motor speed
      motorDrive += 5;
      if(motorDrive > 100){
        motorDrive = 100;
      }
      *(data->driveMotorSpeedInc) = FALSE;
    }
    if(*(data->driveMotorSpeedDec)){ //decrement motor speed
      motorDrive -= 5;
      *(data->driveMotorSpeedDec) = FALSE;
      if(motorDrive > 60000){ //check if gone negative
        motorDrive = 0;
      }
    }
    //changing PWM duty cycle
    int dutyCycleNano = 500000 - ((motorDrive * 500000) / 100); //convert % to nanoseconds
    FILE *duty;
    duty = fopen("/sys/devices/ocp.3/pwm_test_P9_16.16/duty", "w");
  	fseek(duty,0,SEEK_SET);
  	fprintf(duty,"%d",dutyCycleNano);
  	fflush(duty);
    fclose(duty);


    if(solarPanelProgress >= 100){
      solarPanelProgress = 0;
      *(data->solarPanelState) = TRUE;
      *(data->solarPanelDeploy) = FALSE; //will not allow to deploy past 100
      fflush(stdout);
      delete_task(data->SPDTaskPtr);
    }
    if(solarPanelProgress <= -100){
      solarPanelProgress = 0;
      *(data->solarPanelState) = FALSE;
      *(data->solarPanelRetract) = FALSE; //will not allow to retract further than 0
      fflush(stdout);
      delete_task(data->SPDTaskPtr);
    }
  }
}
