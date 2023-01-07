-- A simple web console in case you don't have access to the shell
-- 
-- Hua Shao <nossiac@163.com>

module("luci.controller.webconsole", package.seeall)
local http = require("luci.http")

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		entry({"admin", "system", "console"}, template("web/web_console"), _(translate("Web Console")), 66)
	end
    entry({"admin", "system", "webcmd"}, call("action_webcmd"))
end

function action_webcmd()
    local cmd = http.formvalue("cmd")
    if cmd then
	    local fp = io.popen(tostring(cmd).." 2>&1")
	    local result =  fp:read("*a")
	    fp:close()
        result = result:gsub("<", "&lt;")
        http.write(tostring(result))
    else
        http.write_json(http.formvalue())
    end
end
