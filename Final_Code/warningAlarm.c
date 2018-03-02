//Alex Kasiniak
//E E 474
//This is the warning and alarm subsystem. It flashes the LEDS at the apporpriate rate depending on whether
//power or fuel are low. It also updates the fuel low and battery low boolean variables. The class has
//an input of the WAD data struct and no outputs.
//As a side note, much of the code for the turnOn and turnOff functions was referenced from this
//website: http://eionix.blogspot.com/2015/02/program-bbb-using-c-blink-onboard-led.html

#include "kernel_util.h"
#include <stdio.h>

void turnOnLED1();
void turnOnLED2();
void turnOnLED3();
void turnOffLED1();
void turnOffLED2();
void turnOffLED3();

void warningAlarm(void* WAD)
{
        static unsigned int call = 0;
        static bool LED1 = FALSE;
        static bool LED2 = FALSE;
        static bool LED3 = FALSE;

        warningAlarmData* data = (warningAlarmData*) WAD;

        *(data->batteryLow) = FALSE;
        *(data->fuelLow) = FALSE;
        if((int)*(data->fuelLevel) > 50)        
        {       
                if((int)*(data->batteryLevel) > 50) //both are above 50%        
                {
                        turnOffLED1();
                        LED1 = FALSE;
                        turnOffLED2();
                        LED2 = FALSE;
                        turnOnLED3();
                        LED3 = TRUE;
                }
                else if(((int)*(data->batteryLevel) > 10) && ((int)*(data->batteryLevel) <= 50)) //fuel above 50%, battery between 10% and 50%
                {
                        turnOffLED1();
                        LED1 = FALSE;
                        turnOffLED3();
                        LED3 = FALSE;
                        if(call == 0) //on for 1 second
                        {
                                turnOnLED2();
                                LED2 = TRUE;
                        }
                        if(call == 2) //off for 1 second
                        {
                                turnOffLED2();
                                LED2 = FALSE;
                        }
                        call++;
                        if(call == 4)
                        {
                                call = 0;
                        }
                }
                else //fuel above 50%, battery below 10%
                {
                        turnOffLED3();
                        LED3 = FALSE;
                        turnOffLED1();
                        LED1 = FALSE;
                        if(LED2) //on for half a second
                        {
                                turnOffLED2();
                                LED2 = FALSE;
                        }
                        else //off for half a second
                        {
                                turnOnLED2();
                                LED2 = TRUE;
                        }
                        *(data->batteryLow) = TRUE;

                }
        }
        else if (((int)*(data->fuelLevel) > 10) && ((int)*(data->fuelLevel) <= 50))
        {
                if((int)*(data->batteryLevel) > 50) //fuel level between 10% and 50%, battery above 50% 
                {
                        turnOffLED2();
                        LED2 = FALSE;
                        turnOffLED3();
                        LED3 = FALSE;
                        if(call == 0) //turn on for 1 second
                        {
                                turnOnLED1();
                                LED1 = TRUE;
                        }
                        if(call == 2) //turn off for 1 second
                        {
                                turnOffLED1();
                                LED1 = FALSE;
                        }
                        call++;
                        if(call == 4)
                        {
                                call = 0;
                        }
                }
                else if(((int)*(data->batteryLevel) > 10) && ((int)*(data->batteryLevel) <= 50)) //both are between 10% and 50%
                {
                        turnOffLED3();
                        LED3 = FALSE;
                        if(call == 0) //turn on for 1 second
                        {
                                turnOnLED1();
                                LED1 = TRUE;
                                turnOnLED2();
                                LED2 = TRUE;
                        }
                        if(call == 2) //turn off for 1 second
                        {
                                turnOffLED1();
                                LED1 = FALSE;
                                turnOffLED2();
                                LED2 = FALSE;
                        }
                        call++;
                        if(call == 4)
                        {
                                call = 0;
                        }
                }
                else //fuel between 10% and 50%, battery below 10%
                {
                        turnOffLED3();
                        if(LED2) //LED2 on for half a second
                        {
                                turnOffLED2();
                                LED2 = FALSE;
                        }
                        else //LED2 off for half a second
                        {
                                turnOnLED2();
                                LED2 = TRUE;
                        }
                        if(call == 0) //LED1 on for 1 second
                        {
                                turnOnLED1();
                                LED1 = TRUE;
                        }
                        if(call == 2) //LED1 off for 1 second
                        {
                                turnOffLED1();
                                LED1 = FALSE;
                        }
                        call++;
                        if(call == 4 )
                        {
                                call = 0;
                        }
                        *(data->batteryLow) = TRUE;
                }
        }
        else
        {
                *(data->fuelLow) = TRUE;
                if((int)*(data->batteryLevel) > 50)
                {
                        turnOffLED3();
                        LED3 = FALSE;
                        turnOffLED2();
                        LED2 = FALSE;
                        if(LED1) //turn on for half a second
                        {
                                turnOffLED1();
                                LED1 = FALSE;
                        }
                        else //turn off for half a second
                        {
                                turnOnLED1();
                                LED1 = TRUE;
                        }
                }
                else if(((int)*(data->batteryLevel) > 10) && ((int)*(data->batteryLevel) <= 50))
                {
                        turnOffLED3();
                        if(LED1) //LED1 on for half a second
                        {
                                turnOffLED1();
                                LED1 = FALSE;
                        }
                        else //LED1 off for half a second
                        {
                                turnOnLED1();
                                LED1 = TRUE;
                        }
                        if(call == 0) //LED2 on for 1 second
                        {
                                turnOnLED2();
                                LED2 = TRUE;
                        }
                        if(call == 2) //LED2 off for 1 second
                        {
                                turnOffLED2();
                                LED2 = FALSE;
                        }
                        call++;
                        if(call == 4)
                        {
                                call = 0;
                        }
                }
                else
                {
                        *(data->batteryLow) = TRUE;
                        turnOffLED3();
                        LED3 = FALSE;
                        if(LED1) //LED1 on for half a second
                        {
                                turnOffLED1();
                                LED1 = FALSE;
                        }
                        else //LED1 off for half a second
                        {
                                turnOnLED1();
                                LED1 = TRUE;
                        }
                        if(LED2) //LED2 on for half a second
                        {
                                turnOffLED2();
                                LED2 = FALSE;
                        }
                        else //LED2 off for half a second
                        {
                                turnOnLED2();
                                LED2 = TRUE;
                        }
                }
        }
}

void turnOnLED1()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr1/brightness";

        if(LEDHandler=fopen(LEDPath,"r+")){
        fwrite("1", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOnLED2()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr2/brightness";

        if(LEDHandler=fopen(LEDPath,"r+")){
        fwrite("1", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOnLED3()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr3/brightness";

        if(LEDHandler=fopen(LEDPath,"r+")){
        fwrite("1", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOffLED1()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr1/brightness";

        if(LEDHandler=fopen(LEDPath,"r+")){
        fwrite("0", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOffLED2()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr2/brightness";

        if(LEDHandler=fopen(LEDPath,"r+")){
        fwrite("0", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}

void turnOffLED3()
{
        FILE* LEDHandler = NULL;
        char *LEDPath="/sys/class/leds/beaglebone:green:usr3/brightness";

        if(LEDHandler=fopen(LEDPath,"r+")){
        fwrite("0", 1, 1,LEDHandler);
        fclose(LEDHandler);
    }
}