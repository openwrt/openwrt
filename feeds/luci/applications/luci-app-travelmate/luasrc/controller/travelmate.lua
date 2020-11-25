-- Copyright 2017-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

module("luci.controller.travelmate", package.seeall)

local sys  = require("luci.sys")
local util = require("luci.util")
local http = require("luci.http")
local i18n = require("luci.i18n")
local json = require("luci.jsonc")
local uci  = require("luci.model.uci").cursor()

function index()
	if not nixio.fs.access("/etc/config/travelmate") then
		return
	end
	entry({"admin", "services", "travelmate"}, firstchild(), _("Travelmate"), 40).dependent = false
	entry({"admin", "services", "travelmate", "tab_from_cbi"}, cbi("travelmate/overview_tab", {hideresetbtn=true, hidesavebtn=true}), _("Overview"), 10).leaf = true
	entry({"admin", "services", "travelmate", "stations"}, template("travelmate/stations"), _("Wireless Stations"), 20).leaf = true
	entry({"admin", "services", "travelmate", "log"}, template("travelmate/logread"), _("View Logfile"), 30).leaf = true
	entry({"admin", "services", "travelmate", "advanced"}, firstchild(), _("Advanced"), 100)
	entry({"admin", "services", "travelmate", "advanced", "configuration"}, form("travelmate/configuration_tab"), _("Edit Travelmate Configuration"), 110).leaf = true
	entry({"admin", "services", "travelmate", "advanced", "cfg_wireless"}, form("travelmate/cfg_wireless_tab"), _("Edit Wireless Configuration"), 120).leaf = true
	entry({"admin", "services", "travelmate", "advanced", "cfg_network"}, form("travelmate/cfg_network_tab"), _("Edit Network Configuration"), 130).leaf = true
	entry({"admin", "services", "travelmate", "advanced", "cfg_firewall"}, form("travelmate/cfg_firewall_tab"), _("Edit Firewall Configuration"), 140).leaf = true

	entry({"admin", "services", "travelmate", "logread"}, call("logread"), nil).leaf = true
	entry({"admin", "services", "travelmate", "status"}, call("status_update"), nil).leaf = true
	entry({"admin", "services", "travelmate", "action"}, call("trm_action"), nil).leaf = true
	entry({"admin", "services", "travelmate", "wifiscan"}, template("travelmate/wifi_scan")).leaf = true
	entry({"admin", "services", "travelmate", "wifiadd"}, form("travelmate/wifi_add", {hideresetbtn=true, hidesavebtn=true})).leaf = true
	entry({"admin", "services", "travelmate", "wifiedit"}, form("travelmate/wifi_edit", {hideresetbtn=true, hidesavebtn=true})).leaf = true
	entry({"admin", "services", "travelmate", "wifidelete"}, form("travelmate/wifi_delete", {hideresetbtn=true, hidesavebtn=true})).leaf = true
	entry({"admin", "services", "travelmate", "wifiorder"}, form("travelmate/wifi_order", {hideresetbtn=true, hidesavebtn=true})).leaf = true
end

function trm_action(name)
	if name == "do_restart" then
		luci.sys.call("/etc/init.d/travelmate restart >/dev/null 2>&1")
	end
	luci.http.prepare_content("text/plain")
	luci.http.write("0")
end

function status_update()
	local rt_file
	local content

	rt_file = uci:get("travelmate", "global", "trm_rtfile") or "/tmp/trm_runtime.json"

	if nixio.fs.access(rt_file) then
		content = json.parse(nixio.fs.readfile(rt_file) or "")
		http.prepare_content("application/json")
		http.write_json(content)
	end
end

function logread()
	local content = util.trim(util.exec("logread -e 'travelmate-'")) or ""

	if content == "" then
		content = "No travelmate related logs yet!"
	end
	http.write(content)
end
