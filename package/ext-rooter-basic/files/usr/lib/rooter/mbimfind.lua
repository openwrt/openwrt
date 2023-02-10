#!/usr/bin/lua

drv = {}
idV = arg[1]
idP = arg[2]

retval = 0
echo = 0

printf = function(s,...)
	if pflag ~= 0 then
		io.write(s:format(...))
		local ss = s:format(...)
		if echo == 1 then
			os.execute("/usr/lib/rooter/logprint.sh " .. ss)
		end
	end
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- MAIN
-- C: #Ifs= 3 Cfg#= 3 Atr=a0 MxPwr=500mA

Cfgs = 1
local i=0
local file = io.open("/tmp/prembim", "r")
repeat
	local line = file:read("*line")
	if line == nil then
		break
	end
	if string.len(line) > 5 then
		s, e = line:find("D:")
		if s ~= nil then
			cs, ce = line:find("#Cfgs= ")
			Cfgs = trim(line:sub(ce+1))
			--printf("Cfgs = %s\n", Cfgs)
		end
		
		s, e = line:find("Vendor=")
		if s ~= nil then
			cs, ce = line:find(" ", e)
			m_idV = trim(line:sub(e+1, cs-1))
			s, e = line:find("ProdID=")
			cs, ce = line:find(" ", e)
			m_idP = trim(line:sub(e+1, cs-1))
			--printf("%s %s\n", m_idV, m_idP)
			if m_idV == idV and m_idP == idP then
				if Cfgs == "1" then
					break
				end
				Inter = 0
				repeat
					line = file:read("*line")
					if line == nil then
						break
					end
					if string.len(line) > 5 then
						s, e = line:find("C:")
						if s ~= nil then
							s, e = line:find("Cfg#= ")
							cs, ce = line:find(" ", e+1)
							Inter = trim(line:sub(e+1, cs))
							--printf("Inter = %s\n", Inter)
						end
						s, e = line:find("T:")
						if s ~= nil then
							break
						end
						s, e = line:find("Cls=02")
						if s ~= nil then
							s, e = line:find("Sub=0e Prot=00 Driver=")
							if s ~= nil then
								retval=tonumber(Inter)
								--printf("retval = %d\n", retval)
								break
							end
						end
					end
				until 1==0
				break
			end		
		end
	end
until 1==0
file:close()


os.exit(retval)