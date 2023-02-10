module("luci.controller.admin.modem", package.seeall) 

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	
	local multilock = uci:get("custom", "multiuser", "multi") or "0"
	local rootlock = uci:get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		entry({"admin", "modem"}, firstchild(), translate("Modem"), 25).dependent=false
		entry({"admin", "modem", "prof"}, cbi("rooter/profiles"), translate("Connection Profile"), 2)
		entry({"admin", "modem", "nets"}, template("rooter/net_status"), translate("Network Status"), 30)
		entry({"admin", "modem", "debug"}, template("rooter/debug"), translate("Debug Information"), 50)
		entry({"admin", "modem", "cust"}, cbi("rooter/customize"), translate("Custom Modem Ports"), 55)
		entry({"admin", "modem", "log"}, template("rooter/log"), translate("Connection Log"), 60)
		entry({"admin", "modem", "misc"}, template("rooter/misc"), translate("Miscellaneous"), 40)
	end
	if (multilock == "1" and rootlock == "0") then
		entry({"admin", "system", "misc"}, template("rooter/bandlock"), translate("Band Lock and Scan"), 40)
	end
	
	entry({"admin", "modem", "get_csq"}, call("action_get_csq"))
	entry({"admin", "modem", "change_port"}, call("action_change_port"))
	entry({"admin", "modem", "change_mode"}, call("action_change_mode"))
	entry({"admin", "modem", "change_modem"}, call("action_change_modem"))
	entry({"admin", "modem", "change_modemdn"}, call("action_change_modemdn"))
	entry({"admin", "modem", "change_misc"}, call("action_change_misc"))
	entry({"admin", "modem", "change_miscdn"}, call("action_change_miscdn"))
	entry({"admin", "modem", "get_log"}, call("action_get_log"))
	entry({"admin", "modem", "check_misc"}, call("action_check_misc"))
	entry({"admin", "modem", "pwrtoggle"}, call("action_pwrtoggle"))
	entry({"admin", "modem", "disconnect"}, call("action_disconnect"))
	entry({"admin", "modem", "connect"}, call("action_connect"))
	entry({"admin", "modem", "get_atlog"}, call("action_get_atlog"))
	entry({"admin", "modem", "send_atcmd"}, call("action_send_atcmd"))
	entry({"admin", "modem", "change_rate"}, call("action_change_rate"))
	entry({"admin", "modem", "change_phone"}, call("action_change_phone"))
	entry({"admin", "modem", "clear_log"}, call("action_clear_log"))
	entry({"admin", "modem", "externalip"}, call("action_externalip"))
	entry({"admin", "modem", "send_scancmd"}, call("action_send_scancmd"))
	entry({"admin", "modem", "send_lockcmd"}, call("action_send_lockcmd"))
	entry({"admin", "modem", "extping"}, call("action_extping"))
	entry({"admin", "modem", "change_cell"}, call("action_change_cell"))
	entry({"admin", "modem", "change_proto"}, call("action_change_proto"))
	entry({"admin", "modem", "setpin"}, call("action_setpin"))
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function action_get_atlog()
	local file
	local rv ={}

	file = io.open("/tmp/atlog", "r")
	if file ~= nil then
		local tmp = file:read("*all")
		rv["log"] = tmp
		file:close()
	else
		rv["log"] = "No entries in log file"
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_get_log()
	local file
	local rv ={}

	file = io.open("/usr/lib/rooter/log/connect.log", "r")
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

function action_disconnect()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/rooter/connect/disconnect.sh")
end

function action_connect()
	local set = luci.http.formvalue("set")
	miscnum = luci.model.uci.cursor():get("modem", "general", "miscnum")
	os.execute("/tmp/links/reconnect" .. miscnum .. " " .. miscnum)
end

function action_pwrtoggle()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/rooter/pwrtoggle.sh " .. set)
end

function action_send_atcmd()
	local rv ={}
	modnum = luci.model.uci.cursor():get("modem", "general", "miscnum")
	local file
	local set = luci.http.formvalue("set")
	fixed = string.gsub(set, "\"", "~")
	os.execute("/usr/lib/rooter/luci/atcmd.sh \'" .. fixed .. "\'")

	result = "/tmp/result" .. modnum .. ".at"
	file = io.open(result, "r")
	if file ~= nil then
		rv["result"] = file:read("*all")
		file:close()
		os.execute("/usr/lib/rooter/luci/luaops.sh delete /tmp/result" .. modnum .. ".at")
	else
		rv["result"] = " "
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_check_misc()
	local rv ={}
	local bnd1 = {}
	local bnd2 = {}
	local bnd31 = {}
	local bnd32 = {}
	local bnd33 = {}
	local at1 = {}
	local at2 = {}
	local file
	local file1
	local active
	local connect

	miscnum = luci.model.uci.cursor():get("modem", "general", "miscnum")
	conn = "Modem #" .. miscnum
	rv["conntype"] = conn
	empty = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "empty")
	if empty == "1" then
		active = "0"
		rv["netmode"] = "-"
	else
		active = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "active")
		if active == "1" then
			connect = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "connected")
			if connect == "0" then
				active = "1"
			else
				active = "2"
			end
			uVid = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "uVid")
			rv["uVid"] = uVid
			uPid = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "uPid")
			rv["uPid"] = uPid
			file = io.open("/etc/fake", "r")
			if file == nil then
				rv["fake"] = "0"
			else
				rv["fake"] = "1"
				file:close()
			end
			
			rv["cenable"] = luci.model.uci.cursor():get("custom", "bandlock", "cenable" .. miscnum)
			if rv["cenable"] == nil then
				rv["cenable"] = "0"
			end
			rv["earfcn"] = luci.model.uci.cursor():get("custom", "bandlock", "earfcn" .. miscnum)
			if rv["earfcn"] == nil then
				rv["earfcn"] = "0"
			end
			rv["pci"] = luci.model.uci.cursor():get("custom", "bandlock", "pci" .. miscnum)
			if rv["pci"] == nil then
				rv["pci"] = "0"
			end
			
			rv["earfcn1"] = luci.model.uci.cursor():get("custom", "bandlock", "earfcn1" .. miscnum)
			if rv["earfcn1"] == nil then
				rv["earfcn1"] = "0"
			end
			rv["pci1"] = luci.model.uci.cursor():get("custom", "bandlock", "pci1" .. miscnum)
			if rv["pci1"] == nil then
				rv["pci1"] = "0"
			end
			
			rv["earfcn2"] = luci.model.uci.cursor():get("custom", "bandlock", "earfcn2" .. miscnum)
			if rv["earfcn2"] == nil then
				rv["earfcn2"] = "0"
			end
			rv["pci2"] = luci.model.uci.cursor():get("custom", "bandlock", "pci2" .. miscnum)
			if rv["pci2"] == nil then
				rv["pci2"] = "0"
			end
			
			rv["earfcn3"] = luci.model.uci.cursor():get("custom", "bandlock", "earfcn3" .. miscnum)
			if rv["earfcn3"] == nil then
				rv["earfcn3"] = "0"
			end
			rv["pci3"] = luci.model.uci.cursor():get("custom", "bandlock", "pci3" .. miscnum)
			if rv["pci3"] == nil then
				rv["pci3"] = "0"
			end
			
			file = io.open("/tmp/bmask", "r")
			if file == nil then
				rv["bndstr"] = "0"
				rv["bndsup"] = "0"
			else
				line = file:read("*line")
				rv["bndstr"] = line
				line = file:read("*line")
				rv["bndstr5g"] = line
				line = file:read("*line")
				rv["bndstr5gsa"] = line
				line = file:read("*line")
				rv["bndsup"] = line
				line = file:read("*line")
				rv["bndsup5g"] = line
				line = file:read("*line")
				rv["bndsup5gsa"] = line
				line = file:read("*line")
				ca = line
				if ca ~= nil then
					line = file:read("*line")
					ca3 = line
				end
				file:close()
				
				indx = 0
				if ca ~= nil then
					file = io.open("/usr/lib/rooter/luci/" .. ca, "r")
					if file ~= nil then
						line = file:read("*line")
						repeat
							s, e = line:find(" ")
							b1 = trim(line:sub(1, s-1))
							bnd1[indx] = b1
							b2 = trim(line:sub(s+1))
							bnd2[indx] = b2
							indx = indx +1
							line = file:read("*line")
						until line == nil
						file:close()
					end
				end
				rv['b1'] = bnd1
				rv['b2'] = bnd2
				rv['indx'] = tostring(indx)
				
				indx3 = 0
				if ca3 ~= nil then
					file = io.open("/usr/lib/rooter/luci/" .. ca3, "r")
					if file ~= nil then
						line = file:read("*line")
						repeat
							s, e = line:find(" ")
							b1 = trim(line:sub(1, s-1))
							bnd31[indx3] = b1
							cs, ce = line:find(" ", s+1)
							b2 = trim(line:sub(s+1, cs-1))
							bnd32[indx3] = b2
							b3 = trim(line:sub(cs+1))
							bnd33[indx3] = b3
							indx3 = indx3 +1
							line = file:read("*line")
						until line == nil
						file:close()
					end
				end
				rv['b31'] = bnd31
				rv['b32'] = bnd32
				rv['b33'] = bnd33
				rv['indx3'] = tostring(indx3)
				
			end
		end
		netmode = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "netmode")
		rv["netmode"] = netmode
	end
	rv["pin"] = luci.model.uci.cursor():get("modem", "general", "pin")
	rv["plock"] = luci.model.uci.cursor():get("custom", "atcmd", "lock")
	if rv["plock"] == "1" then
		rv["atlock"] = luci.model.uci.cursor():get("custom", "menu", "full")
		generic = luci.model.uci.cursor():get("custom", "atcmd", "generic")
		aindx = 0
		if active == "0" then
			file = io.open(generic, "r")
		else
			os.execute("/usr/lib/custom/locktype.sh " .. miscnum )
			file1 = io.open("/tmp/modemlock", "r")
			if file1 ~= nil then
				linex = file1:read("*line")
				file1:close()
				file = io.open(linex, "r")
			else
				file = io.open(generic, "r")
			end
		end
		if file ~= nil then
			line = file:read("*line")
			repeat
				at1[aindx] = line
				line = file:read("*line")
				at2[aindx] = line
				aindx = aindx +1
				line = file:read("*line")
			until line == nil
			file:close()
			rv['at1'] = at1
			rv['at2'] = at2
			rv['aindx'] = tostring(aindx)
		end
	end
	rv["active"] = active
	file = io.open("/tmp/gpiopin", "r")
	if file == nil then
		rv.gpio = "0"
	else
		rv.gpio = "1"
		line = file:read("*line")
		line = file:read("*line")
		if line ~= nil then
			rv.gpio = "2"
		end
		file:close()
	end
	file = io.open("/sys/bus/usb/drivers/usb/usb1", "r")
	if file == nil then
		rv["usb"] = "0"
	else
		io.close(file)
		rv["usb"] = "1"
	end
	file = io.open("/sys/bus/usb/drivers/usb/usb2", "r")
	if file ~= nill then
		io.close(file)
		rv["usb"] = "2"
	end
	proto = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "proto")
	rv["proto"] = proto
	
	model = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "model")
	rv["model"] = model

	celltype = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "celltype")
	rv["celltype"] = celltype
	cmode = luci.model.uci.cursor():get("modem", "modem" .. miscnum, "cmode")
	if cmode == "0" then
		rv["netmode"] = "10"
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function lshift(x, by)
  return x * 2 ^ by
end

function rshift(x, by)
  return math.floor(x / 2 ^ by)
end

function action_change_mode()
	local set = tonumber(luci.http.formvalue("set"))
	local modemtype = rshift(set, 4)
	local temp = lshift(modemtype, 4)
	local netmode = set - temp
	os.execute("/usr/lib/rooter/luci/modechge.sh " .. modemtype .. " " .. netmode)
end

function action_change_port()
	local set = tonumber(luci.http.formvalue("set"))
	if set ~= nil and set > 0 then
		if set == 1 then
			os.execute("/usr/lib/rooter/luci/portchge.sh dwn")
		else
			os.execute("/usr/lib/rooter/luci/portchge.sh up")
		end
	end
end

function action_change_misc()
	os.execute("/usr/lib/rooter/luci/modemchge.sh misc 1")
end

function action_change_miscdn()
	os.execute("/usr/lib/rooter/luci/modemchge.sh misc 0")
end

function action_change_modem()
	os.execute("/usr/lib/rooter/luci/modemchge.sh modem 1")
end

function action_change_modemdn()
	os.execute("/usr/lib/rooter/luci/modemchge.sh modem 0")
end

function action_get_csq()
	modnum = luci.model.uci.cursor():get("modem", "general", "modemnum")
	local file
	stat = "/tmp/status" .. modnum .. ".file"
	file = io.open(stat, "r")

	local rv ={}

	rv["port"] = file:read("*line")
	rv["csq"] = file:read("*line")
	rv["per"] = file:read("*line")
	rv["rssi"] = file:read("*line")
	rv["modem"] = file:read("*line")
	rv["cops"] = file:read("*line")
	rv["mode"] = file:read("*line")
	rv["lac"] = file:read("*line")
	rv["lacn"] = file:read("*line")
	rv["cid"] = file:read("*line")
	rv["cidn"] = file:read("*line")
	rv["mcc"] = file:read("*line")
	rv["mnc"] = file:read("*line")
	rv["rnc"] = file:read("*line")
	rv["rncn"] = file:read("*line")
	rv["down"] = file:read("*line")
	rv["up"] = file:read("*line")
	rv["ecio"] = file:read("*line")
	rv["rscp"] = file:read("*line")
	rv["ecio1"] = file:read("*line")
	rv["rscp1"] = file:read("*line")
	rv["netmode"] = file:read("*line")
	rv["cell"] = file:read("*line")
	rv["modtype"] = file:read("*line")
	rv["conntype"] = file:read("*line")
	rv["channel"] = file:read("*line")
	rv["phone"] = file:read("*line")
	file:read("*line")
	rv["lband"] = file:read("*line")
	rv["tempur"] = file:read("*line")
	rv["proto"] = file:read("*line")
	rv["pci"] = file:read("*line")
	rv["sinr"] = file:read("*line")
	rv["lat"] = file:read("*line")
	rv["long"] = file:read("*line")

	file:close()

	cmode = luci.model.uci.cursor():get("modem", "modem" .. modnum, "cmode")
	if cmode == "0" then
		rv["netmode"] = "10"
	end

	rssi = rv["rssi"]
	ecio = rv["ecio"]
	rscp = rv["rscp"]
	ecio1 = rv["ecio1"]
	rscp1 = rv["rscp1"]

	if ecio == nil then
		ecio = "-"
	end
	if ecio1 == nil then
		ecio1 = "-"
	end
	if rscp == nil then
		rscp = "-"
	end
	if rscp1 == nil then
		rscp1 = "-"
	end

	if ecio ~= "-" then
		rv["ecio"] = ecio .. " dB"
	end
	if rscp ~= "-" then
		rv["rscp"] = rscp .. " dBm"
	end
	if ecio1 ~= " " then
		rv["ecio1"] = " (" .. ecio1 .. " dB)"
	end
	if rscp1 ~= " " then
		rv["rscp1"] = " (" .. rscp1 .. " dBm)"
	end

	if not nixio.fs.access("/etc/netspeed") then
		rv["crate"] = translate("Fast (updated every 10 seconds)")
	else
		rv["crate"] = translate("Slow (updated every 60 seconds)")
	end

	stat = "/tmp/msimdata" .. modnum
	file = io.open(stat, "r")
	if file == nil then
		rv["modid"] = " "
		rv["imei"] = " "
		rv["imsi"] = " "
		rv["iccid"] = " "
		rv["host"] = "0"
	else
		rv["modid"] = file:read("*line")
		rv["imei"] = file:read("*line")
		rv["imsi"] = file:read("*line")
		rv["iccid"] = file:read("*line")
		rv["host"] = file:read("*line")
		file:close()
	end
	
	gpsdata = "/tmp/gpsdata1"
	file = io.open(gpsdata, "r")
	if file == nil then
		rv["lat"] = "-"
		rv["long"] = "-"
	else
		rv["lat"] = file:read("*line")
		rv["long"] = file:read("*line")
		file:close()
	end
	
	stat = "/tmp/msimnum" .. modnum
	file = io.open(stat, "r")
	if file == nil then
		rv["phone"] = "-"
		rv["phonen"] = " "
	else
		rv["phone"] = file:read("*line")
		rv["phonen"] = file:read("*line")
		file:close()
	end
	
	stat = "/tmp/simpin" .. modnum
	file = io.open(stat, "r")
	if file == nil then
		rv["simerr"] = "0"
	else
		typ = file:read("*line")
		if typ == "0" then
			rv["simerr"] = "1"
		else
			if typ == "1" then
				rv["simerr"] = "2"
			else
				if typ == "2" then
					rv["simerr"] = "3"
				else
					rv["simerr"] = "4"
				end
			end
		end
		file:close()
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_change_rate()
	local set = luci.http.formvalue("set")
	if set == "1" then
		os.execute("rm -f /etc/netspeed")
	else
		os.execute("echo \"0\" > /etc/netspeed")
	end
end

function action_change_phone()
	local set = luci.http.formvalue("set")
	s, e = string.find(set, "|")
	pno = string.sub(set, 1, s-1)
	pnon = string.sub(set, e+1)
	modnum = luci.model.uci.cursor():get("modem", "general", "modemnum")
	os.execute("/usr/lib/rooter/common/phone.sh " .. modnum .. " " .. pno .. " \"" .. pnon .. "\"")
end

function action_clear_log()
        local file
        file = io.open("/usr/lib/rooter/log/connect.log", "w")
        file:close()
        os.execute("/usr/lib/rooter/log/logger 'Connection Log Cleared by User'")
end

function action_externalip()
	local rv ={}

	os.execute("rm -f /tmp/ipip; wget -O /tmp/ipip http://ipecho.net/plain > /dev/null 2>&1")
	file = io.open("/tmp/ipip", "r")
	if file == nil then
		rv["extip"] = translate("Not Available")
	else
		rv["extip"] = file:read("*line")
		if rv["extip"] == nil then
			rv["extip"] = translate("Not Available")
		end
		file:close()
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_send_scancmd()
	local rv ={}
	local file
	os.execute("/usr/lib/rooter/luci/scancmd.sh")

	result = "/tmp/scan"
	file = io.open(result, "r")
	if file ~= nil then
		rv["result"] = file:read("*all")
		file:close()
		os.execute("/usr/lib/rooter/luci/luaops.sh delete /tmp/scan")
	else
		rv["result"] = " "
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_send_lockcmd()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/rooter/luci/lock.sh " .. set)
end

function action_extping()
	local rv ={}
	
	enable = luci.model.uci.cursor():get("ping", "ping", "enable")
	if enable == "0" then
		rv["extping"] = translate("Not Enabled")
	else
		conn = luci.model.uci.cursor():get("ping", "ping", "conn")
		if conn == "1" then
			rv["extping"] = translate("Enabled, Waiting for Modem to Connect")
		else
			if conn == "2" then
				rv["extping"] = translate("Enabled, Ping Test was Good")
			else
				if conn == "3" then
					rv["extping"] = translate("Enabled, Ping Test Failed, Restarting Modem, Waiting for Reconnection")
				else
					if conn == "4" then
						rv["extping"] = translate("Enabled, Connected, Waiting for Ping Test")
					else
						rv["extping"] = translate("Enabled, Unknown State")
					end
				end
			end
		end
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_change_cell()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/rooter/luci/setcell.sh " .. "\"" .. set .. "\"")
end

function action_change_proto()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/rooter/luci/protochnge.sh " ..set)
end

function action_setpin()
	local set = luci.http.formvalue("set")
	os.execute("uci set modem.general.pin=" .. set .. "; uci commit modem")
end