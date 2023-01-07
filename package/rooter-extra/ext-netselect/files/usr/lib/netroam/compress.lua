#!/usr/bin/lua

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

lang = trim(arg[1])
copseq = "/tmp/copseq"
netw = {}
netwa = {}
long = {}
mccmnc = {}
freq = {}
germ = 0

file = io.open(copseq, "r")
i = 0
repeat
	line = file:read("*line")
	if line == nil then
		break
	end
	if line == "2" then
		if lang == "de" then
			netw[i] = "( Aktuelles Netzwerk    )"
		else
			netw[i] = "( Current Network   )"
		end
	end
	if line == "1" then
		if lang == "de" then
			netw[i] = "( Verfügbares Netzwerk  )"
		else
			netw[i] = "( Availible Network )"
		end
	end
	if line == "3" then
		if lang == "de" then
			netw[i] = "( Verbotenes Netzwerk   )"
		else
			netw[i] = "( Forbidden Network )"
		end
	end
	if line == "0" then
		if lang == "de" then
			netw[i] = "( Unbekanntes Netzwerk  )"
		else
			netw[i] = "( Unknown Network   )"
		end
	end
	netwa[i] = line
	line = file:read("*line")
	long[i] = line
	line = file:read("*line")
	line = file:read("*line")
	mccmnc[i] = line
	line = file:read("*line")
	freq[i] = line
	i = i + 1
until 1==0
file:close()
maxlen = 0	
for j = 0, i-1 do
	nlen = string.len(long[j])
	if nlen > maxlen then
		maxlen = nlen
	end
end	
for j = 0, i-1 do
	name = long[j] .. "                        "
	long[j] = string.sub(name, 1, maxlen + 1)
end
list = 0
netfnd = {}
netmcc = {}
netfreq = {}
netavail = {}
netlong = {}
for j = 0, i-1 do
	if list == 0 then
		netfnd[list] = long[j] .. netw[j] .. "   "
		netmcc[list] = mccmnc[j]
		netfreq[list] = freq[j]
		netavail[list] = netwa[j]
		netlong[list] = long[j]
		list = list + 1
	else
		match = 0
		for k = 0, list-1 do
			if mccmnc[j] == netmcc[k] then
				match = 1
				indx = k
				break
			end
		end
		if match == 1 then
			k = indx
			if netfreq[k] == "7" and freq[j] == "2" then
				netfreq[k] = "9"
			end
			if netfreq[k] == "2" and freq[j] == "7" then
				netfreq[k] = "9"
			end
		else
			netfnd[list] = long[j] .. netw[j] .. "   "
			netmcc[list] = mccmnc[j]
			netfreq[list] = freq[j]
			netavail[list] = netwa[j]
			netlong[list] = long[j]
			list = list + 1
		end
	end
end
tfile = io.open(copseq .. "xx", "w")
file = io.open(copseq .. "mc", "w")
lfile = io.open(copseq .. "lg", "w")
afile = io.open(copseq .. "av", "w")
for k = 0, list-1 do
	if netfreq[k] == "9" then
		netfnd[k] = netfnd[k] .. "3G/LTE    MCC/MNC - " .. string.sub(netmcc[k], 1, 3) .. " " .. string.sub(netmcc[k], 4)
	end
	if netfreq[k] == "2" then
		netfnd[k] = netfnd[k] .. "3G        MCC/MNC - " .. string.sub(netmcc[k], 1, 3) .. " " .. string.sub(netmcc[k], 4)
	end
	if netfreq[k] == "7" then
		netfnd[k] = netfnd[k] .. "LTE       MCC/MNC - " .. string.sub(netmcc[k], 1, 3) .. " " .. string.sub(netmcc[k], 4)
	end
	tfile:write(netfnd[k], "\n")
	file:write(netmcc[k], "\n")
	lfile:write(netavail[k], "\n")
	afile:write(netlong[k], "\n")
end
tfile:close()
file:close()
lfile:close()
afile:close()