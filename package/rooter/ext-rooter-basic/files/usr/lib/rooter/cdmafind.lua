#!/usr/bin/lua

drv = {}
idV = arg[1]
idP = arg[2]

retval = 0

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- MAIN

local i=0
local file = io.open("/tmp/cdma", "r")
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
						s, e = line:find("Product=")
						if s ~= nil then
							s, e = line:find(" CDMA")
							if s ~= nil then
								retval = 1
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