-- Copyright 2019 Shun Li <riverscn@gmail.com>
-- Licensed to the public under the GNU General Public License v3.

local sys = require "luci.sys"

m = Map("iptvhelper", translate("IPTV Helper"), translate("Help you configure IPTV easily. <a href=\"https://github.com/riverscn/openwrt-iptvhelper\">Github</a>"))

s = m:section(TypedSection, "tvbox", translate("IPTV topbox parameters"))
s.addremove = true
s.anonymous = false

enable=s:option(Flag, "disabled", translate("Enabled"))
enable.enabled = "0"
enable.disabled = "1"
enable.default = "1"
enable.rmempty = false
respawn=s:option(Flag, "respawn", translate("Respawn"))
respawn.default = false
ipset=s:option(Flag, "ipset", translate("Create ipset"),
    translate("You can use it in mwan3."))
ipset.default = true
dns_redir=s:option(Flag, "dns_redir", translate("Redirect topbox's DNS"),
    translate("You may need it to jailbreak your topbox."))
dns_redir.default = false

host = s:option(Value, "mac", translate("Topbox MAC Address"),
	translate("It is usually on the bottom side of topbox."))
host.rmempty = false
host.datatype = "macaddr"
sys.net.mac_hints(function(mac, name)
	host:value(mac, "%s (%s)" %{ mac, name })
end)

return m