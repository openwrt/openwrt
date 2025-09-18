m = Map("bluewan-auto", "BLUE4WAN Auto PPPoE")

s = m:section(TypedSection, "settings", "Settings")
s.addremove = false

iface = s:option(Value, "iface", "Interface")
username = s:option(Value, "username", "PPPoE Username")
password = s:option(Value, "password", "PPPoE Password")
enable = s:option(Flag, "enable", "Enable")
enable.default = enable.enabled

return m
