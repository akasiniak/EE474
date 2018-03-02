#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "kernel_util.h"
#include "task_functions.h"
#define DEFAULTMOTORSPEED 50
void solarPanelControl(void* SPD);
int main(){
  solarPanelControlData SPD;
  void* p = &SPD;
  int i;
  bool state = FALSE;
  bool deploy = FALSE;
  bool retract = FALSE;
  bool inc = FALSE;
  bool dec = FALSE;
  unsigned short t = 0;
  SPD.solarPanelState = &state;
  SPD.solarPanelDeploy = &deploy;
  SPD.solarPanelRetract = &retract;
  SPD.driveMotorSpeedInc = &inc;
  SPD.driveMotorSpeedDec = &dec;
  SPD.timer = &t;
  FILE *pwm, *period, *run;

  //enable pins
  pwm = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  fseek(pwm,0,SEEK_SET);
  fprintf(pwm, "am33xx_pwm");
  fflush(pwm);

  fprintf(pwm, "bone_pwm_P9_16");
  fflush(pwm);

  //set period
  period = fopen("/sys/devices/ocp.3/pwm_test_P9_16.16/period", "w");
  fseek(period,0,SEEK_SET);
  fprintf(period,"%d", 500000);
  fflush(period);

  //enable signal for solarpanel
  run = fopen("/sys/devices/ocp.3/pwm_test_P9_16.16/run", "w");
  fseek(run,0,SEEK_SET);
  fprintf(run,"%d",1);
  fflush(run);

  fclose(pwm);
  fclose(period);
  fclose(run);

  while(1){
    solarPanelControl(p);
    printf("solar panel state: %d\n", state);
    printf("solar panel deploy: %d\n", deploy);
    printf("solar panel retract: %d\n", retract);
    printf("speed increase: %d\n", inc);
    printf("speed decrease: %d\n", dec);
    printf("\n");
    for(i = 0; i < 100000000; i++);
  }
  return 0;
}

void solarPanelControl(void* SPD) {
  solarPanelControlData* data = (solarPanelControlData*) SPD;

  printf("solar Panel Control called\n");
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
      //delete_task(&KCDTask);
    } else {
      //add_task(&KCDTask);
    }
    //add motor drive if deploying, subtract motor drive if retracting
    solarPanelProgress += motorDrive * *(data->solarPanelDeploy) - motorDrive * *(data->solarPanelRetract);

    if(*(data->driveMotorSpeedInc)){ //increment motor speed
      motorDrive += 5;
      if(motorDrive > 100){
        motorDrive = 100;
      }
      *(data->driveMotorSpeedInc) = FALSE;
      defaultSpeed = FALSE;
    }
    if(*(data->driveMotorSpeedDec)){ //decrement motor speed
      motorDrive -= 5;
      *(data->driveMotorSpeedDec) = FALSE;
      if(motorDrive > 60000){ //check if gone negative
        motorDrive = 0;
      }
      defaultSpeed = FALSE;
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
      //delete_task(&SPDTask);
    }
    if(solarPanelProgress <= -100){
      solarPanelProgress = 0;
      *(data->solarPanelState) = FALSE;
      *(data->solarPanelRetract) = FALSE; //will not allow to retract further than 0
      //delete_task(&SPDTask);
    }
    printf("solar panel progress: %d", solarPanelProgress);
  }
}
