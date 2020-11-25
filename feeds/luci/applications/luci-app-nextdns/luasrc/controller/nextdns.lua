-- Copyright 2019 Olivier Poitrey (rs@nextdns.io)
-- This is free software, licensed under the Apache License, Version 2.0

module("luci.controller.nextdns", package.seeall)

local util  = require("luci.util")
local i18n  = require("luci.i18n")
local templ = require("luci.template")
local http  = require("luci.http")

function index()
	if not nixio.fs.access("/etc/config/nextdns") then
		return
	end

	entry({"admin", "services", "nextdns"}, firstchild(), _("NextDNS"), 60).dependent = false
	entry({"admin", "services", "nextdns", "overview"}, cbi("overview", {hideresetbtn=true, hidesavebtn=true}), _("Overview"), 10).leaf = true
	entry({"admin", "services", "nextdns", "log"}, template("nextdns/logread"), _("Logs"), 30).leaf = true

	entry({"admin", "services", "nextdns", "logread"}, call("logread"), nil).leaf = true

end

function logread()
	local content = util.trim(util.exec("logread -e 'nextdns'")) or ""

	if content == "" then
		content = "No nextdns related logs yet!"
	end
	http.write(content)
end

