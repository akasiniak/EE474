////////////////////////////
// Author: Allan Gilmore  //
// date: 7/10/17		  //
////////////////////////////

// Description: This file contains the function prototypes for the various tasks.

#ifndef TASK_FUNC
#define TASK_FUNC

void powerSubsystem(void* PSD);

void ThrusterSubsystem(void* TSD);

void SatelliteComs(void* SCD, rw coms);

void consoleDisplay(void* CDD);

void warningAlarm(void* WAD);

void VehicleComms(void* VCD);

void solarPanelControl(void* SPD);

void ConsoleKeypadTask(void* KCD);

void StartUp(void* SUD);

#endif
