module("luci.controller.modlog", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page
	page = entry({"admin", "modem", "modlog"}, template("modlog/modlog"), _(translate("Modem Logging")), 61)
	page.dependent = true

	entry({"admin", "status", "modlog"}, call("action_modlog"))
end

function action_modlog()
	local file
	local rv ={}

	file = io.open("/tmp/modlog.log", "r")
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