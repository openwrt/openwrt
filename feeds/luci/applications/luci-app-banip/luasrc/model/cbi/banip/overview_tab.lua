-- Copyright 2018-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs   = require("nixio.fs")
local uci  = require("luci.model.uci").cursor()
local net  = require "luci.model.network".init()
local util = require("luci.util")
local dump = util.ubus("network.interface", "dump", {})

m = Map("banip", translate("banIP"),
	translate("Configuration of the banIP package to block ip adresses/subnets via IPSet. ")
	..translatef("For further information "
	.. "<a href=\"%s\" target=\"_blank\">"
	.. "check the online documentation</a>", "https://github.com/openwrt/packages/blob/master/net/banip/files/README.md"))

-- Main banIP Options

s = m:section(NamedSection, "global", "banip")

o1 = s:option(Flag, "ban_enabled", translate("Enable banIP"))
o1.default = o1.disabled
o1.rmempty = false

o2 = s:option(Flag, "ban_automatic", translate("Automatic WAN Interface Detection"))
o2.default = o2.enabled
o2.rmempty = false

o3 = s:option(MultiValue, "ban_iface", translate("Manual WAN Interface Selection"),
	translate("Select your preferred interface(s) manually."))
if dump then
	local i, v
	for i, v in ipairs(dump.interface) do
		if v.interface ~= "loopback" then
			local device = v.l3_device or v.device or "-"
			o3:value(v.interface, " " .. v.interface .. " (" .. device .. ") ")
		end
	end
end
o3:depends("ban_automatic", 0)
o3.widget = "checkbox"
o3.rmempty = true

o4 = s:option(Flag, "ban_realtime", translate("SSH/LuCI RT Monitor"),
	translate("Starts a small log/banIP monitor in the background to block SSH/LuCI brute force attacks in realtime."))
o4.enabled = "true"
o4.default = o4.disabled
o4.rmempty = false

-- Runtime Information

ds = s:option(DummyValue, "_dummy")
ds.template = "banip/runtime"

-- Source Table

bl = m:section(TypedSection, "source", translate("IPSet Sources"))
bl.template = "banip/sourcelist"

name_4 = bl:option(Flag, "ban_src_on", translate("enable IPv4"))
name_4.rmempty = false

name_6 = bl:option(Flag, "ban_src_on_6", translate("enable IPv6"))
name_6.rmempty = false

type = bl:option(ListValue, "ban_src_ruletype", translate("SRC/DST"))
type:value("src")
type:value("dst")
type:value("src+dst")
type.default = "src"
type.rmempty = false

des = bl:option(DummyValue, "ban_src_desc", translate("Description"))

cat = bl:option(DynamicList, "ban_src_cat", translate("ASN/Country"))
cat.datatype = "uciname"
cat.optional = true

-- Extra options

e = m:section(NamedSection, "extra", "banip", translate("Extra Options"),
	translate("Options for further tweaking in case the defaults are not suitable for you."))

e1 = e:option(Flag, "ban_debug", translate("Verbose Debug Logging"),
	translate("Enable verbose debug logging in case of any processing error."))
e1.rmempty = false

e2 = e:option(Flag, "ban_nice", translate("Low Priority Service"),
	translate("Set the nice level to 'low priority' and banIP background processing will take less resources from the system. ")
	..translate("This change requires a manual service stop/re-start to take effect."))
e2.disabled = "0"
e2.enabled = "10"
e2.rmempty = false

e3 = e:option(Value, "ban_backupdir", translate("Backup Directory"),
	translate("Target directory for banIP backups. Default is '/tmp', please use preferably a non-volatile disk if available."))
e3.datatype = "directory"
e3.default = "/tmp"
e3.rmempty = true

e4 = e:option(Value, "ban_maxqueue", translate("Max. Download Queue"),
	translate("Size of the download queue to handle downloads &amp; IPset processing in parallel (default '4'). ")
	.. translate("For further performance improvements you can raise this value, e.g. '8' or '16' should be safe."))
e4.default = 4
e4.datatype = "range(1,32)"
e4.rmempty = false

e5 = e:option(ListValue, "ban_sshdaemon", translate("SSH Daemon"),
	translate("Select the SSH daemon for logfile parsing, to detect break-in events."))
e5:value("dropbear")
e5:value("sshd")
e5.default = "dropbear"
e5.rmempty = true

e6 = e:option(Flag, "ban_autoblacklist", translate("Local Save Blacklist Addons"),
  translate("Blacklist auto addons are stored temporary in the IPSet and saved permanently in the local blacklist. Disable this option to prevent the local save."))
e6.default = e6.enabled
e6.rmempty = true

e7 = e:option(Flag, "ban_autowhitelist", translate("Local Save Whitelist Addons"),
  translate("Whitelist auto addons are stored temporary in the IPSet and saved permanently in the local whitelist. Disable this option to prevent the local save."))
e7.default = e7.enabled
e7.rmempty = true

-- Optional Extra Options

e20 = e:option(ListValue, "ban_fetchutil", translate("Download Utility"),
	translate("Select your preferred download utility."))
e20:value("uclient-fetch")
e20:value("wget")
e20:value("curl")
e20:value("aria2c")
e20.optional = true

e21 = e:option(Value, "ban_fetchparm", translate("Download Options"),
	translate("Special options for the selected download utility, e.g. '--timeout=20 -O'."))
e21.optional = true

e22 = e:option(Value, "ban_triggerdelay", translate("Trigger Delay"),
	translate("Additional trigger delay in seconds before banIP processing begins."))
e22.default = 2
e22.datatype = "range(1,60)"
e22.optional = true

e23 = e:option(ListValue, "ban_starttype", translate("Start Type"),
	translate("Select the used start type during boot."))
e23:value("start")
e23:value("reload")
e23.default = "start"
e23.optional = true

e30 = e:option(Value, "ban_wan_input_chain", translate("WAN Input Chain IPv4"))
e30.default = "input_wan_rule"
e30.datatype = "uciname"
e30.optional = true

e31 = e:option(Value, "ban_wan_forward_chain", translate("WAN Forward Chain IPv4"))
e31.default = "forwarding_wan_rule"
e31.datatype = "uciname"
e31.optional = true

e32 = e:option(Value, "ban_lan_input_chain", translate("LAN Input Chain IPv4"))
e32.default = "input_lan_rule"
e32.datatype = "uciname"
e32.optional = true

e33 = e:option(Value, "ban_lan_forward_chain", translate("LAN Forward Chain IPv4"))
e33.default = "forwarding_lan_rule"
e33.datatype = "uciname"
e33.optional = true

e34 = e:option(ListValue, "ban_target_src", translate("SRC Target IPv4"))
e34:value("REJECT")
e34:value("DROP")
e34.default = "DROP"
e34.optional = true

e35 = e:option(ListValue, "ban_target_dst", translate("DST Target IPv4"))
e35:value("REJECT")
e35:value("DROP")
e35.default = "REJECT"
e35.optional = true

e36 = e:option(Value, "ban_wan_input_chain_6", translate("WAN Input Chain IPv6"))
e36.default = "input_wan_rule"
e36.datatype = "uciname"
e36.optional = true

e37 = e:option(Value, "ban_wan_forward_chain_6", translate("WAN Forward Chain IPv6"))
e37.default = "forwarding_wan_rule"
e37.datatype = "uciname"
e37.optional = true

e38 = e:option(Value, "ban_lan_input_chain_6", translate("LAN Input Chain IPv6"))
e38.default = "input_lan_rule"
e38.datatype = "uciname"
e38.optional = true

e39 = e:option(Value, "ban_lan_forward_chain_6", translate("LAN Forward Chain IPv6"))
e39.default = "forwarding_lan_rule"
e39.datatype = "uciname"
e39.optional = true

e40 = e:option(ListValue, "ban_target_src_6", translate("SRC Target IPv6"))
e40:value("REJECT")
e40:value("DROP")
e40.default = "DROP"
e40.optional = true

e41 = e:option(ListValue, "ban_target_dst_6", translate("DST Target IPv6"))
e41:value("REJECT")
e41:value("DROP")
e41.default = "REJECT"
e41.optional = true

return m
