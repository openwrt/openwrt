--[[
login
]]--

module("luci.controller.login", package.seeall)

function index()
	entry({"admin", "theme", "login"}, form("back-login"), _("Login Backgrounds"),40)
end
