#!/usr/bin/lua

modem = arg[1]
addr = arg[2]
dcs = arg[3]
txt = arg[4]
suffix = arg[5]

if package.path:find(";/usr/lib/sms/?.lua") == nil then
	package.path = package.path .. ";/usr/lib/sms/?.lua"
end

local pack7bit = require "pack7bit"

udl = string.format("%02X", math.floor(#txt / 2))
da = "81"
if addr:sub(1, 1) == "+" then
	da = "91"
	addr = addr:sub(2)
elseif addr:sub(1, 1) == "-" then
	addr = addr:sub(2)
end
da = string.format("%02X", #addr) .. da
if (#addr % 2) > 0 then
	addr = addr .. "F"
end
k = #addr
j = 1
repeat
	da = da .. addr:sub(j + 1, j + 1) .. addr:sub(j, j)
	j = j + 2
until j > k
if dcs == "00" then
	pack7bit.pack(udl, txt)
	ud = pack7bit["pdu"]
else
	ud = txt
end
pdu = "001100" .. da .. "00" .. dcs .. "AD" .. udl .. ud
pdul = string.format("%03d", (math.floor(#pdu / 2) - 1))
os.execute("/usr/lib/sms/sendsms.sh " .. modem .. " " .. pdul .. "," .. suffix .. "," .. pdu .. " &")
