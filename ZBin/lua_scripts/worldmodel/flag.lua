module(..., package.seeall)

nothing      					= 0x00000000
-- slowly      					= 0x00000001
-- quickly	    				= 0x00000002
-- do_not_stop 					= 0x00000008
dodge_ball						= 0x00000010
-- pos_only	 					= 0x00000020
avoid_shoot_line 				= 0x00000040
-- not_dribble     				= 0x00000080
dribbling						= 0x00000100
dodge_our_defense_box			= 0x00000200
not_avoid_their_vehicle			= 0x00000400
-- accurately						= 0x00000800
turn_around_front				= 0x00001000
dodge_referee_area				= 0x00002000
-- specify_speed	   			= 0x00004000
-- specify_rotate_speed			= 0x00008000
-- specify_acceleration 		= 0x00010000
-- specify_rotate_acceleration	= 0x00020000
break_through					= 0x00040000
not_avoid_our_vehicle			= 0x00080000
avoid_stop_ball_circle	 		= 0x00100000
avoid_their_ballplacement_area  = 0x00200000
avoid_half_field				= 0x00400000
-- specify_direction			= 0x01000000	
min_dss						    = 0x02000000
our_ball_placement				= 0x04000000
allow_dss						= 0x08000000
free_kick                       = 0x20000000
use_immortal                    = 0x40000000
force_kick						= 0x80000000

-- kick flag
kick                            = 0x00000001
chip                            = 0x00000002
dribble                         = 0x00000004
safe                            = 0x00000008