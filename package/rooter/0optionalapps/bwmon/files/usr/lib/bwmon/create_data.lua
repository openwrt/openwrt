#!/usr/bin/lua

dirname = '/tmp/bwmon/data'
filepost = "-mac_data.js.bk"
bw = {}
maclist = {}
devices = {}
totaldevices = 0
totaldown = 0
totalup = 0
total = 0

printf = function(s,...)
		local ss = s:format(...)
		os.execute("/usr/lib/rooter/logprint.sh " .. ss)
end

function ltrim(s)
  return s:match'^%s*(.*)'
end

function calc(total)
	if total < 1000 then
		tstr = string.format("%.2f", total)
		tfm = " K"
	else
		if total < 1000000 then
			tstr = string.format("%.2f", total/1000)
			tfm = " MB"
		else
			tstr = string.format("%.2f", total/1000000)
			tfm = " GB"
		end
	end
	str = tstr .. tfm
	return ltrim(str)
end

function monthly(datafile)
	file = io.open(datafile, "r")
	i = 0
	dayx = 0
	repeat
		line = file:read("*line")
		if line == nil then
			break
		end
		s, e = line:find("start day")
		if s ~= nil then
				dayx = dayx + 1
				repeat
					line = file:read("*line")
					s, e = line:find("end day")
					if s ~= nil then
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
		end					
	until 1==0
	file:close()
	return dayx
end

function totals(bw, maclist, dayz)
	totaldown = 0
	totalup = 0
	utotaldown = 0
	utotalup = 0
	j=0
	while maclist[j] ~= nil do
		totaldown = totaldown + bw[maclist[j]]['down']
		totalup = totalup + bw[maclist[j]]['up']
		utotaldown = utotaldown + bw[maclist[j]]['offdown']
		utotalup = utotalup + bw[maclist[j]]['offup']
		j = j + 1
	end
	total = totalup + totaldown
	ptotal = (total / dayz) * 30
end

function showdevices(bw, maclist)
	k = 0
	while maclist[k] ~= nil do
		k = k + 1
	end
	if k > 0 then
		j = 0
		while maclist[j] ~= nil do
			dtot = bw[maclist[j]]['down'] + bw[maclist[j]]['up']
			devices[j] = bw[maclist[j]]['ip'] .."|" .. maclist[j]
			devices[j] = devices[j] .. "|" .. calc(bw[maclist[j]]['down']) .. "|" .. calc(bw[maclist[j]]['up'])
			devices[j] = devices[j] .. "|" .. calc(dtot) .. "|" .. bw[maclist[j]]['name']
			j = j + 1
		end
	end
	totaldevices = j
end

os.execute("echo 0 > /tmp/lockbw")
dataname = nil
f = io.popen('/bin/ls ' .. dirname)
for name in f:lines() do 
	s, e = name:find(filepost)
	if s ~= nil then
		dataname = dirname .. "/" .. name
	end
end
f:close()

if dataname ~= nil then
	days = monthly(dataname)
	totals(bw, maclist, days)
	tfile = io.open("/tmp/bwdata", "w")
	tfile:write(days, "\n")
	tfile:write(tostring(total), "\n")
	tfile:write(calc(total), "\n")
	tfile:write(tostring(totaldown), "\n")
	tfile:write(calc(totaldown), "\n")
	tfile:write(tostring(totalup), "\n")
	tfile:write(calc(totalup), "\n")
	tfile:write(tostring(ptotal), "\n")
	tfile:write(calc(ptotal), "\n")
	--
	-- allocated bandwidth in K
	--
	bwallo='rm -f /tmp/bwallo; x=$(uci -q get custom.bwallocate.allocate); echo $x >> /tmp/bwallo; x=$(uci -q get custom.bwallocate.password); echo $x >> /tmp/bwallo'
	os.execute(bwallo)
	file = io.open("/tmp/bwallo", "r")
	if file == nil then
		allo = 1000000000
		passw = "password"
	else
		allos = file:read("*line")
		allo = tonumber(allos) * 1000000
		passw = file:read("*line")
		file:close()
	end
	tfile:write(tostring(allo), "\n")
	tfile:write(calc(allo), "\n")
	tfile:write(passw, "\n")
	
	showdevices(bw, maclist)
	tfile:write(tostring(totaldevices), "\n")
	if totaldevices > 0 then
		for i=0, totaldevices-1 do
			tfile:write(devices[i], "\n")
		end
	end
	tfile:close()
else
	tfile = io.open("/tmp/bwdata", "w")
	tfile:write("0\n")
	tfile:write("0\n")
	tfile:close()
end
os.execute("rm -f /tmp/lockbw")
os.execute("/usr/lib/bwmon/excede.sh " .. tostring(total) .. " " .. tostring(allo) .. " " .. tostring(ptotal))
os.execute("/usr/lib/bwmon/savetot.sh \"" .. calc(total) .. "\"")
os.execute("/usr/lib/bwmon/perday.lua")
