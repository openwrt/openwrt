-- Copyright 2017 Kapil Dev Mishra <kdm6389@gmail.com.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.nfs-share", package.seeall)

function index()
	if not nixio.fs.access("/etc/exports") then
		return
	end
	
	local page

	page = entry({"admin", "services", "nfs-share"}, cbi("nfs-share"), _("Network Share - NFS"))
	page.dependent = true
end
