#!/usr/bin/lua

local oaddr = arg[1]
local txt = arg[2]
local pid = arg[3]

if package.path:find(";/usr/lib/sms/?.lua") == nil then
	package.path = package.path .. ";/usr/lib/sms/?.lua"
end

local utf8togsm = require "utf8togsm"
local pack7bit = require "pack7bit"

local isok = true

if #oaddr > 11 then
	txt = oaddr .. " " .. txt
	oaddr = "ROOter"
end

if #txt == 0 then
	txt = "Usage: /usr/lib/sms/sys2sms.sh 'from' 'text to write here'"
end

utf8togsm.chktxt(txt)
local msg = utf8togsm["msg"]
local dcs = utf8togsm["dcs"]
local ud = utf8togsm["txt"]

local udl = string.format("%02X", math.floor(#ud / 2))

if msg ~= nil then
	isok = false
end

if isok and dcs == "00" then
	pack7bit.pack(udl, ud)
	ud = pack7bit["pdu"]
end
if #oaddr == 0 or oaddr == ' ' then
	oaddr = "ROOter"
end
if oaddr:sub(-1) == ' ' then
	oaddr = oaddr:sub(1, -2)
end
local oaddrl = #oaddr * 2
if oaddrl > 14 then
	oaddrl = oaddrl - 2
elseif oaddrl > 6 then
	oaddrl = oaddrl - 1
end
oaddrl = string.format("%02X", oaddrl)

utf8togsm.chktxt(oaddr)
oaddr = utf8togsm["txt"]

if utf8togsm["dcs"] == "08" then
	isok = false
end

pack7bit.pack(string.format("%02X", math.floor(#oaddr / 2)), oaddr)
oaddr = pack7bit["pdu"]

dtg = os.date("%y%m%d%H%M%S%z", os.time())
sign = dtg:sub(13, 13)
tz = dtg:sub(-4)
dtgif = ''
for j = 1, 11, 2 do
	dtgif = dtgif .. dtg:sub(j + 1, j + 1) .. dtg:sub(j, j)
end
tz = string.format("%02d", math.floor((tonumber(tz:sub(1, 2) * 4)) + tonumber(tz:sub(3, 4) / 15)))
tz = tz:sub(2, 2) .. tz:sub(1, 1)
if sign == "-" then
	tz = tz:sub(1, 1) .. string.format("%X", (tonumber(tz:sub(2, 2)) + 8))
end

pdu = "0004" .. oaddrl .. "D0" .. oaddr .. "00" .. dcs .. dtgif .. tz .. udl .. ud

if isok then
	pdul = string.format("%03d", (math.floor(#pdu / 2) - 1))
	os.execute("echo " .. pdul .. " "  .. pdu .. " > /tmp/pdu" .. pid)
end
