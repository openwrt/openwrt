module("luci.controller.speedtest", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local page
	entry({"admin", "speed"}, firstchild(), translate("Speed Test"), 95).dependent=false
	page = entry({"admin", "speed", "speedtest"}, template("speedtest/speedtest"), translate("OpenSpeedTest"), 71)
	page.dependent = true
end
