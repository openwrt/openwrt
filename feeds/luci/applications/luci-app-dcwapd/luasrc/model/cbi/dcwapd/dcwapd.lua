-- Copyright 2019 EWSI
-- Licensed to the public under the Apache License 2.0.

local wa = require "luci.tools.webadmin"
local fs = require "nixio.fs"
local ntm = require "luci.model.network"
local uc = require "luci.model.uci".cursor()

local sys = require "luci.sys"

local devices = sys.net.devices()

local m, s
local i, v
local enable, tmpdir
local pri_ssid, pri_bridge, data_channels
local dat_ssid, dat_bridge, ifaces
local mac, filters
local packetsize, srcip, srcport, proto, dstport

m = Map("dcwapd", translate("Dual Channel Wi-Fi AP Daemon"),
	translate("With <abbr title=\"Dual Channel Wi-Fi AP Daemon\">Dual Channel WiFi</abbr> you " ..
		"can use two simultaneous Wi-Fi connections to decrease wireless traffic " ..
		"congestion and increase throughput."))

-- General section
s = m:section(NamedSection, "general", translate("General"), translate("General Settings"))
s.addremove = false
s.dynamic = false
s.optional = false
s.anonymous = true

-- Enabled state option
enable = s:option(Flag, "enabled", translate("Enable"))
enable.default = false
enable.optional = false
enable.rmempty = false

-- Temp dir option
tmpdir = s:option(Value, "tmpdir", translate("Temp Directory"), translate("Specify the temporary directory for dcwapd file storage."))
tmpdir.optional = false
tmpdir.rmempty = false

-- Channel sets section
s = m:section(TypedSection, "channel-set", translate("Channel Sets"), translate("Define primary channels and their corresponding data channels."))
s.addremove= true
s.dynamic = false
s.optional = false
s.anonymous = false

-- Enabled state option
enable = s:option(Flag, "enabled", translate("Enable"))
enable.default = false
enable.optional = false
enable.rmempty = false

-- SSID option
pri_ssid = s:option(Value, "ssid", translate("SSID"))
pri_ssid.optional = false
pri_ssid.rmempty = false
pri_ssid.size = 0
for i, v in ipairs(devices) do
	ntm.init()
	local net = ntm:get_wifinet(v)
	if net then
		pri_ssid:value(net:ssid())
		pri_ssid.size = pri_ssid.size + 1
	end
end

-- Primary bridge option
pri_bridge = s:option(Value, "bridge", translate("Bridge"))
pri_bridge.optional = false
pri_bridge.rmempty = false
pri_bridge.size = 0
for i, v in ipairs(devices) do
	ntm.init()
	local net = ntm:get_wifinet(v)
	if net then
		local nw = net:get_network()
		if nw then
			pri_bridge:value("br-" .. nw:name())
			pri_bridge.size = pri_bridge.size + 1
		end
	end
end

-- Data channels list
data_channels = s:option(MultiValue, "data_channels", translate("Data Channels"))
data_channels.widget = "checkbox"
data_channels.optional = false
data_channels.rmempty = false
data_channels.size = 0
uc:foreach("dcwapd", "datachannel", function(s)
	if s['.name'] then
		data_channels:value(s['.name'])
		data_channels.size = data_channels.size + 1
	end
end)

-- Data channels section
s = m:section(TypedSection, "datachannel", translate("Data Channels"), translate("Define data channels over which outbound filtered packets will flow."))
s.anonymous = false
s.addremove = true

-- SSID option
dat_ssid = s:option(Value, "ssid", translate("SSID"))
dat_ssid.optional = false
dat_ssid.rmempty = false
dat_ssid.size = 0
for i, v in ipairs(devices) do
	ntm.init()
	local net = ntm:get_wifinet(v)
	if net then
		dat_ssid:value(net:ssid())
		dat_ssid.size = dat_ssid.size + 1
	end
end

-- Data bridge option
dat_bridge = s:option(Value, "bridge", translate("Bridge"))
dat_bridge.optional = false
dat_bridge.rmempty = false
dat_bridge.size = 0
for i, v in ipairs(devices) do
	ntm.init()
	local net = ntm:get_wifinet(v)
	if net then
		local nw = net:get_network()
		if nw then
			dat_bridge:value("br-" .. nw:name())
			dat_bridge.size = dat_bridge.size + 1
		end
	end
end

-- Data interfaces list
ifaces = s:option(MultiValue, "interfaces", translate("Interfaces"))
ifaces.widget = "checkbox"
ifaces.optional = true
ifaces.rmempty = false
ifaces.size = 0
table.sort(devices)
for i, v in ipairs(devices) do
	ntm.init()
	local net = ntm:get_wifinet(v)
	if net then
		ifaces:value(v)
		ifaces.size = ifaces.size + 1
	end
end

-- Filter sets section
s = m:section(TypedSection, "filter-set", translate("Filter Sets"), translate("Select filters to apply to matching MAC addresses."))
s.addremove = true
s.dynamic = false
s.anonymous = false
s.optional = false

-- MAC address option
mac = s:option(Value, "mac", translate("MAC Address"))
mac.optional = false
mac.rmempty = false

-- Filters list
filters = s:option(MultiValue, "filters", translate("Filters"))
filters.widget = "checkbox"
filters.optional = false
filters.rmempty = false
filters.size = 0
uc:foreach("dcwapd", "filter", function(s)
	if s['.name'] then
		filters:value(s['.name'])
		filters.size = filters.size + 1
	end
end)

-- Filters section
s = m:section(TypedSection, "filter", translate("Filters"), translate("Define filter rules to apply to outbound packets. Matching packets will flow over the data channel."))
s.template = "cbi/tblsection"
s.anonymous = false
s.addremove = true
s.sortable = true

-- Packet Size
packetsize = s:option(Value, "packet_size", translate("Packet size"))
packetsize.rmempty = false
packetsize:value("*", "*")
packetsize.default = "*"

-- Source IP
srcip = s:option(Value, "source_ip", translate("Source IP"))
srcip.rmempty = false
srcip:value("*", "*")
srcip.default = "*"

-- Source Port
srcport = s:option(Value, "source_port", translate("Source port"))
srcport.rmempty = false
srcport:value("*", "*")
srcport.default = "*"

-- Protocol
proto = s:option(Value, "protocol", translate("Protocol"))
proto:value("*", "*")
proto:value("tcp", "TCP")
proto:value("udp", "UDP")
proto:value("icmp", "ICMP")
proto.rmempty = false
proto.default = "*"

-- Destination Port
dstport = s:option(Value, "dest_port", translate("Destination port"))
dstport.rmempty = false
dstport:value("*", "*")
dstport.default = "*"

return m
