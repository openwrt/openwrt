-- Copyright 2017-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

module("luci.controller.adblock", package.seeall)

local sys   = require("luci.sys")
local util  = require("luci.util")
local http  = require("luci.http")
local i18n  = require("luci.i18n")
local json  = require("luci.jsonc")
local uci   = require("luci.model.uci").cursor()

function index()
	if not nixio.fs.access("/etc/config/adblock") then
		return
	end
	entry({"admin", "services", "adblock"}, firstchild(), _("Adblock"), 30).dependent = false
	entry({"admin", "services", "adblock", "tab_from_cbi"}, cbi("adblock/overview_tab", {hideresetbtn=true, hidesavebtn=true}), _("Overview"), 10).leaf = true
	if nixio.fs.access("/usr/sbin/tcpdump") then
		entry({"admin", "services", "adblock", "report"}, template("adblock/report"), _("DNS Query Report"), 20).leaf = true
	end
	entry({"admin", "services", "adblock", "log"}, template("adblock/logread"), _("Logfile"), 30).leaf = true
	entry({"admin", "services", "adblock", "advanced"}, firstchild(), _("Advanced"), 100)
	entry({"admin", "services", "adblock", "advanced", "blacklist"}, form("adblock/blacklist_tab"), _("Edit Blacklist"), 110).leaf = true
	entry({"admin", "services", "adblock", "advanced", "whitelist"}, form("adblock/whitelist_tab"), _("Edit Whitelist"), 120).leaf = true
	entry({"admin", "services", "adblock", "advanced", "configuration"}, form("adblock/configuration_tab"), _("Edit Configuration"), 130).leaf = true
	entry({"admin", "services", "adblock", "advanced", "query"}, template("adblock/query"), _("Query domains"), 140).leaf = true
	entry({"admin", "services", "adblock", "advanced", "result"}, call("queryData"), nil, 150).leaf = true
	entry({"admin", "services", "adblock", "logread"}, call("logread"), nil).leaf = true
	entry({"admin", "services", "adblock", "status_update"}, call("status_update"), nil).leaf = true
	entry({"admin", "services", "adblock", "report_json"}, call("report_json"), nil).leaf = true
	entry({"admin", "services", "adblock", "report_text"}, call("report_text"), nil).leaf = true
	entry({"admin", "services", "adblock", "action"}, call("adb_action"), nil).leaf = true
end

function adb_action(name, ...)
	local domain = select(1, ...) or ""
	local search = select(2, ...) or "+"
	local count  = select(3, ...) or "50"
	local filter = select(4, ...) or "false"
	local print  = select(5, ...) or "false"

	local report_params = {
		search,
		count,
		filter,
		print
	}

	if name == "do_suspend" then
		luci.sys.call("/etc/init.d/adblock suspend >/dev/null 2>&1")
	elseif name == "do_resume" then
		luci.sys.call("/etc/init.d/adblock resume >/dev/null 2>&1")
	elseif name == "do_refresh" then
		luci.sys.call("/etc/init.d/adblock reload >/dev/null 2>&1")
	elseif name == "do_report" then
		luci.sys.call("/etc/init.d/adblock report " ..table.concat(report_params, " ").. " >/dev/null 2>&1")
		local rep_dir  = uci:get("adblock", "extra", "adb_repdir") or "/tmp"
		repeat
			nixio.nanosleep(1)
		until not nixio.fs.access(rep_dir.. "/adb_report.raw")
	elseif name == "do_filter" then
		luci.sys.call("/etc/init.d/adblock report " ..table.concat(report_params, " ").. " >/dev/null 2>&1")
		local rep_dir  = uci:get("adblock", "extra", "adb_repdir") or "/tmp"
		repeat
			nixio.nanosleep(1)
		until nixio.fs.access(rep_dir.. "/adb_report.final")
	elseif name == "add_blacklist" then
		local file = uci:get("adblock", "blacklist", "adb_src") or "/etc/adblock/adblock.blacklist"
		if nixio.fs.access(file) then
			local blacklist = nixio.fs.readfile(file)
			if not string.find(blacklist, domain, 1, true)
			then
				nixio.fs.writefile(file, blacklist.. domain.. "\n")
			end
		end
	elseif name == "add_whitelist" then
		local file = uci:get("adblock", "global", "adb_whitelist") or "/etc/adblock/adblock.whitelist"
		if nixio.fs.access(file) then
	 		local whitelist = nixio.fs.readfile(file)
			if not string.find(whitelist, domain, 1, true)
			then
				nixio.fs.writefile(file, whitelist.. domain.. "\n")
			end
		end
	end
	if name == "do_suspend" or name == "do_resume" or name == "do_refresh" then
		local pid_file = "/var/run/adblock.pid"
		if nixio.fs.access(pid_file) then
			repeat
				nixio.nanosleep(1)
			until nixio.fs.readfile(pid_file) == ""
		end
	end
	luci.http.prepare_content("text/plain")	
	luci.http.write("0")
end

function status_update()
	local rt_file
	local content

	rt_file = uci:get("adblock", "global", "adb_rtfile") or "/tmp/adb_runtime.json"

	if nixio.fs.access(rt_file) then
		content = json.parse(nixio.fs.readfile(rt_file) or "")
		http.prepare_content("application/json")
		http.write_json(content)
	end
end

function report_json()
	local rep_dir
	local rep_file
	local content

	rep_dir  = uci:get("adblock", "extra", "adb_repdir") or "/tmp"
	rep_file = rep_dir.. "/adb_report.json"
	http.prepare_content("application/json")

	if nixio.fs.access(rep_file) then
		content = json.parse(nixio.fs.readfile(rep_file) or "")
		http.write_json(content)
	else
		http.write_json("{}")
	end
end

function report_text()
	local file
	local rep_dir
	local rep_file
	local content

	rep_dir  = uci:get("adblock", "extra", "adb_repdir") or "/tmp"
	rep_file = rep_dir.. "/adb_report.final"
	http.prepare_content("text/plain")

	if nixio.fs.access(rep_file) then
		file = io.open(rep_file, "r")
		content = file:read("*all")
		file:close()
		http.write(content)
	else
		http.write("")
	end
end

function logread()
	local content = util.trim(util.exec("logread -e 'adblock-'")) or ""

	if content == "" then
		content = "No adblock related logs yet!"
	end
	http.write(content)
end

function queryData(domain)
	if domain then
		luci.http.prepare_content("text/plain")
		local cmd = "/etc/init.d/adblock query %s 2>&1"
		local util = io.popen(cmd % util.shellquote(domain))
		if util then
			while true do
				local line = util:read("*l")
				if not line then
					break
				end
				luci.http.write(line)
				luci.http.write("\n")
			end
			util:close()
		end
	end
end
