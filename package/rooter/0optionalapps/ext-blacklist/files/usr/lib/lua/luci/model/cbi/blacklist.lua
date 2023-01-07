require("nixio.fs")

m = Map("blacklist", translate("Blacklist Devices by Mac Address"))

m.on_after_save = function(self)
	luci.sys.call("/usr/lib/blacklist/chkblack.sh &")
end

s = m:section(TypedSection, "devices", translate("Blacklisted Devices"), translate("Block these devices from using the Internet"))
s.anonymous = true
s.addremove = true

o = s:option(Value, "src_mac", translate("Device MAC address"))
	o.datatype = "list(macaddr)"
	o.placeholder = translate("any")

	luci.sys.net.mac_hints(function(mac, name)
		o:value(mac, "%s (%s)" %{ mac, name })
	end)

return m