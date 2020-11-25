-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.admin.status", package.seeall)

function index()
	local page

	entry({"admin", "status", "overview"}, template("admin_status/index"), _("Overview"), 1)

	entry({"admin", "status", "iptables"}, template("admin_status/iptables"), _("Firewall"), 2).leaf = true
	entry({"admin", "status", "iptables_dump"}, call("dump_iptables")).leaf = true
	entry({"admin", "status", "iptables_action"}, post("action_iptables")).leaf = true

	entry({"admin", "status", "routes"}, template("admin_status/routes"), _("Routes"), 3)
	entry({"admin", "status", "syslog"}, call("action_syslog"), _("System Log"), 4)
	entry({"admin", "status", "dmesg"}, call("action_dmesg"), _("Kernel Log"), 5)
	entry({"admin", "status", "processes"}, view("status/processes"), _("Processes"), 6)

	entry({"admin", "status", "realtime"}, alias("admin", "status", "realtime", "load"), _("Realtime Graphs"), 7)

	entry({"admin", "status", "realtime", "load"}, view("status/load"), _("Load"), 1)
	entry({"admin", "status", "realtime", "bandwidth"}, view("status/bandwidth"), _("Traffic"), 2)
	entry({"admin", "status", "realtime", "wireless"}, view("status/wireless"), _("Wireless"), 3).uci_depends = { wireless = true }
	entry({"admin", "status", "realtime", "connections"}, view("status/connections"), _("Connections"), 4)

	entry({"admin", "status", "nameinfo"}, call("action_nameinfo")).leaf = true
end

function action_syslog()
	local syslog = luci.sys.syslog()
	luci.template.render("admin_status/syslog", {syslog=syslog})
end

function action_dmesg()
	local dmesg = luci.sys.dmesg()
	luci.template.render("admin_status/dmesg", {dmesg=dmesg})
end

function dump_iptables(family, table)
	local prefix = (family == "6") and "ip6" or "ip"
	local ok, lines = pcall(io.lines, "/proc/net/%s_tables_names" % prefix)
	if ok and lines then
		local s
		for s in lines do
			if s == table then
				luci.http.prepare_content("text/plain")
				luci.sys.process.exec({
					"/usr/sbin/%stables" % prefix, "-w", "-t", table,
					"--line-numbers", "-nxvL"
				}, luci.http.write)
				return
			end
		end
	end

	luci.http.status(404, "No such table")
	luci.http.prepare_content("text/plain")
end

function action_iptables()
	if luci.http.formvalue("zero") then
		if luci.http.formvalue("family") == "6" then
			luci.util.exec("/usr/sbin/ip6tables -Z")
		else
			luci.util.exec("/usr/sbin/iptables -Z")
		end
	elseif luci.http.formvalue("restart") then
		luci.util.exec("/etc/init.d/firewall restart")
	end

	luci.http.redirect(luci.dispatcher.build_url("admin/status/iptables"))
end
