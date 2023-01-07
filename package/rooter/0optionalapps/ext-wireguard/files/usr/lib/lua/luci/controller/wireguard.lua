-- Copyright 2016-2017 Dan Luedtke <mail@danrl.com>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.wireguard", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		entry({"admin", "vpn", "wireguard"}, cbi("wireguard"), _("Wireguard"), 63)
		entry( {"admin", "vpn", "wireguard", "client"},    cbi("wireguard-client"),    nil ).leaf = true
		entry( {"admin", "vpn", "wireguard", "server"},    cbi("wireguard-server"),    nil ).leaf = true
	end
	  
	  entry( {"admin", "vpn", "wireguard", "wupload"},   call("conf_upload"))
	  entry( {"admin", "vpn", "generateconf"},   call("conf_gen"))
	  entry( {"admin", "vpn", "textconf"},   call("text_gen"))
	  entry( {"admin", "vpn", "wirestatus"},   call("wirestatus"))
end

function conf_upload()
	local fs     = require("nixio.fs")
	local http   = require("luci.http")
	local util   = require("luci.util")
	local uci    = require("luci.model.uci").cursor()
	local upload = http.formvalue("ovpn_file")
	local name   = http.formvalue("instance_name2")
	local file   = "/etc/openvpn/" ..name.. ".conf"

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
			os.execute("/usr/lib/wireguard/conf.sh " .. name .. " " .. file)
		end
	end
	http.redirect(luci.dispatcher.build_url('admin/vpn/wireguard'))
end

function conf_gen()
	os.execute("/usr/lib/wireguard/create.sh")
end

function text_gen()
	local set = luci.http.formvalue("set")
	os.execute("/usr/lib/wireguard/text.sh " .. "\"" .. set .. "\"")
end

function wirestatus()
	local data = { }
  local last_device = ""

  local wg_dump = io.popen("wg show all dump")
  if wg_dump then
    local line
    for line in wg_dump:lines() do
      local line = string.split(line, "\t")
      if not (last_device == line[1]) then
        last_device = line[1]
        data[line[1]] = {
          name                 = line[1],
          public_key           = line[3],
          listen_port          = line[4],
          fwmark               = line[5],
          peers                = { }
        }
      else
        local peer = {
          public_key           = line[2],
          endpoint             = line[4],
          allowed_ips          = { },
          latest_handshake     = line[6],
          transfer_rx          = line[7],
          transfer_tx          = line[8],
          persistent_keepalive = line[9]
        }
        if not (line[4] == '(none)') then
          for ipkey, ipvalue in pairs(string.split(line[5], ",")) do
            if #ipvalue > 0 then
              table.insert(peer['allowed_ips'], ipvalue)
            end
          end
        end
        table.insert(data[line[1]].peers, peer)
      end
    end
  end

    luci.http.prepare_content("application/json")
    luci.http.write_json(data)
end