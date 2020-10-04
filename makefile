CC = g++
CFLAGS=-g -c -std=c++17

Demineur.o: Demineur.cpp olcPixelGameEngine.h
	${CC} -c Demineur.cpp -o Demineur.o ${CFLAGS}

prog: Demineur.o
	${CC} Demineur.o -o prog ${CFLAGS}


