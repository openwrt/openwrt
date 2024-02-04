local api = require "luci.passwall2.api"
local appname = api.appname
local has_ss = api.is_finded("ss-redir")
local has_ss_rust = api.is_finded("sslocal")
local has_singbox = api.finded_com("singbox")
local has_xray = api.finded_com("xray")
local has_hysteria2 = api.finded_com("hysteria")
local ss_aead_type = {}
local trojan_type = {}
local vmess_type = {}
local vless_type = {}
local hysteria2_type = {}
if has_ss then
	local s = "shadowsocks-libev"
	table.insert(ss_aead_type, s)
end
if has_ss_rust then
	local s = "shadowsocks-rust"
	table.insert(ss_aead_type, s)
end
if has_singbox then
	local s = "sing-box"
	table.insert(trojan_type, s)
	table.insert(ss_aead_type, s)
	table.insert(vmess_type, s)
	table.insert(vless_type, s)
	table.insert(hysteria2_type, s)
end
if has_xray then
	local s = "xray"
	table.insert(trojan_type, s)
	table.insert(ss_aead_type, s)
	table.insert(vmess_type, s)
	table.insert(vless_type, s)
end
if has_hysteria2 then
	local s = "hysteria2"
	table.insert(hysteria2_type, s)
end

m = Map(appname)
m.redirect = api.url("node_subscribe")
api.set_apply_on_parse(m)

s = m:section(NamedSection, arg[1])
s.addremove = false
s.dynamic = false

o = s:option(Value, "remark", translate("Subscribe Remark"))
o.rmempty = false

o = s:option(TextValue, "url", translate("Subscribe URL"))
o.rows = 5
o.rmempty = false

o = s:option(Flag, "allowInsecure", translate("allowInsecure"), translate("Whether unsafe connections are allowed. When checked, Certificate validation will be skipped."))
o.default = "1"
o.rmempty = false

o = s:option(ListValue, "filter_keyword_mode", translate("Filter keyword Mode"))
o.default = "5"
o:value("0", translate("Close"))
o:value("1", translate("Discard List"))
o:value("2", translate("Keep List"))
o:value("3", translate("Discard List,But Keep List First"))
o:value("4", translate("Keep List,But Discard List First"))
o:value("5", translate("Use global config"))

o = s:option(DynamicList, "filter_discard_list", translate("Discard List"))
o:depends("filter_keyword_mode", "1")
o:depends("filter_keyword_mode", "3")
o:depends("filter_keyword_mode", "4")

o = s:option(DynamicList, "filter_keep_list", translate("Keep List"))
o:depends("filter_keyword_mode", "2")
o:depends("filter_keyword_mode", "3")
o:depends("filter_keyword_mode", "4")

if #ss_aead_type > 0 then
	o = s:option(ListValue, "ss_aead_type", translatef("%s Node Use Type", "SS AEAD"))
	o.default = "global"
	o:value("global", translate("Use global config"))
	for key, value in pairs(ss_aead_type) do
		o:value(value)
	end
end

if #trojan_type > 0 then
	o = s:option(ListValue, "trojan_type", translatef("%s Node Use Type", "Trojan"))
	o.default = "global"
	o:value("global", translate("Use global config"))
	for key, value in pairs(trojan_type) do
		o:value(value)
	end
end

if #vmess_type > 0 then
	o = s:option(ListValue, "vmess_type", translatef("%s Node Use Type", "VMess"))
	o.default = "global"
	o:value("global", translate("Use global config"))
	for key, value in pairs(vmess_type) do
		o:value(value)
	end
end

if #vless_type > 0 then
	o = s:option(ListValue, "vless_type", translatef("%s Node Use Type", "VLESS"))
	o.default = "global"
	o:value("global", translate("Use global config"))
	for key, value in pairs(vless_type) do
		o:value(value)
	end
end

if #hysteria2_type > 0 then
	o = s:option(ListValue, "hysteria2_type", translatef("%s Node Use Type", "Hysteria2"))
	o.default = "global"
	o:value("global", translate("Use global config"))
	for key, value in pairs(hysteria2_type) do
		o:value(value)
	end
end

---- Enable auto update subscribe
o = s:option(Flag, "auto_update", translate("Enable auto update subscribe"))
o.default = 0
o.rmempty = false

---- Week update rules
o = s:option(ListValue, "week_update", translate("Week update rules"))
o:value(7, translate("Every day"))
for e = 1, 6 do o:value(e, translate("Week") .. e) end
o:value(0, translate("Week") .. translate("day"))
o.default = 0
o:depends("auto_update", true)

---- Day update rules
o = s:option(ListValue, "time_update", translate("Day update rules"))
for e = 0, 23 do o:value(e, e .. translate("oclock")) end
o.default = 0
o:depends("auto_update", true)

o = s:option(Value, "user_agent", translate("User-Agent"))
o.default = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.122 Safari/537.36"

return m
