#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "kernel_util.h"
#include "task_functions.h"
void warningAlarm(void* WAD);
void turnOnLED1();
void turnOnLED2();
void turnOnLED3();
void turnOffLED1();
void turnOffLED2();
void turnOffLED3();
int main(){
  warningAlarmData WAD;
  void* d = &WAD;
  int i;
  bool fw = FALSE;
  bool bw = FALSE;
  unsigned int battery = 2; //36000
  unsigned short fuel = 75;
  bool bot = TRUE;
  unsigned short t = 0;
  WAD.fuelLow = &fw;
  WAD.batteryLow = &bw;
  WAD.batteryLevelPtr = &battery;
  WAD.fuelLevel = &fuel;
  WAD.batteryOverTemperature = &bot;
  WAD.timer = &t;
  // Initialize LEDs
  FILE* LEDHandler = NULL;

  char *LEDPath0 = "/sys/class/leds/beaglebone:green:usr0/brightness";
  char *LEDPath1 = "/sys/class/leds/beaglebone:green:usr1/brightness";
  char *LEDPath2 = "/sys/class/leds/beaglebone:green:usr2/brightness";
  char *LEDPath3 = "/sys/class/leds/beaglebone:green:usr3/brightness";

  if ((LEDHandler = fopen(LEDPath0, "r+"))) {
    fwrite("0", 1, 1, LEDHandler);
    fclose(LEDHandler);
  }
  if ((LEDHandler = fopen(LEDPath1, "r+"))) {
    fwrite("0", 1, 1, LEDHandler);
    fclose(LEDHandler);
  }
  if ((LEDHandler = fopen(LEDPath2, "r+"))) {
    fwrite("0", 1, 1, LEDHandler);
    fclose(LEDHandler);
  }
  if ((LEDHandler = fopen(LEDPath3, "r+"))) {
    fwrite("0", 1, 1, LEDHandler);
    fclose(LEDHandler);
  }

  FILE *switchPin, *pinMode;

  // initalize GPIO pin
  switchPin = fopen("/sys/class/gpio/export", "w");
  fseek(switchPin, 0, SEEK_SET);
  fprintf(switchPin, "%d", 20);
  fflush(switchPin);
  fclose(switchPin);

  // Set pin as input
  pinMode = fopen("/sys/class/gpio/gpio20/direction", "w");
  fseek(pinMode, 0, SEEK_SET);
  fprintf(pinMode, "out");
  fflush(pinMode);
  fclose(pinMode);
  while(1){
    warningAlarm(d);
    usleep(50000);
  }
  return 0;
}

void warningAlarm(void* WAD)
{
  static unsigned int call = 0;
  static unsigned int milli = 0;
  static bool LED1 = FALSE;
  static bool LED2 = FALSE;

  warningAlarmData* data = (warningAlarmData*) WAD;

  *(data->batteryLow) = FALSE;
  *(data->fuelLow) = FALSE;
  if(!(*(data->batteryOverTemperature)))
  {
    if((int)*(data->fuelLevel) > 50)
    {
      if((*(data->batteryLevelPtr)/36000.0) > 50) //both are above 50%
      {
        turnOffLED1();
        turnOffLED2();
        turnOnLED3();
      }
      else if(((*(data->batteryLevelPtr)/36000.0) >= 10 && (*(data->batteryLevelPtr)/36000.0) <= 50)) //fuel above 50%, battery between 10% and 50%
      {
        turnOffLED1();
        turnOffLED3();
        if(call == 0) //on for 1 second
        {
            turnOnLED2();
        }
        if(call == 10) //off for 1 second
        {
            turnOffLED2();
        }
        call++;
        if(call == 20)
        {
            call = 0;
        }
      }
      else //fuel above 50%, battery below 10%
      {
        turnOffLED3();
        turnOffLED1();
        if(call == 0) //on for half a second
        {
          turnOnLED2();
        }
        if(call == 5)
        {
          turnOffLED2();
        }
        call++;
        if(call == 10)
        {
          call = 0;
        }
        *(data->batteryLow) = TRUE;
      }
    }
    else if (((int)*(data->fuelLevel) > 10) && ((int)*(data->fuelLevel) <= 50))
    {
      if((*(data->batteryLevelPtr)/36000.0) > 50) //fuel level between 10% and 50%, battery above 50%
      {
        turnOffLED2();
        turnOffLED3();
        if(call == 0) //turn on for 1 second
        {
          turnOnLED1();
        }
        if(call == 10) //turn off for 1 second
        {
          turnOffLED1();
        }
        call++;
        if(call == 20)
        {
          call = 0;
        }
      }
      else if(((*(data->batteryLevelPtr)/36000.0) > 10) && (*(data->batteryLevelPtr)/36000.0) <= 50) //both are between 10% and 50%
      {
        turnOffLED3();
        if(call == 0) //turn on for 1 second
        {
            turnOnLED1();
            turnOnLED2();
        }
        if(call == 10) //turn off for 1 second
        {
          turnOffLED1();
          turnOffLED2();
        }
        call++;
        if(call == 20)
        {
          call = 0;
        }
      }
      else //fuel between 10% and 50%, battery below 10%
      {
        turnOffLED3();
        if(call == 0) //LED1 on for 1 second, LED2 for half second
        {
          turnOnLED1();
          turnOnLED2();
        }
        if(call == 5) //LED2 off for half second
        {
          turnOffLED2();
        }
        if(call == 10) //LED1 off for 1 second
        {
          turnOffLED1();
          turnOnLED2();
        }
        if(call == 15) //LED2 off for half a second
        {
          turnOffLED2();
        }
        call++;
        if(call == 20)
        {
          call = 0;
        }
        *(data->batteryLow) = TRUE;
      }
    }
    else //fuel below 10%
    {
      *(data->fuelLow) = TRUE;
      if((*(data->batteryLevelPtr)/36000.0) > 50) //fuel below 10% battery greater than 50%
      {
              turnOffLED3();
              turnOffLED2();
              if(call == 0) //turn on for half a second
              {
                turnOnLED1();
              }
              if(call == 5) //turn off for half a second
              {
                turnOffLED1();
              }
      }
      else if(((*(data->batteryLevelPtr)/36000.0) > 10) && (*(data->batteryLevelPtr)/36000.0) <= 50) //fuel below 10% battery between 10 and 50%
      {
          turnOffLED3();
          if(call == 0) //LED2 on for 1 second, LED1 for half second
          {
            turnOnLED1();
            turnOnLED2();
        }
        if(call == 5) //LED1 off for half second
        {
          turnOffLED1();
        }
        if(call == 10) //LED2 off for 1 second, LED 1 on for half a second
        {
          turnOffLED2();
          turnOnLED1();
        }
        if(call == 15) //LED1 off for half a second
        {
          turnOffLED1();
        }
        call++;
        if(call == 20)
        {
          call = 0;
        }
        *(data->batteryLow) = TRUE;
      }
      else //both below 10%
      {
        *(data->batteryLow) = TRUE;
        turnOffLED3();
        if(call == 0) //LED1 on for half a second
        {
          turnOnLED1();
          turnOnLED2();
        }
        if(call == 5)
        {
          turnOffLED1();
          turnOffLED2();
        }
          call++;
          if(call == 10)
          {
            call = 0;
          }
      }
    }
  }
  else
  {
    //chirp the alarm
    FILE *switchValue;
    switchValue = fopen("/sys/class/gpio/gpio20/value", "w");
    fseek(switchValue, 0, SEEK_SET);
    fprintf(switchValue, "1");
    fflush(switchValue);
    fseek(switchValue, 0, SEEK_SET);
    fprintf(switchValue, "0");
    fflush(switchValue);
    fclose(switchValue);
    if(milli >= 15000)
    {
      printf("15 seconds have passed\n");
      fflush(stdout);
      if(call <= 50)
      {
        if(LED1)
        {
          LED1 = FALSE;
          turnOffLED1();
        }
        else
        {
          LED1 = TRUE;
          turnOnLED1();
        }
        if(LED2)
        {
          LED2 = FALSE;
          turnOffLED2();
        }
        else
        {
          LED2 = TRUE;
          turnOnLED2();
        }
      }
      else
      {
        LED1 = TRUE;
        turnOnLED1();
        LED2 = TRUE;
        turnOnLED2();
      }
      call++;
      if(call == 100)
      {
        call = 0;
      }
    }
    milli += 100;
  }
}

void turnOnLED1()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr1/brightness";

        if ((LEDHandler=fopen(LEDPath,"r+"))) {
        fwrite("1", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOnLED2()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr2/brightness";

        if ((LEDHandler=fopen(LEDPath,"r+"))) {
        fwrite("1", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOnLED3()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr3/brightness";

        if ((LEDHandler=fopen(LEDPath,"r+"))) {
        fwrite("1", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOffLED1()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr1/brightness";

        if ((LEDHandler=fopen(LEDPath,"r+"))) {
        fwrite("0", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOffLED2()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr2/brightness";

        if ((LEDHandler=fopen(LEDPath,"r+"))) {
        fwrite("0", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOffLED3()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr3/brightness";

        if ((LEDHandler=fopen(LEDPath,"r+"))) {
        fwrite("0", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}
