-- Copyright 2018-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

module("luci.controller.banip", package.seeall)

local util  = require("luci.util")
local http  = require("luci.http")
local i18n  = require("luci.i18n")
local json  = require("luci.jsonc")
local uci   = require("luci.model.uci").cursor()

function index()
	if not nixio.fs.access("/etc/config/banip") then
		return
	end
	entry({"admin", "services", "banip"}, firstchild(), _("banIP"), 40).dependent = false
	entry({"admin", "services", "banip", "tab_from_cbi"}, cbi("banip/overview_tab", {hideresetbtn=true, hidesavebtn=true}), _("Overview"), 10).leaf = true
	entry({"admin", "services", "banip", "ipset"}, template("banip/ipsetview"), _("IPSet-Lookup"), 20).leaf = true
	entry({"admin", "services", "banip", "ripe"}, template("banip/ripeview"), _("RIPE-Lookup"), 30).leaf = true
	entry({"admin", "services", "banip", "log"}, template("banip/logview"), _("View Logfile"), 40).leaf = true
	entry({"admin", "services", "banip", "advanced"}, firstchild(), _("Advanced"), 100)
	entry({"admin", "services", "banip", "advanced", "blacklist"}, form("banip/blacklist_tab"), _("Edit Blacklist"), 110).leaf = true
	entry({"admin", "services", "banip", "advanced", "whitelist"}, form("banip/whitelist_tab"), _("Edit Whitelist"), 120).leaf = true
	entry({"admin", "services", "banip", "advanced", "configuration"}, form("banip/configuration_tab"), _("Edit Configuration"), 130).leaf = true
	entry({"admin", "services", "banip", "ipsetview"}, call("ipset_view"), nil).leaf = true
	entry({"admin", "services", "banip", "ripeview"}, call("ripe_view"), nil).leaf = true
	entry({"admin", "services", "banip", "logview"}, call("log_view"), nil).leaf = true
	entry({"admin", "services", "banip", "status"}, call("status_update"), nil).leaf = true
	entry({"admin", "services", "banip", "action"}, call("ban_action"), nil).leaf = true
end

function ban_action(name)
	if name == "do_refresh" then
		luci.sys.call("/etc/init.d/banip refresh >/dev/null 2>&1")
	elseif name == "do_reload" then
		luci.sys.call("/etc/init.d/banip reload >/dev/null 2>&1")
	end
	luci.http.prepare_content("text/plain")
	luci.http.write("0")
end

function status_update()
	local rt_file
	local content

	rt_file = uci:get("banip", "global", "ban_rtfile") or "/tmp/ban_runtime.json"

	if nixio.fs.access(rt_file) then
		content = json.parse(nixio.fs.readfile(rt_file) or "")
		http.prepare_content("application/json")
		http.write_json(content)
	end
end

function log_view()
	local content = util.trim(util.exec("logread -e 'banIP-' 2>/dev/null")) or ""

	if content == "" then
		content = "No banIP related logs yet!"
	end
	http.write(content)
end

function ipset_view(ipset, filter)
	local content

	if not (ipset or filter) then
		return
	end

	if filter == "false" then
		content = util.trim(util.exec("ipset -L " .. ipset .. " 2>/dev/null"))
	else
		content = util.trim(util.exec("ipset -L " .. ipset .. " 2>/dev/null | grep -e 'packets [1-9]\\|^[A-Z]'"))
	end

	if content == "" then
		content = "IPSet is empty!"
	end
	http.write(content)
end

function ripe_view(query, input)
	local content

	if not (query or input) then
		return
	end

	content = util.trim(util.exec("uclient-fetch --no-check-certificate -O- https://stat.ripe.net/data/" ..query.. "/data.json?resource=" ..input.. " 2>/dev/null"))
	
	if content == "" then
		content = "No response!"
	end
	http.write(content)
end
