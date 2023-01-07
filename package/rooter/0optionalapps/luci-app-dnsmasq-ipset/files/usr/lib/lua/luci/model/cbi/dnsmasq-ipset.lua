-- Auther Qier LU <lvqier@gmail.com>

local m, s, o, p

m = Map("dnsmasq-ipset", translate("DNSmasq IPSet"), translate("IPSet lists for DNSMasq-full"))

m.on_after_save = function(self)
	luci.sys.call("/etc/init.d/dnsmasq-ipset reload &")
end

s = m:section(TypedSection, "ipsets", translate("IPSet Lists"))
s.anonymous = true
s.addremove = true

o = s:option(Value, "ipset_name", translate("IPSet Name"))
o.placeholder = "target ipset"
o.default = "rooter"
o.rmempty = false

o = s:option(Flag, "enabled", translate("Enabled"))

o = s:option(DynamicList, "managed_domain", translate("Managed Domain List"))

return m
