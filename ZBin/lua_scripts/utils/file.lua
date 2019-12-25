module(..., package.seeall)

function open(file_name, mode)
	-- local recFile
	-- return function()
		recFile = io.open(file_name, mode)
		return recFile
	-- end
end

function write(rec_file, fmt, ...)
	rec_file:write(string.format(fmt,...))
end

function close(rec_file)
	rec_file:close()
end