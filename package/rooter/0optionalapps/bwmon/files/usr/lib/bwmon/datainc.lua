#!/usr/bin/lua

prev = arg[1] -- previous increment in GB
incr = arg[2] -- fixed increment in GB
used = arg[3] -- amt used in Kb

used = used / 1000000 -- used in GB
current = prev + incr
running = "0"
if used >= current then
	while used >= (current + incr) do
		current = current + incr
	end
	prev = current
	running = "1"
end

tfile = io.open("/tmp/bwinc", "w")
tfile:write("prev=\"", prev, "\"\n")
tfile:write("runn=\"", running, "\"")
tfile:close()		