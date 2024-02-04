local api = require "luci.passwall2.api"
local appname = api.appname
local uci = api.uci
local fs = require "nixio.fs"
local types_dir = "/usr/lib/lua/luci/model/cbi/passwall2/client/type/"

if not arg[1] or not uci:get(appname, arg[1]) then
	luci.http.redirect(api.url("node_list"))
end

m = Map(appname, translate("Node Config"))
m.redirect = api.url()
api.set_apply_on_parse(m)

s = m:section(NamedSection, arg[1], "nodes", "")
s.addremove = false
s.dynamic = false

o = s:option(DummyValue, "passwall2", " ")
o.rawhtml  = true
o.template = "passwall2/node_list/link_share_man"
o.value = arg[1]

o = s:option(Value, "remarks", translate("Node Remarks"))
o.default = translate("Remarks")
o.rmempty = false

o = s:option(ListValue, "type", translate("Type"))

local type_table = {}
for filename in fs.dir(types_dir) do
	table.insert(type_table, filename)
end
table.sort(type_table)

for index, value in ipairs(type_table) do
	local p_func = loadfile(types_dir .. value)
	setfenv(p_func, getfenv(1))(m, s)
end

return m
