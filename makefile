
planets: planets.o
	gcc -g -o planets planets.o -lplot -lXaw -lXmu -lXt -lXext -lSM -lICE -lX11 -lpng -lz -lm

planets.o: planets.c
	gcc -g -Wall -pedantic -std=c99 -c planets.c planets.h

clean:
	rm *.o *~ planets *.gch
