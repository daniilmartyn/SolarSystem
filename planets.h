#ifndef PLANETS
#define PLANETS

typedef struct {
		char name[20];
		char color[20];
		double size;
		double mass;
		double xPos;
		double yPos;
		double xVel;
		double yVel;		
		double newxPos;
		double newyPos;
		double newxVel;
		double newyVel;
		int orbitFlag;
	} Planets;

#endif