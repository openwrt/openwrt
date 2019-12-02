mp = Map("ipsec", translate("IPSec VPN Server"))
mp.description = translate(
                     "IPSec VPN connectivity using the native built-in VPN Client on iOS or Andriod (IKEv1 with PSK and Xauth)")

s = mp:section(TypedSection, "users", translate("Users Manager"))
s.addremove = true
s.anonymous = true
s.template = "cbi/tblsection"

enabled = s:option(Flag, "enabled", translate("Enabled"))
enabled.rmempty = false
username = s:option(Value, "username", translate("User name"))
username.placeholder = translate("User name")
username.rmempty = true
password = s:option(Value, "password", translate("Password"))
password.rmempty = true

return mp
