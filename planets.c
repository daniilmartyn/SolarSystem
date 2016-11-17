#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <plot.h>
#include "planets.h"
#define G 6.67259E-11
#define OBJECTS 7
#define TRUE 1
#define FALSE 0

/* 
	This program read in data from planets.dat file and plays an animation of a not-to-scale 
	solar system with orbiting planets and other bodies. To plot the bodies Xwindows Plotter is
	used. Every body follows basic physics.
*/


_Bool isComment(char *buff);
void plotPlanets(Planets solar[],plPlotter *plotter);
double daysToSec(int days);
double secToDays(int sec);
void updateVelocity(int object, Planets *solarSystem, int dt);
void updatePosition(int object, Planets *solarSystem, int dt);
void moveVelPos(int j, Planets *solarSystem);
double denominator(Planets Xi, Planets Xn);

int main(int argc, char** argv) {

	if(argc != 2){
		fprintf(stderr, "Error: Invalid number of arguments\nUsage: myprog <path/planet.dat>\n");
		exit(1);
	}

	FILE *fd = fopen(argv[1], "r");
	
	if(fd == NULL){
		fprintf(stderr, "Error: unable to open file\n");
		exit(1);
	}

	Planets solarSystem[20];
	
	int i = 0;
	char buff[100];
	while(!feof(fd)){ // populate data fields within the Planets stuct

		fgets(buff,100,fd);
		if(!isComment(buff)){
			sscanf(buff,"%s %s %lf %lf %lf %lf %lf %lf",solarSystem[i].name,
														solarSystem[i].color,
														&(solarSystem[i].size),
														&(solarSystem[i].mass),
														&(solarSystem[i].xPos),
														&(solarSystem[i].yPos),
														&(solarSystem[i].xVel),
														&(solarSystem[i].yVel));
			solarSystem[i].orbitFlag = 0;
			i++;
		}
	}

	fclose(fd);
	
  /* plotter device structures */
  plPlotter *plotter;
  plPlotterParams *plotterParams;

  /* create a plotter parametric structure */
  plotterParams = pl_newplparams();
  pl_setplparam(plotterParams, "BITMAPSIZE", "750x750");
  pl_setplparam(plotterParams, "USE_DOUBLE_BUFFERING", "no");
  pl_setplparam(plotterParams, "BG_COLOR", "black");

  /* create the plotter device and open it */
  if ((plotter = pl_newpl_r("X", stdin, stdout, stderr, plotterParams)) == NULL) {
    fprintf(stderr, "Couldn't create Xwindows plotter\n");
    exit(1);
  } else if (pl_openpl_r(plotter) < 0) {
    fprintf(stderr, "Couldn't open Xwindows plotter\n");
    exit(1);
  }

  /* set our coordinate space in the plotter window */
  pl_fspace_r(plotter, -5.9E+11, -5.9E+11, 5.9E+11, 5.9E+11);

  /* pick a type for the pen and the fill */
  pl_pentype_r(plotter,1);
  pl_filltype_r(plotter,1);
  
  long dt = 300; // increment by 300 seconds
  
  /* run the animation of the solar system */
  for(long i = 0; i < daysToSec(10000); i+=dt){
	for(int j = 0; j < OBJECTS; j++){
		updateVelocity(j, solarSystem, dt);
		updatePosition(j, solarSystem, dt);
		moveVelPos(j, solarSystem);
		
		if((solarSystem[j].orbitFlag == FALSE) && (solarSystem[j].yPos < 0)){
		
			printf("%s orbit = %.2f days\n", solarSystem[j].name, secToDays(i)*2);// prints out how many earth days
			solarSystem[j].orbitFlag = TRUE;									// it takes a planet to make one orbit around the sun
		}																		// only works if all planets are initially on the positive x-axis
	}
	if(i%42300 == 0){ // print every half earth day
		
		pl_erase_r(plotter);
		pl_flushpl_r(plotter);
		plotPlanets(solarSystem, plotter);
	}
  }
    
  /* close and cleanup the plotter stuff */
  if (pl_closepl_r(plotter) < 0) {
    fprintf(stderr, "Couldn't close plotter\n");
  } else if (pl_deletepl_r(plotter) < 0) {
    fprintf(stderr, "Couldn't delete plotter\n");
  }
  return 0;
}

void plotPlanets(Planets solar[],plPlotter *plotter){
	
	for(int i=0; i < OBJECTS; i++){

		/* set some colors and draw a circle */
		pl_pencolorname_r(plotter,solar[i].color);
		pl_fillcolorname_r(plotter, solar[i].color);
		pl_fcircle_r(plotter, solar[i].xPos, solar[i].yPos, (solar[i].size)*1.2E+11);

	}  
}

_Bool isComment(char *buff){	// only works if you have no whitespace before the data you want to keep
								// if you have a whitespace before the start of your data on the same line, the entire line
								// will be treated as a comment, and not counted
	if(buff[0] == '#' || isspace(buff[0])){//
		return 1;
	}else{ return 0;}
}

double daysToSec(int days){
	return (double) 86400*days;
}

double secToDays(int sec){
	return (double) sec/86400;
}

void moveVelPos(int j, Planets *solarSystem){
	solarSystem[j].xVel = solarSystem[j].newxVel;
	solarSystem[j].yVel = solarSystem[j].newyVel;
	solarSystem[j].xPos = solarSystem[j].newxPos;
	solarSystem[j].yPos = solarSystem[j].newyPos;
}

void updateVelocity(int object, Planets *solarSystem, int dt){
	
	double xTemp = 0;
	double yTemp = 0;
	
	for(int i = 0; i < OBJECTS; i++){
		if(i==object){continue;}
		
		xTemp += ((solarSystem[i].xPos - solarSystem[object].xPos)*solarSystem[i].mass*G)/denominator(solarSystem[i], solarSystem[object]);
		yTemp += ((solarSystem[i].yPos - solarSystem[object].yPos)*solarSystem[i].mass*G)/denominator(solarSystem[i], solarSystem[object]);
	}
	
	solarSystem[object].newxVel = solarSystem[object].xVel + dt*xTemp;
	solarSystem[object].newyVel = solarSystem[object].yVel + dt*yTemp;
}

void updatePosition(int object, Planets *solarSystem, int dt){
	solarSystem[object].newxPos = solarSystem[object].xPos + dt*solarSystem[object].xVel;
	solarSystem[object].newyPos = solarSystem[object].yPos + dt*solarSystem[object].yVel;
}

double denominator(Planets Xi, Planets Xn){
	double xVar = Xi.xPos - Xn.xPos;
	double yVar = Xi.yPos - Xn.yPos;
	double magnitude = sqrt((xVar*xVar)+(yVar*yVar));
	return magnitude*magnitude*magnitude;
}