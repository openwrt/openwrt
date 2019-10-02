-- Licensed to the public under the GNU General Public License v3.

local m, s, o
local openclash = "openclash"
local uci = luci.model.uci.cursor()


m = Map(openclash,  translate("Servers manage and Config create"))
m.pageaction = false

s = m:section(TypedSection, "openclash")
s.anonymous = true

o = s:option(Flag, "create_config", translate("Create Config"))
o.description = translate("Create Config By One-Click Only Need Proxys")
o.default = 0

o = s:option(ListValue, "rule_sources", translate("Choose Template For Create Config"))
o.description = translate("Use Other Rules To Create Config")
o:depends("create_config", 1)
o:value("lhie1", translate("lhie1 Rules"))
o:value("ConnersHua", translate("ConnersHua Rules"))
o:value("ConnersHua_return", translate("ConnersHua Return Rules"))

local t = {
    {Commit, Apply}
}

a = m:section(Table, t)

o = a:option(Button, "Commit") 
o.inputtitle = translate("Commit Configurations")
o.inputstyle = "apply"
o.write = function()
  m.uci:set("openclash", "config", "enable", 0)
  m.uci:commit("openclash")
end

o = a:option(Button, "Apply")
o.inputtitle = translate("Apply Configurations")
o.inputstyle = "apply"
o.write = function()
  m.uci:set("openclash", "config", "enable", 0)
  m.uci:commit("openclash")
  luci.sys.call("/usr/share/openclash/yml_proxys_set.sh >/dev/null 2>&1 &")
  luci.http.redirect(luci.dispatcher.build_url("admin", "services", "openclash"))
end

local tt = {
    {Load_Config, Delete_Severs, Delete_Groups}
}
b = m:section(Table, tt)

o = b:option(Button,"Load_Config")
o.inputtitle = translate("Load Config")
o.inputstyle = "apply"
o.write = function()
  m.uci:set("openclash", "config", "enable", 0)
  m.uci:commit("openclash")
  luci.sys.call("sh /usr/share/openclash/yml_groups_get.sh 2>/dev/null &")
  luci.http.redirect(luci.dispatcher.build_url("admin", "services", "openclash"))
end

o = b:option(Button,"Delete_Severs")
o.inputtitle = translate("Delete Severs")
o.inputstyle = "reset"
o.write = function()
  m.uci:set("openclash", "config", "enable", 0)
  m.uci:delete_all("openclash", "servers", function(s) return true end)
  m.uci:commit("openclash")
  luci.http.redirect(luci.dispatcher.build_url("admin", "services", "openclash", "servers"))
end

o = b:option(Button,"Delete_Groups")
o.inputtitle = translate("Delete Groups")
o.inputstyle = "reset"
o.write = function()
  m.uci:set("openclash", "config", "enable", 0)
  m.uci:delete_all("openclash", "groups", function(s) return true end)
  m.uci:commit("openclash")
  luci.http.redirect(luci.dispatcher.build_url("admin", "services", "openclash", "servers"))
end

-- [[ Groups Manage ]]--
s = m:section(TypedSection, "groups", translate("Proxy Groups(No Need Set when Config Create)"))
s.anonymous = true
s.addremove = true
s.sortable = true
s.template = "cbi/tblsection"
s.extedit = luci.dispatcher.build_url("admin/services/openclash/groups-config/%s")
function s.create(...)
	local sid = TypedSection.create(...)
	if sid then
		luci.http.redirect(s.extedit % sid)
		return
	end
end

o = s:option(DummyValue, "type", translate("Group Type"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("None")
end

o = s:option(DummyValue, "name", translate("Group Name"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("None")
end

-- [[ Servers Manage ]]--
s = m:section(TypedSection, "servers", translate("Proxys"))
s.anonymous = true
s.addremove = true
s.sortable = true
s.template = "cbi/tblsection"
s.extedit = luci.dispatcher.build_url("admin/services/openclash/servers-config/%s")
function s.create(...)
	local sid = TypedSection.create(...)
	if sid then
		luci.http.redirect(s.extedit % sid)
		return
	end
end

o = s:option(DummyValue, "type", translate("Type"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("None")
end

o = s:option(DummyValue, "name", translate("Server Alias"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("None")
end

o = s:option(DummyValue, "server", translate("Server Address"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("None")
end

o = s:option(DummyValue, "port", translate("Server Port"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("None")
end

return m
