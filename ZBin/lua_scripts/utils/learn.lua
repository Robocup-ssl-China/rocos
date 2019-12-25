module(..., package.seeall)

function calNormal( num, a, tau)
	return math.exp(-1.0*a*num/tau)
end

function calGussiFunc( num, center, dist )
	local tmp = num - center
	return math.exp(-0.5*tmp*tmp*dist)
end

function calFTarget( rfwrList, x)
	local nume = 0
	local deno = 0
	--m c b1 b0
	for i,v in pairs(rfwrList) do
		local w = calGussiFunc(x, v[2], v[1]*v[1])
		local vecX1 = x - v[2]
		local vecX0 = 1
		nume = nume + w*(v[3]*vecX1 + v[4]*vecX0)
		deno = deno + w
	end

	return nume/deno
end

function calRFWRRes( t, rfwrList, a, tau, k, startP, endP, TNum)
	local x = calNormal(t, a, tau)
	local calF = calFTarget(rfwrList, x)
	local modifyF = calNormal(TNum, a, tau)
	-- 新的算法

	local new_a = math.sqrt(k)/tau
	local new_b = -k*(endP - startP)/(tau*tau*(1-modifyF))
	local new_c = k*(endP + calF)/(tau*tau) - modifyF*new_b
	local new_d = a/tau
	
	local new1 = new_c/(new_a*new_a)
	local new2 = new_b/((new_d - new_a)*(new_d - new_a))
	local new3 = new_c/new_a
	local new4 = new_b/(new_d - new_a)
	local res  = (-new1-new2+(-new3+new4)*t)*math.exp(-t*math.sqrt(k)/tau) + new2*x + new1
	return res
	-- 原始的算法
	-- local item = tau/math.sqrt(k)
	-- local item2 = item*item
	-- local item3 = (item2 - item2*math.exp(-1.0*t/item) - item*t*math.exp(-1.0*t/item))
	-- local item4 = (k*endP + (endP - startP) * calF)
	-- local res  = item3 * item4 / (tau * tau)
	-- return res
end

function  calRFWRVel( t, rfwrList, a, tau, k, startP, endP, TNum)
	local x_this = calRFWRRes(t, rfwrList, a, tau, k, startP, endP, TNum)
	local x_next = calRFWRRes(t+1, rfwrList, a, tau, k, startP, endP, TNum)
	return x_next - x_this
end