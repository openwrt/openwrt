#!/usr/bin/lua

dailyUsageDB = arg[1]

bw = {}
maclist = {}

file = io.open(dailyUsageDB, "r")
i = 0
repeat
	line = file:read("*line")
	if line == nil then
		break
	end
	s, e = line:find("\"mac\":\"")
	bs, be = line:find("\"", e+1)
	mac = line:sub(e+1, bs-1)
	if bw[mac] == nil then
		maclist[i] = mac
		i = i + 1
		bw[mac] = {}
		bw[mac]['down'] = 0
		bw[mac]['offdown'] = 0
		bw[mac]['up'] = 0
		bw[mac]['offup'] = 0
	end
	s, e = line:find("\"down\":\"")
	bs, be = line:find("\"", e+1)
	down = tonumber(line:sub(e+1, bs-1))
	bw[mac]['down'] = bw[mac]['down'] + down
	s, e = line:find("\"up\":\"")
	bs, be = line:find("\"", e+1)
	up = tonumber(line:sub(e+1, bs-1))
	bw[mac]['up'] = bw[mac]['up'] + up
	s, e = line:find("\"offdown\":\"")
	bs, be = line:find("\"", e+1)
	offdown = tonumber(line:sub(e+1, bs-1))
	bw[mac]['offdown'] = bw[mac]['offdown'] + offdown
	s, e = line:find("\"offup\":\"")
	bs, be = line:find("\"", e+1)
	offup = tonumber(line:sub(e+1, bs-1))
	bw[mac]['offup'] = bw[mac]['offup'] + offup
	s, e = line:find("\"ip\":\"")
	bs, be = line:find("\"", e+1)
	bw[mac]['ip'] = line:sub(e+1, bs-1)
	s, e = line:find("\"name\":\"")
	bs, be = line:find("\"", e+1)
	bw[mac]['name'] = line:sub(e+1, bs-1)
until 1==0
file:close()

j=0
file = io.open(dailyUsageDB .. ".bk", "w")
while maclist[j] ~= nil do
	mac = maclist[j]
	dline = "\"mac\":\"" .. mac .. "\",\"down\":\"" .. bw[mac]['down'] .. "\",\"up\":\"" .. bw[mac]['up'] .. "\",\"offdown\":\"0\",\"offup\":\"0\",\"ip\":\"" .. bw[mac]['ip'] .. "\",\"name\":\"" .. bw[mac]['name'] .. "\""
	file:write(dline, "\n")
	j = j + 1
end
file:close()
	
