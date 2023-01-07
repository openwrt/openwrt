module("luci.controller.texting", package.seeall)
function index()
	local lock = luci.model.uci.cursor():get("custom", "menu", "full")
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		if lock == "1" then
			if (multilock == "1" and rootlock == "1") then
				entry({"admin", "adminmenu", "texting"}, cbi("fullmenu/texting"), "Random Texting", 9)
			else
				entry({"admin", "adminmenu", "texting"}, cbi("fullmenu/texting"), "---Random Texting", 9)
			end
		end
	end
	
	entry({"admin", "services", "chksms"}, call("action_chksms"))
end

function action_chksms()
	local rv = {}
	os.execute("/usr/lib/fullmenu/chksms.sh")
	file = io.open("/tmp/texting", "r")
	if file ~= nil then
		rv["sms"] = "1"
		file:close()
	else
		rv["sms"] = "0"
	end
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end