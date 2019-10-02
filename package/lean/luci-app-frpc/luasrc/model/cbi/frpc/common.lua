-- Copyright 2019 Xingwang Liao <kuoruan@gmail.com>
-- Licensed to the public under the MIT License.

local uci = require "luci.model.uci".cursor()
local util = require "luci.util"
local fs = require "nixio.fs"
local sys = require "luci.sys"

local m, s, o
local server_table = { }

uci:foreach("frpc", "server", function(s)
	if s.alias then
		server_table[s[".name"]] = s.alias
	elseif s.server_addr and s.server_port then
		local ip = s.server_addr
		if s.server_addr:find(":") then
			ip = "[%s]" % s.server_addr
		end
		server_table[s[".name"]] = "%s:%s" % { ip, s.server_port }
	end
end)

local function frpc_version()
	local file = uci:get("frpc", "main", "client_file")

	if not file or file == "" or not fs.stat(file) then
		return "<em style=\"color: red;\">%s</em>" % translate("Invalid client file")
	end

	if not fs.access(file, "rwx", "rx", "rx") then
		fs.chmod(file, 755)
	end

	local version = util.trim(sys.exec("%s -v 2>/dev/null" % file))
	if version == "" then
		return "<em style=\"color: red;\">%s</em>" % translate("Can't get client version")
	end
	return translatef("Version: %s", version)
end

m = Map("frpc", "%s - %s" % { translate("Frpc"), translate("Common Settings") },
"<p>%s</p><p>%s</p>" % {
	translate("Frp is a fast reverse proxy to help you expose a local server behind a NAT or firewall to the internet."),
	translatef("For more information, please visit: %s",
		"<a href=\"https://github.com/fatedier/frp\" target=\"_blank\">https://github.com/fatedier/frp</a>")
})

m:append(Template("frpc/status_header"))

s = m:section(NamedSection, "main", "frpc")
s.addremove = false
s.anonymous = true

s:tab("general", translate("General Options"))
s:tab("advanced", translate("Advanced Options"))
s:tab("manage", translate("Manage Options"))

o = s:taboption("general", Flag, "enabled", translate("Enabled"))

o = s:taboption("general", Value, "edition", translate("Edition"))
o:value("0.27.0")
o:value("0.26.0")
o:value("0.25.3")
o.rmempty = true

o = s:taboption("general", Button, "updatead", translate("update frpc"))
o.inputtitle = translate("update frpc")
o.inputstyle = "apply"
o.write = function()
	sys.call("nohup sh /usr/share/frpc/update.sh > /tmp/frpc.log 2>&1 &")
end

o = s:taboption("general", Value, "client_file", translate("Client file"), frpc_version())
o.rmempty = false

o = s:taboption("general", ListValue, "server", translate("Server"))
o:value("", translate("None"))
for k, v in pairs(server_table) do
	o:value(k, v)
end

o = s:taboption("general", ListValue, "run_user", translate("Run daemon as user"))
o:value("", translate("-- default --"))
local user
for user in util.execi("cat /etc/passwd | cut -d':' -f1") do
	o:value(user)
end

o = s:taboption("general", Flag, "enable_logging", translate("Enable logging"))

o = s:taboption("general", Value, "log_file", translate("Log file"))
o:depends("enable_logging", "1")
o.placeholder = "/var/log/frpc.log"

o = s:taboption("general", ListValue, "log_level", translate("Log level"))
o:depends("enable_logging", "1")
o:value("trace", translate("Trace"))
o:value("debug", translate("Debug"))
o:value("info", translate("Info"))
o:value("warn", translate("Warn"))
o:value("error", translate("Error"))
o.default = "warn"

o = s:taboption("general", Value, "log_max_days", translate("Log max days"))
o:depends("enable_logging", "1")
o.datatype = "uinteger"
o.placeholder = '3'

o = s:taboption("advanced", Value, "pool_count", translate("Pool count"),
	translate("Connections will be established in advance, default value is zero"))
o.datatype = "uinteger"
o.defalut = '0'
o.placeholder = '0'

o = s:taboption("advanced", Value, "user", translate("Proxy user"),
	translate("Your proxy name will be changed to {user}.{proxy}"))

o = s:taboption("advanced", Flag, "login_fail_exit", translate("Login fail exit"))
o.enabled = "true"
o.disabled = "false"
o.defalut = o.enabled
o.rmempty = false

o = s:taboption("advanced", ListValue, "protocol", translate("Protocol"),
	translate("Communication protocol used to connect to server, default is tcp"))
o:value("tcp", "TCP")
o:value("kcp", "KCP")
o:value("websocket", "Websocket")
o.default = "tcp"

o = s:taboption("advanced", Value, "http_proxy", translate("HTTP proxy"),
	translate("Connect frps by http proxy or socks5 proxy, format: [protocol]://[user]:[passwd]@[ip]:[port]"))

o = s:taboption("advanced", Flag, "tls_enable", translate("TLS enable"),
	translate("If true, Frpc will connect Frps by TLS"))
o.enabled = "true"
o.disabled = "false"

o = s:taboption("advanced", Value, "dns_server", translate("DNS server"))
o.datatype = "host"

o = s:taboption("advanced", Value, "heartbeat_interval", translate("Heartbeat interval"))
o.datatype = "uinteger"
o.placeholder = "30"

o = s:taboption("advanced", Value, "heartbeat_timeout", translate("Heartbeat timeout"))
o.datatype = "uinteger"
o.placeholder = "90"

o = s:taboption("manage", Value, "admin_addr", translate("Admin addr"))
o.datatype = "host"

o = s:taboption("manage", Value, "admin_port", translate("Admin port"))
o.datatype = "port"

o = s:taboption("manage", Value, "admin_user", translate("Admin user"))

o = s:taboption("manage", Value, "admin_pwd", translate("Admin password"))
o.password = true

return m
