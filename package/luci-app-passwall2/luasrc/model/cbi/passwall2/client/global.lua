local api = require "luci.passwall2.api"
local appname = api.appname
local uci = api.uci
local datatypes = api.datatypes
local has_singbox = api.finded_com("singbox")
local has_xray = api.finded_com("xray")

m = Map(appname)
api.set_apply_on_parse(m)

local nodes_table = {}
for k, e in ipairs(api.get_valid_nodes()) do
	nodes_table[#nodes_table + 1] = e
end

local doh_validate = function(self, value, t)
	if value ~= "" then
		local flag = 0
		local util = require "luci.util"
		local val = util.split(value, ",")
		local url = val[1]
		val[1] = nil
		for i = 1, #val do
			local v = val[i]
			if v then
				if not datatypes.ipmask4(v) then
					flag = 1
				end
			end
		end
		if flag == 0 then
			return value
		end
	end
	return nil, translate("DoH request address") .. " " .. translate("Format must be:") .. " URL,IP"
end

m:append(Template(appname .. "/global/status"))

local global_cfgid = uci:get_all(appname, "@global[0]")[".name"]

s = m:section(TypedSection, "global")
s.anonymous = true
s.addremove = false

s:tab("Main", translate("Main"))

-- [[ Global Settings ]]--
o = s:taboption("Main", Flag, "enabled", translate("Main switch"))
o.rmempty = false

---- Node
node = s:taboption("Main", ListValue, "node", "<a style='color: red'>" .. translate("Node") .. "</a>")
node:value("nil", translate("Close"))

-- 分流
if (has_singbox or has_xray) and #nodes_table > 0 then
	local normal_list = {}
	local balancing_list = {}
	local shunt_list = {}
	local iface_list = {}
	for k, v in pairs(nodes_table) do
		if v.node_type == "normal" then
			normal_list[#normal_list + 1] = v
		end
		if v.protocol and v.protocol == "_balancing" then
			balancing_list[#balancing_list + 1] = v
		end
		if v.protocol and v.protocol == "_shunt" then
			shunt_list[#shunt_list + 1] = v
		end
		if v.protocol and v.protocol == "_iface" then
			iface_list[#iface_list + 1] = v
		end
	end

	local function get_cfgvalue(shunt_node_id, option)
		return function(self, section)
			return m:get(shunt_node_id, option) or "nil"
		end
	end
	local function get_write(shunt_node_id, option)
		return function(self, section, value)
			m:set(shunt_node_id, option, value)
		end
	end
	if #normal_list > 0 then
		for k, v in pairs(shunt_list) do
			local vid = v.id
			-- shunt node type, Sing-Box or Xray
			local type = s:taboption("Main", ListValue, vid .. "-type", translate("Type"))
			if has_singbox then
				type:value("sing-box", translate("Sing-Box"))
			end
			if has_xray then
				type:value("Xray", translate("Xray"))
			end
			type.cfgvalue = get_cfgvalue(v.id, "type")
			type.write = get_write(v.id, "type")
			
			-- pre-proxy
			o = s:taboption("Main", Flag, vid .. "-preproxy_enabled", translate("Preproxy"))
			o:depends("node", v.id)
			o.rmempty = false
			o.cfgvalue = get_cfgvalue(v.id, "preproxy_enabled")
			o.write = get_write(v.id, "preproxy_enabled")

			o = s:taboption("Main", Value, vid .. "-main_node", string.format('<a style="color:red">%s</a>', translate("Preproxy Node")), translate("Set the node to be used as a pre-proxy. Each rule (including <code>Default</code>) has a separate switch that controls whether this rule uses the pre-proxy or not."))
			o:depends(vid .. "-preproxy_enabled", "1")
			for k1, v1 in pairs(balancing_list) do
				o:value(v1.id, v1.remark)
			end
			for k1, v1 in pairs(iface_list) do
				o:value(v1.id, v1.remark)
			end
			for k1, v1 in pairs(normal_list) do
				o:value(v1.id, v1.remark)
			end
			if #o.keylist > 0 then
				o.default = o.keylist[1]
			end
			o.cfgvalue = get_cfgvalue(v.id, "main_node")
			o.write = get_write(v.id, "main_node")

			if (has_singbox and has_xray) or (v.type == "sing-box" and not has_singbox) or (v.type == "Xray" and not has_xray) then
				type:depends("node", v.id)
			else
				type:depends("node", "hide") --不存在的依赖，即始终隐藏
			end

			uci:foreach(appname, "shunt_rules", function(e)
				local id = e[".name"]
				local node_option = vid .. "-" .. id .. "_node"
				if id and e.remarks then
					o = s:taboption("Main", Value, node_option, string.format('* <a href="%s" target="_blank">%s</a>', api.url("shunt_rules", id), e.remarks))
					o.cfgvalue = get_cfgvalue(v.id, id)
					o.write = get_write(v.id, id)
					o:depends("node", v.id)
					o.default = "nil"
					o:value("nil", translate("Close"))
					o:value("_default", translate("Default"))
					o:value("_direct", translate("Direct Connection"))
					o:value("_blackhole", translate("Blackhole"))

					local pt = s:taboption("Main", ListValue, vid .. "-".. id .. "_proxy_tag", string.format('* <a style="color:red">%s</a>', e.remarks .. " " .. translate("Preproxy")))
					pt.cfgvalue = get_cfgvalue(v.id, id .. "_proxy_tag")
					pt.write = get_write(v.id, id .. "_proxy_tag")
					pt:value("nil", translate("Close"))
					pt:value("main", translate("Preproxy Node"))
					pt.default = "nil"
					for k1, v1 in pairs(balancing_list) do
						o:value(v1.id, v1.remark)
					end
					for k1, v1 in pairs(iface_list) do
						o:value(v1.id, v1.remark)
					end
					for k1, v1 in pairs(normal_list) do
						o:value(v1.id, v1.remark)
						pt:depends({ [node_option] = v1.id, [vid .. "-preproxy_enabled"] = "1" })
					end
				end
			end)

			local id = "default_node"
			o = s:taboption("Main", Value, vid .. "-" .. id, string.format('* <a style="color:red">%s</a>', translate("Default")))
			o.cfgvalue = get_cfgvalue(v.id, id)
			o.write = get_write(v.id, id)
			o:depends("node", v.id)
			o.default = "_direct"
			o:value("_direct", translate("Direct Connection"))
			o:value("_blackhole", translate("Blackhole"))
			for k1, v1 in pairs(balancing_list) do
				o:value(v1.id, v1.remark)
			end
			for k1, v1 in pairs(iface_list) do
				o:value(v1.id, v1.remark)
			end
			for k1, v1 in pairs(normal_list) do
				o:value(v1.id, v1.remark)
			end

			local id = "default_proxy_tag"
			o = s:taboption("Main", ListValue, vid .. "-" .. id, string.format('* <a style="color:red">%s</a>', translate("Default Preproxy")), translate("When using, localhost will connect this node first and then use this node to connect the default node."))
			o.cfgvalue = get_cfgvalue(v.id, id)
			o.write = get_write(v.id, id)
			o:value("nil", translate("Close"))
			o:value("main", translate("Preproxy Node"))
			for k1, v1 in pairs(normal_list) do
				if v1.protocol ~= "_balancing" then
					o:depends({ [vid .. "-default_node"] = v1.id, [vid .. "-preproxy_enabled"] = "1" })
				end
			end
		end
	else
		local tips = s:taboption("Main", DummyValue, "tips", " ")
		tips.rawhtml = true
		tips.cfgvalue = function(t, n)
			return string.format('<a style="color: red">%s</a>', translate("There are no available nodes, please add or subscribe nodes first."))
		end
		tips:depends({ node = "nil", ["!reverse"] = true })
		for k, v in pairs(shunt_list) do
			tips:depends("node", v.id)
		end
		for k, v in pairs(balancing_list) do
			tips:depends("node", v.id)
		end
	end
end

o = s:taboption("Main", Flag, "localhost_proxy", translate("Localhost Proxy"), translate("When selected, localhost can transparent proxy."))
o.default = "1"
o.rmempty = false

node_socks_port = s:taboption("Main", Value, "node_socks_port", translate("Node") .. " Socks " .. translate("Listen Port"))
node_socks_port.default = 1070
node_socks_port.datatype = "port"

s:tab("DNS", translate("DNS"))

o = s:taboption("DNS", ListValue, "remote_dns_protocol", translate("Remote DNS Protocol"))
o:value("tcp", "TCP")
o:value("doh", "DoH")
o:value("udp", "UDP")

---- DNS Forward
o = s:taboption("DNS", Value, "remote_dns", translate("Remote DNS"))
o.datatype = "or(ipaddr,ipaddrport)"
o.default = "1.1.1.1"
o:value("1.1.1.1", "1.1.1.1 (CloudFlare)")
o:value("1.1.1.2", "1.1.1.2 (CloudFlare-Security)")
o:value("8.8.4.4", "8.8.4.4 (Google)")
o:value("8.8.8.8", "8.8.8.8 (Google)")
o:value("9.9.9.9", "9.9.9.9 (Quad9-Recommended)")
o:value("208.67.220.220", "208.67.220.220 (OpenDNS)")
o:value("208.67.222.222", "208.67.222.222 (OpenDNS)")
o:depends("remote_dns_protocol", "tcp")
o:depends("remote_dns_protocol", "udp")

---- DoH
o = s:taboption("DNS", Value, "remote_dns_doh", translate("Remote DNS DoH"))
o.default = "https://1.1.1.1/dns-query"
o:value("https://1.1.1.1/dns-query", "CloudFlare")
o:value("https://1.1.1.2/dns-query", "CloudFlare-Security")
o:value("https://8.8.4.4/dns-query", "Google 8844")
o:value("https://8.8.8.8/dns-query", "Google 8888")
o:value("https://9.9.9.9/dns-query", "Quad9-Recommended")
o:value("https://208.67.222.222/dns-query", "OpenDNS")
o:value("https://dns.adguard.com/dns-query,176.103.130.130", "AdGuard")
o:value("https://doh.libredns.gr/dns-query,116.202.176.26", "LibreDNS")
o:value("https://doh.libredns.gr/ads,116.202.176.26", "LibreDNS (No Ads)")
o.validate = doh_validate
o:depends("remote_dns_protocol", "doh")

o = s:taboption("DNS", Value, "remote_dns_client_ip", translate("Remote DNS EDNS Client Subnet"))
o.description = translate("Notify the DNS server when the DNS query is notified, the location of the client (cannot be a private IP address).") .. "<br />" ..
				translate("This feature requires the DNS server to support the Edns Client Subnet (RFC7871).")
o.datatype = "ipaddr"
o:depends({ __hide = true })

o = s:taboption("DNS", ListValue, "remote_dns_detour", translate("Remote DNS Outbound"))
o.default = "remote"
o:value("remote", translate("Remote"))
o:value("direct", translate("Direct"))

o = s:taboption("DNS", Flag, "remote_fakedns", "FakeDNS", translate("Use FakeDNS work in the shunt domain that proxy."))
o.default = "0"
o.rmempty = false

o = s:taboption("DNS", ListValue, "remote_dns_query_strategy", translate("Remote Query Strategy"))
o.default = "UseIPv4"
o:value("UseIP")
o:value("UseIPv4")
o:value("UseIPv6")

o = s:taboption("DNS", TextValue, "dns_hosts", translate("Domain Override"))
o.rows = 5
o.wrap = "off"
o:depends({ __hide = true })
o.remove = function(self, section)
	local node_value = node:formvalue(global_cfgid)
	if node_value ~= "nil" then
		local node_t = m:get(node_value) or {}
		if node_t.type == "Xray" then
			AbstractValue.remove(self, section)
		end
	end
end

o = s:taboption("DNS", Button, "clear_ipset", translate("Clear IPSET"), translate("Try this feature if the rule modification does not take effect."))
o.inputstyle = "remove"
function o.write(e, e)
	luci.sys.call("[ -n \"$(nft list sets 2>/dev/null | grep \"passwall2_\")\" ] && sh /usr/share/" .. appname .. "/nftables.sh flush_nftset || sh /usr/share/" .. appname .. "/iptables.sh flush_ipset > /dev/null 2>&1 &")
	luci.http.redirect(api.url("log"))
end

for k, v in pairs(nodes_table) do
	if v.type == "Xray" then
		s.fields["remote_dns_client_ip"]:depends({ node = v.id, remote_dns_protocol = "tcp" })
		s.fields["remote_dns_client_ip"]:depends({ node = v.id, remote_dns_protocol = "doh" })
		s.fields["dns_hosts"]:depends({ node = v.id })
	end
end

s:tab("log", translate("Log"))
o = s:taboption("log", Flag, "close_log", translate("Close Node Log"))
o.rmempty = false

loglevel = s:taboption("log", ListValue, "loglevel", translate("Log Level"))
loglevel.default = "warning"
loglevel:value("debug")
loglevel:value("info")
loglevel:value("warning")
loglevel:value("error")

s:tab("faq", "FAQ")

o = s:taboption("faq", DummyValue, "")
o.template = appname .. "/global/faq"

-- [[ Socks Server ]]--
o = s:taboption("Main", Flag, "socks_enabled", "Socks " .. translate("Main switch"))
o.rmempty = false

s = m:section(TypedSection, "socks", translate("Socks Config"))
s.template = "cbi/tblsection"
s.anonymous = true
s.addremove = true
s.extedit = api.url("socks_config", "%s")
function s.create(e, t)
	local uuid = api.gen_short_uuid()
	t = uuid
	TypedSection.create(e, t)
	luci.http.redirect(e.extedit:format(t))
end

o = s:option(DummyValue, "status", translate("Status"))
o.rawhtml = true
o.cfgvalue = function(t, n)
	return string.format('<div class="_status" socks_id="%s"></div>', n)
end

---- Enable
o = s:option(Flag, "enabled", translate("Enable"))
o.default = 1
o.rmempty = false

socks_node = s:option(ListValue, "node", translate("Socks Node"))

local n = 1
uci:foreach(appname, "socks", function(s)
	if s[".name"] == section then
		return false
	end
	n = n + 1
end)

o = s:option(Value, "port", "Socks " .. translate("Listen Port"))
o.default = n + 1080
o.datatype = "port"
o.rmempty = false

if has_singbox or has_xray then
	o = s:option(Value, "http_port", "HTTP " .. translate("Listen Port") .. " " .. translate("0 is not use"))
	o.default = 0
	o.datatype = "port"
end

for k, v in pairs(nodes_table) do
	node:value(v.id, v["remark"])
	if v.type == "Socks" then
		if has_singbox or has_xray then
			socks_node:value(v.id, v["remark"])
		end
	else
		socks_node:value(v.id, v["remark"])
	end
end

m:append(Template(appname .. "/global/footer"))

return m
