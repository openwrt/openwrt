-- Copyright 2018 Rosy Song <rosysong@rosinson.com>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.rosy-file-server.rosy-file-server", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/rosyfs") then
		return
	end

	local root = node()
	if not root.target then
		root.target = alias("httpfs")
		root.index = true
	end

	page          = node()
	page.lock     = true
	page.target   = alias("httpfs")
	page.subindex = true
	page.index    = false

	page          = node("httpfs")
	page.title    = _("Rosy File Server")
	page.target   = alias("httpfs", "rosy-file-server")
	page.order    = 5
	page.setuser  = "root"
	page.setgroup = "root"
	page.index    = true

	entry({"httpfs", "rosy-file-server"},
		form("rosy-file-server/rosy-file-server"), _("Rosy File Server"), 10)

	entry({"admin", "services", "rosyfs"},
		cbi("rosy-file-server/rosyfs"), _("Rosy File Server"), 61)
end

