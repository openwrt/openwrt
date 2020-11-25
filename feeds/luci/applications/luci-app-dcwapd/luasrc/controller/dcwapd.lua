-- Copyright 2019 EWSI
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.dcwapd", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/dcwapd") then
		return
	end
	local page
	page = entry({"admin", "network", "dcwapd"}, cbi("dcwapd/dcwapd"), _("Dual Channel WiFi"))
	page.dependent = true
end
