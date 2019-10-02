local fs = require "nixio.fs"

m = Map("nfs", translate("NFS"))

-- NFS Share --
s = m:section(TypedSection, "share", translate("Shared Directories"))
s.anonymous = true
s.addremove = true
s.template = "cbi/tblsection"

en = s:option(Flag, "enabled", translate("Enable"))
en.rmempty = false
en.default = "1"

ph1 = s:option(Value, "path", translate("Path"))
ph1.placeholder = "/mnt/sda1"
ph1.rmempty = false
ph1.optional = false

ct = s:option(Value, "clients", translate("Clients"))
ct.placeholder = "192.168.1.0/24"
ct.rmempty = false
ct.optional = false

am = s:option(ListValue, "permission", translate("Permission"))
am:value("0", translate("anonymous"))
am:value("1", translate("root"))
am:value("2", translate("full"))
am.default = "0"
am.rmempty = false

ro = s:option(Flag, "read_only", translate("Read-only"))
ro.rmempty = false
ro.default = "0"

is = s:option(Flag, "insecure", translate("Insecure"))
is.default = "1"
is.rmempty = false

g = m:section(TypedSection, "general")
g.anonymous = true

en=g:option(Flag, "external_access", translate(""), translate("Allow external network access"))
en.rmempty = false
en.default = "0"

-- NFS Mount --
c = m:section(TypedSection, "mount", translate("Mounted Points"))
c.anonymous = true
c.addremove = true
c.template = "cbi/tblsection"

en = c:option(Flag, "enabled", translate("Enable"))
en.default = "1"
en.rmempty = false

ho = c:option(Value, "host", translate("Host"))
ho.placeholder = "192.168.1.1"
ho.rmempty = false
ho.optional = false

ph2 = c:option(Value, "path", translate("Path"))
ph2.placeholder = "/media/share"
ph2.rmempty = false
ph2.optional = false

mo = c:option(Value, "mount_on", translate("Mount On"))
mo.placeholder = "/mnt/nfs"

ro = c:option(Flag, "read_only", translate("Read-only"))
ro.rmempty = false
ro.default = "0"

if nixio.fs.access("/etc/config/fstab") then
		ph1.titleref = luci.dispatcher.build_url("admin", "system", "fstab")
		mo.titleref = luci.dispatcher.build_url("admin", "system", "fstab")
end

return m
