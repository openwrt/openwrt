module("luci.controller.bwmon", package.seeall) 

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page
	entry({"admin", "nlbw"}, firstchild(), translate("Bandwidth Monitor"), 80).dependent=false
	page = entry({"admin", "nlbw", "bwmon"}, template("bwmon/bwmon"), translate("ROOter Bandwidth Monitor"), 70)
	page.dependent = true
	
	entry({"admin", "nlbw", "check_bw"}, call("action_check_bw"))
	entry({"admin", "nlbw", "change_bw"}, call("action_change_bw"))
	entry({"admin", "nlbw", "change_roll"}, call("action_change_roll"))
	entry({"admin", "nlbw", "change_enable"}, call("action_change_enable"))
	entry({"admin", "nlbw", "change_bwenable"}, call("action_change_bwenable"))
	entry({"admin", "nlbw", "change_backup"}, call("action_change_backup"))
	entry({"admin", "nlbw", "change_external"}, call("action_change_external"))
	entry({"admin", "nlbw", "change_bwwan"}, call("action_change_bwwan"))
end

function action_check_bw()
	local rv = {}
	local maclist = {}
	
	file = io.open("/tmp/bwdata", "r")
	if file ~= nil then
		rv['days'] = file:read("*line")
		if rv['days'] ~= "0" then
			rv['total'] = file:read("*line")
			rv['ctotal'] = file:read("*line")
			rv['totaldown'] = file:read("*line")
			rv['ctotaldown'] = file:read("*line")
			rv['totalup'] = file:read("*line")
			rv['ctotalup'] = file:read("*line")
			rv['ptotal'] = file:read("*line")
			rv['cptotal'] = file:read("*line")
			rv['atotal'] = file:read("*line")
			rv['catotal'] = file:read("*line")
			rv['password'] = file:read("*line")
			j = file:read("*line")
			if j == nil then
				j = 0
			end
			rv['macsize'] = j
			if j ~=0 then
				for i=0, j-1 do
					maclist[i] = file:read("*line")
				end
				rv['maclist'] = maclist
			end
		end
		file:close()
	else
		rv['days'] = 0
	end
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if multilock == "0" then
		rv['lock']  = luci.model.uci.cursor():get("custom", "bwallocate", "lock")
	else
		if rootlock == "0" then
			rv['lock']  = "1"
		else
			rv['lock']  = "0"
		end
	end
	rv['rollover'] = luci.model.uci.cursor():get("custom", "bwallocate", "rollover")
	rv['enabled'] = luci.model.uci.cursor():get("custom", "bwallocate", "enabled")
	rv['bwenabled'] = luci.model.uci.cursor():get("bwmon", "general", "enabled")
	rv['backup'] = luci.model.uci.cursor():get("bwmon", "general", "backup")
	rv['external'] = luci.model.uci.cursor():get("bwmon", "general", "external")
	rv['bwwan'] = luci.model.uci.cursor():get("bwmon", "bwwan", "wan")
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_change_bw()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/bwmon/allocate.sh " .. set)
	
end

function action_change_roll()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/bwmon/rollover.sh " .. set)
	
end

function action_change_enable()
	local set = luci.http.formvalue("set")
	os.execute("uci set custom.bwallocate.enabled=" .. set .. "; uci commit custom")
	
end

function action_change_bwenable()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/bwmon/change.sh " .. set)
	
end

function action_change_backup()
	local set = luci.http.formvalue("set")
	os.execute("uci set bwmon.general.backup=" .. set .. "; uci commit bwmon")
	
end

function action_change_external()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/bwmon/external.sh " .. set)
	
end

function action_change_bwwan()
	local set = luci.http.formvalue("set")
	os.execute("uci set bwmon.bwwan.wan=" .. set .. "; uci commit bwmon")
	
end