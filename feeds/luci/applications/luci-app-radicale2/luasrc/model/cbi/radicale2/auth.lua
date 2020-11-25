-- Licensed to the public under the Apache License 2.0.

local rad2 = luci.controller.radicale2
local fs = require("nixio.fs")
local util = require("luci.util")

local m = Map("radicale2", translate("Radicale 2.x"),
	      translate("A lightweight CalDAV/CardDAV server"))

local s = m:section(NamedSection, "auth", "section", translate("Authentication"))
s.addremove = true
s.anonymous = false

local at = s:option(ListValue, "type", translate("Authentication Type"))
at:value("", translate("Default (htpasswd file from users below)"))
at:value("htpasswd", translate("htpasswd file (manually populated)"))
at:value("none", translate("No authentication"))
at:value("remote_user", translate("REMOTE_USER from web server"))
at:value("http_x_remote_user", translate("X-Remote-User from web server"))
at.default = ""
at.rmempty = true

local o = s:option(Value, "htpasswd_filename", translate("Filename"), translate("htpasswd-formatted file filename"))
o:depends("type", "htpasswd")
o.rmempty = true
o.placeholder = "/etc/radicale2/users"
o.default = ""

local hte = s:option(ListValue, "htpasswd_encryption", translate("Encryption"), translate("Password encryption method"))
hte:depends("type", "htpasswd")
hte:depends("type", "")
hte:value("plain", translate("Plaintext"))
hte:value("sha1", translate("SHA1"))
hte:value("ssha", translate("SSHA"))
hte:value("crypt", translate("crypt"))
if rad2.pymodexists("passlib") then
	hte:value("md5", translate("md5"))
	if rad2.pymodexists("bcrypt") then
		hte:value("bcrypt", translate("bcrypt"))
	end
end
hte.default = "plain"
hte.rmempty = true

if not rad2.pymodexists("bcrypt") then
	o = s:option(DummyValue, "nobcrypt", translate("Insecure hashes"), translate("Install python3-passlib and python3-bcrypt to enable a secure hash"))
else
	o = s:option(DummyValue, "nobcrypt", translate("Insecure hashes"), translate("Select bcrypt above to enable a secure hash"))
	o:depends("htpasswd_encrypt","")
	o:depends("htpasswd_encrypt","plain")
	o:depends("htpasswd_encrypt","sha1")
	o:depends("htpasswd_encrypt","ssha")
	o:depends("htpasswd_encrypt","crypt")
	o:depends("htpasswd_encrypt","md5")
end

o = s:option(Value, "delay", translate("Retry Delay"), translate("Required time between a failed authentication attempt and trying again"))
o.rmempty = true
o.default = 1
o.datatype = "uinteger"
o:depends("type", "")
o:depends("type", "htpasswd")
o:depends("type", "remote_user")
o:depends("type", "http_x_remote_user")

s = m:section(TypedSection, "user", translate("User"), translate("Users and Passwords"))
s.addremove = true
s.anonymous = true

o = s:option(Value, "name", translate("Username"))
o.rmempty = true
o.placeholder = "johndoe"

if rad2.pymodexists("passlib") then

local plainpass = s:option(Value, "plain_pass", translate("Plaintext Password"))
plainpass.placeholder = "Example password"
plainpass.password = true

local ppconfirm = s:option(Value, "plain_pass_confirm", translate("Confirm Plaintext Password"))
ppconfirm.placeholder = "Example password"
ppconfirm.password = true

plainpass.cfgvalue = function(self, section)
	return self:formvalue(section)
end

plainpass.write = function(self, section)
	return true
end


ppconfirm.cfgvalue = plainpass.cfgvalue
ppconfirm.write = plainpass.write

plainpass.validate = function(self, value, section)
	if self:cfgvalue(section) ~= ppconfirm:cfgvalue(section) then
		return nil, translate("Password and confirmation do not match")
	end
	return AbstractValue.validate(self, value, section)
end

ppconfirm.validate = function(self, value, section)
	if self:cfgvalue(section) ~= plainpass:cfgvalue(section) then
		return nil, translate("Password and confirmation do not match")
	end
	return AbstractValue.validate(self, value, section)
end

local pass = s:option(Value, "password", translate("Encrypted Password"), translate("If 'Plaintext Password' filled and matches 'Confirm Plaintext Password' then this field becomes of hash of that password, otherwise this field remains the existing hash (you can also put your own hash value for the type of hash listed above)."))
pass.password = true
pass.rmempty = false

function encpass(self, section)
	local plainvalue = plainpass:cfgvalue(section)
	local pvc = ppconfirm:cfgvalue(section)
	local encvalue, err

	if not plainvalue or not pvc or plainvalue == "" or pvc == "" or plainvalue ~= pvc then
		return nil
	end
	local enctype = hte:formvalue("auth")
	if not enctype then
		enctype = hte:cfgvalue("auth")
	end
	if not enctype or enctype == "" or enctype == "plain" then
		return plainvalue
	end

	encvalue, err = util.ubus("rad2-enc", "encrypt", { type = enctype, plainpass = plainvalue })
	if not encvalue then
		return nil
	end

	return encvalue and encvalue.encrypted_password
end

pass.cfgvalue = function(self, section)
	if not plainpass:formvalue(section) then
		return Value.cfgvalue(self, section)
	else
		return Value.formvalue(self, section)
	end
end

pass.formvalue = function(self, section)
	if not plainpass:formvalue(section) then
		return Value.formvalue(self, section)
	else
		return encpass(self, section) or Value.formvalue(self, section)
	end
end

else
local pass = s:option(Value, "password", translate("Encrypted Password"), translate("Generate this field using an generator for Apache htpasswd-style authentication files (for the hash format you have chosen above), or install python3-passlib to enable the ability to create the hash by entering the plaintext in a field that will appear on this page if python3-passlib is installed."))
pass.password = true
pass.rmempty = false

end -- python3-passlib installed

-- TODO: Allow configuration of rights file from this page
local s = m:section(NamedSection, "section", "rights", translate("Rights"), translate("User-based ACL Settings"))
s.addremove = true
s.anonymous = false

o = s:option(ListValue, "type", translate("Rights Type"))
o:value("", translate("Default (owner only)"))
o:value("owner_only", translate("RO: None, RW: Owner"))
o:value("authenticated", translate("RO: None, RW: Authenticated Users"))
o:value("owner_write", translate("RO: Authenticated Users, RW: Owner"))
o:value("from_file", translate("Based on settings in 'Rights File'"))
o:value("none", translate("RO: All, RW: All"))
o.default = ""
o.rmempty = true

rights_file = s:option(FileUpload, "file", translate("Rights File"))
rights_file.rmempty = true
rights_file:depends("type", "from_file")

o = s:option(Button, "remove_conf",
	translate("Remove configuration for rights file"),
	translate("This permanently deletes the rights file and configuration to use same."))
o.inputstyle = "remove"
o:depends("type", "from_file")

function o.write(self, section)
	if cert_file:cfgvalue(section) and fs.access(o:cfgvalue(section)) then fs.unlink(rights_file:cfgvalue(section)) end
	self.map:del(section, "file")
	self.map:del(section, "rights_file")
	luci.http.redirect(luci.dispatcher.build_url("admin", "services", "radicale2", "auth"))
end

-- TODO: Allow configuration rights file from this page

return m
