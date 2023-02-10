#!/usr/bin/lua

cmd = arg[1]
interface = arg[2]

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

if interface ~= "wan" and interface ~= "wwan" then
	s, e = interface:find("wan")
	if s ~= nil then
		mnum = trim(interface:sub(e+1))
		if cmd == "1" then
			os.execute("/usr/lib/rooter/connect/disablemw3.sh " .. mnum)
			line = "echo \"0\" > /tmp/mdown" .. mnum
		else
			line = "rm -f /tmp/mdown" .. mnum
		end
		os.execute(line)
	end
end