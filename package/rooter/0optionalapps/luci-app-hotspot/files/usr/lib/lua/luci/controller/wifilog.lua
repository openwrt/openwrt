module("luci.controller.wifilog", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
	local page
		page = entry({"admin", "hotspot", "wifilog"}, template("wifilog/wifilog"), _(translate("Hotspot Logging")), 61)
		page.dependent = true
	end

	entry({"admin", "status", "wifilog"}, call("action_wifilog"))
end

function action_wifilog()
	local file
	local rv ={}

	file = io.open("/tmp/wifilog.log", "r")
	if file ~= nil then
		local tmp = file:read("*all")
		rv["log"] = tmp
		file:close()
	else
		rv["log"] = translate("No entries in log file")
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end