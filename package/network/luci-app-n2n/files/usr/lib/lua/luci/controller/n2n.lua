module("luci.controller.n2n", package.seeall)

function index()
	--require("luci.i18n")
	--luci.i18n.loadc("n2n")
	if not nixio.fs.access("/etc/config/n2n") then
		return
	end
	
	local page = entry({"admin", "services", "n2n"}, cbi("n2n"), luci.i18n.translate("N2N VPN"), 85)
	page.i18n = "n2n"
	page.dependent = true
	
	
	local page = entry({"mini", "services", "n2n"}, cbi("n2n", {autoapply=true}), luci.i18n.translate("N2N VPN"), 85)
	page.i18n = "n2n"
	page.dependent = true
end
