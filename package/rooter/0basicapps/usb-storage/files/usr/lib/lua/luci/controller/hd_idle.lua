-- Copyright 2008 Yanira <forum-2008@email.de>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.hd_idle", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	if not nixio.fs.access("/etc/config/hd-idle") then
		return
	end

	local page
	page = entry({"admin", "services", "hd_idle"}, cbi("hd_idle"), _(translate("Hard Drive Idle")), 60)
	page.dependent = true
end
