-- 根据不同的位置选择不同的定位球
-- 注意：此文件中只能定义一些常量，并且以时时更新的数据做判断
-- 使用gOurIndirectTable中的元素和本文件中的local变量进行控制切换
-- 其区别是gOurIndirectTable中的元素为状态保持量，而local变量为时时控制量

-- 在进入每一个定位球时，需要在第一次进时进行保持
if vision:getCycle() - gOurIndirectTable.lastRefCycle > 6 then
    if type(gOppoConfig.CornerKick) == "function" then
    	gCurrentPlay = cond.getOpponentScript("Ref_CornerKickV",gOppoConfig.CornerKick(),20)
    else
    	gCurrentPlay = cond.getOpponentScript("Ref_CornerKickV",gOppoConfig.CornerKick,20)
    end
    -- gCurrentPlay = "Ref_CornerKickMessi"
end