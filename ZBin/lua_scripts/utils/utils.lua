function _(f)
	local res
	if type(f) == "function" then
		return f()
	end
	return f
end