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

m = Map("appfilter", translate("App Filter"), translate(""))

function get_hostname_by_mac(dst_mac)
    leasefile = "/tmp/dhcp.leases"
    local fd = io.open(leasefile, "r")
    if not fd then
        return
    end
    while true do
        local ln = fd:read("*l")
        if not ln then
            break
        end
        local ts, mac, ip, name, duid = ln:match("^(%d+) (%S+) (%S+) (%S+) (%S+)")
        print(ln)
        if dst_mac == mac then
            fd:close()
            return name
        end
    end
    fd:close()
    return nil
end

function get_cmd_result(command)
    local fd
    local result
    fd = io.popen(command);
    if not fd then
        return ""
    end
    result = fd:read("*l");
    fd:close()
    return result
end

s = m:section(TypedSection, "user", translate("Select users"))
s.anonymous = true
users = s:option(MultiValue, "users", "", translate(
    "It takes effect for all users by default, and only takes effect for the selected users when checked"))
users.widget = "checkbox"
-- users.widget="select"
users.size = 1

local fd = io.open("/tmp/dev_list", "r")
if not fd then
    return m
end
while true do
    local line = fd:read("*l")
    if not line then
        break
    end
    if not string.match(line, "^Id") then
        local ip = get_cmd_result(string.format("echo '%s' | awk '{print $3}'", line))
        local mac = get_cmd_result(string.format("echo '%s' | awk '{print $2}'", line))
        local hostname = get_cmd_result(string.format("echo '%s' | awk '{print $4}'", line))
        if mac ~= nil then
            if not hostname or hostname == "*" then
                users:value(mac, mac);
            else
                users:value(mac, hostname .. "(" .. mac .. ")");
            end
        end
    end
end
fd:close()

local config_users = m.uci:get_all("appfilter.user.users")
if config_users ~= nil and config_users ~= false then
    local r = utl.split(config_users, "%s+", nil, true)
    local max = table.getn(r)
    for i = 1, max, 1 do
        users:value(r[i], r[i]);
    end
end
return m
