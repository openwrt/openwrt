#!/usr/bin/lua

local modem = arg[1]
local dest = arg[2]
local txt = arg[3]
local pid = arg[4]
local rfname = "/tmp/smssendstatus" .. pid

if package.path:find(";/usr/lib/sms/?.lua") == nil then
	package.path = package.path .. ";/usr/lib/sms/?.lua"
end

local utf8togsm = require "utf8togsm"
utf8togsm.chktxt(txt)
local msg = utf8togsm["msg"]
local dcs = utf8togsm["dcs"]
txt = utf8togsm["txt"]

if msg == nil then
	os.execute("if [ -e " .. rfname .. " ]; then rm " ..rfname .. "; fi")
	os.execute("lua /usr/lib/sms/sendsms.lua " .. modem .. " " .. dest .. " " .. dcs .. " " .. txt .. " " .. pid)
else
	os.execute('echo "SMS sending failed - text is too long" > ' .. rfname)
end
