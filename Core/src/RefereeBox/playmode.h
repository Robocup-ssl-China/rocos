#ifndef _PLAYMODE_H_
#define _PLAYMODE_H_
#include "referee_commands.h"
enum PlayMode{ 
	PMStop,PMStart,PMHalt,PMReady,
	PMFirstHalf,PMHalfTime,PMSecondHalf,
	PMOverTime1,PMOverTime2,PMPenaltyShootout,
	PMTimeoutYellow,PMTimeoutBlue,PMTimeoutEnd,
	PMCancel,PMGoalYellow,PMGoalBlue,PMSubGoalYellow,PMSubGoalBlue,
	PMYellowCardYellow,PMYellowCardBlue,PMRestart,
	PMKickoffYellow,PMKickoffBlue,PMPenaltyYellow,PMPenaltyBlue,
	PMDirectYellow,PMDirectBlue,PMIndirectYellow,PMIndirectBlue,
    PMBallPlacementYellow,PMBallPlacementBlue,
	PMNone
};
struct PlayModePair{
	PlayModePair(const PlayMode m,const char c, const char* w) : mode(m),ch(c),what(w){ }
	const PlayMode mode;
	const char ch;
	const char* what;
};

const PlayModePair playModePair[] = { 
	PlayModePair( PMStop,				COMM_STOP,				"Stop"				),
	PlayModePair( PMStart,				COMM_START,				"Start"				),
	PlayModePair( PMHalt,				COMM_HALT,				"Halt"				),
	PlayModePair( PMReady,				COMM_READY,				"Ready"				),
	PlayModePair( PMFirstHalf,			COMM_FIRST_HALF,		"First Half"		),
	PlayModePair( PMHalfTime,			COMM_HALF_TIME,			"Half Time"			),
	PlayModePair( PMSecondHalf,			COMM_SECOND_HALF,		"Second Half"		),
	PlayModePair( PMOverTime1,			COMM_OVER_TIME1,		"Over Time 1"		),
	PlayModePair( PMOverTime2,			COMM_OVER_TIME2,		"Over Time 2"		),
	PlayModePair( PMPenaltyShootout,	COMM_PENALTY_SHOOTOUT,	"Penalty Shootout"	),
	PlayModePair( PMTimeoutYellow,		COMM_TIMEOUT_YELLOW,	"Timeout Yellow"	),
	PlayModePair( PMTimeoutBlue,		COMM_TIMEOUT_BLUE,		"Timeout Blue"		),
	PlayModePair( PMTimeoutEnd,			COMM_TIMEOUT_END,		"Timeout End"		),
	PlayModePair( PMCancel,				COMM_CANCEL,			"Cancel"			),
	PlayModePair( PMGoalYellow,			COMM_GOAL_YELLOW,		"Goal Yellow"		),
	PlayModePair( PMGoalBlue,			COMM_GOAL_BLUE,			"Goal Blue"			),
	PlayModePair( PMSubGoalYellow,		COMM_SUBGOAL_YELLOW,	"Sub Goal Yellow"	),
	PlayModePair( PMSubGoalBlue,		COMM_SUBGOAL_BLUE,		"Sub Goal Blue"		),
	PlayModePair( PMYellowCardYellow,	COMM_YELLOWCARD_YELLOW, "Yellow Card Yellow"),
	PlayModePair( PMYellowCardBlue,		COMM_YELLOWCARD_BLUE,	"Yellow Card Blue"	),
	PlayModePair( PMRestart,			COMM_RESTART,			"Restart"			),
	PlayModePair( PMKickoffYellow,		COMM_KICKOFF_YELLOW,	"Kickoff Yellow"	),
	PlayModePair( PMKickoffBlue,		COMM_KICKOFF_BLUE,		"Kickoff Blue"		),
	PlayModePair( PMPenaltyYellow,		COMM_PENALTY_YELLOW,	"Penalty Yellow"	),
	PlayModePair( PMPenaltyBlue,		COMM_PENALTY_BLUE,		"Penalty Blue"		),
	PlayModePair( PMDirectYellow,		COMM_DIRECT_YELLOW,		"Direct Yellow"		),
	PlayModePair( PMDirectBlue,			COMM_DIRECT_BLUE,		"Direct Blue"		),
	PlayModePair( PMIndirectYellow,		COMM_INDIRECT_YELLOW,	"Indirect Yellow"	),
    PlayModePair( PMIndirectBlue,		COMM_INDIRECT_BLUE,		"Indirect Blue"		),
    PlayModePair( PMBallPlacementYellow,COMM_BALL_PLACEMENT_YELLOW,"BallPlacement Yellow"),
    PlayModePair( PMBallPlacementBlue,  COMM_BALL_PLACEMENT_BLUE  ,"BallPlacement Blue"  ),
};
#endif
