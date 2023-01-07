#!/usr/bin/lua

printf = function(s,...)
	if pflag ~= 0 then
		io.write(s:format(...))
		local ss = s:format(...)
		if echo == 1 then
			os.execute("/usr/lib/rooter/logprint.sh " .. ss)
		end
	end
end

function sleep(n)
  os.execute("sleep " .. tonumber(n))
end

function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end

while file_exists("/tmp/sysinfo/board_name") == false do
	sleep(1)
end

if file_exists("/etc/custom") then
	file = io.open("/etc/custom", "r")
	board = file:read("*line")
	model = file:read("*line")
	hostname = file:read("*line")
	file:close()

	os.execute("/usr/lib/custom/hostname.sh " .. hostname)
	os.execute("/usr/lib/custom/wifi.sh &")
end



