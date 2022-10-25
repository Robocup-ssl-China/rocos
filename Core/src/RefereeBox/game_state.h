// game_state.h
//
// This class implements the transition system that defines the rules
// of the game.  The state changes based on the input from the referee
// and whether the ball has been kicked since the last referee
// command.  Calling the method transition() with these inputs will
// have the class maintain the state of the game.
//
// In addition, their are query methods for determining the current
// game state and what behavior is allowed in these game states.
//
// Created by:  Michael Bowling (mhb@cs.cmu.edu)
//
/* LICENSE: */

#ifndef __game_state_h__
#define __game_state_h__

// We want this to be usable by other teams, so below are the
// constants that this module makes use of.  If this is not being used
// within the CMDragons system then the #if 1 should be changed to a
// #if 0.
//
// These constants should match with those found in constants.h.
//

const int TEAM_BLUE = (1 << 8);
const int TEAM_YELLOW = (1 << 9);

#include "referee_commands.h"
#include "GDebugEngine.h"
#include "staticparams.h"
class GameState {
	static const int GAME_ON =  (1 << 0);
	static const int GAME_OFF = (1 << 1);
	static const int HALTED =   (1 << 2);

	static const int KICKOFF =  (1 << 3);
	static const int PENALTY =  (1 << 4);
	static const int DIRECT =   (1 << 5);
	static const int INDIRECT = (1 << 6);
	static const int RESTART = (KICKOFF | PENALTY | DIRECT | INDIRECT);

	static const int BLUE =     (1 << 8);
	static const int YELLOW =   (1 << 9);

	static const int READY =    (1 << 10);
	static const int NOTREADY = (1 << 11);

	static const int TIME_OUT = (1 << 12);

    static const int PLACE_BALL = (1 << 13);
	
	int state;
	bool isGameOver; // 比赛结束或者半场结束
	// The set of possible states are:
	//
	// { GAME_ON, GAME_OFF, HALTED, NEUTRAL,
	//   { { KICKOFF, PENALTY, DIRECT, INDIRECT } |
	//     { BLUE, YELLOW } | { READY, NOTREADY } } }
	//

	int color;

public:
    int _next_command;
	GameState() : color(BLUE),state(GAME_ON),isGameOver(false){ }

    void init(int _color) { color = _color == PARAM::YELLOW ? YELLOW : BLUE; }

	int get() const{ return state; }
	void set(int _state) { state = _state; }

	// This is the state machine transition function.  It takes the last
	// ref_command as input 
    void transition(char ref_command, bool ball_kicked, int next_command = 0) {
//        GDebugEngine::Instance()->gui_debug_msg()
        _next_command = next_command;
		// CVisionModule::SetNewVision将根据裁判盒线程得到的裁判指令,以及球是否踢出的图像信息判断, 调用此函数, 进行比赛当前状态的设置.
		if( ref_command == COMM_HALF_TIME || ref_command == COMM_OVER_TIME1 || ref_command == COMM_OVER_TIME2 ){
			isGameOver = true;
		}else{
			isGameOver = false;
		}
		if (ref_command == COMM_HALT) { 
			state = HALTED; return; 
		}

		if (ref_command == COMM_STOP) {
			state = GAME_OFF; return; 
		}
        if (ref_command == COMM_BALL_PLACEMENT_BLUE) {
            state = PLACE_BALL | GAME_OFF | BLUE; return;
        }
        if (ref_command == COMM_BALL_PLACEMENT_YELLOW) {
            state = PLACE_BALL | GAME_OFF | YELLOW; return;
        }

		if (ref_command == COMM_START) {
			state = GAME_ON; return; 
		}

		if (ref_command == COMM_READY && state & NOTREADY) {
			state &= ~NOTREADY; state |= READY; return; 
		}

		if ( ref_command == COMM_TIMEOUT_BLUE ){
			state = TIME_OUT | BLUE;
		}

		if ( ref_command == COMM_TIMEOUT_YELLOW ){
			state = TIME_OUT | YELLOW;
		}

		if (state & READY && ball_kicked) {
			state = GAME_ON; return; 
		}

        if (state & GAME_OFF) {
			switch (ref_command) {
			case COMM_KICKOFF_BLUE: 
				state = KICKOFF | BLUE | NOTREADY; return;
			case COMM_KICKOFF_YELLOW: 
				state = KICKOFF | YELLOW | NOTREADY; return;
			case COMM_PENALTY_BLUE: 
				state = PENALTY | BLUE | NOTREADY; return;
			case COMM_PENALTY_YELLOW: 
				state = PENALTY | YELLOW | NOTREADY; return;
			case COMM_DIRECT_BLUE: 
                state = DIRECT | BLUE | READY; return;
			case COMM_DIRECT_YELLOW: 
				state = DIRECT | YELLOW | READY; return;
			case COMM_INDIRECT_BLUE: 
				state = INDIRECT | BLUE | READY; return;
			case COMM_INDIRECT_YELLOW: 
				state = INDIRECT | YELLOW | READY; return;
			default: break;
			}
		}
//        std::cout << "mdbg : " << state << " " << color << " " << directKick() << " " << ourDirectKick() << " " << theirDirectKick() << std::endl;
	}

	bool gameOn() const{ return (state == GAME_ON); }
    bool gameOff() const { return (state & GAME_OFF); }
	bool gameOver() const { return isGameOver; }
    bool restart() const{ return (state & RESTART) != 0; }
	bool ourRestart() const{ return restart() && (state & color); }
	bool theirRestart() const{ return restart() && ! (state & color); }

    bool kickoff() const{ return (state & KICKOFF) != 0; }
	bool ourKickoff() const{ return kickoff() && (state & color); }
	bool theirKickoff() const{ return kickoff() && ! (state & color); }

    bool penaltyKick() const{ return (state & PENALTY) != 0; }
	bool ourPenaltyKick() const{ return penaltyKick() && (state & color); }
	bool theirPenaltyKick() const{ return penaltyKick() && ! (state & color); }

    bool directKick() const{ return (state & DIRECT) != 0; }
	bool ourDirectKick() const{ return directKick() && (state & color); }
	bool theirDirectKick() const{ return directKick() && ! (state & color); }

    bool indirectKick() const{ return (state & INDIRECT) != 0; }
	bool ourIndirectKick() const{ return indirectKick() && (state & color); }
	bool theirIndirectKick() const{ return indirectKick() && ! (state & color); }

	bool freeKick() const{ return directKick() || indirectKick(); }
	bool ourFreeKick() const{ return ourDirectKick() || ourIndirectKick(); }
	bool theirFreeKick() const{ return theirDirectKick() || theirIndirectKick(); }

    bool ballPlacement() const{ return (state & PLACE_BALL) != 0; }
    bool ourBallPlacement() const{ return ballPlacement() && (state & color); }
    bool theirBallPlacement() const{ return ballPlacement() && ! (state & color); }

	bool canMove() const{ return (state != HALTED); }

    bool allowedNearBall() const{ return gameOn() || ((state & color)); }

	bool canKickBall() const{ return gameOn() || (ourRestart() && (state & READY)); }
	
	bool canEitherKickBall() const { return gameOn() || (restart() && (state & READY)); }

	bool isOurTimeout()const { return (state & TIME_OUT) && ( state & color); }

	bool isTheirTimeout()const { return (state & TIME_OUT) && !(state & color); }
};

#endif  
