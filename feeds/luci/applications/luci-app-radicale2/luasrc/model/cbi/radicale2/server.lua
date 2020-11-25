-- Licensed to the public under the Apache License 2.0.

local fs = require("nixio.fs")
local rad2 = require "luci.controller.radicale2"
local http = require("luci.http")

local m = Map("radicale2", translate("Radicale 2.x"),
	      translate("A lightweight CalDAV/CardDAV server"))

s = m:section(SimpleSection, translate("Radicale v2 Web UI"))
s.addremove = false
s.anonymous = true

o = s:option(DummyValue, "radicale2_webui_go", translate("Go to Radicale v2 Web UI"))
o.template = "cbi/raduigo"
o.section = "cbi-radicale2_webui"

local s = m:section(NamedSection, "server", "section", translate("Server Settings"))
s.addremove = true
s.anonymous = false

o.section = "cbi-radicale2_web_ui"

local lhttp = nil
local certificate_file = nil
local key_file = nil
local certificate_authority_file = nil

s:tab("general", translate("General Settings"))
s:tab("advanced", translate("Advanced Settings"))

lhttp = s:taboption("general", DynamicList, "host", translate("HTTP(S) Listeners (address:port)"))
lhttp.datatype = "list(ipaddrport(1))"
lhttp.placeholder = "127.0.0.1:5232"

o = s:taboption("advanced", Value, "max_connection", translate("Max Connections"), translate("Maximum number of simultaneous connections"))
o.rmempty = true
o.placeholder = 20
o.datatype = "uinteger"


o = s:taboption("advanced", Value, "max_content_length", translate("Max Content Length"), translate("Maximum size of request body (bytes)"))
o.rmempty = true
o.datatype = "uinteger"
o.placeholder = 100000000

o = s:taboption("advanced", Value, "timeout", translate("Timeout"), translate("Socket timeout (seconds)"))
o.rmempty = true
o.placeholder = 30
o.datatype = "uinteger"

sslon = s:taboption("general", Flag, "ssl", translate("SSL"), translate("Enable SSL connections"))
sslon.rmempty = true
sslon.default = o.disabled
sslon.formvalue = function(self, section)
	if not rad2.pymodexists('ssl') then
		return false
	end
	return Flag.formvalue(self, section)
end

cert_file = s:taboption("general", FileUpload, "certificate", translate("Certificate"))
cert_file.rmempty = true
cert_file:depends("ssl", true)

key_file = s:taboption("general", FileUpload, "key", translate("Private Key"))
key_file.rmempty = true
key_file:depends("ssl", true)

ca_file = s:taboption("general", FileUpload, "certificate_authority", translate("Client Certificate Authority"), translate("For verifying client certificates"))
ca_file.rmempty = true
ca_file:depends("ssl", true)

o = s:taboption("advanced", Value, "ciphers", translate("Allowed Ciphers"), translate("See python3-openssl documentation for available ciphers"))
o.rmempty = true
o:depends("ssl", true)

o = s:taboption("advanced", Value, "protocol", translate("Use Protocol"), translate("See python3-openssl documentation for available protocols"))
o.rmempty = true
o:depends("ssl", true)
o.placeholder = "PROTOCOL_TLSv1_2"

o = s:taboption("general", Button, "remove_conf",
	translate("Remove configuration for certificate, key, and CA"),
	translate("This permanently deletes the cert, key, and configuration to use same."))
o.inputstyle = "remove"
o:depends("ssl", true)

function o.write(self, section)
	if cert_file:cfgvalue(section) and fs.access(cert_file:cfgvalue(section)) then fs.unlink(cert_file:cfgvalue(section)) end
	if key_file:cfgvalue(section) and fs.access(key_file:cfgvalue(section)) then fs.unlink(key_file:cfgvalue(section)) end
	if ca_file:cfgvalue(section) and fs.access(key_file:cfgvalue(section)) then fs.unlink(ca_file:cfgvalue(section)) end
	self.map:del(section, "certificate")
	self.map:del(section, "key")
	self.map:del(section, "certificate_authority")
	self.map:del(section, "protocol")
	self.map:del(section, "ciphers")
	luci.http.redirect(luci.dispatcher.build_url("admin", "services", "radicale2", "server"))
end

if not rad2.pymodexists('ssl') then
	o = s:taboption("general", DummyValue, "sslnotpreset", translate("SSL not available"), translate("Install package python3-openssl to support SSL connections"))
end

o = s:taboption("advanced", Flag, "dns_lookup", translate("DNS Lookup"), translate("Lookup reverse DNS for clients for logging"))
o.rmempty = true
o.default = o.enabled

o = s:taboption("advanced", Value, "realm", translate("Realm"), translate("HTTP(S) Basic Authentication Realm"))
o.rmempty = true
o.placeholder = "Radicale - Password Required"

local s = m:section(NamedSection, "web", "section", translate("Web UI"))
s.addremove = true
s.anonymous = false

o = s:option(ListValue, "type", translate("Web UI Type"))
o:value("", "Default (Built-in)")
o:value("internal", "Built-in")
o:value("none", "None")
o.default = ""
o.rmempty = true

local s = m:section(NamedSection, "headers", "section", translate("Headers"), translate("HTTP(S) Headers"))
s.addremove = true
s.anonymous = false

o = s:option(Value, "cors", translate("CORS"), translate("Header: X-Access-Control-Allow-Origin"))
o.rmempty = true
o.placeholder = "*"

local s = m:section(NamedSection, "encoding", "section", translate("Document Encoding"))
s.addremove = true
s.anonymous = false

o = s:option(Value, "request", translate("Request"), translate("Encoding for responding to requests/events"))
o.rmempty = true
o.placeholder = "utf-8"

o = s:option(Value, "stock", translate("Storage"), translate("Encoding for storing local collections"))
o.rmempty = true
o.placeholder = "utf-8"

return m
