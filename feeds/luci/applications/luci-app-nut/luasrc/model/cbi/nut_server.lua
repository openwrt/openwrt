-- Copyright 2015 Daniel Dickinson <openwrt@daniel.thecshore.com>
-- Licensed to the public under the Apache License 2.0.

local m, s, o

local nixio = require "nixio"
require "luci.util"

m = Map("nut_server", translate("Network UPS Tools (Server)"),
	translate("Network UPS Tools Server Configuration"))

s = m:section(TypedSection, "user", translate("NUT Users"))
s.addremove = true
s.anonymous = true

o = s:option(Value, "username", translate("Username"))
o.optional = false

o = s:option(Value, "password", translate("Password"))
o.password = true
o.optional = false

o = s:option(MultiValue, "actions", translate("Allowed actions"))
o.widget = "select"
o:value("set", translate("Set variables"))
o:value("fsd", translate("Forced Shutdown"))
o.optional = true

o = s:option(DynamicList, "instcmd", translate("Instant commands"), translate("Use upscmd -l to see full list which the commands your UPS supports (requires upscmd package)"))
o.optional = true

o = s:option(ListValue, "upsmon", translate("Role"))
o:value("slave", translate("Slave"))
o:value("master", translate("Master"))
o.optional = false

s = m:section(TypedSection, "listen_address", translate("Addresses on which to listen"))
s.addremove = true
s.anonymous = true

o = s:option(Value, "address", translate("IP Address"))
o.optional = false
o.datatype = "ipaddr"
o.placeholder = "127.0.0.1"

o = s:option(Value, "port", translate("Port"))
o.optional = true
o.datatype = "port"
o.placeholder = "3493"

s = m:section(NamedSection, "upsd", "upsd", translate("UPS Server Global Settings"))
s.addremove = true

o = s:option(Value, "maxage", translate("Maximum Age of Data"), translate("Period after which data is considered stale"))
o.datatype = "uinteger"
o.optional = true
o.placeholder = 15

o = s:option(Value, "runas", translate("RunAs User"), translate("Drop privileges to this user"))
o.optional = true
o.placeholder = "nut"

o = s:option(Value, "statepath", translate("Path to state file"))
o.optional = true
o.placeholder = "/var/run/nut"

o = s:option(Value, "maxconn", translate("Maximum connections"))
o.optional = true
o.datatype = "uinteger"
o.placeholder = 24

if luci.util.checklib("/usr/sbin/upsd", "libssl.so") then
	o = s:option(Value, "certfile", translate("Certificate file (SSL)"))
	o.optional = true
end

s = m:section(NamedSection, "driver_global", "driver_global", translate("Driver Global Settings"))
s.addremove = true

o = s:option(Value, "chroot", translate("chroot"), translate("Run drivers in a chroot(2) environment"))
o.optional = true

o = s:option(Value, "driverpath", translate("Driver Path"), translate("Path to drivers (instead of default)"))
o.optional = true
o.placeholder = "/lib/lnut"

o = s:option(Value, "maxstartdelay", translate("Maximum Start Delay"), translate("Default for UPSes without this field."))
o.optional = true
o.datatype = "uinteger"

o = s:option(Value, "maxretry", translate("Maxium Retries"), translate("Maximum number of times to try starting a driver."))
o.optional = true
o.placeholder = 1
o.datatype = "uinteger"

o = s:option(Value, "retrydelay", translate("Retry Delay"), translate("Time in seconds between driver start retry attempts."))
o.optional = true
o.placeholder = 5
o.datatype = "uinteger"

o = s:option(Value, "pollinterval", translate("Poll Interval"), translate("Maximum time in seconds between refresh of UPS status"))
o.optional = true
o.placeholder = 2
o.datatype = "uinteger"

o = s:option(Flag, "synchronous", translate("Synchronous Communication"), translate("Driver waits for data to be consumed by upsd before publishing more."))
o.optional = true
o.default = false

o = s:option(Value, "user", translate("RunAs User"), translate("User as which to execute driver; requires device file accessed by driver to be read-write for that user."))
o.optional = true
o.placeholder = "nut"

s = m:section(TypedSection, "driver", translate("Driver Configuration"),
	translate("The name of this section will be used as UPS name elsewhere"))
s.addremove = true
s.anonymous = false

driverlist = nixio.fs.dir("/lib/nut")

o = s:option(Value, "bus", translate("USB Bus(es) (regex)"))
o.optional = true
o.datatype = "uinteger"

o = s:option(Value, "community", translate("SNMP Community"))
o.optional = true
o.placeholder = "private"

o = s:option(Value, "desc", translate("Description (Display)"))
o.optional = true

o = s:option(ListValue, "driver", translate("Driver"))
for driver in driverlist do
	o:value(driver)
end
o.optional = false

o = s:option(Flag, "enable_usb_serial", translate("Set USB serial port permissions"), translate("Enables a hotplug script that makes all ttyUSB devices (e.g. serial USB) group read-write as user 'nut'"))
o.optional = true
o.default = false

o = s:option(Flag, "ignorelb", translate("Ignore Low Battery"))
o.optional = true
o.default = false

o = s:option(Flag, "interruptonly", translate("Interrupt Only"))
o.optional = true
o.default = false

o = s:option(Value, "interruptsize", translate("Interrupt Size"), translate("Bytes to read from interrupt pipe"))
o.optional = true
o.datatype = "uinteger"

o = s:option(Value, "maxreport", translate("Max USB HID Length Reported"), translate("Workaround for buggy firmware"))
o.optional = true
o.datatype = "uinteger"

o = s:option(Value, "maxstartdelay", translate("Maxium Start Delay"), translate("Time in seconds that upsdrvctl will wait for driver to finish starting"))
o.optional = true
o.datatype = "uinteger"
o.placeholder = 45

o = s:option(Value, "mfr", translate("Manufacturer (Display)"))
o.optional = true

o = s:option(Value, "model", translate("Model (Display)"))
o.optional = true

o = s:option(Flag, "nolock", translate("No Lock"), translate("Don't lock port when starting driver"))
o.optional = true
o.default = false

o = s:option(Flag, "notransferoids", translate("No low/high voltage transfer OIDs"))
o.optional = true
o.default = false

o = s:option(Value, "offdelay", translate("Off Delay(s)"), translate("Delay for kill power command"))
o.optional = true
o.placeholder = 20

n = s:option(Value, "ondelay", translate("On Delay(s)"), translate("Delay to power on UPS if power returns after kill power"))
n.optional = true
n.placeholder = 30

function o.validate(self, cfg, value)
   if n:cfgvalue(cfg) <= value then
	return nil
   end
end

function n.validate(self, cfg, value)
   if o:cfgvalue(cfg) >= value then
	return nil
   end
end

o = s:option(Value, "pollfreq", translate("Polling Frequency(s)"))
o.optional = true
o.datatype = "integer"
o.placeholder = 30

o = s:option(Value, "port", translate("Port"))
o.optional = false
o.default = "auto"

o = s:option(Value, "product", translate("Product (regex)"))
o.optional = true

o = s:option(Value, "productid", translate("USB Product Id"))
o.optional = true

o = s:option(Value, "sdorder", translate("Driver Shutdown Order"))
o.optional = true
o.datatype = "uinteger"

o = s:option(Value, "sdtime", translate("Additional Shutdown Time(s)"))
o.optional = true

o = s:option(Value, "serial", translate("Serial Number"))
o.optional = true

o = s:option(Value, "snmp_retries", translate("SNMP retries"))
o.optional = true
o.datatype = "uinteger"

o = s:option(Value, "snmp_timeout", translate("SNMP timeout(s)"))
o.optional = true
o.datatype = "uinteger"

o = s:option(ListValue, "snmp_version", translate("SNMP version"))
o.optional = true
o:value("v1", translate("SNMPv1"))
o:value("v2c", translate("SNMPv2c"))
o:value("v3", translate("SNMPv3"))
o:value("", "")
o.default = ""

o = s:option(Value, "vendor", translate("Vendor (regex)"))
o.optional = true

o = s:option(Value, "vendorid", translate("USB Vendor Id"))
o.optional = true

o = s:option(Flag, "synchronous", translate("Synchronous Communication"), translate("Driver waits for data to be consumed by upsd before publishing more."))
o.optional = true
o.default = false

return m
