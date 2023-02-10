#!/usr/bin/lua

function ltrim(s)
  return s:match'^%s*(.*)'
end

file=arg[1]
filet=arg[1]

overall = {}

cntr = 1
filein = io.open(file, "r")
maxslot = -1
repeat
	message = {}
	local line = filein:read("*line")
	if line == nil then
		break
	end
	if cntr < 2 then
		maxused = line
		line = filein:read("*line")
		maxslots = line
		cntr=2
	else
		message['slot'] = line
		message['phone'] = filein:read("*line")
		nline = filein:read("*line")
		message['nline'] = nline
		nline = tonumber(nline)
		nc = nline
		ncntr = 1
		msg=""
		message['msgnum'] = "xxx"
		message['msgord'] = "xxx"
		message['msgmax'] = "xxx"
		lines = filein:read("*line")
		if lines == nil then
			s = nil
		else
			s, msgnum, msgord, msgmax = lines:match("(Msg# (%d+),(%d+)/(%d+))")
		end
		if s == nil then
			nc = nc + 1
		else
			message['msgnum'] = msgnum
			message['msgord'] = msgord
			message['msgmax'] = msgmax
			lines = filein:read("*line")
		end
		msg = lines
		if nc > 2 then
			for i=1,nc-2,1
			do
				lines = filein:read("*line")
				if lines == "" then
					msg = msg .. "\n"
				else
					msg = msg .. "\n" .. lines
				end
			end
			--print(nln, msg)
			--msg = msg .. "\n"
		end
		message['msg'] = msg
		message['numlines'] = nc - 1
		sht = filein:read("*line")
		s, sht1, sht2 = sht:match("((.*)Msg# %d+,%d+/%d%s*(.*))")
		if s ~= nil then
			sht = sht1 .. sht2
		end
		message['short'] = sht
		overall[message['slot']] = message
		if maxslot < tonumber(message['slot']) then
			maxslot = tonumber(message['slot'])
		end
	end
until 1==0
filein:close()

fileout = io.open(filet, "w")
fileout:write(maxused, "\n")
fileout:write(maxslots, "\n")

for i=0,maxslot,1
do
	msgbuild = {}
	shortmsg = {}
	if overall[tostring(i)] ~= nil then
		--print(i, overall[tostring(i)]['msgnum'])
		if overall[tostring(i)]['msgnum'] == "xxx" then
			fileout:write(overall[tostring(i)]['slot'], "\n")
			fileout:write(overall[tostring(i)]['phone'], "\n")
			fileout:write(overall[tostring(i)]['nline'], "\n")
			fileout:write(overall[tostring(i)]['msg'], "\n")
			fileout:write(overall[tostring(i)]['short'], "\n")
		else
			msgnum = overall[tostring(i)]['msgnum']
			msgtmp = overall[tostring(i)]['slot']
			msgord = overall[tostring(i)]['msgord']
			msgmax = tonumber(overall[tostring(i)]['msgmax'])
			msg = overall[tostring(i)]['msg']
			numlines = overall[tostring(i)]['numlines']
			--print(numlines)
			msgbuild[overall[tostring(i)]['msgord']] = overall[tostring(i)]['msg']
			shortmsg[overall[tostring(i)]['msgord']] = overall[tostring(i)]['short']
			for j=i+1,maxslot,1
			do
				if overall[tostring(j)] ~= nil then
					if overall[tostring(j)]['msgnum'] == msgnum then
						numlines = numlines + (overall[tostring(j)]['numlines'])
						--print(overall[tostring(j)]['numlines'])
						msgtmp = msgtmp .. " " .. overall[tostring(j)]['slot']
						msgbuild[overall[tostring(j)]['msgord']] = overall[tostring(j)]['msg']
						shortmsg[overall[tostring(j)]['msgord']] = overall[tostring(j)]['short']
						overall[tostring(j)] = nil
					end
				end
			end
			msg=""
			mflg = 0
			short = nil
			for j=1,msgmax,1
			do
				if msgbuild[tostring(j)] ~= nil then
					msg = msg .. msgbuild[tostring(j)]
					if short == nil then
						short = shortmsg[tostring(j)]
					end
				else
					mflg = 1
				end
			end
			fileout:write(msgtmp, "\n")
			fileout:write(overall[tostring(i)]['phone'], "\n")
			if mflg == 0 then
				s, preshort = short:match("((.* %d+%-%d+%-%d+ %d+:%d+:%d+%s+[+-]%d+h ))")
				if preshort ~= nil then
					stxt = ''
					j = 0
					k = 1
					ch = ''
					while j < 20 do
						ch = string.byte(msg:sub(k, k))
						if ch == nil then
							j = 20
						elseif ch == 10 or ch == 13 then
							stxt = stxt .. ' '
							k = k + 1
						elseif ch < 127 then
							stxt = stxt .. string.char(ch)
							k = k + 1
						elseif ch < 0xE0 then
							stxt = stxt .. msg:sub(k, k + 1)
							k = k + 2
						elseif ch < 0xF0 then
							stxt = stxt .. msg:sub(k, k + 2)
							k = k + 3
						else
							stxt = stxt .. msg:sub(k, k + 3)
							k = k + 4
						end
						j = j + 1
					end
					if preshort:sub(1, 1) == ' ' then
						jj = 49
					else
						jj = 51
					end
					short = (preshort .. '     '):sub(1, jj) .. stxt .. "  ..."
				end
			else
				msg = "Partial Message : " .. msg
				t = short:gsub("%s+", " ")
				short = "Partial Message " .. t
			end
			endc = string.sub(msg, -2)
			if endc == "\n\n" then
				mlen = msg:len() - 2
				msg = string.sub(msg,1,mlen)
			end
			local _,n = msg:gsub("\n","")
			fileout:write(tostring(n+1), "\n")
			fileout:write(msg, "\n")
			fileout:write(short, "\n")
		end
	end
end
fileout:close()
