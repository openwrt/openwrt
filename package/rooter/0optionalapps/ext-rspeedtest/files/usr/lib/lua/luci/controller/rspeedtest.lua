module("luci.controller.rspeedtest", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page
	page = entry({"admin", "speed", "rspeedtest"}, template("speedtest/rspeedtest"), translate("SpeedTest by Ookla"), 72)
	page.dependent = true
	
	entry({"admin", "speed", "closeserver"}, call("action_closeserver"))
	entry({"admin", "speed", "stoptest"}, call("action_stoptest"))
	entry({"admin", "speed", "getspeed"}, call("action_getspeed"))
	entry({"admin", "speed", "getspeeddata"}, call("action_getspeeddata"))
end

function action_closeserver()
	local rv = {}
	
	os.execute("/usr/lib/speedtest/info.sh")
	result = "/tmp/sinfo"
	file = io.open(result, "r")
	if file ~= nil then
		rv["status"] = file:read("*line")
		if rv["status"] ~= "0" then
			rv["ip"] = file:read("*line")
			rv["isp"] = file:read("*line")
			rv["city"] = file:read("*line")
			rv["prov"] = file:read("*line")
		end
		file:close()
	else
		rv["status"] = "0"
	end
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_stoptest()
	os.execute("/usr/lib/speedtest/stop.sh")
end

function action_getspeed()
	local rv = {}
	
	os.execute("/usr/lib/speedtest/getspeed.sh ")

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_getspeeddata()
	local rv = {}

	result = "/tmp/getspeed"
	file = io.open(result, "r")
	if file ~= nil then
		rv["dlsize"] = file:read("*line")
		rv["dlelapse"] = file:read("*line")
		rv["ulsize"] = file:read("*line")
		rv["ulelapse"] = file:read("*line")
		file:close()
	else
		rv["dlsize"] = "0"
		rv["ulsize"] = "0"
	end
	result = "/tmp/spworking"
	file = io.open(result, "r")
	if file ~= nil then
		rv["working"] = file:read("*line")
		file:close()
	else
		rv["working"] = "0"
	end
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end
