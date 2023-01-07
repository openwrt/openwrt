module("luci.controller.umount", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page

	page = entry({"admin", "services", "umount"}, cbi("umount", {hidesavebtn=true, hideresetbtn=true}), translate("Safely Eject Drive"), 25)
	page.dependent = true
end
