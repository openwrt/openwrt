-- Copyright 2015 Daniel Dickinson <openwrt@daniel.thecshore.com>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.nut", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/nut_server") and not nixio.fs.access("/etc/config/nut_monitor") and not nixio.fs.access("/etc/config/nut_cgi") then
		return
	end

	entry({"admin", "services", "nut"}, firstchild(), _("Network UPS Tools"))

	if nixio.fs.access("/etc/config/nut_server") then
		entry({"admin", "services", "nut", "server"}, cbi("nut_server"), _("Network UPS Tools (Server)"), 20)
	end

	if nixio.fs.access("/etc/config/nut_monitor") then
		entry({"admin", "services", "nut", "monitor"}, cbi("nut_monitor"), _("Network UPS Tools (Monitor)"), 30)
	end

	if nixio.fs.access("/etc/config/nut_cgi") then
		entry({"admin", "services", "nut", "cgi"}, cbi("nut_cgi"), _("Network UPS Tools (CGI)"), 40)
	end
end

