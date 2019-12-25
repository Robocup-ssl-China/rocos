/*
grSim - RoboCup Small Size Soccer Robots Simulator
Copyright (C) 2011, Parsian Robotic Center (eew.aut.ac.ir/~parsian/grsim)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pcylinder.h"

PCylinder::PCylinder(dReal x,dReal y,dReal z,dReal radius,dReal length,dReal mass,dReal red,dReal green,dReal blue,int texid,bool robot)
          : PObject(x,y,z,red,green,blue,mass)
{
    m_radius = radius;
    m_length = length;    
    m_texid = texid;
    m_robot = robot;
}

PCylinder::~PCylinder()
{
}


void PCylinder::setMass(dReal mass)
{
  m_mass = mass;
  dMass m;
  dMassSetCylinderTotal (&m,m_mass,1,m_radius,m_length);
  dBodySetMass (body,&m);
}

void PCylinder::init()
{
  body = dBodyCreate (world);
  initPosBody();
  setMass(m_mass);
  {
    geom = dCreateCylinder(0,m_radius,m_length);
  }
  dGeomSetBody (geom,body);
  dSpaceAdd (space,geom);
}
