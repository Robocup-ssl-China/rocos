function _(f,...)
	if type(f) == "function" then
		return f(...)
	end
	return f
end