'use strict';

import { append, append_raw, append_vars, network_append, network_append_raw, network_append_vars,
	 set_default, dump_network, flush_network } from 'wifi.common';
import * as netifd from 'wifi.netifd';
import * as iface from 'wifi.iface';
import * as fs from 'fs';

function set_fixed_freq(data, config) {
	if (!data.frequency)
		return;

	set_default(config, 'fixed_freq', 1);
	set_default(config, 'frequency', data.frequency);

	if (data.htmode in [ 'VHT80', 'HE80' ])
		set_default(config, 'max_oper_chwidth', 1);
	else if (data.htmode in [ 'VHT160', 'HE160' ])
		set_default(config, 'max_oper_chwidth', 2);
	else if (data.htmode in [ 'VHT20', 'VHT40', 'HE20', 'HE40' ])
		set_default(config, 'max_oper_chwidth', 0);
	else
		set_default(config, 'disable_vht', true);

	if (data.htmode in [ 'NOHT' ])
		set_default(config, 'disable_ht', true);
	else if (data.htmode in [ 'HT20', 'VHT20', 'HE20' ])
		set_default(config, 'disable_ht40', true);
	else if (data.htmode in [ 'VHT40', 'VHT80', 'VHT160', 'HE40', 'HE80', 'HE160' ])
		set_default(config, 'ht40', true);

	if (wildcard(data.htmode, 'VHT*'))
		set_default(config, 'vht', 1);
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

function setup_sta(data, config) {
	iface.parse_encryption(config);

	if (config.auth_type in [ 'sae', 'owe', 'eap2', 'eap192' ])
		set_default(config, 'ieee80211w', 2);
	else if (config.auth_type in [ 'psk-sae' ])
		set_default(config, 'ieee80211w', 1);

	set_default(config, 'ieee80211r', 0);
	set_default(config, 'multi_ap', 0);
	set_default(config, 'default_disabled', 0);

//multiap_flag_file="${_config}.is_multiap"
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

	case 'psk':
	case 'psk2':
	case 'sae':
	case 'psk-sae':
		if (config.mode != 'mesh')
			iface.wpa_key_mgmt(config);

		if (config.mode == 'mesh' || config.auth_type == 'sae')
			config.sae_password = `"${config.key}"`;
		else
			config.psk = `"${config.key}"`;

		break;

	case 'eap':
	case 'eap2':
	case 'eap192':
		iface.wpa_key_mgmt(config);
		set_default(config, 'erp', config.fils);

		if (config.ca_cert_usesystem && fs.stat('/etc/ssl/certs/ca-certificates.crt'))
			config.ca_cert = '/etc/ssl/certs/ca-certificates.crt';

		switch(config.eap_type) {
		case 'fast':
		case 'peap':
		case 'ttls':
			set_default(config, 'auth', 'MSCHAPV2');
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
	}

	config.basic_rate = ratelist(config.basic_rate);
	config.mcast_rate = ratestr(config.mcast_rate);
	config.ssid = `"${config.ssid}"`;

	network_append_vars(config, [
		'scan_ssid', 'noscan', 'disabled', 'multi_ap_backhaul_sta',
		'ocv', 'key_mgmt', 'psk', 'sae_password', 'pairwise', 'group', 'bssid',
		'proto', 'mesh_fwding', 'mesh_rssi_threshold', 'frequency', 'fixed_freq',
		'disable_ht', 'disable_ht40', 'disable_vht', 'vht', 'max_oper_chwidth',
		'ht40', 'ssid', 'beacon_int', 'ieee80211w', 'basic_rate', 'mcast_rate',
		'bssid_blacklist', 'bssid_whitelist', 'erp', 'ca_cert', 'identity',
		'anonymous_identity', 'client_cert', 'private_key', 'private_key_passwd',
		'subject_match', 'altsubject_match', 'domain_match', 'domain_suffix_match',
		'ca_cert2', 'client_cert2', 'private_key2', 'private_key2_passwd', 'password'
	]);
}

export function generate(config_list, data, interface) {
	flush_network();

	if (interface.bridge &&
	    (interface.config.mode == 'adhoc' ||
	     (interface.config.mode == 'sta' && !interface.config.wds && !interface.config.multi_ap))){
		netifd.setup_failed('BRIDGE_NOT_ALLOWED');
		return 1;
	}

	interface.config.country = data.config.country_code;
	interface.config.beacon_int = data.config.beacon_int;

	append_vars(interface.config, [ 'country', 'beacon_int' ]);

	setup_sta(data.config, interface.config);

	let file_name = `/var/run/wpa-supplicant-${interface.config.ifname}.conf`;
	if (fs.stat(file_name))
		fs.rename(file_name, file_name + '.prev');
	dump_network(file_name);

	let config = {
		mode: interface.config.mode,
		ctrl: '/var/run/wpa_supplicant',
		iface: interface.config.ifname,
		config: file_name,
		'4addr': !!interface.config.wds,
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
	let ret = global.ubus.call('wpa_supplicant', 'config_set', {
		phy: data.phy,
		radio: data.config.radio,
		config,
		defer: true,
		num_global_macaddr: data.config.num_global_macaddr,
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
	});
};
