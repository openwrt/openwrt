--[[
ext-theme
]]--

module("luci.controller.exttheme", package.seeall)

function index()
	entry({"admin", "theme"}, firstchild(), "Themes", 99).dependent=false
	entry({"admin", "theme", "theme"}, cbi("themes"), _("Change GUI Theme"), 20)
end
