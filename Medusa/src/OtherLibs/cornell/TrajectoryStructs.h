//====================================================================
//  TrajectoryStructs.h
//
//  Classes for trajectory part
//====================================================================

#ifndef TRAJECTORYSTRUCTS_H
#define TRAJECTORYSTRUCTS_H

const int MAX_TRAJECTORY_STEPS = 60;

class ObjectPosVel{
public:
	double xPos, yPos, rotation;
	double xVel, yVel, rotVel;
public:
	ObjectPosVel(){} 
	ObjectPosVel(double xp,double yp, double rot,double xv, double yv, double rotv)
	{
		xPos = xp;
		yPos = yp;
		rotation = rot;
		xVel = xv;
		yVel = yv;
		rotVel = rotv;
	}
	void getParam(double xp,double yp, double rot,double xv, double yv, double rotv);

	void setXPos(double x) { xPos=x; }
	void setYPos(double y) { yPos=y; }
	void setXVel(double xv) { xVel =xv; }
	void setYVel(double yv) { yVel = yv; }
	void setVel(double xv, double yv) {  xVel =xv; yVel = yv; }
	void getPosVel(ObjectPosVel & other) { other = *this; }
	double getRotation() { return rotation; }
	double getXPos() { return xPos; }
	double getYPos() { return yPos; }
	double getXVel() { return xVel; }
	double getYVel() { return yVel; }

};

class ObjectPath{
  ObjectPosVel path[MAX_TRAJECTORY_STEPS];
  int count;
  double length;
public:
  ObjectPath() { count = 0; length =0; }
  double getLength();
  void clearPath();
  int getNumSteps() const { return count; }
  bool get(int step, ObjectPosVel & nextStep);
  bool set(int step, const ObjectPosVel & nextStep);
  bool set(int step, const double xPos, const double yPos, const double rotation, const double xVel, const double yVel, const double rotVel);
  bool addStep(const ObjectPosVel & nextStep);
  bool addStep(const double xPos, const double yPos, const double rotation, const double xVel, const double yVel, const double rotVel);
  bool writeToFile(const char * fname);

  ObjectPosVel getStart();
  ObjectPosVel getFinal();
};

#endif //TRAJECTORYSTRUCTS_H
