
local m, s, o
local openclash = "openclash"
local uci = luci.model.uci.cursor()
local sys = require "luci.sys"
local sid = arg[1]
local fs = require "luci.openclash"

font_red = [[<b style=color:red>]]
font_off = [[</b>]]
bold_on  = [[<strong>]]
bold_off = [[</strong>]]

function IsYamlFile(e)
   e=e or""
   local e=string.lower(string.sub(e,-5,-1))
   return e == ".yaml"
end
function IsYmlFile(e)
   e=e or""
   local e=string.lower(string.sub(e,-4,-1))
   return e == ".yml"
end

m = Map(openclash, translate("Edit Proxy-Provider"))
m.pageaction = false
m.redirect = luci.dispatcher.build_url("admin/services/openclash/servers")
if m.uci:get(openclash, sid) ~= "proxy-provider" then
	luci.http.redirect(m.redirect)
	return
end

-- [[ Provider Setting ]]--
s = m:section(NamedSection, sid, "proxy-provider")
s.anonymous = true
s.addremove   = false

o = s:option(ListValue, "config", translate("Config File"))
o:value("all", translate("Use For All Config File"))
local e,a={}
for t,f in ipairs(fs.glob("/etc/openclash/config/*"))do
	a=fs.stat(f)
	if a then
    e[t]={}
    e[t].name=fs.basename(f)
    if IsYamlFile(e[t].name) or IsYmlFile(e[t].name) then
       o:value(e[t].name)
    end
  end
end

o = s:option(Flag, "manual", translate("Custom Tag"))
o.rmempty = false
o.default = "0"
o.description = translate("Mark as Custom Node to Prevent Retention config from being Deleted When Enabled")

o = s:option(ListValue, "type", translate("Provider Type"))
o.rmempty = true
o.description = translate("Choose The Provider Type")
o:value("http")
o:value("file")
o:value("inline")

o = s:option(Value, "name", translate("Provider Name"))
o.rmempty = false
o.default = "Proxy-provider - "..sid

o = s:option(ListValue, "path", translate("Provider Path"))
o.description = translate("Update Your Proxy Provider File From Config Luci Page")
local p,h={}
for t,f in ipairs(fs.glob("/etc/openclash/proxy_provider/*"))do
	h=fs.stat(f)
	if h then
    p[t]={}
    p[t].name=fs.basename(f)
    if IsYamlFile(p[t].name) or IsYmlFile(p[t].name) then
       o:value("./proxy_provider/"..p[t].name)
    end
  end
end
o.rmempty = false
o:depends("type", "file")

o = s:option(Value, "provider_url", translate("Provider URL"))
o.rmempty = false
o:depends("type", "http")

o = s:option(Value, "provider_filter", translate("Provider Filter"))
o.rmempty = true
o.placeholder = "bgp|sg"

o = s:option(Value, "provider_interval", translate("Provider Interval(s)"))
o.default = "3600"
o.rmempty = false
o:depends("type", "http")

o = s:option(ListValue, "health_check", translate("Provider Health Check"))
o:value("false", translate("Disable"))
o:value("true", translate("Enable"))
o.default = true

o = s:option(Value, "health_check_url", translate("Health Check URL"))
o:value("http://cp.cloudflare.com/generate_204")
o:value("http://www.gstatic.com/generate_204")
o:value("https://cp.cloudflare.com/generate_204")
o.rmempty = false

o = s:option(Value, "health_check_interval", translate("Health Check Interval(s)"))
o.default = "300"
o.rmempty = false

-- [[ other-setting ]]--
o = s:option(Value, "other_parameters", translate("Other Parameters"))
o.template = "cbi/tvalue"
o.rows = 20
o.wrap = "off"
o.description = font_red..bold_on..translate("Edit Your Other Parameters Here")..bold_off..font_off
o.rmempty = true
function o.cfgvalue(self, section)
	if self.map:get(section, "other_parameters") == nil then
		return "# Example:\n"..
		"# Only support YAML, four spaces need to be reserved at the beginning of each line to maintain formatting alignment\n"..
		"# 示例：\n"..
		"# 仅支持 YAML, 每行行首需要多保留四个空格以使脚本处理后能够与上方配置保持格式对齐\n"..
		"#    header:\n"..
		"#      User-Agent:\n"..
		"#      - \"Clash/v1.18.0\"\n"..
		"#      - \"mihomo/1.18.3\"\n"..
		"#      Authorization:\n"..
		"#      - \"token 1231231\"\n"..
		"#    override:\n"..
		"#      skip-cert-verify: true\n"..
		"#      additional-prefix: \"provider1 prefix |\"\n"..
		"#      additional-suffix: \"| provider1 suffix\"\n"..
		"#      proxy-name:\n"..
		"#      - pattern: \"IPLC-(.*?)倍\"\n"..
		"#        target: \"iplc x $1\"\n"..
		"#    exclude-type: \"ss|http\"\n"..
		"\n"..
		"# inline Example:\n"..
		"#    payload:\n"..
		"#      - name: \"ss1\"\n"..
		"#        type: ss\n"..
		"#        server: server\n"..
		"#        port: 443\n"..
		"#        cipher: chacha20-ietf-poly1305\n"..
		"#        password: \"password\""
	else
		return Value.cfgvalue(self, section)
	end
end
function o.validate(self, value)
	if value then
		value = value:gsub("\r\n?", "\n")
		value = value:gsub("%c*$", "")
	end
	return value
end

o = s:option(DynamicList, "groups", translate("Proxy Group (Support Regex)"))
o.description = font_red..bold_on..translate("No Need Set when Config Create, The added Proxy Groups Must Exist")..bold_off..font_off
o.rmempty = true
o:value("all", translate("All Groups"))
m.uci:foreach("openclash", "groups",
		function(s)
			if s.name ~= "" and s.name ~= nil then
			   o:value(s.name)
			end
		end)

local t = {
    {Commit, Back}
}
a = m:section(Table, t)

o = a:option(Button,"Commit", " ")
o.inputtitle = translate("Commit Settings")
o.inputstyle = "apply"
o.write = function()
   m.uci:commit(openclash)
   luci.http.redirect(m.redirect)
end

o = a:option(Button,"Back", " ")
o.inputtitle = translate("Back Settings")
o.inputstyle = "reset"
o.write = function()
   m.uci:revert(openclash, sid)
   luci.http.redirect(m.redirect)
end

m:append(Template("openclash/toolbar_show"))
m:append(Template("openclash/config_editor"))
return m
