-- Copyright 2017-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs      = require("nixio.fs")
local uci     = require("luci.model.uci").cursor()
local http    = require("luci.http")
local util    = require("luci.util")
local scripts = util.split(util.trim(util.exec("ls /etc/travelmate/*.login 2>/dev/null")), "\n", nil, true) or {}

m = SimpleForm("edit", translate("Edit Wireless Uplink Configuration"))
m.submit = translate("Save")
m.cancel = translate("Back to overview")
m.reset = false

function m.on_cancel()
	http.redirect(luci.dispatcher.build_url("admin/services/travelmate/stations"))
end

m.hidden = {
	cfg = http.formvalue("cfg")
}

local s = uci:get_all("wireless", m.hidden.cfg)

if s ~= nil then
	wssid = m:field(Value, "ssid", translate("SSID"))
	wssid.datatype = "rangelength(1,32)"
	wssid.default = s.ssid or ""

	bssid = m:field(Value, "bssid", translate("BSSID"))
	bssid.datatype = "macaddr"
	bssid.default = s.bssid or ""

	if string.match(s.encryption, '%+') and not string.match(s.encryption, '^wep') then
		s.pos = string.find(s.encryption, '%+')
		s.cipher = string.sub(s.encryption, s.pos + 1)
		s.encryption = string.sub(s.encryption, 0, s.pos - 1)
	end

	if s.encryption and s.encryption ~= "none" then
		if string.match(s.encryption, '^wep') then
			encr = m:field(ListValue, "encryption", translate("Encryption"))
			encr:value("wep", "WEP")
			encr:value("wep+open", "WEP Open System")
			encr:value("wep+mixed", "WEP mixed")
			encr:value("wep+shared", "WEP Shared Key")
			encr.default = s.encryption

			wkey = m:field(Value, "key", translate("Passphrase"))
			wkey.datatype = "wepkey"
			wkey.password = true
			wkey.default = s.key
		elseif string.match(s.encryption, '^wpa') then
			encr = m:field(ListValue, "encryption", translate("Encryption"))
			encr:value("wpa", "WPA Enterprise")
			encr:value("wpa-mixed", "WPA/WPA2 Enterprise mixed")
			encr:value("wpa2", "WPA2 Enterprise")
			encr.default = s.encryption

			ciph = m:field(ListValue, "cipher", translate("Cipher"))
			ciph:value("auto", translate("Automatic"))
			ciph:value("ccmp", translate("Force CCMP (AES)"))
			ciph:value("tkip", translate("Force TKIP"))
			ciph:value("tkip+ccmp", translate("Force TKIP and CCMP (AES)"))
			ciph.default = s.cipher

			eaptype = m:field(ListValue, "eap_type", translate("EAP-Method"))
			eaptype:value("tls", "TLS")
			eaptype:value("ttls", "TTLS")
			eaptype:value("peap", "PEAP")
			eaptype:value("fast", "FAST")
			eaptype.default = s.eap_type or "peap"

			authentication = m:field(ListValue, "auth", translate("Authentication"))
			authentication:value("PAP")
			authentication:value("CHAP")
			authentication:value("MSCHAP")
			authentication:value("MSCHAPV2")
			authentication:value("EAP-GTC")
			authentication:value("EAP-MD5")
			authentication:value("EAP-MSCHAPV2")
			authentication:value("EAP-TLS")
			authentication:value("auth=PAP")
			authentication:value("auth=MSCHAPV2")
			authentication.default = s.auth or "EAP-MSCHAPV2"

			ident = m:field(Value, "identity", translate("Identity"))
			ident.default = s.identity or ""

			wkey = m:field(Value, "password", translate("Passphrase"))
			wkey.datatype = "wpakey"
			wkey.password = true
			wkey.default = s.password

			cacert = m:field(Value, "ca_cert", translate("Path to CA-Certificate"))
			cacert.rmempty = true
			cacert.default = s.ca_cert or ""
			
			clientcert = m:field(Value, "client_cert", translate("Path to Client-Certificate"))
			clientcert:depends("eap_type","tls")
			clientcert.rmempty = true
			clientcert.default = s.client_cert or ""

			privkey = m:field(Value, "priv_key", translate("Path to Private Key"))
			privkey:depends("eap_type","tls")
			privkey.rmempty = true
			privkey.default = s.priv_key or ""

			privkeypwd = m:field(Value, "priv_key_pwd", translate("Password of Private Key"))
			privkeypwd:depends("eap_type","tls")
			privkeypwd.datatype = "wpakey"
			privkeypwd.password = true
			privkeypwd.rmempty = true
			privkeypwd.default = s.priv_key_pwd or ""
		else
			encr = m:field(ListValue, "encryption", translate("Encryption"))
			encr:value("psk", "WPA-PSK")
			encr:value("psk2", "WPA2-PSK")
			encr:value("psk-mixed", "WPA/WPA2 mixed")
			encr:value("sae", "WPA3-SAE")
			encr:value("owe", "OWE (open network)")
			encr:value("sae-mixed", "WPA2/WPA3 mixed")
			encr.default = s.encryption

			ciph = m:field(ListValue, "cipher", translate("Cipher"))
			ciph:value("auto", translate("Automatic"))
			ciph:value("ccmp", translate("Force CCMP (AES)"))
			ciph:value("tkip", translate("Force TKIP"))
			ciph:value("tkip+ccmp", translate("Force TKIP and CCMP (AES)"))
			ciph:depends("encryption", "psk")
			ciph:depends("encryption", "psk2")
			ciph:depends("encryption", "psk-mixed")
			ciph.default = s.cipher or "auto"

			wkey = m:field(Value, "key", translate("Passphrase"))
			wkey.datatype = "wpakey"
			wkey.password = true
			wkey:depends("encryption", "psk")
			wkey:depends("encryption", "psk2")
			wkey:depends("encryption", "psk-mixed")
			wkey:depends("encryption", "sae")
			wkey:depends("encryption", "sae-mixed")
			wkey.default = s.key
		end
	end
else
	m.on_cancel()
end

local login_section = (s.device or "") .. "_" .. (s.ssid or "") .. "_" .. (s.bssid or "")
login_section = login_section:gsub("[^%w_]", "_")
local cmd = uci:get("travelmate", login_section, "command")
local cmd_args_default = uci:get("travelmate", login_section, "command_args")
cmd_list = m:field(ListValue, "cmdlist", translate("Auto Login Script"),
	translate("External script reference which will be called for automated captive portal logins."))
cmd_args = m:field(Value, "cmdargs", translate("Optional Arguments"),
	translate("Space separated list of additional arguments passed to the Auto Login Script, i.e. username and password"))
for _, z in ipairs(scripts) do
	cmd_list:value(z)
	cmd_args:depends("cmdlist", z)
end
cmd_list:value("none")
cmd_list.default = cmd or "none"
cmd_args.default = cmd_args_default

function wssid.write(self, section, value)
	uci:set("wireless", m.hidden.cfg, "ssid", wssid:formvalue(section))
	uci:set("wireless", m.hidden.cfg, "bssid", bssid:formvalue(section))
	if encr then
		if string.find(encr:formvalue(section), '^wep') then
			uci:set("wireless", m.hidden.cfg, "encryption", encr:formvalue(section))
			uci:set("wireless", m.hidden.cfg, "key", wkey:formvalue(section) or "")
		elseif string.find(encr:formvalue(section), '^wpa') then
			uci:set("wireless", m.hidden.cfg, "eap_type", eaptype:formvalue(section))
			uci:set("wireless", m.hidden.cfg, "auth", authentication:formvalue(section))
			uci:set("wireless", m.hidden.cfg, "identity", ident:formvalue(section) or "")
			uci:set("wireless", m.hidden.cfg, "password", wkey:formvalue(section) or "")
			uci:set("wireless", m.hidden.cfg, "ca_cert", cacert:formvalue(section) or "")
			uci:set("wireless", m.hidden.cfg, "client_cert", clientcert:formvalue(section) or "")
			uci:set("wireless", m.hidden.cfg, "priv_key", privkey:formvalue(section) or "")
			uci:set("wireless", m.hidden.cfg, "priv_key_pwd", privkeypwd:formvalue(section) or "")
		elseif encr:formvalue(section) ~= "owe" then
			uci:set("wireless", m.hidden.cfg, "key", wkey:formvalue(section) or "")
		end
		if ciph and ciph:formvalue(section) and ciph:formvalue(section) ~= "auto" then
			uci:set("wireless", m.hidden.cfg, "encryption", encr:formvalue(section) .. "+" .. ciph:formvalue(section))
		else
			uci:set("wireless", m.hidden.cfg, "encryption", encr:formvalue(section))
		end
	end

	if not uci:get("travelmate", login_section) and cmd_list:formvalue(section) ~= "none" then
		uci:set("travelmate", login_section, "login")
	end
	if uci:get("travelmate", login_section) then
		uci:set("travelmate", login_section, "command", cmd_list:formvalue(section))
		uci:set("travelmate", login_section, "command_args", cmd_args:formvalue(section))
		uci:save("travelmate")
		uci:commit("travelmate")
	end
	uci:save("wireless")
	uci:commit("wireless")
	luci.sys.call("env -i /bin/ubus call network reload >/dev/null 2>&1")
	m.on_cancel()
end

return m
