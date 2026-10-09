'use strict';

import {
	append, append_raw, append_vars, network_append, network_append_raw, network_append_vars,
	network_append_string_vars, set_default, dump_network, flush_network,
	wiphy_info, wiphy_band
} from 'wifi.common';
import * as netifd from 'wifi.netifd';
import * as iface from 'wifi.iface';
import * as fs from 'fs';

function set_fixed_freq(data, config) {
	if (!data.frequency)
		return;

	set_default(config, 'fixed_freq', 1);
	set_default(config, 'frequency', data.frequency);

	if (data.htmode in [ 'VHT80', 'HE80', 'EHT80' ])
		set_default(config, 'max_oper_chwidth', 1);
	else if (data.htmode in [ 'VHT160', 'HE160', 'EHT160' ])
		set_default(config, 'max_oper_chwidth', 2);
	else if (data.htmode in [ 'EHT320' ])
		set_default(config, 'max_oper_chwidth', 9);
	else if (data.htmode in [ 'VHT20', 'VHT40', 'HE20', 'HE40', 'EHT20', 'EHT40' ])
		set_default(config, 'max_oper_chwidth', 0);
	else
		set_default(config, 'disable_vht', true);

	if (data.htmode in [ 'NOHT' ])
		set_default(config, 'disable_ht', true);
	else if (data.htmode in [ 'HT20', 'VHT20', 'HE20', 'EHT20' ])
		set_default(config, 'disable_ht40', true);
	else if (data.htmode in [ 'VHT40', 'VHT80', 'VHT160', 'HE40', 'HE80', 'HE160', 'EHT40', 'EHT80', 'EHT160', 'EHT320' ])
		set_default(config, 'ht40', true);

	if (wildcard(data.htmode, 'VHT*'))
		set_default(config, 'vht', 1);

	if (config.mode in [ 'sta', 'adhoc', 'mesh' ])
		set_default(config, 'noscan', true);
}

export function ratestr(rate) {
	if (rate == null)
		return rate;

	let rem = (rate / 100) % 10;
	rate = int(rate / 1000);
	if (rem > 0)
		rate += "." + rem;

	return "" + rate;
};

export function ratelist(rates) {
	if (length(rates) < 1)
		return null;

	return join(",", map(rates, (rate) => ratestr(rate)));
};

const sae_compat_re = /^sae-compat/;

function setup_sta(data, config, phy_features) {
	/* WPA3 Specification v3.5 2.4: no separate Compatibility Mode for STAs */
	if (config.mode == 'sta' && config.encryption)
		config.encryption = replace(config.encryption, sae_compat_re, 'sae-mixed');

	/* use what a 6 GHz AP with the same configuration offers */
	if (config.mode == 'sta')
		config.encryption = iface.encryption_sta_band(config.encryption, data.band,
			config.mlo ? config.mlo_bands : null);

	/* WPA3 Specification v3.5 2.5 items 9 and 10: "A STA that enables EHT
	 * or MLO shall, in its Network Profile, allow AKM suite selector
	 * 00-0F-AC:24" and "allow GCMP-256 to be selected as a pairwise cipher" */
	let eht = wildcard(data.htmode ?? '', 'EHT*');
	if (eht)
		set_default(config, 'sae_ext_key', true);
	let sta_gcmp256 = eht && config.mode == 'sta' && config.gcmp256 !== false &&
		phy_features?.cipher_gcmp256;

	iface.parse_encryption(config, data, phy_features);

	if (sta_gcmp256 && (config.wpa & 2) && config.wpa_pairwise == 'CCMP')
		config.wpa_pairwise = 'CCMP GCMP-256';

	if (config.auth_type in [ 'sae', 'owe', 'eap2', 'eap192', 'dpp' ])
		config.ieee80211w = 2;
	else if (config.auth_type in [ 'psk-sae', 'eap-eap2' ] && !config.ieee80211w)
		config.ieee80211w = 1;

	/* Easy Connect 3.0 8.4.2: PMF for every association with the DPP AKM,
	 * which wpa_supplicant does not enable on its own (wpas_get_ssid_pmf()). */
	if (config.dpp && !config.ieee80211w)
		config.ieee80211w = 1;

	if ((wildcard(data.htmode, 'EHT*') || wildcard(data.htmode, 'HE*')) &&
		config.rsn_override)
		config.rsn_overriding = 1;
	else
		config.rsn_overriding = 0;

	set_default(config, 'ieee80211r', 0);
	set_default(config, 'sae_pwe', 2);
	set_default(config, 'multi_ap', 0);
	set_default(config, 'multi_profile', 1);
	set_default(config, 'default_disabled', 0);

	config.scan_ssid = 1;

	switch(config.mode) {
	case 'sta':
		set_default(config, 'multi_ap_backhaul_sta', config.multi_ap);
		break;

	case 'adhoc':
		config.ap_scan = 2;
		config.scan_ssid = 0;
		network_append('mode', 1);
		set_fixed_freq(data, config);
		break;

	case 'mesh':
		config.ssid = config.mesh_id;
		config.scan_ssid = null;
		network_append('mode', 5);

		set_fixed_freq(data, config);

		if (config.encryption && config.encryption != 'none')
			config.key_mgmt = 'SAE';

		config.ieee80211w = null;
		break;
	}

	if (config.mode != 'mesh' ) {
		switch(config.wpa) {
		case 1:
			config.proto = 'WPA';
			break;

		case 2:
			config.proto = 'RSN';
			break;
		}
	}

	switch(config.auth_type) {
	case 'none':
		break;

	case 'owe':
		iface.wpa_key_mgmt(config);
		break;

	case 'dpp':
		iface.wpa_key_mgmt(config);
		break;

	case 'wps':
		config.key_mgmt = 'WPS';
		break;

	case 'psk':
	case 'psk2':
	case 'sae':
	case 'psk-sae':
		if (config.mode != 'mesh')
			iface.wpa_key_mgmt(config);

		if (config.mode == 'mesh' || config.auth_type == 'sae')
			config.sae_password = `"${config.key}"`;
		else if (length(config.key) == 64)
			config.psk = config.key;
		else
			config.psk = `"${config.key}"`;

		break;

	case 'eap':
	case 'eap2':
	case 'eap-eap2':
	case 'eap192':
		iface.wpa_key_mgmt(config);
		set_default(config, 'erp', config.fils);

		if (config.ca_cert_usesystem && fs.stat('/etc/ssl/certs/ca-certificates.crt'))
			config.ca_cert = '/etc/ssl/certs/ca-certificates.crt';

		const eap_method_map = { fast: 'FAST', peap: 'PEAP', ttls: 'TTLS', tls: 'TLS' };
		if (eap_method_map[config.eap_type])
			config.eap = eap_method_map[config.eap_type];

		switch(config.eap_type) {
		case 'fast':
		case 'peap':
		case 'ttls':
			set_default(config, 'auth', 'MSCHAPV2');

			let auth = config.auth;
			let phase2proto = 'auth=';
			if (index(auth, 'auth') == 0) {
				/* user already provided a full "auth=..." spec */
				phase2proto = '';
			} else if (index(auth, 'EAP-') == 0) {
				/* inner EAP method, e.g. EAP-MSCHAPV2 -> MSCHAPV2 */
				auth = substr(auth, 4);
				if (config.eap_type == 'ttls')
					phase2proto = 'autheap=';
			}
			config.phase2 = `"${phase2proto}${auth}"`;

			if (config.auth == 'EAP-TLS') {
				if (config.ca_cert2_usesystem && fs.stat('/etc/ssl/certs/ca-certificates.crt'))
					config.ca_cert2 = '/etc/ssl/certs/ca-certificates.crt';
			}
			break;
		}

	}

	if (config.wpa_pairwise == 'GCMP') {
		config.pairwise = 'GCMP';
		config.group = 'GCMP';
	} else if (config.wpa_pairwise) {
		config.pairwise = config.wpa_pairwise;
	}

	config.key_mgmt ??= 'NONE';

	/*
	 * Map UCI basic_rate to the correct wpa_supplicant network field:
	 *   mesh  -> mesh_basic_rates  (space-separated, 100 kb/s units)
	 *   other -> rates             (comma-separated Mbps, e.g. "5.5,11")
	 * "basic_rate" itself is not a valid wpa_supplicant network field.
	 */
	let brates = config.basic_rate;
	config.basic_rate = null;
	if (brates != null && length(brates) > 0) {
		if (config.mode == 'mesh')
			config.mesh_basic_rates = join(" ", map(brates, (br) => "" + int(br / 100)));
		else
			config.rates = ratelist(brates);
	}

	config.mcast_rate = ratestr(config.mcast_rate);

	/*
	 * Certificate constraint lists are semicolon-separated strings in the
	 * wpa_supplicant config, while UCI stores them as arrays. Join them here
	 * so they are emitted as a single quoted value below.
	 */
	for (let key in [ 'altsubject_match', 'altsubject_match2',
			  'domain_match', 'domain_match2',
			  'domain_suffix_match', 'domain_suffix_match2' ])
		if (type(config[key]) == 'array')
			config[key] = length(config[key]) ? join(';', config[key]) : null;

	network_append_string_vars(config, [ 'ssid',
		'identity', 'anonymous_identity', 'password',
		'ca_cert', 'ca_cert2', 'client_cert', 'client_cert2',
		'subject_match', 'subject_match2',
		'altsubject_match', 'altsubject_match2',
		'domain_match', 'domain_match2',
		'domain_suffix_match', 'domain_suffix_match2',
		'private_key', 'private_key_passwd', 'private_key2', 'private_key2_passwd',
		'dpp_connector',
		 ]);
	network_append_vars(config, [
		'rsn_overriding', 'scan_ssid', 'noscan', 'disabled', 'multi_ap_profile', 'multi_ap_backhaul_sta',
		'ocv', 'beacon_prot', 'key_mgmt', 'sae_pwe', 'psk', 'sae_password', 'pairwise', 'group', 'bssid',
		'proto', 'mesh_fwding', 'mesh_rssi_threshold', 'frequency', 'fixed_freq',
		'disable_ht', 'disable_ht40', 'disable_vht', 'vht', 'max_oper_chwidth',
		'ht40', 'beacon_int', 'ieee80211w', 'rates', 'mesh_basic_rates', 'mcast_rate',
		'bssid_blacklist', 'bssid_whitelist', 'erp', 'eap', 'phase2',
		'dpp_csign', 'dpp_netaccesskey',
	]);
}


function freq_in_range(freq_ranges, freq)
{
	if (!freq_ranges)
		return true;

	freq *= 1000;
	for (let range in freq_ranges)
		if (freq >= range.start && freq <= range.end)
			return true;
}

function wiphy_frequencies(phy, band, radio) {
	phy = wiphy_info(phy);
	band = wiphy_band(phy, band);
	if (!band)
		return;

	let ranges;
	for (let r in phy.radios)
		if (r.index == radio)
			ranges = r.freq_ranges;

	let freqs = [];
	for (let chan in band.freqs)
		if (!chan.disabled && freq_in_range(ranges, chan.freq))
			push(freqs, chan.freq);

	if (length(freqs) > 0)
		return freqs;
}


export function generate(config_list, data, interface) {
	flush_network();

	if (interface.bridge &&
	    (interface.config.mode == 'adhoc' ||
	     (interface.config.mode == 'sta' && !interface.config.wds && !interface.config.multi_ap))){
		netifd.setup_failed('BRIDGE_NOT_ALLOWED');
		return 1;
	}

	/* RECONFIGURE drops a control interface that the file does not name */
	interface.config.ctrl_interface = '/var/run/wpa_supplicant';
	interface.config.country = data.config.country_code;
	interface.config.beacon_int = data.config.beacon_int;
	if (!data.config.scan_list)
		data.config.scan_list = wiphy_frequencies(data.phy, data.config.band, data.config.radio);

	if (data.config.scan_list)
		interface.config.freq_list = join(" ", data.config.scan_list);

	append_vars(interface.config, [ 'ctrl_interface', 'country', 'beacon_int', 'freq_list' ]);

	let phy_features = {};
	if (wildcard(data.config.htmode ?? '', 'EHT*'))
		phy_features.cipher_gcmp256 = iface.phy_cipher_gcmp256(wiphy_info(data.phy));

	setup_sta(data.config, interface.config, phy_features);

	let file_name = `/var/run/wpa-supplicant-${interface.config.ifname}.conf`;
	if (fs.stat(file_name))
		fs.rename(file_name, file_name + '.prev');
	dump_network(file_name);

	let config = {
		mode: interface.config.mode,
		ctrl: interface.config.ctrl_interface,
		iface: interface.config.ifname,
		config: file_name,
		'4addr': !!interface.config.wds,
		mlo: !!interface.config.mlo,
		freq_list: data.config.scan_list,
		powersave: false
	};

	if (!interface.config.default_macaddr)
		config.macaddr = interface.config.macaddr;

	if (interface.config.wds)
		config.bridge = interface.bridge;

	push(config_list, config);

	return config;
};

export function setup(config, data) {
	if (!global.ubus.list('wpa_supplicant'))
		system('ubus wait_for wpa_supplicant');
	let ret = global.ubus.call('wpa_supplicant', 'config_set', {
		phy: data.phy,
		radio: data.config.radio,
		config,
		defer: true,
		num_global_macaddr: data.config.num_global_macaddr,
		macaddr_base: data.config.macaddr_base ?? "",
	});

	if (ret)
		netifd.add_process('/usr/sbin/wpa_supplicant', ret.pid, true, true);
	else
		netifd.setup_failed('SUPPLICANT_START_FAILED');
};


export function start(data) {
	global.ubus.call('wpa_supplicant', 'config_set', {
		phy: data.phy,
		radio: data.config.radio,
		num_global_macaddr: data.config.num_global_macaddr,
		macaddr_base: data.config.macaddr_base ?? "",
	});
};
