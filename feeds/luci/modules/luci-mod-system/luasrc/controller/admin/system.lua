-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008-2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.admin.system", package.seeall)

function index()
	entry({"admin", "system", "system"}, view("system/system"), _("System"), 1)

	entry({"admin", "system", "admin"}, firstchild(), _("Administration"), 2)
	entry({"admin", "system", "admin", "password"}, view("system/password"), _("Router Password"), 1)

	entry({"admin", "system", "admin", "dropbear"}, view("system/dropbear"), _("SSH Access"), 2)
		.uci_depends = { dropbear = true }

	entry({"admin", "system", "admin", "sshkeys"}, view("system/sshkeys"), _("SSH-Keys"), 3)
		.uci_depends = { dropbear = true }

	entry({"admin", "system", "startup"}, view("system/startup"), _("Startup"), 45)
	entry({"admin", "system", "crontab"}, view("system/crontab"), _("Scheduled Tasks"), 46)

	entry({"admin", "system", "mounts"}, view("system/mounts"), _("Mount Points"), 50)
		.file_depends = { "/sbin/block" }

	entry({"admin", "system", "leds"}, view("system/leds"), _("LED Configuration"), 60)
		.file_depends = { "/sys/class/leds" }

	entry({"admin", "system", "flash"}, view("system/flash"), _("Backup / Flash Firmware"), 70)

	entry({"admin", "system", "reboot"}, view("system/reboot"), _("Reboot"), 90)
end
