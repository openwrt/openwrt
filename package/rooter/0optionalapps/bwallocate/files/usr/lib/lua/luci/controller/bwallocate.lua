module("luci.controller.bwallocate", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local lock = luci.model.uci.cursor():get("custom", "menu", "full")
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		if lock == "1" then
			local lock1 = luci.model.uci.cursor():get("custom", "bwallocate", "lock")
			if lock1 == "1" then
				if (multilock == "1" and rootlock == "1") then
					entry({"admin", "adminmenu", "bwmenu"}, cbi("fullmenu/bwmenu"), translate("Bandwidth Allocation"), 6)
				else
					entry({"admin", "adminmenu", "bwmenu"}, cbi("fullmenu/bwmenu"), translate("---Bandwidth Allocation"), 6)
				end
			end
		end
	end
end