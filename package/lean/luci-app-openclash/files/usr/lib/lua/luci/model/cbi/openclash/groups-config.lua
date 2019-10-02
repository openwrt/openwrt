
local m, s, o
local openclash = "openclash"
local uci = luci.model.uci.cursor()
local fs = require "nixio.fs"
local sys = require "luci.sys"
local sid = arg[1]


m = Map(openclash, translate("Edit Group"))
m.pageaction = false
m.redirect = luci.dispatcher.build_url("admin/services/openclash/servers")
if m.uci:get(openclash, sid) ~= "groups" then
	luci.http.redirect(m.redirect)
	return
end

-- [[ Groups Setting ]]--
s = m:section(NamedSection, sid, "groups")
s.anonymous = true
s.addremove   = false

o = s:option(ListValue, "type", translate("Group Type"))
o.rmempty = true
o.description = translate("Choose The Operation Mode")
o:value("select", translate("Select"))
o:value("url-test", translate("URL-Test"))
o:value("fallback", translate("Fallback"))
o:value("load-balance", translate("Load-Balance"))

o = s:option(Value, "name", translate("Group Name"))
o.rmempty = false

o = s:option(Value, "test_url", translate("Test URL"))
o.default = "http://www.gstatic.com/generate_204"
o.rmempty = false
o:depends("type", "url-test")
o:depends("type", "fallback")
o:depends("type", "load-balance")

o = s:option(Value, "test_interval", translate("Test Interval(s)"))
o.default = "300"
o.rmempty = false
o:depends("type", "url-test")
o:depends("type", "fallback")
o:depends("type", "load-balance")

o = s:option(DynamicList, "other_group", translate("Other Group"))
o.description = translate("The added Proxy Groups Must Exist")
uci:foreach("openclash", "groups",
		function(s)
		  if s.name ~= "" and s.name ~= nil and s.name ~= m.uci:get(openclash, sid, "name") then
			   o:value(s.name)
			end
		end)
o:value("DIRECT")
o:value("REJECT")
o:depends("type", "select")
o.rmempty = true

local t = {
    {Commit, Back}
}
a = m:section(Table, t)

o = a:option(Button,"Commit")
o.inputtitle = translate("Commit Configurations")
o.inputstyle = "apply"
o.write = function()
   m.uci:commit(openclash)
   sys.call("/usr/share/openclash/yml_groups_name_ch.sh start")
   luci.http.redirect(luci.dispatcher.build_url("admin", "services", "openclash", "servers"))
end

o = a:option(Button,"Back")
o.inputtitle = translate("Back Configurations")
o.inputstyle = "reset"
o.write = function()
   m.uci:revert(openclash)
   luci.http.redirect(luci.dispatcher.build_url("admin", "services", "openclash", "servers"))
end

return m
