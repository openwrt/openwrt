-- Copyright 2015 Daniel Dickinson <openwrt@daniel.thecshore.com>
-- Licensed to the public under the Apache License 2.0.

local m, s, o
require "luci.util"

m = Map("nut_monitor", translate("Network UPS Tools (Monitor)"),
	translate("Network UPS Tools Monitoring Configuration"))

s = m:section(NamedSection, "upsmon", "upsmon", translate("Global Settings"))
s.addremove = true
s.optional = true

o = s:option(Value, "runas", translate("RunAs User"), translate("upsmon drops privileges to this user"))
o.placeholder = "nutmon"

o = s:option(Value, "minsupplies", translate("Minimum required number or power supplies"))
o.datatype = "uinteger"
o.placeholder = 1
o.optional = true

o = s:option(Value, "shutdowncmd", translate("Shutdown command"))
o.optional = true
o.placeholder = "/sbin/halt"

o = s:option(Value, "notifycmd", translate("Notify command"))
o.optional = true

o = s:option(Value, "pollfreq", translate("Poll frequency"))
o.datatype = "uinteger"
o.placeholder = 5
o.optional = true

o = s:option(Value, "pollfreqalert", translate("Poll frequency alert"))
o.datatype = "uinteger"
o.optional = true
o.placeholder = 5

o = s:option(Value, "hotsync", translate("Hot Sync"))
o.optional = true
o.placeholder = 15

o = s:option(Value, "deadtime", translate("Deadtime"))
o.datatype = "uinteger"
o.optional = true
o.placeholder = 15

o = s:option(Value, "onlinemsg", translate("Online message"))
o.optional = true

o = s:option(Value, "onbattmsg", translate("On battery message"))
o.optional = true

o = s:option(Value, "lowbattmsg", translate("Low battery message"))
o.optional = true

o = s:option(Value, "fsdmsg", translate("Forced shutdown message"))
o.optional = true

o = s:option(Value, "comokmsg", translate("Communications restored message"))
o.optional = true

o = s:option(Value, "combadmsg", translate("Communications lost message"))
o.optional = true

o = s:option(Value, "shutdownmsg", translate("Shutdown message"))
o.optional = true

o = s:option(Value, "replbattmsg", translate("Replace battery message"))
o.optional = true

o = s:option(Value, "nocommsg", translate("No communications message"))
o.optional = true

o = s:option(Value, "noparentmsg", translate("No parent message"))
o.optional = true

validatenotify = function(self, value)
	val = StaticList.validate(self, value)
	if val then
		for k, v in pairs(val) do
			if (v == 'IGNORE') then
				return nil, "Ignore must the only option selected, when selected"
			end
		end
	end
	return val
end

o = s:option(StaticList, "defaultnotify", translate("Notification defaults"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.default = "SYSLOG"
o.validate = validatenotify

o = s:option(StaticList, "onlinenotify", translate("Notify when back online"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "onbattnotify", translate("Notify when on battery"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "nowbattnotify", translate("Notify when low battery"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "nowbattnotify", translate("Notify when low battery"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "fsdnotify", translate("Notify when force shutdown"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "comoknotify", translate("Notify when communications restored"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "combadnotify", translate("Notify when communications lost"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "shutdownotify", translate("Notify when shutting down"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

o = s:option(StaticList, "replbattnotify", translate("Notify when battery needs replacing"))
o.optional = true
o.widget = "select"
o:value("EXEC", translate("Execute notify command"))
o:value("SYSLOG", translate("Write to syslog"))
o:value("IGNORE", translate("Ignore"))
o.validate = validatenotify

local have_ssl_support = luci.util.checklib("/usr/sbin/upsmon", "libssl.so")

if have_ssl_support then
	o = s:option(Value, "certpath", translate("CA Certificate path"), translate("Path containing ca certificates to match against host certificate"))
	o.optional = true
	o.placeholder = "/etc/ssl/certs"

	o = s:option(Flag, "certverify", translate("Verify all connection with SSL"), translate("Require SSL and make sure server CN matches hostname"))
	o.optional = true
	o.default = false
end

s = m:section(TypedSection, "master", translate("UPS Master"))
s.optional = true
s.addremove = true
s.anonymous = true

o = s:option(Value, "upsname", translate("Name of UPS"), translate("As configured by NUT"))
o.optional = false

o = s:option(Value, "hostname", translate("Hostname or address of UPS"))
o.optional = false
s.datatype = "host"

o = s:option(Value, "port", translate("Port"))
o.optional = true
o.placeholder = 3493
o.datatype = "port"

o = s:option(Value, "powervalue", translate("Power value"))
o.optional = false
o.datatype = "uinteger"
o.default = 1

o = s:option(Value, "username", translate("Username"))
o.optional = false

o = s:option(Value, "password", translate("Password"))
o.optional = false
o.password = true

s = m:section(TypedSection, "slave", translate("UPS Slave"))
s.optional = true
s.addremove = true
s.anonymous = true

o = s:option(Value, "upsname", translate("Name of UPS"), translate("As configured by NUT"))
o.optional = false

o = s:option(Value, "hostname", translate("Hostname or address of UPS"))
o.optional = false
s.datatype = "host"

o = s:option(Value, "port", translate("Port"))
o.optional = true
o.placeholder = 3493
o.datatype = "port"

o = s:option(Value, "powervalue", translate("Power value"))
o.optional = false
o.datatype = "uinteger"
o.default = 1

o = s:option(Value, "username", translate("Username"))
o.optional = false

o = s:option(Value, "password", translate("Password"))
o.optional = false
o.password = true

return m
