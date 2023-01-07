#!/usr/bin/lua

mfile = "/tmp/sysinfo/model"
nfile="/etc/custom"
local file = io.open(nfile, "r")
if file == nil then
	return
end
linex = file:read("*line")
file:close()
file = io.open(mfile, "w")
if file == nil then
	return
end

file:write(linex,"\n")
file:close()