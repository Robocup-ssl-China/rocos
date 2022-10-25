//====================================================================
//  TrajectoryStructs.cpp
//
//  Classes for Trajectory part
//====================================================================

#include "TrajectoryStructs.h"
#include <stdio.h>
#include <cmath>
#include <iostream>
using namespace std;
#include <fstream>

void ObjectPosVel::getParam(double xp,double yp, double rot,double xv, double yv, double rotv)
{
	xPos = xp;
	yPos = yp;
	rotation = rot;
	xVel = xv;
	yVel = yv;
	rotVel = rotv;
}
void ObjectPath::clearPath()
{
  count = 0;
  length =0;
}

bool ObjectPath::get(int step, ObjectPosVel & nextStep)
{
  if (step >= count) return false;

  nextStep.xPos     = path[step].xPos;
  nextStep.yPos     = path[step].yPos;
  nextStep.rotation = path[step].rotation;
  nextStep.xVel     = path[step].xVel;
  nextStep.yVel     = path[step].yVel;
  nextStep.rotVel   = path[step].rotVel;

//  memcpy(&nextStep, &path[step], sizeof(ObjectPosVel));
  return true;
}
  
bool ObjectPath::set(int step, const ObjectPosVel & nextStep)
{
  if (step >= count) return false;

  path[step].xPos     = nextStep.xPos;
  path[step].yPos     = nextStep.yPos;
  path[step].rotation = nextStep.rotation;
  path[step].xVel     = nextStep.xVel;
  path[step].yVel     = nextStep.yVel;
  path[step].rotVel   = nextStep.rotVel;

//  memcpy(&path[step], &nextStep, sizeof(ObjectPosVel));
  return true;
}

bool ObjectPath::set(int step, const double xPos, const double yPos, const double rotation,
            const double xVel, const double yVel, const double rotVel)
{
  if (step >= count) return false;

  path[step].xPos     = xPos;
  path[step].yPos     = yPos;
  path[step].rotation = rotation;
  path[step].xVel     = xVel;
  path[step].yVel     = yVel;
  path[step].rotVel   = rotVel;
  return true;
}

bool ObjectPath::addStep(const ObjectPosVel & nextStep)
{
  if (MAX_TRAJECTORY_STEPS == count) return false;

  path[count].xPos     = nextStep.xPos;
  path[count].yPos     = nextStep.yPos;
  path[count].rotation = nextStep.rotation;
  path[count].xVel     = nextStep.xVel;
  path[count].yVel     = nextStep.yVel;
  path[count].rotVel   = nextStep.rotVel;

//  memcpy(&path[count], &nextStep, sizeof(ObjectPosVel));
  count++;
  return true;
}

bool ObjectPath::addStep(const double xPos, const double yPos, const double rotation,
            const double xVel, const double yVel, const double rotVel)
{
  if (MAX_TRAJECTORY_STEPS == count) return false;

  path[count].xPos      = xPos;
  path[count].yPos      = yPos;
  path[count].rotation  = rotation;
  path[count].xVel      = xVel;
  path[count].yVel      = yVel;
  path[count].rotVel    = rotVel;
  count++;
  return true;
}
/**
* return the length of the path
*/
double ObjectPath::getLength()
{
	int i;
	double dx,dy;
	for(i=0;i<count-1;i++)
	{
		dx=abs(path[i].xPos-path[i+1].xPos);
		dy=abs(path[i].yPos-path[i+1].yPos);
		length+=sqrt(dx*dx+dy*dy);
	}

	return length;
}

ObjectPosVel ObjectPath::getStart()
{
	return path[0];
}
 
ObjectPosVel ObjectPath::getFinal()
{
	return path[count-1];
}

bool ObjectPath::writeToFile(const char * fname)
{
    FILE * f = fopen(fname, "a");
    ObjectPosVel nextStep;

    if(!f) return false;

    fprintf(f,"%d\n",count);
    for (int i=0; i < count; i++)
    {
      get(i, nextStep);
      fprintf(f,"(%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f)\n",
        nextStep.xPos,
        nextStep.yPos,
        nextStep.rotation,
        nextStep.xVel,
        nextStep.yVel,
        nextStep.rotVel
      );
    }
    fclose(f);
    return true;
}


