module("luci.controller.sms", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local fs = require "nixio.fs"
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		if not fs.stat("/etc/nosms") then
			local page
			page = entry({"admin", "modem", "sms"}, template("rooter/sms"), translate("SMS Messaging"), 35)
			page.dependent = true
		end
	end

	entry({"admin", "modem", "check_read"}, call("action_check_read"))
	entry({"admin", "modem", "del_sms"}, call("action_del_sms"))
	entry({"admin", "modem", "send_sms"}, call("action_send_sms"))
	entry({"admin", "modem", "change_sms"}, call("action_change_sms"))
	entry({"admin", "modem", "change_smsdn"}, call("action_change_smsdn"))
	entry({"admin", "modem", "change_smsflag"}, call("action_change_smsflag"))
	entry({"admin", "modem", "delall_sms"}, call("action_delall_sms"))
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function action_send_sms()
	if package.path:find(";/usr/lib/sms/?.lua") == nil then
		package.path = package.path .. ";/usr/lib/sms/?.lua"
	end
	smsnum = luci.model.uci.cursor():get("modem", "general", "smsnum")
	local set = luci.http.formvalue("set")
	local number = trim(string.sub(set, 1, 20))
	local txt = string.sub(set, 21)
	local utf8togsm = require "utf8togsm"
	utf8togsm.chktxt(txt)
	local msg = utf8togsm["msg"]
	local dcs = utf8togsm["dcs"]
	txt = utf8togsm["txt"]
	local rv = {}
	local file = nil
	local k = 1
	local status
	local rfname = "/tmp/smssendstatus0000"
	if msg == nil then
		os.execute("if [ -e " .. rfname .. " ]; then rm " ..rfname .. "; fi")
		os.execute("lua /usr/lib/sms/sendsms.lua " .. smsnum .. " " .. number .. " " .. dcs .. " " .. txt .. " 0000")
		os.execute("sleep 3")
		repeat
			file = io.open(rfname, "r")
			if file == nil then
				os.execute("sleep 1")
			end
			k = k + 1
		until k > 25 or file ~=nil
		if file == nil then
			status = translate('Sending attempt timed out (fail)')
		else
			status = file:read("*line")
			file:close()
			os.remove (rfname)
		end
	else
		status = msg
	end
	rv["status"] = status
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_delall_sms()
	smsnum = luci.model.uci.cursor():get("modem", "general", "smsnum")
	os.execute("/usr/lib/sms/delall.sh " .. smsnum)
end

function action_del_sms()
	local set = luci.http.formvalue("set")
	if set ~= nil then
		smsnum = luci.model.uci.cursor():get("modem", "general", "smsnum")
		os.execute("/usr/lib/sms/delsms.sh " .. smsnum .. " "  .. set)
		os.execute("touch /tmp/smswakeup" .. smsnum)
	end
end

function action_check_read()
	local rv ={}
	local file
	local line
	smsnum = luci.model.uci.cursor():get("modem", "general", "smsnum")
	conn = "Modem #" .. smsnum
	rv["conntype"] = conn
	support = luci.model.uci.cursor():get("modem", "modem" .. smsnum, "sms")
	rv["ready"] = "0"
	rv["menable"] = "0"
	rv["mslots"] = "0"
	if support == "1" then
		rv["ready"] = "1"
		result = "/tmp/smsresult" .. smsnum .. ".at"
		file = io.open(result, "r")
		if file ~= nil then
			file:close()
			file = io.open("/tmp/smstext" .. smsnum, "r")
			if file == nil then
				rv["ready"] = "3"
			else
				rv["menable"] = luci.model.uci.cursor():get("modem", "sms", "menable")
				rv["mslots"] = luci.model.uci.cursor():get("modem", "sms", "slots")
				rv["ready"] = "2"
				local tmp = file:read("*line")
				rv["used"] = tmp
				tmp = file:read("*line")
				rv["max"] = tmp
				full = nil

				repeat
					for j = 1, 4 do
						line = file:read("*line")
						if line ~= nil then
							if j == 3 then
								full = full .. string.char(29)
								local i = tonumber(line)
								for k = 1, i do
									line = file:read("*line")
									full = full .. line
									if k < i then
										full = full .. '\n'
									end
								end
							else
								if full == nil then
									full = line
								else
									full = full .. string.char(29) .. line
								end
							end
						end
					end
				until line == nil
				file:close()
				rv["line"] = full
			end
		end
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_change_sms()
	os.execute("/usr/lib/rooter/luci/modemchge.sh sms 1")
end

function action_change_smsdn()
	os.execute("/usr/lib/rooter/luci/modemchge.sh sms 0")
end

function action_change_smsflag()
	local set = tonumber(luci.http.formvalue("set"))
	os.execute("/usr/lib/sms/toggle.sh " .. set)
end
