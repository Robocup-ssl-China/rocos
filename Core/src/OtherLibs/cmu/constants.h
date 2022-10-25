/*
 * TITLE:        constants.h
 *
 * PURPOSE:      This is file contains the major system constants
 *               
 * WRITTEN BY:   Michael Bowling, James R Bruce, Brett Browning
 */
/* LICENSE:
  =========================================================================
    CMDragons'02 RoboCup F180 Source Code Release
  -------------------------------------------------------------------------
    Copyright (C) 2002 Manuela Veloso, Brett Browning, Mike Bowling,
                       James Bruce; {mmv, brettb, mhb, jbruce}@cs.cmu.edu
    School of Computer Science, Carnegie Mellon University
  -------------------------------------------------------------------------
    This software is distributed under the GNU General Public License,
    version 2.  If you do not have a copy of this licence, visit
    www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
    Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY,
    including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  ------------------------------------------------------------------------- */

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "staticparams.h"
const int MAX_ROBOTS = PARAM::Field::MAX_PLAYER * 2;
const int MAX_TEAM_ROBOTS = PARAM::Field::MAX_PLAYER;

//==== Field Dimensions (mm) =========================================//

// diagonal is of 2800 x 2300 is 3623.53
const double FIELD_LENGTH = PARAM::Field::PITCH_LENGTH;
const double FIELD_WIDTH = PARAM::Field::PITCH_WIDTH;
const double FIELD_LENGTH_H = (FIELD_LENGTH/2);
const double FIELD_WIDTH_H = (FIELD_WIDTH/2);

const double GOAL_WIDTH = PARAM::Field::GOAL_WIDTH;
const double GOAL_DEPTH = PARAM::Field::GOAL_DEPTH;
const double DEFENSE_WIDTH = PARAM::Field::PENALTY_AREA_WIDTH;
const double DEFENSE_DEPTH = PARAM::Field::PENALTY_AREA_DEPTH;
const double WALL_WIDTH = PARAM::Field::PITCH_MARGIN;

const double GOAL_WIDTH_H = (GOAL_WIDTH   /2);
const double GOAL_DEPTH_H = (GOAL_DEPTH   /2);
const double DEFENSE_WIDTH_H = (DEFENSE_WIDTH/2);
const double DEFENSE_DEPTH_H = (DEFENSE_DEPTH/2);

const double CENTER_CIRCLE_RADIUS = PARAM::Field::CENTER_CIRCLE_R;
#endif
