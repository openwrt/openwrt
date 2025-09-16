module("luci.controller.Secondsystem", package.seeall)

function index()
    entry({"admin", "system", "Secondsystem"}, alias("admin", "system", "Secondsystem", "settings"), _("官方系统"), 50)
    entry({"admin", "system", "Secondsystem", "settings"}, template("Secondsystem/settings"), _("Settings"), 10)
    entry({"admin", "system", "Secondsystem", "switch"}, call("action_switch"), nil)
    entry({"admin", "system", "Secondsystem", "reboots"}, call("action_reboots"), nil)
end

function action_switch()
    local sys = require "luci.sys"
    local http = require "luci.http"
    local command2 = 'AT+CFUN=1,1'
    local sendat2 = 'sendat 2 "' .. command2 .. '"'
    local confirm = http.formvalue("confirm")
    
    if confirm and confirm == "yes" then
        sys.call("fw_setenv boot_system 0")
        --sys.call(sendat2)
        sys.call("reboot")
    else
        luci.http.redirect(luci.dispatcher.build_url("admin", "system", "Secondsystem", "settings"))
    end
end

function action_reboots()
    local sys = require "luci.sys"
    local http = require "luci.http"
    local command = 'AT+CFUN=1,1'
    local sendat = 'sendat 2 "' .. command .. '"'
    local confirm = http.formvalue("confirm")
    
    if confirm and confirm == "yes" then
        sys.call(sendat)
        sys.call("reboot")
    else
        luci.http.redirect(luci.dispatcher.build_url("admin", "system", "Secondsystem", "settings"))
    end
end
