local e = require "nixio.fs"
local e = require "luci.sys"
-- local t = luci.sys.exec("cat /usr/share/passwall/dnsmasq.d/gfwlist.conf|grep -c ipset")

m = Map("passwall")


-- [[ Subscribe Settings ]]--
s = m:section(TypedSection, "global_subscribe", translate("Server Subscribe"))
s.anonymous = true
---- Enable auto update subscribe
o = s:option(Flag, "auto_update_subscribe",
             translate("Enable auto update subscribe"))
o.default = 0
o.rmempty = false

---- Subscribe via proxy
o = s:option(Flag, "subscribe_by_ss", translate("Subscribe via proxy"))
o.default = 0
o.rmempty = false

---- Week update rules
o = s:option(ListValue, "week_update_subscribe", translate("Week update rules"))
o:value(7, translate("Every day"))
for e = 1, 6 do o:value(e, translate("Week") .. e) end
o:value(0, translate("Week") .. translate("day"))
o.default = 0
o:depends("auto_update_subscribe", 1)

---- Day update rules
o = s:option(ListValue, "time_update_subscribe", translate("Day update rules"))
for e = 0, 23 do o:value(e, e .. translate("oclock")) end
o.default = 0
o:depends("auto_update_subscribe", 1)
-- [[ Add the server via the link ]]--
s:append(Template("passwall/server_list/link_add_server"))
---- Subscribe URL
o = s:option(DynamicList, "baseurl_ssr", translate("SSR Subscribe URL"),
             translate(
                 "Servers unsubscribed will be deleted in next update; Please summit the Subscribe URL first before manually update."))
o = s:option(DynamicList, "baseurl_v2ray", translate("V2ray Subscribe URL"),
             translate(
                 "Servers unsubscribed will be deleted in next update; Please summit the Subscribe URL first before manually update."))

---- Subscribe Manually update
o = s:option(Button, "_update", translate("Manually update"))
o.inputstyle = "apply"
function o.write(e, e)
    luci.sys
        .call("nohup /usr/share/passwall/subscription.sh > /dev/null 2>&1 &")
    luci.http.redirect(luci.dispatcher.build_url("admin", "vpn", "passwall",
                                                 "log"))
end

---- Subscribe Delete All
o = s:option(Button, "_stop", translate("Delete All Subscribe"))
o.inputstyle = "remove"
function o.write(e, e)
    luci.sys.call(
        "nohup /usr/share/passwall/subscription.sh stop > /dev/null 2>&1 &")
    luci.http.redirect(luci.dispatcher.build_url("admin", "vpn", "passwall",
                                                 "log"))
end



return m
