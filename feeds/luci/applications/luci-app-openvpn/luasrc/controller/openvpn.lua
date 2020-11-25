-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.openvpn", package.seeall)

function index()
	entry( {"admin", "vpn", "openvpn"}, cbi("openvpn"), _("OpenVPN") )
	entry( {"admin", "vpn", "openvpn", "basic"},    cbi("openvpn-basic"),    nil ).leaf = true
	entry( {"admin", "vpn", "openvpn", "advanced"}, cbi("openvpn-advanced"), nil ).leaf = true
	entry( {"admin", "vpn", "openvpn", "file"},     form("openvpn-file"),    nil ).leaf = true
	entry( {"admin", "vpn", "openvpn", "upload"},   call("ovpn_upload"))
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
