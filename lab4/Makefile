satellite: satellite_main.o ConsoleDisplay.o powerSubsystem.o satellitecoms.o schedulerFunction.o solarPanelControl.o thrustersubsystem.o VehicleComms.o rand1.o warningAlarm.o ConsoleKeypadTask.o TransportDistance.o ImageCaputure.o optfft.o
	gcc -g -Wall -lpthread -o satellite satellite_main.o ConsoleDisplay.o powerSubsystem.o satellitecoms.o schedulerFunction.o solarPanelControl.o thrustersubsystem.o VehicleComms.o rand1.o warningAlarm.o ConsoleKeypadTask.o TransportDistance.o ImageCaputure.o optfft.o

satellite_main.o: satellite_main.c kernel_util.h task_functions.h
	gcc -Wall -g -c satellite_main.c

ConsoleDisplay.o: ConsoleDisplay.c kernel_util.h
	gcc -Wall -g -c ConsoleDisplay.c

powerSubsystem.o: powerSubsystem.c kernel_util.h task_functions.h
	gcc -Wall -g -c powerSubsystem.c -lpthread

satellitecoms.o: satellitecoms.c kernel_util.h task_functions.h
	gcc -Wall -g -c satellitecoms.c

schedulerFunction.o: schedulerFunction.c kernel_util.h task_functions.h
	gcc -Wall -g -c schedulerFunction.c

solarPanelControl.o: solarPanelControl.c kernel_util.h task_functions.h
	gcc -Wall -g -c solarPanelControl.c

thrustersubsystem.o: thrustersubsystem.c kernel_util.h task_functions.h
	gcc -Wall -g -c thrustersubsystem.c

VehicleComms.o: VehicleComms.c kernel_util.h task_functions.h
	gcc -Wall -g -c VehicleComms.c

rand1.o: rand1.c kernel_util.h task_functions.h
	gcc -Wall -g -c rand1.c

warningAlarm.o: warningAlarm.c kernel_util.h task_functions.h
	gcc -Wall -g -c warningAlarm.c

ConsoleKeypadTask.o: ConsoleKeypadTask.c kernel_util.h task_functions.h
	gcc -Wall -g -c ConsoleKeypadTask.c

TransportDistance.o: TransportDistance.c kernel_util.h
	gcc -Wall -g -c TransportDistance.c

ImageCaputure.o: ImageCaputure.c kernel_util.h optfft.h
	gcc -Wall -g -c ImageCaputure.c

optfft.o: optfft.c
	gcc -Wall -g -c optfft.c

clean:
	rm *.o satellite