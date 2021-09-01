assembler: functions.o main.o firstRun.o secondRun.o createFiles.o
	gcc -ansi -Wall -g -pedantic functions.o main.o firstRun.o secondRun.o createFiles.o -o assembler
functions.o: functions.c functions.h
	gcc -c -ansi -Wall -g -pedantic functions.c -o functions.o 
main.o: main.c functions.h
	gcc -c -ansi -Wall -g -pedantic main.c -o main.o
firstRun.o: firstRun.c functions.h dataForRuns.h
	gcc -c -ansi -Wall -g -pedantic firstRun.c -o firstRun.o
secondRun.o: secondRun.c dataForRuns.h functions.h
	gcc -c -ansi -Wall -g -pedantic secondRun.c -o secondRun.o
createFiles.o: createFiles.c dataForRuns.h functions.h
	gcc -c -ansi -Wall -g -pedantic createFiles.c -o createFiles.o
