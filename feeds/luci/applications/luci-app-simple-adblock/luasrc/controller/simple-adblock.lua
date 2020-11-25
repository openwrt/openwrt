module("luci.controller.simple-adblock", package.seeall)
function index()
	if nixio.fs.access("/etc/config/simple-adblock") then
		entry({"admin", "services", "simple-adblock"}, cbi("simple-adblock"), _("Simple AdBlock"))
	end
end
