--[[
ext-theme
]]--

module("luci.controller.splash", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	entry({"admin", "splash"}, firstchild(), translate("Splash Screen"), 99).dependent=false
	entry({"admin", "splash", "splash"}, cbi("splashm"), _(translate("Configuration")), 20)
end
