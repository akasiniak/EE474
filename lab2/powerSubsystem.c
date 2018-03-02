//Alex Kasiniak
//E E 474
//This is the power subsystem. It updates the power consumption, power generation, solar panel state, and battery level variables.
//The class has an input of the PSD data struct, and no outputs, and updates the variables according to the requirements in the spec.
#include "kernel_util.h"

void powerSubsystem(void* PSD)
{
	powerSubsystemData* data = (powerSubsystemData*) PSD;

	if (0 == *(data->timer)) {

		static unsigned short motorDrive = 0;
		if((int)motorDrive == 1) //if the motor drive was used previously, turn it off
		{
			motorDrive = 0;
		}
		static int call = 1;
		
		//0 is an even numbered call, 1 is an odd numbered call
		if(call == 1)
		{
			call = 0;
		} 
		else
		{
			call = 1;
		}

		static int consumptionDecrement = 0;

		//updating power consumption
		if((int)*(data->powerConsumption) < 5 && consumptionDecrement)
		{
			consumptionDecrement = 0;
		}
		else if((int)*(data->powerConsumption) > 10)
		{
			consumptionDecrement = 1;
		} 
		
		if(consumptionDecrement) 
		{
			//add one on odd calls, subtract two on even
			(call)
			{
				*(data->powerConsumption) = *(data->powerConsumption) + 1;
			}
			else
			{
				*(data->powerConsumption) = *(data->powerConsumption) - 2;
			}
			
		}
		else
		{
			//add two on even calls, subtract 1 on odd
			if(call)
			{
				*(data->powerConsumption) = *(data->powerConsumption) - 1;
			}
			else
			{
				*(data->powerConsumption) = *(data->powerConsumption) + 2;
			}
		}

		//updating power generation and batteryLevel
		if(*(data->solarPanelState))
		{
			if((int)*(data->batteryLevel) > 95) //put away solar panel if battery level higher than 95
			{
				*(data->solarPanelState) = FALSE;
				*(data->powerGeneration) = 0;
				motorDrive = 1;
			}
			else
			{
				*(data->batteryLevel) = *(data->batteryLevel) + *(data->powerGeneration);
				*(data->batteryLevel) = *(data->batteryLevel) - *(data->powerConsumption);
				if((int)*(data->batteryLevel) > 60000) //if battery level goes negative, need to set it back to 0
				{
					*(data->batteryLevel) = 0;
				} else if((int)*(data->batteryLevel) > 100) //battery level can't exceed 100
				{
					*(data->batteryLevel) = 100;
				}
				if((int)*(data->batteryLevel) > 50) //if battery level goes above 50, add 1 on odd numbered calls
				{
					*(data->powerGeneration) = *(data->powerGeneration) + 2 - call;
				}
				else
				{
					*(data->powerGeneration) = *(data->powerGeneration) + 2 * (1-call);
				}
			}
		}
		else
		{
			*(data->batteryLevel) = *(data->batteryLevel) - 3 * *(data->powerConsumption);
			if((int)*(data->batteryLevel) > 60000) //if battery level goes negative, need to set it back to 0
			{
				*(data->batteryLevel) = 0;
			} else if((int)*(data->batteryLevel) > 100) //battery level can't exceed 100
			{
				*(data->batteryLevel) = 100;
			}
			if((int)*(data->batteryLevel) <= 10) //if battery level equals or is less than 10, bring out solar panel
			{
				*(data->solarPanelState) = TRUE;
				motorDrive = 1;
			}
		}
	}
}