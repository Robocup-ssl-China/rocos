function _c(f,...) -- auto convert for closure
	if type(f) == "function" then
		return f(...)
	end
	return f
end