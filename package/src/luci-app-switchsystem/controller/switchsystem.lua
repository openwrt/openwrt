-- SPDX-License-Identifier: GPL-2.0-or-later
module("luci.controller.SwitchSystem", package.seeall)

function index()
    entry({"admin", "system", "switchsystem"}, alias("admin", "system", "switchsystem", "settings"), _("Switch System"), 50)
    entry({"admin", "system", "switchsystem", "settings"}, template("cbi/switchsystem/custom_html"), _("Settings"), 10)
    entry({"admin", "system", "switchsystem", "switch"}, call("action_switch"), _("Switch System"), 20).dependent=false
    entry({"admin", "system", "switchsystem", "reboot"}, call("action_reboot"), _("Reboot System"), 30).dependent=false
end

function action_switch()
    local fs = io.open("/tmp/switchsystem_cmd", "w")
    if fs then
        fs:write("switch_to_official\n")
        fs:close()
    end
    luci.http.redirect(luci.dispatcher.build_url("admin", "system", "switchsystem", "settings"))
end

function action_reboot()
    os.execute("reboot")
    luci.http.redirect(luci.dispatcher.build_url("admin", "system", "switchsystem", "settings"))
end
