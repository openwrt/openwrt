module("luci.controller.zerotier", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local fs = require "nixio.fs"
	local lock = luci.model.uci.cursor():get("custom", "menu", "full")
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		if lock == "1" then
			if fs.stat("/etc/config/zerotier") then
				local page
				if (multilock == "1" and rootlock == "1") then
					page = entry({"admin", "adminmenu", "zerotier"}, template("zerotier/zerotier"), translate("Zerotier"), 7)
					page.dependent = true
				else
					page = entry({"admin", "adminmenu", "zerotier"}, template("zerotier/zerotier"), translate("---Router ID"), 7)
					page.dependent = true
				end
			end
		end
	end
	
	entry({"admin", "services", "getid"}, call("action_getid"))
	entry({"admin", "services", "sendid"}, call("action_sendid"))
	entry({"admin", "services", "get_ids"}, call("action_get_ids"))
end

function action_getid()
	local rv = {}
	id = luci.model.uci.cursor():get("zerotier", "zerotier", "join")
	rv["netid"] = id
	secret = luci.model.uci.cursor():get("zerotier", "zerotier", "secret")
	if secret == nil then
		secret = "xxxxxxxxxx"
	end
	rv["routerid"] = string.sub(secret,1,10)
	rv["password"] = luci.model.uci.cursor():get("custom", "zerotier", "password")
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_sendid()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/zerotier/netid.sh 1 " .. set)
end

function action_get_ids()
	local rv = {}
	id = luci.model.uci.cursor():get("zerotier", "zerotier", "join")
	rv["netid"] = id
	secret = luci.model.uci.cursor():get("zerotier", "zerotier", "secret")
	if secret ~= nil then
		rv["routerid"] = string.sub(secret,1,10)
	else
		rv["routerid"] = "xxxxxxxxxx"
	end
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end