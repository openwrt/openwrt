-- Licensed to the public under the Apache License 2.0.

local m = Map("radicale2", translate("Radicale 2.x"),
	      translate("A lightweight CalDAV/CardDAV server"))

local s = m:section(NamedSection, "logging", "section", translate("Logging"))
s.addremove = true
s.anonymous = false

local logging_file = nil

logging_file = s:option(FileUpload, "config", translate("Logging File"), translate("Log configuration file (no file means default procd which ends up in syslog"))
logging_file.rmempty = true
logging_file.default = ""

o = s:option(Button, "remove_conf", translate("Remove configuration for logging"),
		translate("This permanently deletes configuration for logging"))
o.inputstyle = "remove"

function o.write(self, section)
	if logging_file:cfgvalue(section) and fs.access(logging_file:cfgvalue(section)) then fs.unlink(loggin_file:cfgvalue(section)) end
	self.map:del(section, "config")
	luci.http.redirect(luci.dispatcher.build_url("admin", "services", "radicale2", "logging"))
end

o = s:option(Flag, "debug", translate("Debug"), translate("Send debug information to logs"))
o.rmempty = true
o.default = o.disabled

o = s:option(Flag, "full_environment", translate("Dump Environment"), translate("Include full environment in logs"))
o.rmempty = true
o.default = o.disabled

o = s:option(Flag, "mask_passwords", translate("Mask Passwords"), translate("Redact passwords in logs"))
o.rmempty = true
o.default = o.enabled

-- TODO: Allow configuration logging file from this page

return m
