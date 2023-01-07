#!/usr/bin/lua

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

ulat = arg[1]
ulon = arg[2]

url = {}
slat = {}
slon = {}
city = {}
dist = {}
prov = {}

radius = 6371

cnt = 0
clos = 99999999.99
index = 0
file = io.open("/tmp/slist", "r")
if file ~= nil then
	repeat
		line = file:read("*line")
		if line == nil then
			break
		end
		cnt = cnt + 1
		s, e = line:find(" ")
		if s ~= nil then
			url[cnt] = trim(line:sub(0, s-1))
			cs, ce = line:find(" ", e+1)
			slat[cnt] = trim(line:sub(e, cs-1))
			s, e = line:find(" ", ce+1)
			slon[cnt] = trim(line:sub(ce, s-1))
			cs, ce = line:find("\"", e+1)
			s, e = line:find("\"", ce+1)
			city[cnt] = trim(line:sub(ce+1, s-1))
			prov[cnt] = trim(line:sub(e+1))
			
			dlat = math.rad(slat[cnt] - ulat)
			dlon = math.rad(slon[cnt] - ulon)
			a = (math.sin(dlat / 2) * math.sin(dlat / 2) + math.cos(math.rad(ulat)) * math.cos(math.rad(slat[cnt])) * math.sin(dlon / 2) * math.sin(dlon / 2))
			c = 2 * math.atan(math.sqrt(a), math.sqrt(1 - a))
			dist[cnt] = radius * c

			if dist[cnt] < clos then
				clos = dist[cnt]
				index = cnt
			end
		end
	until 1==0
	file:close()
	tfile = io.open("/tmp/close", "w")
	tfile:write("CURL=\"", url[index], "\"\n")
	tfile:write("CITY=\"", city[index], "\"\n")
	tfile:write("PROV=\"", prov[index], "\"")
	tfile:close()
end


