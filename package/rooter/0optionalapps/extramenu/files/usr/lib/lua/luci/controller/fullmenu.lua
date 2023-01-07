module("luci.controller.fullmenu", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page
	entry({"admin", "adminmenu"}, firstchild(), translate("Administration"), 24).dependent=false
	local df = luci.model.uci.cursor():get("custom", "menu", "default")
	if df == '0' then
		local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
			page = entry({"admin", "adminmenu", "fullmenu"}, template("fullmenu/fullmenu"), translate("Unlock / Lock Menus"), 5)
			page.dependent = true
		end
	end
	
	entry({"admin", "menu", "getmenu"}, call("action_getmenu"))
	entry({"admin", "menu", "setmenu"}, call("action_setmenu"))
	
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