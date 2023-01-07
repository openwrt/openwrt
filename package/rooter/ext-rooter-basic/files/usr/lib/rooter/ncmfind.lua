#!/usr/bin/lua

drv = {}
idV = arg[1]
idP = arg[2]

printf = function(s,...)
	io.write(s:format(...))
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

local Inter=0
local file = io.open("/tmp/wdrv", "r")
repeat
	local line = file:read("*line")
	if line == nil then
		break
	end
	if string.len(line) > 5 then
		s, e = line:find("Vendor=")
		if s ~= nil then
			cs, ce = line:find(" ", e)
			m_idV = trim(line:sub(e+1, cs-1))
			s, e = line:find("ProdID=")
			cs, ce = line:find(" ", e)
			m_idP = trim(line:sub(e+1, cs-1))
			if m_idV == idV and m_idP == idP then
				repeat
					line = file:read("*line")
					if line == nil then
						break
					end
					if string.len(line) > 5 then
						s, e = line:find("Prot=")
						if s ~= nil then
							cs, ce = line:find(" ", e+1)
							proto = trim(line:sub(e+1, cs))
							if proto == "2" or proto == "02" or proto == "12" or proto == "32" or proto == "42" or proto == "62" or proto == "72" then
								break
							else
								s, e = line:find("=option")
								if s ~= nil then
									Inter = Inter + 1
								end
							end
						end
						s, e = line:find("T:")
						if s ~= nil then
							break
						end
					end
				until 1==0
				break
			end		
		end
	end
until 1==0
file:close()

os.exit(Inter)