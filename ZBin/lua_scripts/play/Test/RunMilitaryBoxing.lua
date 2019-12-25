local TOTAL = 5
local centerXF = 2000
local centerYF = 0
local radiusF = 350
local radius = 550
local rotateSpeedF = 0.015
local rotateSpeed = 0.02
local count = 0
local stateCount = 0
--[[]]
local smallR = 250
local centerX = 2000
local centerY = 0
local count = 0
local Rcount= 0 
local dirFlag=1
local lineInterval = 600
local rowInterval = 600
--[[]]
local run = function()
    count = count + 1
    if bufcnt(true,180) then
        stateCount = stateCount + 1
        if stateCount > 10 then
            stateCount = 0
            return "initCircle"
        end
        if stateCount > 5 then
            return "state" .. 5
        else
            return "state" .. stateCount
        end
    end
end
local runPos = function(n,TOTAL)
    assert(n<TOTAL)
    return function()
        local center = CGeoPoint:new_local(centerXF,centerYF) + Utils.Polar2Vector(radiusF,count*rotateSpeedF)
        local pos = center + Utils.Polar2Vector(radius,count*rotateSpeed+math.pi*2/TOTAL*n)
        return pos
    end
end
local sttPos_circle = function(n,TOTAL)
    assert(n<TOTAL)
    return CGeoPoint:new_local(1000+n*300,1800)
end

--[[]]
---[[
local function circlePos( radius,number )
    return CGeoPoint:new_local(centerX, centerY) + Utils.Polar2Vector(radius,math.pi*2/5*number)
end
--]]
local function circlePosMove( radius,number )
    return function ()
        return CGeoPoint:new_local(centerX, centerY) +  Utils.Polar2Vector(radius+200*(Rcount-2),math.pi*2/5*(number+dirFlag*(count-2)))
    end

end
--[[]]
local sttPos_snake = function(n,TOTAL)
    assert(n<TOTAL)
    return CGeoPoint:new_local(2400,300*(n-TOTAL/2.0))
end
local STEP = 300
local X = {  8, 8, 8, 8, 8, 8, 9, 9, 9,10,
            10,10, 9, 9, 8, 8, 7, 6, 6, 5,
             5, 4, 4, 3, 3, 2, 2, 2, 3, 4,
             4, 5, 5, 6, 6, 7, 8, 8, 8, 7,
             7, 6, 6, 5, 5, 4, 3, 3, 3, 4,
             5, 5, 6, 6, 7, 7, 8, 8, 8, 7,
             6, 6, 5, 5, 4, 4, 3, 2, 2, 2,
             3, 3, 4, 4, 5, 5, 6, 6, 7, 8,
             8, 9, 9,10,10,10, 9, 9, 9, 8}
local Y = { -2,-1, 0, 1, 2, 3, 3, 4, 5, 5,
             6, 7, 7, 8, 8, 9, 9, 9, 8, 8,
             7, 7, 6, 6, 5, 5, 4, 3, 3, 3,
             4, 4, 5, 5, 6, 6, 6, 5, 4, 4,
             3, 3, 2, 2, 1, 1, 1, 0,-1,-1,
            -1,-2,-2,-3,-3,-4,-4,-5,-6,-6,
            -6,-5,-5,-4,-4,-3,-3,-3,-4,-5,
            -5,-6,-6,-7,-7,-8,-8,-9,-9,-9,
            -8,-8,-7,-7,-6,-5,-5,-4,-3,-3}
local count_2 = 0
local update = function()
    count_2 = (count_2 + 1) % 90
end
local p = function(n)
    return function()
        local index = (count_2 + n) % 90 + 1
        return CGeoPoint:new_local(X[index]*STEP,Y[index]*STEP)
    end
end
local d = function(n)
    return function()
        local index = (count_2 + n) % 90 + 1
        local next_index = (count_2 + n + 1) % 90 + 1
        return (CGeoPoint:new_local(X[next_index]*STEP,Y[next_index]*STEP) - CGeoPoint:new_local(X[index]*STEP,Y[index]*STEP)):dir()
    end
end
--[[]]

gPlayTable.CreatePlay{

firstState = "state0",
["state0"] = {
    switch = run,
    Assister = task.goSpeciPos(sttPos_circle(0,5)),
    Leader   = task.goSpeciPos(sttPos_circle(1,5)),
    Special  = task.goSpeciPos(sttPos_circle(2,5)),
    Defender = task.goSpeciPos(sttPos_circle(3,5)),
    Middle   = task.goSpeciPos(sttPos_circle(4,5)),
    match = "(ALSMD)"
},
["state1"] = {
    switch = run,
    Assister = task.goSpeciPos(runPos(0,1)),
    Leader   = task.goSpeciPos(sttPos_circle(1,5)),
    Special  = task.goSpeciPos(sttPos_circle(2,5)),
    Defender = task.goSpeciPos(sttPos_circle(3,5)),
    Middle   = task.goSpeciPos(sttPos_circle(4,5)),
    match = "[ALSMD]"
},
["state2"] = {
    switch = run,
    Assister = task.goSpeciPos(runPos(0,2)),
    Leader   = task.goSpeciPos(runPos(1,2)),
    Special  = task.goSpeciPos(sttPos_circle(2,5)),
    Defender = task.goSpeciPos(sttPos_circle(3,5)),
    Middle   = task.goSpeciPos(sttPos_circle(4,5)),
    match = "[ALSMD]"
},
["state3"] = {
    switch = run,
    Assister = task.goSpeciPos(runPos(0,3)),
    Leader   = task.goSpeciPos(runPos(1,3)),
    Special  = task.goSpeciPos(runPos(2,3)),
    Defender = task.goSpeciPos(sttPos_circle(3,5)),
    Middle   = task.goSpeciPos(sttPos_circle(4,5)),
    match = "[ALSMD]"
},
["state4"] = {
    switch = run,
    Assister = task.goSpeciPos(runPos(0,4)),
    Leader   = task.goSpeciPos(runPos(1,4)),
    Special  = task.goSpeciPos(runPos(2,4)),
    Defender = task.goSpeciPos(runPos(3,4)),
    Middle   = task.goSpeciPos(sttPos_circle(4,5)),
    match = "[ALSMD]"
},
["state5"] = {
    switch = run,
    Assister = task.goSpeciPos(runPos(0,5)),
    Leader   = task.goSpeciPos(runPos(1,5)),
    Special  = task.goSpeciPos(runPos(2,5)),
    Defender = task.goSpeciPos(runPos(3,5)),
    Middle   = task.goSpeciPos(runPos(4,5)),
    match = "[ALSMD]"
},

["initCircle"]={
    switch= function ()
        if bufcnt(
            player.toTargetDist("Assister") <200 and 
            player.toTargetDist("Leader")   <200 and 
            player.toTargetDist("Special")  <200 and 
            player.toTargetDist("Defender") <200 and 
            player.toTargetDist("Middle")   <200 
            ,20,300) then
            count=2
            Rcount=2
        --if count==2 then
            return "largeCircle"
        --end
        --return "initCircle"
        end
    end,
    Assister = task.goSpeciPos(circlePos(smallR,1),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Leader   = task.goSpeciPos(circlePos(smallR,2),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Special  = task.goSpeciPos(circlePos(smallR,3),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Middle   = task.goSpeciPos(circlePos(smallR,4),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Defender = task.goSpeciPos(circlePos(smallR,5),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    match    = "(ALSMD)"
},
---[[
["largeCircle"]={
    switch= function ()
        if bufcnt(
            player.toTargetDist("Assister") <200 and 
            player.toTargetDist("Leader")   <200 and 
            player.toTargetDist("Special")  <200 and 
            player.toTargetDist("Defender") <200 and 
            player.toTargetDist("Middle")   <200 
            ,20,300) then
            count=count+1   
            Rcount=Rcount+1
            if count==7 then 
                return "smallCircle"
            end
            return "largeCircle"
        end
    end,
    Assister = task.goSpeciPos(circlePosMove(smallR,1),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Leader   = task.goSpeciPos(circlePosMove(smallR,2),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Special  = task.goSpeciPos(circlePosMove(smallR,3),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Middle   = task.goSpeciPos(circlePosMove(smallR,4),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Defender = task.goSpeciPos(circlePosMove(smallR,5),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    match    = "{ALSMD}"
},

["smallCircle"]={
    switch= function ()
        if bufcnt(
            player.toTargetDist("Assister") <200 and 
            player.toTargetDist("Leader")   <200 and 
            player.toTargetDist("Special")  <200 and 
            player.toTargetDist("Defender") <200 and 
            player.toTargetDist("Middle")   <200 
            ,20,300) then
            count=count+1
            Rcount=Rcount-1 
            if count==12 then
                if dirFlag==-1 then
                    return "goLine"
                end
                dirFlag=-dirFlag
                count=2
                return "largeCircle"
            end
            return "smallCircle"
        end
    end,
    Assister = task.goSpeciPos(circlePosMove(smallR,1),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Leader   = task.goSpeciPos(circlePosMove(smallR,2),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Special  = task.goSpeciPos(circlePosMove(smallR,3),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Middle   = task.goSpeciPos(circlePosMove(smallR,4),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    Defender = task.goSpeciPos(circlePosMove(smallR,5),player.toPointDir(CGeoPoint:new_local(centerX,centerY))),
    match    = "{ALSMD}"
},
--]]
["goLine"]={
    switch = function ()
        if bufcnt(
            player.toTargetDist("Assister") <200 and 
            player.toTargetDist("Leader")   <200 and 
            player.toTargetDist("Special")  <200 and 
            player.toTargetDist("Defender") <200 and 
            player.toTargetDist("Middle")   <200 
            ,20,300) then
            return "goDown"
        end
    end,
    Assister = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY-2*rowInterval      )   ,-math.pi),
    Leader   = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY-rowInterval        )   ,-math.pi),
    Special  = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY                    )   ,-math.pi),
    Middle   = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY+rowInterval        )   ,-math.pi),
    Defender = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY+2*rowInterval      )   ,-math.pi),
    match    = "(ALSMD)"    
},
["goDown"]={
    switch = function ()
        if bufcnt(
            player.toTargetDist("Assister") <200 and 
            player.toTargetDist("Leader")   <200 and 
            player.toTargetDist("Special")  <200 and 
            player.toTargetDist("Defender") <200 and 
            player.toTargetDist("Middle")   <200 
            ,20,300) then
            return "turnAround"
        end
    end,
    Assister = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY-2*rowInterval  ),-math.pi),
    Leader   = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY-rowInterval    ),-math.pi),
    Special  = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY                ),-math.pi),
    Middle   = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY+rowInterval    ),-math.pi),
    Defender = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY+2*rowInterval  ),-math.pi),
    match    = "(ALSMD)"    
},

["turnAround"]= {
    switch = function()
        if bufcnt(
            math.abs(player.dir("Assister")) <0.1 and
            math.abs(player.dir("Leader")) <0.1 and
            math.abs(player.dir("Special")) <0.1 and
            math.abs(player.dir("Middle")) <0.1 and
            math.abs(player.dir("Defender")) <0.1 
            ,20,300) then
        return "goBack"
        end
    end,
    Assister = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY-2*rowInterval  ),0),
    Leader   = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY-rowInterval    ),0),
    Special  = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY                ),0),
    Middle   = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY+rowInterval    ),0),
    Defender = task.goSpeciPos(CGeoPoint:new_local(centerX-lineInterval, centerY+2*rowInterval  ),0),
    match    = "{ALSMD}"    
},

["goBack"]= {
    switch = function ()
        if bufcnt(
            player.toTargetDist("Assister") <200 and 
            player.toTargetDist("Leader")   <200 and 
            player.toTargetDist("Special")  <200 and 
            player.toTargetDist("Defender") <200 and 
            player.toTargetDist("Middle")   <200 
            ,20,300) then
            return "start"
        end
    end,
    Assister = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY-2*rowInterval  ),0),
    Leader   = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY-rowInterval    ),0),
    Special  = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY                ),0),
    Middle   = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY+rowInterval    ),0),
    Defender = task.goSpeciPos(CGeoPoint:new_local(centerX+lineInterval, centerY+2*rowInterval  ),0),
    match    = "{ALSMD}"
},
["start"] = {
    switch = function()
        if bufcnt(true,180) then
            update()
            return "run"
        end 
    end,
    Assister = task.goSpeciPos(sttPos_snake(0,5),math.pi/2),
    Leader   = task.goSpeciPos(sttPos_snake(1,5),math.pi/2),
    Special  = task.goSpeciPos(sttPos_snake(2,5),math.pi/2),
    Middle   = task.goSpeciPos(sttPos_snake(3,5),math.pi/2),
    Defender = task.goSpeciPos(sttPos_snake(4,5),math.pi/2),
    match = "(ALSMD)"
},
["run"] = {
    switch = function()
        if bufcnt(true,17) then
            update()
            return "run"
        end
    end,
    Assister = task.goSimplePos(p(0),d(0)),
    Leader   = task.goSimplePos(p(1),d(1)),
    Special  = task.goSimplePos(p(2),d(2)),
    Middle   = task.goSimplePos(p(3),d(3)),
    Defender = task.goSimplePos(p(4),d(4)),
    match = "{ALSMD}"
},

name = "RunMilitaryBoxing",
applicable ={
    exp = "a",
    a = true
},
attribute = "attack",
timeout = 99999
}

--[[
0
1
2
3
4
5
--]]