-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.openvpn", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		entry({"admin", "vpn"}, firstchild(), translate("VPN"), 30).dependent=false
		entry( {"admin", "vpn", "openvpn"}, cbi("openvpn"), _(translate("OpenVPN")), 1 )
		entry( {"admin", "vpn", "openvpn-server"}, cbi("openvpn-server"), _(translate("--OpenVPN Extras")), 2 )

		entry( {"admin", "vpn", "openvpn", "basic"},    cbi("openvpn-basic"),    nil ).leaf = true
		entry( {"admin", "vpn", "openvpn", "advanced"}, cbi("openvpn-advanced"), nil ).leaf = true
		entry( {"admin", "vpn", "openvpn", "file"},     form("openvpn-file"),    nil ).leaf = true
		entry( {"admin", "vpn", "openvpn", "upload"},   call("ovpn_upload"))
	end
	
	entry({"admin", "vpn", "vpnstatus"}, call("action_vpnstatus"))
	entry({"admin", "vpn", "rsastatus"}, call("action_status"))
	entry({"admin", "vpn", "rsagenerate"}, call("action_generate"))
	entry({"admin", "vpn", "rsastop"}, call("action_stop"))
end

function ovpn_upload()
	local fs     = require("nixio.fs")
	local http   = require("luci.http")
	local util   = require("luci.util")
	local uci    = require("luci.model.uci").cursor()
	local upload = http.formvalue("ovpn_file")
	local name   = http.formvalue("instance_name2")
	local file   = "/etc/openvpn/" ..name.. ".ovpn"

	if name and upload then
		local fp

		http.setfilehandler(
			function(meta, chunk, eof)
				local data = util.trim(chunk:gsub("\r\n", "\n")) .. "\n"
				data = util.trim(data:gsub("[\128-\255]", ""))

				if not fp and meta and meta.name == "ovpn_file" then
					fp = io.open(file, "w")
				end
				if fp and data then
					fp:write(data)
				end
				if fp and eof then
					fp:close()
				end
			end
		)

		if fs.access(file) then
			if not uci:get_first("openvpn", name) then
				uci:set("openvpn", name, "openvpn")
				uci:set("openvpn", name, "config", file)
				uci:save("openvpn")
				uci:commit("openvpn")
			end
		end
	end
	http.redirect(luci.dispatcher.build_url('admin/vpn/openvpn'))
end

function action_status()
	local rv = {}
	
	file = io.open("/tmp/easyrsa", "r")
	if file ~= nil then
		rv["status"] = file:read("*line")
		file:close()	
	else
		rv["status"] = "0"
	end
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_generate()
	os.execute("/usr/lib/easyrsa/generate.sh &")
end

function action_stop()
	os.execute("/usr/lib/easyrsa/stop.sh")
end

function action_vpnstatus()
	local rv ={}

	os.execute("/usr/lib/easyrsa/vpng.sh")
	local file = io.open("/tmp/vpn", "r")
	if file ~= nil then
		i = 0
		name = file:read("*line")
		while name ~= nil do
			vpntype = file:read("*line")
			enabled = file:read("*line")
			started = file:read("*line")
			port = file:read("*line")
			proto = file:read("*line")
			remote = file:read("*line")
			clients = file:read("*line")
			tooltip = file:read("*line")
			laststart = file:read("*line")
			rv[#rv+1] 	= {
				name 		= name,
				vpntype 	= vpntype,
				enabled		= enabled,
				started		= started,
				port		= port,
				proto		= proto,
				remote		= remote,
				clients		= clients,
				tooltip		= tooltip,
				laststart	= laststart
			}
			name = file:read("*line")
		end
		file:close()
	else
		rv[#rv+1] 	= {
			enabled		= "-1",
		}
	end
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end