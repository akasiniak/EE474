////////////////////////////
// Author: Allan Gilmore  //
// date: 7/10/17		  //
////////////////////////////

// Description: This program the data structs for the different tasks as well
//				as the TCB struct.

#ifndef KERNEL_UTIL
#define KERNEL_UTIL

// Struct for a task control block
struct taskBlock {
	void (*myTask)(void*);
	void* taskDataPtr;
};
typedef struct taskBlock TCB;

enum myBool { FALSE = 0, TRUE = 1};
typedef enum myBool bool;

// power subsystem data struct
struct PSD {
	bool* solarPanelState;
	unsigned short* batteryLevel;
	unsigned short* powerConsumption;
	unsigned short* powerGeneration;
	unsigned short* timer;
};
typedef struct PSD powerSubsystemData;

// thruster subsystem data struct
struct TSD {
	unsigned int* thrusterCommand;
	unsigned short* fuelLevel;
	unsigned short* timer;
};
typedef struct TSD thrusterSubsystemData;

// satellite communication system data struct
struct SCD {
	bool* fuelLow;
	bool* batteryLow;
	bool* solarPanelState;
	unsigned short* batteryLevel;
	unsigned short* fuelLevel;
	unsigned short* powerConsumption;
	unsigned short* powerGeneration;
	unsigned int* thrusterCommand;
	unsigned short* timer;
};
typedef struct SCD satelliteComsData;

// console display system data struct
struct CDD {
	bool* fuelLow;
	bool* batteryLow;
	bool* solarPanelState;
	unsigned short* batteryLevel;
	unsigned short* fuelLevel;
	unsigned short* powerConsumption;
	unsigned short* powerGeneration;
	unsigned short* timer;
};
typedef struct CDD consoleDisplayData;

// warnings and alarms system data struct
struct WAD {
	bool* fuelLow;
	bool* batteryLow;
	unsigned short* batteryLevel;
	unsigned short* fuelLevel;
	unsigned short* timer;
};
typedef struct WAD warningAlarmData;


#endif



