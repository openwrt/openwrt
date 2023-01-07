-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Licensed to the public under the Apache License 2.0.

local fs = require("nixio.fs")

local basicParams = {
	--
	-- Widget, Name, Default(s), Description
	--
	{ ListValue,
		"verb",
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		translate("Set output verbosity") },
	{ Value,
		"nice",
		0,
		translate("Change process priority") },
	{ Value,
		"port",
		1194,
		translate("TCP/UDP port # for both local and remote") },
	{ ListValue,
		"dev_type",
		{ "tun", "tap" },
		translate("Type of used device") },
	{ Value,
		"ifconfig",
		"10.200.200.3 10.200.200.1",
		translate("Set tun/tap adapter parameters") },
	{ Value,
		"server",
		"10.200.200.0 255.255.255.0",
		translate("Configure server mode") },
	{ Value,
		"server_bridge",
		"192.168.1.1 255.255.255.0 192.168.1.128 192.168.1.254",
		translate("Configure server bridge") },
	{ Flag,
		"nobind",
		0,
		translate("Do not bind to local address and port") },
	{ ListValue,
		"comp_lzo",
		{"yes","no","adaptive"},
		translate("Use fast LZO compression") },
	{ Value,
		"keepalive",
		"10 60",
		translate("Helper directive to simplify the expression of --ping and --ping-restart in server mode configurations") },
	{ Flag,
		"client",
		0,
		translate("Configure client mode") },
	{ Flag,
		"client_to_client",
		0,
		translate("Allow client-to-client traffic") },
	{ DynamicList,
		"remote",
		"vpnserver.example.org",
		translate("Remote host name or ip address") },
	{ FileUpload,
		"secret",
		"/etc/openvpn/secret.key",
		translate("Enable Static Key encryption mode (non-TLS)") },
	{ ListValue,
		"key_direction",
		{ 0, 1 },
		translate("The key direction for 'tls-auth' and 'secret' options") },
	{ FileUpload,
		"pkcs12",
		"/etc/easy-rsa/keys/some-client.pk12",
		translate("PKCS#12 file containing keys") },
	{ FileUpload,
		"ca",
		"/etc/easy-rsa/keys/ca.crt",
		translate("Certificate authority") },
	{ FileUpload,
		"dh",
		"/etc/easy-rsa/keys/dh1024.pem",
		translate("Diffie Hellman parameters") },
	{ FileUpload,
		"cert",
		"/etc/easy-rsa/keys/some-client.crt",
		translate("Local certificate") },
	{ FileUpload,
		"key",
		"/etc/easy-rsa/keys/some-client.key",
		translate("Local private key") },
	{ FileUpload,
		"auth_user_pass",
		"/etc/easy-rsa/keys/some-client.pass",
		translate("Password file") },
}

local has_ipv6 = fs.access("/proc/net/ipv6_route")
if has_ipv6 then
	table.insert( basicParams, { ListValue,
		"proto",
		{ "udp", "tcp-client", "tcp-server", "udp4", "tcp4-client", "tcp4-server","udp6", "tcp6-client", "tcp6-server" },
		translate("Use protocol")
	})
else
	table.insert( basicParams, { ListValue,
		"proto",
		{ "udp", "tcp-client", "tcp-server" },
		translate("Use protocol")
	})
end

local m = Map("openvpn")
m.redirect = luci.dispatcher.build_url("admin", "vpn", "openvpn")
m.apply_on_parse = true

local p = m:section( SimpleSection )
p.template = "openvpn/pageswitch"
p.mode     = "basic"
p.instance = arg[1]


local s = m:section( NamedSection, arg[1], "openvpn" )

for _, option in ipairs(basicParams) do
	local o = s:option(
		option[1], option[2],
		option[2], option[4]
	)

	o.optional = true

	if option[1] == DummyValue then
		o.value = option[3]
	elseif option[1] == FileUpload then

		o.initial_directory = "/etc/openvpn"

		function o.cfgvalue(self, section)
			local cfg_val = AbstractValue.cfgvalue(self, section)

			if cfg_val then
				return cfg_val
			end
		end

		function o.formvalue(self, section)
			local sel_val = AbstractValue.formvalue(self, section)
			local txt_val = luci.http.formvalue("cbid."..self.map.config.."."..section.."."..self.option..".textbox")

			if sel_val and sel_val ~= "" then
				return sel_val
			end

			if txt_val and txt_val ~= "" then
				return txt_val
			end
		end

		function o.remove(self, section)
			local cfg_val = AbstractValue.cfgvalue(self, section)
			local txt_val = luci.http.formvalue("cbid."..self.map.config.."."..section.."."..self.option..".textbox")
			
			if cfg_val and fs.access(cfg_val) and txt_val == "" then
				fs.unlink(cfg_val)
			end
			return AbstractValue.remove(self, section)
		end
	elseif option[1] == Flag then
		o.default = nil
	else
		if option[1] == DynamicList then
			function o.cfgvalue(...)
				local val = AbstractValue.cfgvalue(...)
				return ( val and type(val) ~= "table" ) and { val } or val
			end
		end

		if type(option[3]) == "table" then
			if o.optional then o:value("", "-- remove --") end
			for _, v in ipairs(option[3]) do
				v = tostring(v)
				o:value(v)
			end
			o.default = tostring(option[3][1])
		else
			o.default = tostring(option[3])
		end
	end

	for i=5,#option do
		if type(option[i]) == "table" then
			o:depends(option[i])
		end
	end
end

return m
