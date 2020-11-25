-- Copyright 2012 Gabor Varga <vargagab@gmail.com>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.transmission", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/transmission") then
		return
	end

	local page = entry({"admin", "services", "transmission"}, cbi("transmission"), _("Transmission"))
	page.dependent = true

end
