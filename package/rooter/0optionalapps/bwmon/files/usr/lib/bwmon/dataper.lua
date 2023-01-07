#!/usr/bin/lua

alloc = arg[1] -- allocate increment in GB
per = arg[2] -- fixed percentage in GB
used = arg[3] -- amt used in Kb

--used = used / 10000 -- used in GB
percent = (used / alloc) * 100

running = "0"
if percent >= tonumber(per) then
	running = "1"
end

tfile = io.open("/tmp/bwper", "w")
tfile:write("runn=\"", running, "\"")
tfile:close()		