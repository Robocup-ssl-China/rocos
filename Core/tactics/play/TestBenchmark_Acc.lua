local p = {
    CGeoPoint:new_local(-3000,-1000),
    CGeoPoint:new_local(3000,-1000)
}
local TEST_X = false
local FAIL_DEGREE = 30.0

local p_dir = (p[2]-p[1]):dir()
local task_dir = TEST_X and p_dir or p_dir+math.pi/2

local ROBOT_OFFSET = Utils.Polar2Vector(400,p_dir+math.pi/2)

local task_max_acc = 2000
local task_max_vel = 3500
local task_flag = flag.not_avoid_our_vehicle

local det_max_vel = 0
local det_rot_err = 0.0
local trigger_cycle = 0

local result_list = { -- {task_max_acc, task_max_vel, det_max_vel, det_rot_err, time(s)} - {3000,2000,3000,0.0}
}
local MAX_TEST_ACC_STEP = 800
local MIN_TEST_ACC_STEP = 300
-- default test acc
local set_new_task_param = function()
    local last_acc = task_max_acc
    local last_vel = task_max_vel
    local last_det_vel = det_max_vel
    local last_det_err = det_rot_err
    local last_success = det_rot_err < FAIL_DEGREE
    local total_success_cnt = 0.0
    local total_testing_cnt = 0
    for i=1,#result_list do
        local res = result_list[i]
        total_success_cnt = total_success_cnt + (res[4] < FAIL_DEGREE and 1 or 0)
        total_testing_cnt = total_testing_cnt + 1
    end
    local success_rate = total_success_cnt / total_testing_cnt
    local step = MAX_TEST_ACC_STEP*success_rate - (1-success_rate)*MAX_TEST_ACC_STEP + (last_success and MIN_TEST_ACC_STEP or -MIN_TEST_ACC_STEP)
    task_max_acc = math.max(2000,math.min(9999,task_max_acc+step))
end

local state_reset = function(store)
    if store then
        local times = (vision:getCycle() - trigger_cycle)*1.0/(1.0*param.frameRate)
        local result = {task_max_acc, task_max_vel, det_max_vel, det_rot_err, times}
        table.insert(result_list,result)
        set_new_task_param()
        -- store only the last 10
        if #result_list > 20 then
            table.remove(result_list,1)
        end
    end
    det_max_vel = 0
    det_rot_err = 0.0
    trigger_cycle = vision:getCycle()
end

local debug_F = function()
    local sx,sy = 200,-1000
    local span = 140
    local sp = CGeoPoint:new_local(sx,sy)
    local v = CVector:new_local(0,-span)

    local role = "Leader"
    local rawVel = player.rawVelMod(role)
    local rawDir = player.dir(role)
    det_max_vel = math.max(det_max_vel,rawVel)
    det_rot_err = math.max(det_rot_err,math.abs(rawDir-task_dir)*180/math.pi)

    debugEngine:gui_debug_line(p[1],p[2],param.GRAY)
    debugEngine:gui_debug_msg(sp+v*0,string.format("Set     ACC : %4.0f",task_max_acc),param.BLUE)
    debugEngine:gui_debug_msg(sp+v*1,string.format("Set MAX VEL : %4.0f",task_max_vel),param.BLUE)
    debugEngine:gui_debug_msg(sp+v*2,string.format("Det MAX VEL : %4.0f",rawVel),param.BLUE)
    debugEngine:gui_debug_msg(sp+v*3,string.format("Det MAX VEL : %4.0f",det_max_vel),param.GREEN)
    debugEngine:gui_debug_msg(sp+v*4,string.format("Rot MAX ERR°: %4.1f",det_rot_err),det_rot_err < FAIL_DEGREE and param.GREEN or param.RED)


    local rx,ry = 2000,-1000
    local span = 85
    local rp = CGeoPoint:new_local(rx,ry)
    local rv = CVector:new_local(0,-span)
    debugEngine:gui_debug_msg(rp+rv*0,                  " N,  ACC, DetV, RotE, Time",param.ORANGE,0,80)
    for i=1,#result_list do
        local res = result_list[i]
        debugEngine:gui_debug_msg(rp+rv*i,string.format("%2d, %4.0f, %4.0f, %4.1f, %4.1fs",i,res[1],res[3],res[4],res[5]),res[4] < FAIL_DEGREE and param.GREEN or param.RED, 0, 80)
    end
end

local F_task_max_acc = function()
    return task_max_acc
end

local F_task_max_vel = function()
    return task_max_vel
end

gPlayTable.CreatePlay{

firstState = "start",
["start"] = {
    switch = function()
        debug_F()
        if bufcnt(player.toTargetDist("Leader") < 30, 60) then
            state_reset()
            return "run1"
        end
    end,
    Leader = task.goCmuRush(p[2],task_dir),
    a = task.goCmuRush(p[2]+ROBOT_OFFSET,task_dir),
    match = "(L)(a)"
},
["run1"] = {
    switch = function()
        debug_F()
        if bufcnt(player.toTargetDist("Leader") < 30, 60) then
            state_reset(true)
            return "run2"
        end
    end,
    Leader = task.goCmuRush(p[1],task_dir,F_task_max_acc,task_flag,nil,nil,F_task_max_vel,true),
    a = task.goCmuRush(p[1]+ROBOT_OFFSET,task_dir,F_task_max_acc,task_flag,nil,nil,F_task_max_vel,true),
    match = ""
},
["run2"] = {
    switch = function()
        debug_F()
        if bufcnt(player.toTargetDist("Leader") < 30, 60) then
            state_reset(true)
            return "run1"
        end
    end,
    Leader = task.goCmuRush(p[2],task_dir,F_task_max_acc,task_flag,nil,nil,F_task_max_vel,true),
    a = task.goCmuRush(p[2]+ROBOT_OFFSET,task_dir,F_task_max_acc,task_flag,nil,nil,F_task_max_vel,true),
    match = ""
},


name = "TestBenchmark_Acc",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
