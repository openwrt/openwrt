local ds = require "luci.dispatcher"
local nxo = require "nixio"
local nfs = require "nixio.fs"
local ipc = require "luci.ip"
local sys = require "luci.sys"
local utl = require "luci.util"
local dsp = require "luci.dispatcher"
local uci = require "luci.model.uci"
local lng = require "luci.i18n"
local jsc = require "luci.jsonc"
local http = luci.http
local SYS = require "luci.sys"
local m, s

m = Map("appfilter", translate("App Filter"), translate(
    "Please close the modules that may conflict, such as acceleration, ad filtering, and multi-dial"))

s = m:section(TypedSection, "global", translate("Basic Settings"))
s:option(Flag, "enable", translate("Enable App Filter"), translate(""))
s.anonymous = true

o=s:option(ListValue, "work_mode", translate("Work Mode"),translate("")) 
o.default=0
o:value(0, translate("Gateway Mode"))
o:value(1,translate("Bypass Mode"))

local rule_count = 0
local version = ""

s = m:section(TypedSection, "appfilter", translate("App Filter Rules"), 
translate("If there is no app you want, you can add the app by updating the app feature file"))
s.anonymous = true
s.addremove = false
function get_class_i18n_name(class_name)
    local fd = io.open("/tmp/app_class.txt", "r")
	if not fd then return end
    while true do
        local ln = fd:read("*l")
        if not ln then
            break
        end
        local id, name1, name2 = ln:match("^(%d+) (%S+) (%S+)")
        if  class_name == name1 then
            fd:close()
            return name2
        end
    end
	fd:close()
    return nil
end
local class_fd = io.popen("find /tmp/appfilter/ -type f -name '*.class'")
if class_fd then
    while true do
        local apps
        local class
        local i18n_name
        local path = class_fd:read("*l")
        if not path then
            break
        end

        class = path:match("([^/]+)%.class$")
        i18n_name=get_class_i18n_name(class)
        if nil ~= i18n_name then
            s:tab(class, i18n_name)
        else
            s:tab(class, class)
        end
        apps = s:taboption(class, MultiValue, class .. "apps", translate(""))
        apps.rmempty = true
        apps.widget = "checkbox"
        apps.size = 10

        local fd = io.open(path)
        if fd then
            local line
            while true do
                local cmd
                local cmd_fd
                line = fd:read("*l")
                if not line then
                    break
                end
                if string.len(line) < 5 then
                    break
                end
                if not string.find(line, "#") then
                    cmd = "echo " .. line .. "|awk '{print $1}'"
                    cmd_fd = io.popen(cmd)
                    id = cmd_fd:read("*l");
                    cmd_fd:close()

                    cmd = "echo " .. line .. "|awk '{print $2}'"
                    cmd_fd = io.popen(cmd)
                    name = cmd_fd:read("*l")

                    cmd_fd:close()
                    if not id then
                        break
                    end
                    if not name then
                        break
                    end
                    apps:value(id, name)
                end
            end
            fd:close()
        end
    end
    class_fd:close()
end

return m
