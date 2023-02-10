#!/usr/bin/lua

local uci  = require "luci.model.uci".cursor()
 
logfile = {}
infile = arg[1]
outfile = arg[2]

i=0
ifile = io.open(infile, "r")
if ifile == nil then
	return
end
repeat
	local line = ifile:read("*line")
	if line == nil then
		break
	end
	if string.len(line) > 1 then
		i = i + 1
		logfile[i] = line
	end
until 1==0
ifile:close()

bff = uci:get("variable", "info", "buffersize")
if bff == nil then
	maxs = 50
else
	maxs = tonumber(bff)
end
if i < maxs then
	j = 1
else
	j = i - maxs - 1
end
ofile = io.open(outfile, "w")
for k=j,i do
	if logfile[k] ~= nil then
		ofile:write(logfile[k] .. "\n")
	end
end
ofile:close()