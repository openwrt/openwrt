-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011-2018 Jo-Philipp Wich <jo@mein.io>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.admin.network", package.seeall)

function index()
	local page

--	if page.inreq then
		page = entry({"admin", "network", "switch"}, view("network/switch"), _("Switch"), 20)
		page.uci_depends = { network = { ["@switch[0]"] = "switch" } }

		page = entry({"admin", "network", "wireless"}, view("network/wireless"), _('Wireless'), 15)
		page.uci_depends = { wireless = { ["@wifi-device[0]"] = "wifi-device" } }
		page.leaf = true

		page = entry({"admin", "network", "network"}, view("network/interfaces"), _("Interfaces"), 10)
		page.leaf   = true
		page.subindex = true

		page = node("admin", "network", "dhcp")
		page.uci_depends = { dhcp = true }
		page.target = view("network/dhcp")
		page.title  = _("DHCP and DNS")
		page.order  = 30

		page = node("admin", "network", "hosts")
		page.uci_depends = { dhcp = true }
		page.target = view("network/hosts")
		page.title  = _("Hostnames")
		page.order  = 40

		page  = node("admin", "network", "routes")
		page.target = view("network/routes")
		page.title  = _("Static Routes")
		page.order  = 50

		page = node("admin", "network", "diagnostics")
		page.target = view("network/diagnostics")
		page.title  = _("Diagnostics")
		page.order  = 60
--	end
end
