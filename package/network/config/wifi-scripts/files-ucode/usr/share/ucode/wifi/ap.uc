'use strict';

import * as libuci from 'uci';
import { md5 } from 'digest';
import * as fs from 'fs';

import { append, append_raw, append_value, append_vars, comment, push_config, set_default, touch_file } from 'wifi.common';
import * as netifd from 'wifi.netifd';
import * as iface from 'wifi.iface';

function iface_setup(config) {
	switch(config.fixup) {
	case 'owe':
		config.ignore_broadcast_ssid = true;
		config.ssid = config.ssid + 'OWE';
		break;

	case 'owe-transition':
		let ifname = config.ifname;
		config.ifname = config.owe_transition_ifname;
		config.owe_transition_ifname = ifname;
		config.owe_transition_ssid = config.ssid + 'OWE';
		config.encryption = 'none';
		config.ignore_broadcast_ssid = false;
		iface.prepare(config);
		break;
	}
	
	comment('Setup interface: ' + config.ifname);

	config.bridge = config.network_bridge;
	config.snoop_iface = config.network_ifname;
	if (!config.wds)
		config.wds_bridge = null;
	else
		config.wds_sta = true;

	if (!config.idx)
		append('interface', config.ifname);
	else
		append('bss', config.ifname);

	if (config.multicast_to_unicast || config.proxy_arp)
		config.ap_isolate = 1;

	append('bssid', config.macaddr);

append_vars(config, [
		'ctrl_interface', 'ap_isolate', 'max_num_sta', 'ap_max_inactivity', 'airtime_bss_weight',
		'airtime_bss_limit', 'airtime_sta_weight', 'bss_load_update_period', 'chan_util_avg_period',
		'disassoc_low_ack', 'skip_inactivity_poll', 'ignore_broadcast_ssid', 'uapsd_advertisement_enabled',
		'utf8_ssid', 'multi_ap', 'ssid', 'tdls_prohibit', 'bridge', 'wds_sta', 'wds_bridge',
		'snoop_iface', 'vendor_elements', 'nas_identifier', 'radius_acct_interim_interval',
		'ocv', 'multicast_to_unicast', 'preamble', 'wmm_enabled', 'proxy_arp', 'per_sta_vif', 'mbo',
		'bss_transition', 'wnm_sleep_mode', 'wnm_sleep_mode_no_keys', 'qos_map_set', 'max_listen_int',
		'dtim_period',
	 ]);
}

function iface_authentication_server(config) {
	for (let server in config.auth_server_addr) {
		append('auth_server_addr', server);
		append_vars(config, [ 'auth_server_port', 'auth_server_shared_secret' ]);
	}

	append_vars(config, [ 'radius_auth_req_attr' ]);
}

function iface_accounting_server(config) {
	for (let server in config.acct_server_addr) {
		append('acct_server_addr', server);
		append_vars(config, [ 'acct_server_port', 'acct_server_shared_secret' ]);
	}

	append_vars(config, [ 'radius_acct_req_attr' ]);
}

function iface_auth_type(config) {
	iface.parse_encryption(config);

	if (config.auth_type in [ 'sae', 'owe', 'eap2', 'eap192' ]) {
		config.ieee80211w = 2;
		config.sae_require_mfp = 1;
		config.sae_pwe = 2;
	}

	if (config.auth_type in [ 'psk-sae', 'eap-eap2' ]) {
		config.ieee80211w = 1;
		config.sae_require_mfp = 1;
		config.sae_pwe = 2;
	}

	if (config.own_ip_addr)
		config.dynamic_own_ip_addr = null;

	if (!config.wpa)
		config.wpa_disable_eapol_key_retries = null;

	switch(config.auth_type) {
	case 'none':
	case 'owe':
		config.wps_possible = 1;
		config.wps_state = 1;

		if (config.owe_transition_ssid)
			config.owe_transition_ssid = `"${config.owe_transition_ssid}"`;

		append_vars(config, [
			'owe_transition_ssid', 'owe_transition_bssid', 'owe_transition_ifname',
		]);
		break;

	case 'psk':
	case 'psk2':
	case 'sae':
	case 'psk-sae':
		config.vlan_possible = 1;
		config.wps_possible = 1;

		if (config.auth_type == 'psk' && config.ppsk) {
			iface_authentication_server(config);
			config.macaddr_acl = 2;
			config.wpa_psk_radius = 2;
		} else if (length(config.key) == 64) {
			config.wpa_psk = key;
		} else if (length(config.key) >= 8) {
			config.wpa_passphrase = config.key;
		} else if (!config.wpa_psk_file) {
			 netifd.setup_failed('INVALID_WPA_PSK');
		}

		set_default(config, 'wpa_psk_file', `/var/run/hostapd-${config.ifname}.psk`);
		touch_file(config.wpa_psk_file);
		set_default(config, 'dynamic_vlan', 0);
		break;

	case 'eap':
	case 'eap2':
	case 'eap-eap2':
	case 'eap192':
		config.vlan_possible = 1;

		if (config.fils) {
			set_default(config, 'erp_domain', substr(md5(config.ssid), 0, 4));
			set_default(config, 'fils_realm', config.erp_domain);
			set_default(config, 'erp_send_reauth_start', 1);
			set_default(config, 'fils_cache_id', substr(md5(config.fils_realm), 0, 4));
		}

		if (!config.eap_server) {
			iface_authentication_server(config);
			iface_accounting_server(config);
		}

		if (config.radius_das_client && config.radius_das_secret) {
			set_default(config, 'radius_das_port', 3799);
			set_default(config, 'radius_das_client', `${config.radius_das_client} ${config.radius_das_secret}`);
		}

		set_default(config, 'eapol_version', config.wpa & 1);
		if (!config.eapol_version)
			config.eapol_version = null;
		append('eapol_key_index_workaround', '1');
		append('ieee8021x', '1');

		break;
	}

	append_vars(config, [
		'sae_require_mfp', 'sae_pwe', 'time_advertisement', 'time_zone',
		'wpa_group_rekey', 'wpa_ptk_rekey', 'wpa_gmk_rekey', 'wpa_strict_rekey',
		'macaddr_acl', 'wpa_psk_radius', 'wpa_psk', 'wpa_passphrase', 'wpa_psk_file',
		'eapol_version', 'dynamic_vlan', 'radius_request_cui', 'eap_reauth_period',
		'radius_das_client', 'radius_das_port', 'own_ip_addr', 'dynamic_own_ip_addr',
		'wpa_disable_eapol_key_retries', 'auth_algs', 'wpa', 'wpa_pairwise',
		'erp_domain', 'fils_realm', 'erp_send_reauth_start', 'fils_cache_id'
	]);
}

function iface_ppsk(config) {
	if (!(config.auth_type in [ 'none', 'owe', 'psk', 'sae', 'psk-sae', 'wep' ]) || !config.auth_server_addr)
		return;

	iface_authentication_server(config);
	append('macaddr_acl', '2');
}

function iface_wps(config) {
	push_config(config, 'config_methods', 'wps_pushbutton', 'push_button');
	push_config(config, 'config_methods', 'wps_label', 'label');

	if (config.multi_ap == 1)
		config.wps_possible = false;

	if (config.wps_possible && length(config.config_methods)) {
		config.eap_server = 1;
		set_default(config, 'wps_state', 2);

		if (config.ext_registrar && config.network_bridge)
			set_default(config, 'upnp_iface', config.network_bridge);

		if (config.multi_ap && config.multi_ap_backhaul_ssid) {
			append_vars(config, [ 'multi_ap_backhaul_ssid' ]);
			if (length(config.multi_ap_backhaul_key) == 64)
				append('multi_ap_backhaul_wpa_psk', config.multi_ap_backhaul_key);
			else if (length(config.multi_ap_backhaul_key) > 8)
				append('multi_ap_backhaul_wpa_passphrase', config.multi_ap_backhaul_key);
			else
				netifd.setup_failed('INVALID_WPA_PSK');
		}

		append_vars(config, [
			'wps_state', 'device_type', 'device_name', 'config_methods', 'wps_independent', 'eap_server',
			'ap_pin', 'ap_setup_locked', 'upnp_iface'
		]);
	}
}

function iface_rrm(config) {
	set_default(config, 'rrm_neighbor_report', config.ieee80211k);
	set_default(config, 'rrm_beacon_report', config.ieee80211k);

	append_vars(config, [
		'rrm_neighbor_report', 'rrm_beacon_report', 'rnr', 'ftm_responder',
	]);
}

function iface_ftm(config, phy_features) {
	if (!phy_features.ftm_responder || !config.ftm_responder)
		return;

	append_vars(config, [
		'ftm_responder', 'lci', 'civic'
	]);
}

function iface_macfilter(config) {
	let path = `/var/run/hostapd-${config.ifname}.maclist`;

	switch(config.macfilter) {
	case 'allow':
		append('accept_mac_file', path);
		append('macaddr_acl', 1);
		config.vlan_possible = 1;
		break;

	case 'deny':
		append('deny_mac_file', path);
		append('macaddr_acl', 0);
		break;

	default:
		return;
	}

	let file = fs.open(path, 'w');
	if (!file) {
		warn(`Failed to open ${path}`);
		return;
	}

	if (config.maclist)
		file.write(join('\n', config.maclist));

	let macfile = fs.readfile(config.macfile);
	if (macfile)
		file.write(macfile);
	file.close();
}

function iface_vlan(interface, config, vlans) {
	let path = `/var/run/hostapd-${config.ifname}.vlan`;

	let file = fs.open(path, 'w');
	for (let k, vlan in vlans)
		if (vlan.config.name && vlan.config.vid) {
			let ifname = `${config.ifname}-${vlan.config.name}`;
			file.write(`${vlan.config.vid} ${ifname}\n`);
			netifd.set_vlan(interface, k, ifname);
		}
	file.close();

	set_default(config, 'vlan_file', path);
	append_vars(config, [ 'vlan_file' ]);

	if (!config.vlan_possible || !config.dynamic_vlan)
		return;
	
	set_default(config, 'vlan_no_bridge', !config.vlan_bridge);

	append_vars(config, [
		'dynamic_vlan', 'vlan_naming', 'vlan_bridge', 'vlan_no_bridge',
		'vlan_tagged_interface'
	]);
}

function iface_stations(config, stas) {
	if (!length(stas))
		return;

	let path = `/var/run/hostapd-${config.ifname}.psk`;

	let file = fs.open(path, 'w');
	for (let k, sta in stas)
		if (sta.config.mac && sta.config.key) {
			let station = `${sta.config.mac} ${sta.config.key}\n`;
			if (sta.config.vid)
				station = `vlanid=${sta.config.vid} ` + station;
			file.write(station);
		}
	file.close();

	set_default(config, 'wpa_psk_file', path);
}

function iface_eap_server(config) {
	if (!config.eap_server)
		return;

	set_default(config, 'eap_server', true);
	set_default(config, 'eap_server_erp', true);

	append_vars(config, [
		'eap_server', 'eap_server_erp', 'eap_user_file', 'ca_cert', 'server_cert',
		'private_key', 'private_key_passwd', 'server_id',
	]);
}

function iface_roaming(config) {
	if (!config.ieee80211r || config.wpa < 2)
		return;

	set_default(config, 'mobility_domain', substr(md5(config.ssid), 0, 4));
	set_default(config, 'ft_psk_generate_local', config.auth_type == 'psk');
	set_default(config, 'ft_iface', config.network_ifname);

	if (!config.ft_psk_generate_local) {
		if (!config.r0kh || !config.r1kh) {
			if (!config.auth_secret && !config.key)
				netifd.setup_failed('FT_KEY_CANT_BE_DERIVED');

			let ft_key = md5(`${config.mobility_domain}/${config.auth_secret ?? config.key}`);

			set_default(config, 'r0kh', 'ff:ff:ff:ff:ff:ff * ' + ft_key);
			set_default(config, 'r1kh', '00:00:00:00:00:00 00:00:00:00:00:00 ' + ft_key);
		}

		append_vars(config, [
			'r0kh', 'r1kh', 'r1_key_holder', 'r0_key_lifetime', 'pmk_r1_push'
		]);
	}

	append_vars(config, [
		'mobility_domain', 'ft_psk_generate_local', 'ft_over_ds', 'reassociation_deadline',
		'ft_iface'
	]);
}

function iface_mfp(config) {
	if (!config.ieee80211w || config.wpa < 2) {
		append('ieee80211w', 0);
		return;
	}

	if (config.auth_type == 'eap192')
		config.group_mgmt_cipher = 'BIP-GMAC-256';
	else
		config.group_mgmt_cipher = config.ieee80211w_mgmt_cipher ?? 'AES-128-CMAC';

	append_vars(config, [
		'ieee80211w', 'group_mgmt_cipher', 'assoc_sa_query_max_timeout', 'assoc_sa_query_retry_timeout'
	]);
}

function iface_key_caching(config) {
	if (config.wpa < 2)
		return;

	if (config.network_bridge && config.rsn_preauth) {
		set_default(config, 'okc', true);
		config.rsn_preauth_interfaces = config.network_bridge;

		append_vars(config, [
			'rsn_preauth', 'rsn_preauth_interfaces'
		]);
	} else {
		set_default(config, 'okc', (config.auth_type in  [ 'sae', 'psk-sae', 'owe' ]));
	}

	if (!config.okc && !config.fils)
		config.disable_pmksa_caching = 1;

	append_vars(config, [
		'okc', 'disable_pmksa_caching'
	]);
}

function iface_hs20(config) {
	if (!config.hs20)
		return;
	
	let uci = libuci.cursor();
	let icons = uci.get_all('wireless');
	for (let k, icon in icons)
		if (icon['.type'] == 'hs20-icon')
			append('hs20_icon', `${icon.width}:${icon.heigth}:${icon.lang}:${icon.type}:${k}:${icon.path}`);

	append_vars(config, [
		'hs20', 'disable_dgaf', 'osen', 'anqp_domain_id', 'hs20_deauth_req_timeout', 'osu_ssid', 
		'hs20_wan_metrics', 'hs20_operating_class', 'hs20_t_c_filename', 'hs20_t_c_timestamp',
		'hs20_t_c_server_url', 'hs20_oper_friendly_name', 'hs20_conn_capab', 'osu_provider',
		'operator_icon'
	]);
}

function iface_interworking(config) {
	if (!config.iw_enabled)
		return;
	
	config.interworking = true;
	
	if (config.domain_name)
		config.domain_name = join(',', config.domain_name);

	if (config.anqp_3gpp_cell_net)
		config.domain_name = join(',', config.anqp_3gpp_cell_net);

	append_vars(config, [
		'interworking', 'internet', 'asra', 'uesa', 'access_network_type', 'hessid', 'venue_group',
		'venue_type', 'network_auth_type', 'gas_address3', 'roaming_consortium', 'anqp_elem', 'nai_realm',
		'venue_name', 'venue_url', 'domain_name', 'anqp_3gpp_cell_net',
	]);
}

export function generate(interface, config, vlans, stas, phy_features) {
	config.ctrl_interface = '/var/run/hostapd';

	iface_stations(config, stas);

	iface_setup(config);

	iface_auth_type(config);

	iface_accounting_server(config);

	iface_ppsk(config);

	iface_wps(config);

	iface_rrm(config);

	iface_ftm(config, phy_features);

	iface_macfilter(config);

	iface_vlan(interface, config, vlans);

	iface_eap_server(config);

	iface_roaming(config);

	iface_mfp(config);

	iface_key_caching(config);

	iface_hs20(config);

	iface_interworking(config);

	iface.wpa_key_mgmt(config);
	append_vars(config, [
		'wpa_key_mgmt'
	]);

	/* raw options */
	for (let raw in config.hostapd_options)
		append_raw(raw);

	if (config.default_macaddr)
		append_raw('#default_macaddr');
};
