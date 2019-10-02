-- Licensed to the public under the GNU General Public License v3.

local m, s, o
local shadowsocksr = "shadowsocksr"

local uci = luci.model.uci.cursor()
local server_count = 0
uci:foreach("shadowsocksr", "servers", function(s)
  server_count = server_count + 1
end)

m = Map(shadowsocksr)
m:section(SimpleSection).template  = "shadowsocksr/status"
-- Server Subscribe

s = m:section(TypedSection, "server_subscribe",  translate("Servers subscription and manage"))
s.anonymous = true


o = s:option(Flag, "auto_update", translate("Auto Update"))
o.rmempty = false
o.description = translate("Auto Update Server subscription, GFW list and CHN route")



o = s:option(ListValue, "edition", translate("V2 Edition"))

o:value("4.20.0", "4.20.0")
o:value("4.19.1", "4.19.1")
o:value("4.18.2", "4.18.2")
o:value("4.18.1", "4.18.1")
o:value("4.18.0", "4.18.0")
o.rmempty = false



o = s:option(ListValue, "auto_update_time", translate("Update time (every day)"))
for t = 0,23 do
o:value(t, t..":00")
end
o.default=2
o.rmempty = false

o = s:option(DynamicList, "subscribe_url", translate("Subscribe URL"))
o.rmempty = true

o = s:option(Flag, "proxy", translate("Through proxy update"))
o.rmempty = false
o.description = translate("Through proxy update list, Not Recommended ")

o = s:option(Button,"update",translate("Update"))
o.inputstyle = "reload"
o.write = function()
    luci.sys.call("/usr/share/shadowsocksr/subscribe.sh >/www/check_update.htm")
      luci.sys.call("bash /usr/share/shadowsocksr/subscribe.sh >>/tmp/ssrplus.log 2>&1")
    luci.sys.exec("sleep 2")
    luci.http.redirect(luci.dispatcher.build_url("admin", "services", "shadowsocksr", "subscription"))
end

o = s:option(DummyValue, "", "")
o.rawhtml = true
o.template = "shadowsocksr/update_subscribe"

o = s:option(Button,"update_v2ray",translate("Upgrade V2ray"))
o.inputstyle = "reload"
o.write = function()
  luci.sys.call("bash /usr/share/shadowsocksr/v2ray_update1.sh >>/tmp/ssrplus.log 2>&1")
end




o = s:option(Button,"delete",translate("Delete all severs"))
o.inputstyle = "reset"
o.description = string.format(translate("Server Count") ..  ": %d", server_count)
o.write = function()
    uci:delete_all("shadowsocksr", "servers", function(s) return true end)
    luci.sys.call("uci commit shadowsocksr && /etc/init.d/shadowsocksr stop")
    luci.http.redirect(luci.dispatcher.build_url("admin", "services", "shadowsocksr", "subscription"))
end




return m
