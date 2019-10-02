local e = require "nixio.fs"
local e = require "luci.sys"
-- local t = luci.sys.exec("cat /usr/share/passwall/dnsmasq.d/gfwlist.conf|grep -c ipset")

m = Map("passwall")
-- [[ Rule Settings ]]--
s = m:section(TypedSection, "global_rules", translate("Rule status"))
s.anonymous = true
s:append(Template("passwall/rule/rule_version"))

---- Auto Update
o = s:option(Flag, "auto_update", translate("Enable auto update rules"))
o.default = 0
o.rmempty = false

---- Week Update
o = s:option(ListValue, "week_update", translate("Week update rules"))
o:value(7, translate("Every day"))
for e = 1, 6 do o:value(e, translate("Week") .. e) end
o:value(0, translate("Week") .. translate("day"))
o.default = 0
o:depends("auto_update", 1)

---- Time Update
o = s:option(ListValue, "time_update", translate("Day update rules"))
for e = 0, 23 do o:value(e, e .. translate("oclock")) end
o.default = 0
o:depends("auto_update", 1)

-- [[ V2ray Settings ]]--
s = m:section(TypedSection, "global_app", translate("App Update"),
              translate("Please confirm that your firmware supports FPU."))
s.anonymous = true




---- V2ray client path
s:append(Template("passwall/rule/v2ray_version"))
o = s:option(Value, "v2ray_client_file", translate("V2ray client path"),
             translate(
                 "if you want to run from memory, change the path, such as /tmp/v2ray/, Then save the application and update it manually."))
o.default = "/usr/bin/v2ray/"
o.rmempty = false

---- Kcptun client path
s:append(Template("passwall/rule/kcptun_version"))
o = s:option(Value, "kcptun_client_file", translate("Kcptun client path"),
             translate(
                 "if you want to run from memory, change the path, such as /tmp/kcptun-client, Then save the application and update it manually."))
o.default = "/usr/bin/kcptun-client"
o.rmempty = false

--[[
o = s:option(Button,  "_check_kcptun",  translate("Manually update"), translate("Make sure there is enough space to install Kcptun"))
o.template = "passwall/kcptun"
o.inputstyle = "apply"
o.btnclick = "onBtnClick_kcptun(this);"
o.id = "_kcptun-check_btn"]] --

---- Brook client path
s:append(Template("passwall/rule/brook_version"))
o = s:option(Value, "brook_client_file", translate("Brook client path"),
             translate(
                 "if you want to run from memory, change the path, such as /tmp/brook, Then save the application and update it manually."))
o.default = "/usr/bin/brook"
o.rmempty = false


return m
