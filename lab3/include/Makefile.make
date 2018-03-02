


satelliteSys: satellite_main.o ConsoleDisplay.o powerSubsystem.o satellitecoms.o thrustersubsystem.o warningAlarm.o
	gcc -Wall -g -o satelliteSys satellite_main.o ConsoleDisplay.o powerSubsystem.o satellitecoms.o thrustersubsystem.o warningAlarm.o

satellite_main.o: satellite_main.c kernal_util.h task_functions.h
	gcc -Wall -g -c satellite_main.c

ConsoleDisplay.o: ConsoleDisplay.c kernal_util.h task_functions.h
	gcc -Wall -g -c ConsoleDisplay.c

powerSubsystem.o: powerSubsystem.c kernal_util.h task_functions.h
	gcc -Wall -g -c powerSubsystem.c

satellitecoms.o: satellitecoms.c kernal_util.h task_functions.h
	gcc -Wall -g -c satellitecoms.c

thrustersubsystem.o: thrustersubsystem.c kernal_util.h task_functions.h
	gcc -Wall -g -c thrustersubsystem.c

warningAlarm.o: warningAlarm.c kernal_util.h task_functions.h
	gcc -Wall -g -c warningAlarm.c

clean: 
	rm -rf *.o satelliteSys

