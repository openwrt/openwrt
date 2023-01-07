-- Licensed to the public under the Apache License 2.0.

module("luci.controller.domain", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local lock = luci.model.uci.cursor():get("custom", "menu", "full")
	--if lock == "1" then
		local page
		page = entry({"admin", "adminmenu", "domain"}, cbi("domainfltr"), _(translate("Domain Filter")), 9)
		page.dependent = true
	--end
end
