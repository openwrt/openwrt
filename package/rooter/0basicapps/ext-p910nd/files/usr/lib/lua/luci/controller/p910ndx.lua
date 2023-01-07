-- Copyright 2008 Yanira <forum-2008@email.de>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.p910ndx", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	if not nixio.fs.access("/etc/config/p910nd") then
		return
	end

	local page

	page = entry({"admin", "services", "p910ndx"}, cbi("p910ndx"), _(translate("Print Server")), 60)
	page.dependent = true
end
