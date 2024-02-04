local o = require "luci.dispatcher"
local fs = require "nixio.fs"
local jsonc = require "luci.jsonc"

local sessions = {}
local session_path = "/var/etc/pppoe-server/session"
if fs.access(session_path) then
    for filename in fs.dir(session_path) do
        local session_file = session_path .. "/" .. filename
        local file = io.open(session_file, "r")
        local t = jsonc.parse(file:read("*a"))
        if t then
            t.session_file = session_file
            sessions[#sessions + 1] = t
        end
        file:close()
    end
end

f = SimpleForm("processes", translate("PPPoE Server"), translate("The PPPoE server is a broadband access authentication server that prevents ARP spoofing."))
f.reset = false
f.submit = false

t = f:section(Table, sessions, translate("Online Users"))
t:option(DummyValue, "username", translate("Username"))
t:option(DummyValue, "interface", translate("Interface"))
t:option(DummyValue, "ip", translate("Client IP"))
t:option(DummyValue, "mac", translate("MAC address"))
t:option(DummyValue, "login_time", translate("Login Time"))

_kill = t:option(Button, "_kill", translate("Forced offline"))
_kill.inputstyle = "reset"
function _kill.write(t, s)
    luci.util.execi("rm -f " .. t.map:get(s, "session_file"))
    null, t.tag_error[t] = luci.sys.process.signal(t.map:get(s, "pid"), 9)
    luci.http.redirect(o.build_url("admin/services/pppoe-server/online"))
end

return f
