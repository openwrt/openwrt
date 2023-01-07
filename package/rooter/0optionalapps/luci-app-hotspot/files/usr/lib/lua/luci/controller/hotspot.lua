module("luci.controller.hotspot", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		entry({"admin", "hotspot"}, firstchild(), translate("Wifi Hotspot"), 29).dependent=false
		page = entry({"admin", "hotspot", "hotspot"}, template("hotspot/hotspot"), _(translate("Hotspot Manager")), 5)
		page.dependent = true
	end

	entry({"admin", "services", "check_spot"}, call("action_check_spot"))
	entry({"admin", "services", "set_mode"}, call("action_set_mode"))
	entry({"admin", "services", "set_enabled"}, call("action_set_enabled"))
	entry({"admin", "services", "disconnect"}, call("action_disconnect"))
	entry({"admin", "services", "refreshlist"}, call("action_refresh"))
	entry({"admin", "services", "moveup"}, call("action_moveup"))
	entry({"admin", "services", "movedown"}, call("action_movedown"))
	entry({"admin", "services", "addtolist"}, call("action_addtolist"))
	entry({"admin", "services", "dellist"}, call("action_dellist"))
	entry({"admin", "services", "editlist"}, call("action_editlist"))
	entry({"admin", "services", "mancon"}, call("action_mancon"))
	entry({"admin", "services", "bandchange"}, call("action_bandchange"))
	entry({"admin", "services", "reconn"}, call("action_reconn"))
end

function readhot()
	line = nil
	file = io.open("/tmp/hot", "r")
	if file ~= nil then
		repeat
			tmp = file:read("*line")
			if tmp ~= nil then
				if line == nil then
					line = tmp
				else
					line = line .. "|" .. tmp
				end
			end
		until tmp == nil
		file:close()
	end
	return line
end

function action_check_spot()
	local rv = {}
	local freq = {}
	local set = luci.http.formvalue("set")
	auto = luci.model.uci.cursor():get("travelmate", "global", "trm_auto")
	rv["auto"] = auto
	rv["enabled"] = luci.model.uci.cursor():get("travelmate", "global", "trm_enabled")
	rv["reconn"] = luci.model.uci.cursor():get("travelmate", "global", "reconn")
	if rv["reconn"] == nil then
		rv["reconn"] = "0"
	end

	rv["ssid"] = luci.model.uci.cursor():get("travelmate", "global", "ssid")
	if rv["ssid"] == nil then
		rv["ssid"] = "No Connection"
	end
	fre = luci.model.uci.cursor():get("travelmate", "global", "freq")
	encr = luci.model.uci.cursor():get("wireless", "wwan" .. fre, "encryption")
	if encr == "none" then
		rv["encryp"] = translate("Open")
	else
		rv["encryp"] = translate("Encrypted")
	end
	rv["disable"] = luci.model.uci.cursor():get("wireless", "wwan" .. fre, "disabled")
	
	dual = luci.model.uci.cursor():get("travelmate", "global", "radcnt")
	rv["dual"] = dual
	nrad = tonumber(dual)
	for k = 0, nrad do
		freq[k] = luci.model.uci.cursor():get("travelmate", "global", "radio" .. tostring(k))
	end
	rv['freq'] = freq

	device = luci.model.uci.cursor():get("wireless", "wwan" .. fre, "device")
	device = string.sub(device, 6, 7)
	if fre == "2" then
		fre="0"
	else
		fre="1"
	end
	rv["band"] = fre

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_set_mode()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/hotspot/enable.sh " .. set)
end

function action_set_enabled()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/hotspot/enable.sh " .. set)
end

function action_disconnect()
	os.execute("/usr/lib/hotspot/dis_hot.sh")
end

function action_refresh()
	local rv = {}
	os.execute("/usr/lib/hotspot/inrange.sh")
	hotline = readhot()
	if hotline == nil then
		rv["empty"] = "1"
	else
		rv["empty"] = "0"
		rv["hotline"] = hotline
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function movelist(direc, set, key)
	local temp = {}
	local rv = {}
	local index = 0
	file = io.open("/tmp/hot", "r")
	if file ~= nil then
		repeat
			tmp = file:read("*line")
			if tmp ~= nil then
				temp[index] = tmp
				index = index + 1
			end
		until tmp == nil
		file:close()
		if direc == 0 then
			line2 = temp[set-1]
			temp[set-1] = temp[set]
			temp[set] = line2
		elseif direc == 1 then 
			line2 = temp[set+1]
			temp[set+1] = temp[set]
			temp[set] = line2
		elseif direc == 2 then
			if index > 0 then
				for i=set, index-2 do
					temp[i] = temp[i+1]
				end
			end
			index = index - 1
		elseif direc == 3 then
			s, e = temp[set]:find("|")
			name = temp[set]:sub(1, s-1)
			cs, ce = temp[set]:find("|", e+1)
			enc = temp[set]:sub(e+1, cs-1)
			bs, be = temp[set]:find("|", ce+1)
			ko = temp[set]:sub(ce+1, bs-1)
			fl = temp[set]:sub(be+1)
			temp[set] = name .. "|" .. enc .. "|" .. key .. "|" .. fl
		end
		if index < 0 then
			--os.execute("rm -f /tmp/hot1; rm -f /etc/hotspot")
		else
			file = io.open("/tmp/hot", "w")
			for i=0, index-1 do
				file:write(temp[i], "\n")
			end
			file:close()
			os.execute("/usr/lib/hotspot/copyhot.sh")
		end
	end
end

function action_moveup()
	local set = tonumber(luci.http.formvalue("set"))
	local rv = {}
	movelist(0, set)
	hotline = readhot()
	if hotline == nil then
		rv["empty"] = "1"
	else
		rv["empty"] = "0"
		rv["hotline"] = hotline
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_movedown()
	local set = tonumber(luci.http.formvalue("set"))
	local rv = {}
	movelist(1, set)
	hotline = readhot()
	if hotline == nil then
		rv["empty"] = "1"
	else
		rv["empty"] = "0"
		rv["hotline"] = hotline
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_dellist()
	local set = tonumber(luci.http.formvalue("set"))
	local rv = {}
	movelist(2, set)
	hotline = readhot()
	if hotline == nil then
		rv["empty"] = "1"
	else
		rv["empty"] = "0"
		rv["hotline"] = hotline
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_addtolist()
	local set = luci.http.formvalue("set")
	local rv = {}

	s, e = set:find("|")
	ssid = set:sub(1, e-1)

	file = io.open("/etc/hotspot", "r")
	if file ~= nil then
		i = 0
		repeat
			line = file:read("*line")
			if line == nil then
				break
			else
				s, e = line:find(ssid)
				if s ~= nil then
					i = 1
					break
				end
			end
		until 1==0
		if i == 0 then
			file = io.open("/etc/hotspot", "a")
			file:write(set)
			file:write("\n")
			file:close()
		end
	else
		file = io.open("/etc/hotspot", "w")
		file:write(set)
		file:write("\n")
		file:close()
	end
	os.execute("/usr/lib/hotspot/inrange.sh")
	hotline = readhot()
	if hotline == nil then
		rv["empty"] = "1"
	else
		rv["empty"] = "0"
		rv["hotline"] = hotline
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_editlist()
	local set = luci.http.formvalue("set")
	local rv = {}
	s, e = set:find("|")
	sel = set:sub(1, s-1)
	idx = tonumber(sel)
	key = set:sub(e+1)
	movelist(3, idx, key)
	hotline = readhot()
	if hotline == nil then
		rv["empty"] = "1"
	else
		rv["empty"] = "0"
		rv["hotline"] = hotline
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_mancon()
	local set = luci.http.formvalue("set")
	local rv = {}
	os.execute("/usr/lib/hotspot/manual.sh \"" .. set .. "\"")
end

function action_bandchange()
	local set = luci.http.formvalue("set")
	local rv = {}
	os.execute("/usr/lib/hotspot/band.sh \"" .. set .. "\"")
end

function action_reconn()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/hotspot/reconn.sh \"" .. set .. "\"")
end