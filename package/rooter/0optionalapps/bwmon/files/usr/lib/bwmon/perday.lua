#!/usr/bin/lua

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

local function bubblesort(a)
  repeat
    local swapped = false
    for i = 1, table.getn(a) do
      if a[i - 1] < a[i] then
        a[i], a[i - 1] = a[i - 1], a[i]
        swapped = true
      end -- if
    end -- for
  until swapped == false
end

function ConBytes(line)
	local s, e, bs, be
	s, e = line:find(" ")
	bs, be = line:find("K", e+1)
	if bs == nil then
		bs, be = line:find("MB", e+1)
		if bs == nil then
			val = tonumber(line:sub(1, e-1)) * 1000000
		else
			val = tonumber(line:sub(1, e-1)) * 1000
		end
	else
		val = tonumber(line:sub(1, e-1))
	end
	return val
end

dirname = '/usr/lib/bwmon/data'
filepost = "-mac_data.js"
monthly = dirname .. "/monthly.data"
daylist = {}
dayline = {}
monlist = {}
monline = {}

dataname = nil
f = io.popen('/bin/ls ' .. dirname)
for name in f:lines() do 
	s, e = name:find(filepost)
	if s ~= nil then
		dataname = name
	end
end
f:close()

if dataname ~= nil then
	yearmon = dataname:sub(1, 7)
	datafile = dirname .. "/" .. dataname
	file = io.open(datafile, "r")
	i = 0
	repeat
		line = file:read("*line")
		if line == nil then
			break
		end
		s, e = line:find("start day")
		if s ~= nil then
			day = line:sub(e+1)
			nday = tonumber(day)
			day= tostring(nday)
			if nday < 10 then
				day = "0" .. day
			end
			yearmonday = yearmon .. "-" .. day
			
			daydwn = 0
			dayup = 0
			repeat
				line = file:read("*line")
				s, e = line:find("end day")
				if s ~= nil then
					dayt = dayup + daydwn
					if dayt > 0 then
						daylist[i] = yearmonday
						i = i + 1
						dayline[yearmonday] = {}
						dayline[yearmonday]['down'] = daydwn
						dayline[yearmonday]['up'] = dayup
						dayline[yearmonday]['total'] = dayup + daydwn
					end
					break
				end
				s, e = line:find("\"down\":\"")
				bs, be = line:find("\"", e+1)
				daydwn = daydwn + tonumber(line:sub(e+1, bs-1))
				s, e = line:find("\"up\":\"")
				bs, be = line:find("\"", e+1)
				dayup = dayup + tonumber(line:sub(e+1, bs-1))
			until 1==0
		end
	until 1==0

	if i > 0 then
		tfile = io.open(monthly, "r")
		if tfile == nil then
			for j = 0,i-1
			do
				lin = daylist[j]
				monlist[j] = lin
				monline[lin] = {}
				dwn = calc(dayline[lin]['down'])
				up = calc(dayline[lin]['up'])
				total = calc(dayline[lin]['total'])
				dataline = lin .. "|" .. dwn .. "|" .. up .. "|" .. total
				monline[lin]['data'] = dataline
			end
			k = i
		else
			k = 0
			ksize = tfile:read("*line")
			kdwn = tfile:read("*line")
			kup = tfile:read("*line")
			ktotal = tfile:read("*line")
			repeat
				line = tfile:read("*line")
				if line == nil then
					break
				end
				ymd = line:sub(1,10)
				monlist[k] = ymd
				k = k + 1
				monline[ymd] = {}
				monline[ymd]['data'] = line
			until 1==0
			tfile:close()
			
			for j = 0,i-1
			do
				lin = daylist[j]
				if monline[lin] == nil then
					monlist[k] = lin
					k = k + 1
					monline[lin] = {}
				end
				dwn = calc(dayline[lin]['down'])
				up = calc(dayline[lin]['up'])
				total = calc(dayline[lin]['total'])
				dataline = lin .. "|" .. dwn .. "|" .. up .. "|" .. total
				monline[lin]['data'] = dataline
			end
		end
		
		bubblesort(monlist)
		
		tfile = io.open(monthly, "w")
		if k > 30 then
			k = 30
		end
		tfile:write(tostring(k), "\n")
		fdown = 0
		fup = 0
		ftotal = 0
		for j = 0,k-1
		do
			lin = monlist[j]
			dataline = monline[lin]['data']
			s, e = dataline:find("|")
			if s ~= nil then
				bs, be = dataline:find("|", e+1)
				fdown = ConBytes(dataline:sub(e+1, be-1)) + fdown
				s, e = dataline:find("|", be+1)
				fup = ConBytes(dataline:sub(be+1, e-1)) + fup
				ftotal = ConBytes(dataline:sub(e+1)) + ftotal
			end
		end
		tfile:write(calc(fdown), "\n")
		tfile:write(calc(fup), "\n")
		tfile:write(calc(ftotal), "\n")
		for j = 0,k-1
		do
			lin = monlist[j]
			dataline = monline[lin]['data']
			tfile:write(dataline, "\n")
		end
		tfile:close()
	end
	
	file:close()
end