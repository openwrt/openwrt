local fs = require "nixio.fs"

m = Map("passwall")
-- [[ Rule List Settings ]]--
s = m:section(TypedSection, "global", translate("Set Blacklist And Whitelist"))
s.anonymous = true

---- Whitelist Hosts
local t = "/usr/share/passwall/rule/whitelist_host"
o = s:option(TextValue, "whitelist_host", translate("Whitelist Hosts"))
o.description = translate(
                    "Join the white list of domain names will not go agent.")
o.rows = 5
o.wrap = "off"
o.cfgvalue = function(a, a) return fs.readfile(t) or "" end
o.write = function(o, o, a)
    fs.writefile("/tmp/whitelist_host", a:gsub("\r\n", "\n"))
    if (luci.sys.call(
        "cmp -s /tmp/whitelist_host /usr/share/passwall/rule/whitelist_host") ==
        1) then
        fs.writefile(t, a:gsub("\r\n", "\n"))
        luci.sys.call("rm -f /tmp/dnsmasq.d/whitelist_host.conf >/dev/null")
    end
    fs.remove("/tmp/whitelist_host")
end

---- Whitelist IP
local t = "/usr/share/passwall/rule/whitelist_ip"
o = s:option(TextValue, "whitelist_ip", translate("Whitelist IP"))
o.description = translate(
                    "These had been joined ip addresses will not use proxy.Please input the ip address or ip address segment,every line can input only one ip address.For example,112.123.134.145/24 or 112.123.134.145.")
o.rows = 5
o.wrap = "off"
o.cfgvalue = function(a, a) return fs.readfile(t) or "" end
o.write = function(o, o, a) fs.writefile(t, a:gsub("\r\n", "\n")) end

---- Blacklist Hosts
local t = "/usr/share/passwall/rule/blacklist_host"
o = s:option(TextValue, "blacklist_host", translate("Blacklist Hosts"))
o.description = translate(
                    "These had been joined websites will use proxy.Please input the domain names of websites,every line can input only one website domain.For example,google.com.")
o.rows = 5
o.wrap = "off"
o.cfgvalue = function(a, a) return fs.readfile(t) or "" end
o.write = function(o, o, a)
    fs.writefile("/tmp/blacklist_host", a:gsub("\r\n", "\n"))
    if (luci.sys.call(
        "cmp -s /tmp/blacklist_host /usr/share/passwall/rule/blacklist_host") ==
        1) then
        fs.writefile(t, a:gsub("\r\n", "\n"))
        luci.sys.call("rm -f /tmp/dnsmasq.d/blacklist_host.conf >/dev/null")
    end
    fs.remove("/tmp/blacklist_host")
end

---- Blacklist IP
local t = "/usr/share/passwall/rule/blacklist_ip"
o = s:option(TextValue, "blacklist_ip", translate("Blacklist IP"))
o.description = translate(
                    "These had been joined ip addresses will use proxy.Please input the ip address or ip address segment,every line can input only one ip address.For example,112.123.134.145/24 or 112.123.134.145.")
o.rows = 5
o.wrap = "off"
o.cfgvalue = function(a, a) return fs.readfile(t) or "" end
o.write = function(o, o, a) fs.writefile(t, a:gsub("\r\n", "\n")) end

---- Router Hosts
local t = "/usr/share/passwall/rule/router"
o = s:option(TextValue, "routerlist", translate("Router Hosts"))
o.description = translate(
                    "These had been joined websites will use proxy,but only Router model.Please input the domain names of websites,every line can input only one website domain.For example,google.com.")
o.rows = 5
o.wrap = "off"
o.cfgvalue = function(a, a) return fs.readfile(t) or "" end
o.write = function(o, o, a)
    fs.writefile("/tmp/router", a:gsub("\r\n", "\n"))
    if (luci.sys.call("cmp -s /tmp/router /usr/share/passwall/rule/router") == 1) then
        fs.writefile(t, a:gsub("\r\n", "\n"))
        luci.sys.call("rm -f /tmp/dnsmasq.d/router.conf >/dev/null")
    end
    fs.remove("/tmp/router")
end

return m
