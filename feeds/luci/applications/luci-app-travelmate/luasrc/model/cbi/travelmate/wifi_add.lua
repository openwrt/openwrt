-- Copyright 2017-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs       = require("nixio.fs")
local uci      = require("luci.model.uci").cursor()
local http     = require("luci.http")
local util     = require("luci.util")
local scripts  = util.split(util.trim(util.exec("ls /etc/travelmate/*.login 2>/dev/null")), "\n", nil, true) or {}
local trmiface = uci:get("travelmate", "global", "trm_iface") or "trm_wwan"
local encr_psk = {"psk", "psk2", "psk-mixed", "sae", "owe", "sae-mixed"}
local encr_wpa = {"wpa", "wpa2", "wpa-mixed"}

m = SimpleForm("add", translate("Add Wireless Uplink Configuration"))
m.submit = translate("Save")
m.cancel = translate("Back to overview")
m.reset = false

function m.on_cancel()
	http.redirect(luci.dispatcher.build_url("admin/services/travelmate/stations"))
end

m.hidden = {
	device      = http.formvalue("device"),
	ssid        = http.formvalue("ssid"),
	bssid       = http.formvalue("bssid"),
	description = http.formvalue("description"),
	wep         = tonumber(http.formvalue("wep")) or 0,
	wpa_suites  = http.formvalue("wpa_suites"),
	wpa_version = tonumber(http.formvalue("wpa_version")) or 0
}

if m.hidden.wpa_version == 4 then
	if string.find(m.hidden.description, "OWE") then
		m.hidden.wpa_version = 5
	end
end

if m.hidden.ssid == "" then
	wssid = m:field(Value, "ssid", translate("SSID (hidden)"))
else
	wssid = m:field(Value, "ssid", translate("SSID"))
end
wssid.datatype = "rangelength(1,32)"
wssid.default = m.hidden.ssid or ""

nobssid = m:field(Flag, "no_bssid", translate("Ignore BSSID"))
if m.hidden.ssid == "" then
	nobssid.default = nobssid.disabled
else
	nobssid.default = nobssid.enabled
end

bssid = m:field(Value, "bssid", translate("BSSID"),
	translatef("The BSSID information '%s' is optional and only required for hidden networks", m.hidden.bssid or ""))
bssid:depends("no_bssid", 0)
bssid.datatype = "macaddr"
bssid.default = m.hidden.bssid or ""

if m.hidden.wep == 1 then
	encr = m:field(ListValue, "encryption", translate("Encryption"))
	encr:value("wep", "WEP")
	encr:value("wep+open", "WEP Open System")
	encr:value("wep+mixed", "WEP mixed")
	encr:value("wep+shared", "WEP Shared Key")
	encr.default = "wep+open"

	wkey = m:field(Value, "key", translate("WEP-Passphrase"))
	wkey.password = true
	wkey.datatype = "wepkey"
elseif m.hidden.wpa_version > 0 then
	if m.hidden.wpa_suites == "802.1X" then
		encr = m:field(ListValue, "encryption", translate("Encryption"))
		encr:value("wpa", "WPA Enterprise")
		encr:value("wpa-mixed", "WPA/WPA2 Enterprise mixed")
		encr:value("wpa2", "WPA2 Enterprise")
		encr.default = encr_wpa[m.hidden.wpa_version] or "wpa2"

		ciph = m:field(ListValue, "cipher", translate("Cipher"))
		ciph:value("auto", translate("Automatic"))
		ciph:value("ccmp", translate("Force CCMP (AES)"))
		ciph:value("tkip", translate("Force TKIP"))
		ciph:value("tkip+ccmp", translate("Force TKIP and CCMP (AES)"))
		ciph.default = "auto"

		eaptype = m:field(ListValue, "eap_type", translate("EAP-Method"))
		eaptype:value("tls", "TLS")
		eaptype:value("ttls", "TTLS")
		eaptype:value("peap", "PEAP")
		eaptype:value("fast", "FAST")
		eaptype.default = "peap"

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
		authentication.default = "EAP-MSCHAPV2"

		ident = m:field(Value, "identity", translate("Identity"))

		wkey = m:field(Value, "password", translate("Password"))
		wkey.password = true
		wkey.datatype = "wpakey"

		cacert = m:field(Value, "ca_cert", translate("Path to CA-Certificate"))
		cacert.rmempty = true

		clientcert = m:field(Value, "client_cert", translate("Path to Client-Certificate"))
		clientcert:depends("eap_type","tls")
		clientcert.rmempty = true

		privkey = m:field(Value, "priv_key", translate("Path to Private Key"))
		privkey:depends("eap_type","tls")
		privkey.rmempty = true

		privkeypwd = m:field(Value, "priv_key_pwd", translate("Password of Private Key"))
		privkeypwd:depends("eap_type","tls")
		privkeypwd.datatype = "wpakey"
		privkeypwd.password = true
		privkeypwd.rmempty = true
	else
		encr = m:field(ListValue, "encryption", translate("Encryption"))
		encr:value("psk", "WPA-PSK")
		encr:value("psk2", "WPA2-PSK")
		encr:value("psk-mixed", "WPA/WPA2 mixed")
		encr:value("sae", "WPA3-SAE")
		encr:value("owe", "OWE (open network)")
		encr:value("sae-mixed", "WPA2/WPA3 mixed")
		encr.default = encr_psk[m.hidden.wpa_version] or "psk2"

		ciph = m:field(ListValue, "cipher", translate("Cipher"))
		ciph:value("auto", translate("Automatic"))
		ciph:value("ccmp", translate("Force CCMP (AES)"))
		ciph:value("tkip", translate("Force TKIP"))
		ciph:value("tkip+ccmp", translate("Force TKIP and CCMP (AES)"))
		ciph:depends("encryption", "psk")
		ciph:depends("encryption", "psk2")
		ciph:depends("encryption", "psk-mixed")
		ciph.default = "auto"

		wkey = m:field(Value, "key", translate("WPA-Passphrase"))
		wkey.password = true
		wkey.datatype = "wpakey"
		wkey:depends("encryption", "psk")
		wkey:depends("encryption", "psk2")
		wkey:depends("encryption", "psk-mixed")
		wkey:depends("encryption", "sae")
		wkey:depends("encryption", "sae-mixed")
	end
end

local login_section = (m.hidden.device or "") .. "_" .. (m.hidden.ssid or "") .. "_" .. (m.hidden.bssid or "")
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
	login_section = (m.hidden.device or "") .. "_" .. (wssid:formvalue(section) or "") .. "_" .. (bssid:formvalue(section) or "")
	login_section = login_section:gsub("[^%w_]", "_")
	newsection = uci:section("wireless", "wifi-iface", login_section, {
		mode     = "sta",
		network  = trmiface,
		device   = m.hidden.device,
		ssid     = wssid:formvalue(section),
		bssid    = bssid:formvalue(section),
		disabled = "1"
	})

	if encr then
		if string.find(encr:formvalue(section), '^wep') then
			uci:set("wireless", newsection, "encryption", encr:formvalue(section))
			uci:set("wireless", newsection, "key", wkey:formvalue(section) or "")
		elseif string.find(encr:formvalue(section), '^wpa') then
			uci:set("wireless", newsection, "eap_type", eaptype:formvalue(section))
			uci:set("wireless", newsection, "auth", authentication:formvalue(section))
			uci:set("wireless", newsection, "identity", ident:formvalue(section) or "")
			uci:set("wireless", newsection, "password", wkey:formvalue(section) or "")
			uci:set("wireless", newsection, "ca_cert", cacert:formvalue(section) or "")
			uci:set("wireless", newsection, "client_cert", clientcert:formvalue(section) or "")
			uci:set("wireless", newsection, "priv_key", privkey:formvalue(section) or "")
			uci:set("wireless", newsection, "priv_key_pwd", privkeypwd:formvalue(section) or "")
		elseif encr:formvalue(section) ~= "owe" then
			uci:set("wireless", newsection, "key", wkey:formvalue(section) or "")
		end
		if ciph and ciph:formvalue(section) and ciph:formvalue(section) ~= "auto" then
			uci:set("wireless", newsection, "encryption", encr:formvalue(section) .. "+" .. ciph:formvalue(section))
		else
			uci:set("wireless", newsection, "encryption", encr:formvalue(section))
		end
	else
		uci:set("wireless", newsection, "encryption", "none")
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
	http.redirect(luci.dispatcher.build_url("admin/services/travelmate/stations"))
end

return m
