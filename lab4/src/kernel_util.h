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
	struct taskBlock* next;
	struct taskBlock* prev;
};
typedef struct taskBlock TCB;

// define boolean type
enum myBool { FALSE = 0, TRUE = 1};
typedef enum myBool bool;

// task queue and suspended queue
TCB* taskQ;
TCB* suspendedQ;

// add a task to the end of the task queue
void add_task(TCB* newTask);

// remove a task from the task queue
void delete_task(TCB* task);

// power subsystem data struct
struct PSD {
	bool* solarPanelState;
	bool* solarPanelDeploy;
	bool* solarPanelRetract;
	unsigned int* batteryLevelPtr;
	unsigned short* powerConsumption;
	unsigned short* powerGeneration;
	bool* batteryOverTemp;
	unsigned int* batteryTemp1;
	unsigned int* batteryTemp2;
	TCB* SPDTaskPtr;
	unsigned short* timer;
};
typedef struct PSD powerSubsystemData;

// solar panel subsystem data struct
struct SPD {
	bool* solarPanelState;
	bool* solarPanelDeploy;
	bool* solarPanelRetract;
	bool* driveMotorSpeedInc;
	bool* driveMotorSpeedDec;
	TCB* KCDTaskPtr;
	TCB* SPDTaskPtr;
	unsigned short* timer;
};
typedef struct SPD solarPanelControlData;

//
struct KCD {
	bool* driveMotorSpeedInc;
	bool* driveMotorSpeedDec;
	unsigned short* timer;
};
typedef struct KCD keyboardConsoleData;



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
	unsigned int* batteryLevelPtr;
	unsigned short* fuelLevel;
	unsigned short* powerConsumption;
	unsigned short* powerGeneration;
	unsigned int* thrusterCommand;
	unsigned short* timer;
	int* pipe;
  char* command;
  char* response;
};
typedef struct SCD satelliteComsData;

// vehicle communication task data struct
struct VCD {
	char* command;
	char* response;
	unsigned short* timer;
  int* pipe;
};
typedef struct VCD vehicleCommsData;

// console display system data struct
struct CDD {
	bool* fuelLow;
	bool* batteryLow;
	bool* solarPanelState;
	unsigned int* batteryLevelPtr;
	unsigned short* fuelLevel;
	unsigned short* powerConsumption;
	unsigned short* powerGeneration;
	unsigned int* batteryTemp1;
	unsigned int* batteryTemp2;
	unsigned short* timer;
};
typedef struct CDD consoleDisplayData;

// warnings and alarms system data struct
struct WAD {
	bool* fuelLow;
	bool* batteryLow;
	unsigned int* batteryLevelPtr;
	unsigned short* fuelLevel;
	bool* batteryOverTemp;
	unsigned short* timer;
};
typedef struct WAD warningAlarmData;

typedef enum {wrt, rd} rw;


extern TCB PSDTask;
extern TCB SPDTask;
extern TCB KCDTask;
extern TCB TSDTask;
extern TCB SCDTask;
extern TCB VCDTask;
extern TCB CDDTask;
extern TCB WADTask;

#endif
