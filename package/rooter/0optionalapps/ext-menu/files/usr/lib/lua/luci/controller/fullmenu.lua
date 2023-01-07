module("luci.controller.fullmenu", package.seeall)
function index()
	local fs = require "nixio.fs"
		local page
		page = entry({"admin", "system", "fullmenu"}, template("fullmenu/fullmenu"), "Menu Selection", 96)
		page.dependent = true
		
	entry({"admin", "system", "getmenu"}, call("action_getmenu"))
	entry({"admin", "system", "setmenu"}, call("action_setmenu"))
	
end

function action_getmenu()
	local rv = {}
	id = luci.model.uci.cursor():get("custom", "menu", "full")
	rv["full"] = id
	password = luci.model.uci.cursor():get("custom", "menu", "password")
	rv["password"] = password

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_setmenu()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/fullmenu/setmenu.sh " .. set)
	
end