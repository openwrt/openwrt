-- Licensed to the public under the Apache License 2.0.

module("luci.controller.schedule", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page

	page = entry({"admin", "services", "schedule"}, cbi("schedule"), _(translate("Scheduled Reboot")), 61)
	page.dependent = true
end
