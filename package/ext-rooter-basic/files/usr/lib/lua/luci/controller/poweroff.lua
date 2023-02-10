module("luci.controller.poweroff", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
        local page
        page = entry({"admin", "system", "poweroff"}, template("admin_system/poweroff"), _(translate("System Stop")), 95)
        entry({"admin", "system", "do_poweroff"}, call("action_poweroff"))
        page.dependent = true
end

function action_poweroff()
        local set = luci.http.formvalue("set")
        os.execute("/usr/lib/rooter/shutall.sh")
end
