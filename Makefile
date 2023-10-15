all: sheduler

sheduler: task main
	g++ -o sheduler main.o task.o

main: main.cpp
	g++ -c main.cpp

task: task.cpp
	g++ -c task.cpp