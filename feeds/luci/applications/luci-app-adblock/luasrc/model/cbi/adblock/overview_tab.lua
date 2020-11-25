-- Copyright 2017-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs   = require("nixio.fs")
local uci  = require("luci.model.uci").cursor()
local util = require("luci.util")
local net  = require "luci.model.network".init()
local dump = util.ubus("network.interface", "dump", {})

m = Map("adblock", translate("Adblock"),
	translate("Configuration of the adblock package to block ad/abuse domains by using DNS. ")
	..translatef("For further information "
	.. "<a href=\"%s\" target=\"_blank\">"
	.. "check the online documentation</a>", "https://github.com/openwrt/packages/blob/master/net/adblock/files/README.md"))

-- Main adblock options

s = m:section(NamedSection, "global", "adblock")

o1 = s:option(Flag, "adb_enabled", translate("Enable Adblock"))
o1.default = o1.disabled
o1.rmempty = false

o2 = s:option(ListValue, "adb_dns", translate("DNS Backend (DNS Directory)"),
	translate("List of supported DNS backends with their default list export directory. ")
	..translate("To overwrite the default path use the 'DNS Directory' option in the extra section below."))
o2:value("dnsmasq", "dnsmasq (/tmp)")
o2:value("unbound", "unbound (/var/lib/unbound)")
o2:value("named", "named (/var/lib/bind)")
o2:value("kresd", "kresd (/etc/kresd)")
o2.default = "dnsmasq (/tmp)"
o2.rmempty = false

o3 = s:option(ListValue, "adb_dnsvariant", translate("DNS Blocking Variant"),
	translate("List of supported DNS blocking variants. By default 'nxdomain' will be used for all DNS backends. ")
	..translate("Dnsmasq also supports 'null' block variants, which may provide better response times."))
o3:value("nxdomain", "nxdomain, all DNS backends")
o3:value("null (IPv4)", "null (IPv4), dnsmasq only")
o3:value("null (IPv4/IPv6)", "null (IPv4/IPv6), dnsmasq only")
o3.default = "nxdomain, all DNS backends"
o3.rmempty = false

o4 = s:option(ListValue, "adb_fetchutil", translate("Download Utility"),
	translate("List of supported and fully pre-configured download utilities."))
o4:value("uclient-fetch")
o4:value("wget")
o4:value("curl")
o4:value("aria2c")
o4:value("wget-nossl", "wget-nossl (noSSL)")
o4:value("busybox", "wget-busybox (noSSL)")
o4.default = "uclient-fetch"
o4.rmempty = false

o5 = s:option(ListValue, "adb_trigger", translate("Startup Trigger"),
	translate("List of available network interfaces. Usually the startup will be triggered by the 'wan' interface. ")
	..translate("Choose 'none' to disable automatic startups, 'timed' to use a classic timeout (default 30 sec.) or select another trigger interface."))
o5:value("none")
o5:value("timed")
if dump then
	local i, v
	for i, v in ipairs(dump.interface) do
		if v.interface ~= "loopback" then
			local device = v.l3_device or v.device or "-"
			o5:value(v.interface, v.interface.. " (" ..device.. ")")
		end
	end
end
o5.rmempty = false

-- Runtime information

ds = s:option(DummyValue, "_dummy")
ds.template = "adblock/runtime"

-- Blocklist table

bl = m:section(TypedSection, "source", translate("Blocklist Sources"),
	translate("<b>Caution:</b> To prevent OOM exceptions on low memory devices with less than 64 MB free RAM, please only select a few of them!"))
bl.template = "adblock/blocklist"

name = bl:option(Flag, "enabled", translate("Enabled"))
name.rmempty = false

ssl = bl:option(DummyValue, "adb_src", translate("SSL req."))
function ssl.cfgvalue(self, section)
	local source = self.map:get(section, "adb_src")
	if source and source:match("https://") then
		return translate("Yes")
	else
		return translate("No")
	end
end

des = bl:option(DummyValue, "adb_src_desc", translate("Description"))

cat = bl:option(DynamicList, "adb_src_cat", translate("Archive Categories"))
cat.datatype = "uciname"
cat.optional = true

-- Extra options

e = m:section(NamedSection, "extra", "adblock", translate("Extra Options"),
	translate("Options for further tweaking in case the defaults are not suitable for you."))

e1 = e:option(Flag, "adb_debug", translate("Verbose Debug Logging"),
	translate("Enable verbose debug logging in case of any processing error."))
e1.rmempty = false

e2 = e:option(Flag, "adb_nice", translate("Low Priority Service"),
	translate("Set the nice level to 'low priority' and the adblock background processing will take fewer resources from the system. ")
	..translate("This change requires a manual service stop/re-start to take effect."))
e2.disabled = "0"
e2.enabled = "10"
e2.rmempty = false

e3 = e:option(Flag, "adb_forcedns", translate("Force Local DNS"),
	translate("Redirect all DNS queries from 'lan' zone to the local resolver, applies to udp and tcp protocol on ports 53, 853 and 5353."))
e3.rmempty = false

e4 = e:option(Value, "adb_maxqueue", translate("Max. Download Queue"),
	translate("Size of the download queue to handle downloads &amp; list processing in parallel (default '4'). ")
	..translate("For further performance improvements you can raise this value, e.g. '8' or '16' should be safe."))
e4.default = 4
e4.datatype = "range(1,32)"
e4.rmempty = false

e5 = e:option(Flag, "adb_dnsfilereset", translate("DNS File Reset"),
	translate("Resets the final DNS blockfile 'adb_list.overall' after loading through the DNS backend. ")
	..translate("This option saves an enormous amount of storage space, but starts a small ubus/adblock monitor in the background."))
e5.disabled = "false"
e5.enabled = "true"
e5.rmempty = false

e6 = e:option(Flag, "adb_report", translate("DNS Query Report"),
	translate("Gather dns related network traffic via tcpdump to provide a DNS Query Report on demand. ")
	..translate("Please note: this needs manual 'tcpdump-mini' package installation."))
e6.rmempty = false

e7 = e:option(Value, "adb_repdir", translate("Report Directory"),
	translate("Target directory for dns related report files. Default is '/tmp', please use preferably a non-volatile disk if available."))
e7:depends("adb_report", 1)
e7.datatype = "directory"
e7.default = "/tmp"
e7.rmempty = true

e8 = e:option(Value, "adb_backupdir", translate("Backup Directory"),
	translate("Target directory for adblock source backups. Default is '/tmp', please use preferably a non-volatile disk if available."))
e8.datatype = "directory"
e8.default = "/tmp"
e8.rmempty = true

e9 = e:option(Flag, "adb_mail", translate("E-Mail Notification"),
	translate("Send notification E-Mails in case of a processing error or if domain count is &le; 0. ")
	.. translate("Please note: this needs manual 'msmtp' package installation and setup."))
e9.rmempty = true

e10 = e:option(Value, "adb_mreceiver", translate("E-Mail Receiver Address"),
	translate("Receiver address for adblock notification E-Mails."))
e10:depends("adb_mail", 1)
e10.rmempty = true

-- Optional Extra Options

e20 = e:option(Value, "adb_dnsdir", translate("DNS Directory"),
	translate("Target directory for the generated blocklist 'adb_list.overall'."))
e20.datatype = "directory"
e20.optional = true

e21 = e:option(Value, "adb_blacklist", translate("Blacklist File"),
	translate("Full path to the blacklist file."))
e21.datatype = "file"
e21.default = "/etc/adblock/adblock.blacklist"
e21.optional = true

e22 = e:option(Value, "adb_whitelist", translate("Whitelist File"),
	translate("Full path to the whitelist file."))
e22.datatype = "file"
e22.default = "/etc/adblock/adblock.whitelist"
e22.optional = true

e23 = e:option(Value, "adb_triggerdelay", translate("Trigger Delay"),
	translate("Additional trigger delay in seconds before adblock processing begins."))
e23.datatype = "range(1,60)"
e23.optional = true

e24 = e:option(Value, "adb_maxtld", translate("TLD Compression Threshold"),
	translate("Disable the toplevel domain compression, if the number of blocked domains is greater than this threshold."))
e24.datatype = "min(0)"
e24.default = 100000
e24.optional = true

e25 = e:option(Value, "adb_portlist", translate("Local FW/DNS Ports"),
	translate("Space separated list of firewall ports which should be redirected locally."))
e25.default = "53 853 5353"
e25.optional = true

e26 = e:option(Flag, "adb_dnsinotify", translate("DNS Inotify"),
	translate("Disable adblock triggered restarts and the 'DNS File Reset' for dns backends with autoload features."))
e26.default = nil
e26.enabled = "true"
e26.optional = true

e27 = e:option(Flag, "adb_dnsflush", translate("Flush DNS Cache"),
	translate("Flush DNS Cache after adblock processing."))
e27.default = nil
e27.optional = true

e28 = e:option(ListValue, "adb_repiface", translate("Report Interface"),
	translate("Reporting interface used by tcpdump, set to 'any' for multiple interfaces (default 'br-lan'). ")
	..translate("This change requires a manual service stop/re-start to take effect."))
if dump then
	local i, v
	for i, v in ipairs(dump.interface) do
		if v.interface ~= "loopback" then
			local device = v.device
			if device then
				e28:value(device)
			end
		end
	end
end
e28:value("any")
e28.optional = true

e29 = e:option(Value, "adb_replisten", translate("Report Listen Port(s)"),
	translate("Space separated list of reporting port(s) used by tcpdump (default: '53'). ")
	..translate("This change requires a manual service stop/re-start to take effect."))
e29.default = 53
e29.optional = true

e30 = e:option(Value, "adb_repchunkcnt", translate("Report Chunk Count"),
	translate("Report chunk count used by tcpdump (default '5'). ")
	..translate("This change requires a manual service stop/re-start to take effect."))
e30.datatype = "range(1,10)"
e30.default = 5
e30.optional = true

e31 = e:option(Value, "adb_repchunksize", translate("Report Chunk Size"),
	translate("Report chunk size used by tcpdump in MB (default '1'). ")
	..translate("This change requires a manual service stop/re-start to take effect."))
e31.datatype = "range(1,10)"
e31.default = 1
e31.optional = true

e32 = e:option(Value, "adb_msender", translate("E-Mail Sender Address"),
	translate("Sender address for adblock notification E-Mails."))
e32.default = "no-reply@adblock"
e32.optional = true

e33 = e:option(Value, "adb_mtopic", translate("E-Mail Topic"),
	translate("Topic for adblock notification E-Mails."))
e33.default = "adblock notification"
e33.optional = true

e34 = e:option(Value, "adb_mprofile", translate("E-Mail Profile"),
	translate("Mail profile used in 'msmtp' for adblock notification E-Mails."))
e34.default = "adb_notify"
e34.optional = true

e35 = e:option(Value, "adb_mcnt", translate("E-Mail Notification Count"),
	translate("Raise the minimum notification count, to get E-Mails if the overall count is less or equal to the given limit (default 0), ")
	.. translate("e.g. to receive an E-Mail notification with every adblock run set this value to 200000."))
e35.default = 0
e35.datatype = "min(0)"
e35.optional = true

return m
