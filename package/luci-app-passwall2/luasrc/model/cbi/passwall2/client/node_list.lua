local api = require "luci.passwall2.api"
local appname = api.appname
local sys = api.sys
local datatypes = api.datatypes

m = Map(appname)
api.set_apply_on_parse(m)

-- [[ Other Settings ]]--
s = m:section(TypedSection, "global_other")
s.anonymous = true

o = s:option(ListValue, "auto_detection_time", translate("Automatic detection delay"))
o:value("0", translate("Close"))
o:value("icmp", "Ping")
o:value("tcping", "TCP Ping")

o = s:option(Flag, "show_node_info", translate("Show server address and port"))
o.default = "0"

-- [[ Add the node via the link ]]--
s:append(Template(appname .. "/node_list/link_add_node"))

local auto_detection_time = m:get("@global_other[0]", "auto_detection_time") or "0"
local show_node_info = m:get("@global_other[0]", "show_node_info") or "0"

-- [[ Node List ]]--
s = m:section(TypedSection, "nodes")
s.anonymous = true
s.addremove = true
s.template = "cbi/tblsection"
s.extedit = api.url("node_config", "%s")
function s.create(e, t)
	local uuid = api.gen_short_uuid()
	t = uuid
	TypedSection.create(e, t)
	luci.http.redirect(e.extedit:format(t))
end

function s.remove(e, t)
	m.uci:foreach(appname, "socks", function(s)
		if s["node"] == t then
			m:del(s[".name"])
		end
		for k, v in ipairs(m:get(s[".name"], "autoswitch_backup_node") or {}) do
			if v and v == t then
				sys.call(string.format("uci -q del_list %s.%s.autoswitch_backup_node='%s'", appname, s[".name"], v))
			end
		end
	end)
	m.uci:foreach(appname, "acl_rule", function(s)
		if s["node"] and s["node"] == t then
			m:set(s[".name"], "node", "default")
		end
	end)
	TypedSection.remove(e, t)
	local new_node = "nil"
	local node0 = m:get("@nodes[0]") or nil
	if node0 then
		new_node = node0[".name"]
	end
	if (m:get("@global[0]", "node") or "nil") == t then
		m:set('@global[0]', "node", new_node)
	end
end

s.sortable = true
-- 简洁模式
o = s:option(DummyValue, "add_from", "")
o.cfgvalue = function(t, n)
	local v = Value.cfgvalue(t, n)
	if v and v ~= '' then
		local group = m:get(n, "group") or ""
		if group ~= "" then
			v = v .. " " .. group
		end
		return v
	else
		return ''
	end
end
o = s:option(DummyValue, "remarks", translate("Remarks"))
o.rawhtml = true
o.cfgvalue = function(t, n)
	local str = ""
	local is_sub = m:get(n, "is_sub") or ""
	local group = m:get(n, "group") or ""
	local remarks = m:get(n, "remarks") or ""
	local type = m:get(n, "type") or ""
	str = str .. string.format("<input type='hidden' id='cbid.%s.%s.type' value='%s'/>", appname, n, type)
	if type == "sing-box" or type == "Xray" then
		local protocol = m:get(n, "protocol")
		if protocol == "_balancing" then
			protocol = translate("Balancing")
		elseif protocol == "_shunt" then
			protocol = translate("Shunt")
		elseif protocol == "vmess" then
			protocol = "VMess"
		elseif protocol == "vless" then
			protocol = "VLESS"
		else
			protocol = protocol:gsub("^%l",string.upper)
		end
		type = type .. " " .. protocol
	end
	local address = m:get(n, "address") or ""
	local port = m:get(n, "port") or ""
	str = str .. translate(type) .. "：" .. remarks
	if address ~= "" and port ~= "" then
		if show_node_info == "1" then
			if datatypes.ip6addr(address) then
				str = str .. string.format("（[%s]:%s）", address, port)
			else
				str = str .. string.format("（%s:%s）", address, port)
			end
		end
		str = str .. string.format("<input type='hidden' id='cbid.%s.%s.address' value='%s'/>", appname, n, address)
		str = str .. string.format("<input type='hidden' id='cbid.%s.%s.port' value='%s'/>", appname, n, port)
	end
	return str
end

---- Ping
o = s:option(DummyValue, "ping", "Ping")
o.width = "8%"
o.rawhtml = true
o.cfgvalue = function(t, n)
	local result = "---"
	if auto_detection_time ~= "icmp" then
		result = string.format('<span class="ping"><a href="javascript:void(0)" onclick="javascript:ping_node(\'%s\', this, \'icmp\')">%s</a></span>', n, translate("Test"))
	else
		result = string.format('<span class="ping_value" cbiid="%s">---</span>', n)
	end
	return result
end

---- TCP Ping
o = s:option(DummyValue, "tcping", "TCPing")
o.width = "8%"
o.rawhtml = true
o.cfgvalue = function(t, n)
	local result = "---"
	if auto_detection_time ~= "tcping" then
		result = string.format('<span class="ping"><a href="javascript:void(0)" onclick="javascript:ping_node(\'%s\', this, \'tcping\')">%s</a></span>', n, translate("Test"))
	else
		result = string.format('<span class="tcping_value" cbiid="%s">---</span>', n)
	end
	return result
end

o = s:option(DummyValue, "_url_test", translate("URL Test"))
o.width = "8%"
o.rawhtml = true
o.cfgvalue = function(t, n)
	return string.format('<span class="ping"><a href="javascript:void(0)" onclick="javascript:urltest_node(\'%s\', this)">%s</a></span>', n, translate("Test"))
end

m:append(Template(appname .. "/node_list/node_list"))

return m
