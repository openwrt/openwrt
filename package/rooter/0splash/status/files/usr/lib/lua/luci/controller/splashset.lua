--[[
luci-app-argon-config
]]--

module("luci.controller.splash", package.seeall)

function index()
	if nixio.fs.access("/etc/config/splash") then
		entry({"admin", "theme", "splash"}, cbi("splash"), _("Splash Screen"), 71)
	end
end
