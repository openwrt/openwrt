'use strict';

import { append_value, log } from 'wifi.common';
import * as fs from 'fs';

const WLAN_CIPHER_SUITE_GCMP_256 = 0x000fac09;

export function phy_cipher_gcmp256(phy) {
	return WLAN_CIPHER_SUITE_GCMP_256 in (phy?.cipher_suites ?? []);
};

/*
 * The 6 GHz band allows WPA3 and OWE only (IEEE 802.11-2024 12.12.2,
 * WPA3 Specification v3.5 11.2).
 */
const encryption_6g = {
	'sae-mixed': 'sae',
	'psk3-mixed': 'sae',
	'wpa3-mixed': 'wpa3',
	'wpa': 'wpa3',
	'wpa2': 'wpa3',
	'wpa-mixed': 'wpa3',
	'none': 'owe',
	'psk': 'sae',
	'psk2': 'sae',
	'psk-mixed': 'sae',
};

/*
 * An AP MLD needs RSN on every link. parse_encryption() adds SAE in the
 * RSNE Override 2 element for multi-link associations.
 */
const encryption_mld = {
	'psk': 'psk2',
	'psk-mixed': 'psk2',
	'wpa': 'wpa2',
	'wpa-mixed': 'wpa2',
};

/* IEEE 802.11be-2024 12.6.2: all links of an AP MLD share an AKM */
const encryption_mld_6g = {
	...encryption_mld,
	'none': 'owe',
};

/*
 * One network block serves all links of an MLD station, and wpa_supplicant
 * removes the PSK AKMs for a 6 GHz BSS.
 */
const encryption_sta_mixed_6g = {
	'psk2': 'sae-mixed',
	'wpa2': 'wpa3-mixed',
};

function encryption_map(encryption, modes) {
	let enc = split(encryption ?? 'none', '+', 2);
	if (!modes[enc[0]])
		return encryption;

	enc[0] = modes[enc[0]];
	return join('+', enc);
}

function rsne_offers_rsno2(config, rsno2_pairwise) {
	return !!config.sae_ext_key && config.auth_type in [ 'sae', 'psk-sae' ] &&
		index(split(config.wpa_pairwise ?? '', ' '), rsno2_pairwise) >= 0;
}

/* mld_bands: null for a single-link BSS, else the bands of the AP MLD */
export function encryption_band(encryption, band, mld_bands) {
	if (band == '6g')
		return encryption_map(encryption, encryption_6g);
	if (index(mld_bands ?? [], '6g') >= 0)
		return encryption_map(encryption, encryption_mld_6g);
	if (mld_bands != null)
		return encryption_map(encryption, encryption_mld);

	return encryption;
};

/* mld_bands: null for a single-link station, else the bands of the MLD */
export function encryption_sta_band(encryption, band, mld_bands) {
	if (!length(mld_bands))
		return encryption_band(encryption, band);
	if (index(mld_bands, '6g') < 0)
		return encryption;
	if (length(mld_bands) == 1)
		return encryption_map(encryption, encryption_6g);

	return encryption_map(encryption, encryption_sta_mixed_6g);
};

/* rsno2: null decides the RSNO2E for this link alone, a bool gives the
 * decision of the AP MLD */
export function parse_encryption(config, dev_config, phy_features, rsno2) {
	if (!config.encryption)
		config.encryption = 'none';

	let encryption = split(config.encryption, '+', 2);

	config.wpa = 0;
	for (let k, v in { 'wpa2*': 2, 'wpa3*': 2, '*psk2*': 2, 'psk3*': 2, 'sae*': 2,
			'owe*': 2, 'dpp': 2, 'wpa*mixed*': 3, '*psk*mixed*': 3, 'wpa*': 1, '*psk*': 1, })
		if (wildcard(config.encryption, k)) {
			config.wpa = v;
			break;
		}

	config.auth_type = encryption[0] ?? 'none';

	/*
	 * WPA3 Specification v3.5 2.5 requires SAE-EXT-KEY and GCMP-256 with
	 * EHT or MLO. Some clients fail when these are offered in the RSNE, so on
	 * EHT they go into the RSNE Override 2 element. Explicit sae_ext_key and
	 * gcmp256 options apply to the RSNE; 0 also keeps them out of RSNO2. An
	 * RSNE that already offers the AKM and the pairwise cipher of the RSNO2E
	 * makes the RSNO2E a duplicate, so the BSS sends none. An AP MLD sends
	 * none only where that holds on every link (ap.uc mld_rsno2()).
	 */
	let eht = wildcard(dev_config?.htmode ?? '', 'EHT*');
	let compat = (config.auth_type == 'sae-compat');
	let rsno2_mode = config.auth_type in [ 'sae', 'psk3', 'sae-mixed', 'psk3-mixed' ] ||
		(!!config.mlo && config.auth_type == 'psk2');
	/* all links of an AP MLD must reach the same decision */
	let rsno2_sae = (eht || !!config.mlo) && rsno2_mode && config.sae_ext_key !== false;
	let rsno2_pairwise = (config.gcmp256 !== false && phy_features?.cipher_gcmp256) ? 'GCMP-256' : 'CCMP';
	config.gcmp256 ??= compat && eht;
	config.sae_ext_key ??= compat && eht;

	switch(config.auth_type) {
	case 'owe':
		config.auth_type = 'owe';
		break;

	case 'dpp':
		config.auth_type = 'dpp';
		break;

	case 'wpa3-192':
		config.auth_type = 'eap192';
		config.wpa_pairwise = 'GCMP-256';
		break;

	case 'wpa3-mixed':
		config.auth_type = 'eap-eap2';
		break;

	case 'wpa3':
		config.auth_type = 'eap2';
		break;

	case 'psk':
	case 'psk2':
	case 'psk-mixed':
		config.auth_type = 'psk';
		break;

	case 'sae':
	case 'psk3':
		config.auth_type = 'sae';
		break;

	case 'psk3-mixed':
	case 'sae-mixed':
		config.auth_type = 'psk-sae';
		break;

	case 'sae-compat':
		config.auth_type = 'psk-sae-compat';
		config.wpa_pairwise = 'CCMP';
		if (dev_config.band != '6g')
			config.rsn_override_pairwise = 'CCMP';
		if (config.gcmp256 && phy_features?.cipher_gcmp256)
			config.rsn_override_pairwise_2 = 'GCMP-256';
		else if (config.sae_ext_key)
			config.rsn_override_pairwise_2 = 'CCMP';
		break;

	case 'wpa':
	case 'wpa2':
	case 'wpa-mixed':
		config.auth_type = 'eap';
		break;
	}

	switch(encryption[1]){
	case 'tkip+aes':
	case 'tkip+ccmp':
	case 'aes+tkip':
	case 'ccmp+tkip':
		config.wpa_pairwise = 'CCMP TKIP';
		break;

	case 'ccmp256':
		config.wpa_pairwise = 'CCMP-256';
		break;

	case 'aes':
	case 'ccmp':
		config.wpa_pairwise = 'CCMP';
		break;

	case 'tkip':
		config.wpa_pairwise = 'TKIP';
		break;

	case 'gcmp256':
		config.wpa_pairwise = 'GCMP-256';
		break;

	case 'gcmp':
		config.wpa_pairwise = 'GCMP';
		break;
	}

	if (!config.wpa)
		config.wpa_pairwise ??= null;
	else if (dev_config.band == '60g')
		config.wpa_pairwise ??= 'GCMP';
	else if (config.gcmp256 && phy_features?.cipher_gcmp256)
		config.wpa_pairwise ??= 'GCMP-256 CCMP';
	else
		config.wpa_pairwise ??= 'CCMP';

	config.rsno2_sae = rsno2_sae && (rsno2 ?? !rsne_offers_rsno2(config, rsno2_pairwise));
	if (config.rsno2_sae)
		config.rsn_override_pairwise_2 = rsno2_pairwise;
};

export function wpa_key_mgmt(config, band) {
	if (!config.wpa)
		return;

	switch(config.auth_type) {
	case 'psk':
	case 'psk2':
		append_value(config, 'wpa_key_mgmt', 'WPA-PSK');
		if (config.wpa >= 2 && config.ieee80211r)
			append_value(config, 'wpa_key_mgmt', 'FT-PSK');
		if (config.ieee80211w)
			append_value(config, 'wpa_key_mgmt', 'WPA-PSK-SHA256');
		break;

	case 'eap':
		append_value(config, 'wpa_key_mgmt', 'WPA-EAP');
		if (config.wpa >= 2 && config.ieee80211r)
			append_value(config, 'wpa_key_mgmt', 'FT-EAP');
		if (config.ieee80211w)
			append_value(config, 'wpa_key_mgmt', 'WPA-EAP-SHA256');
		break;

	case 'eap192':
		append_value(config, 'wpa_key_mgmt', 'WPA-EAP-SUITE-B-192');
		if (config.ieee80211r)
			append_value(config, 'wpa_key_mgmt', 'FT-EAP-SHA384');
		break;

	case 'eap-eap2':
		append_value(config, 'wpa_key_mgmt', 'WPA-EAP-SHA256');
		if (config.ieee80211r)
			append_value(config, 'wpa_key_mgmt', 'FT-EAP');

		append_value(config, 'wpa_key_mgmt', 'WPA-EAP');
		break;

	case 'eap2':
		append_value(config, 'wpa_key_mgmt', 'WPA-EAP-SHA256');
		if (config.ieee80211r)
			append_value(config, 'wpa_key_mgmt', 'FT-EAP');
		break;

	case 'sae':
		append_value(config, 'wpa_key_mgmt', 'SAE');
		if (config.sae_ext_key)
			append_value(config, 'wpa_key_mgmt', 'SAE-EXT-KEY');
		if (config.ieee80211r) {
			append_value(config, 'wpa_key_mgmt', 'FT-SAE');
			if (config.sae_ext_key)
				append_value(config, 'wpa_key_mgmt', 'FT-SAE-EXT-KEY');
		}
		break;

	case 'psk-sae':
		append_value(config, 'wpa_key_mgmt', 'SAE');
		if (config.sae_ext_key)
			append_value(config, 'wpa_key_mgmt', 'SAE-EXT-KEY');
		if (config.ieee80211r) {
			append_value(config, 'wpa_key_mgmt', 'FT-SAE');
			if (config.sae_ext_key)
				append_value(config, 'wpa_key_mgmt', 'FT-SAE-EXT-KEY');
		}

		append_value(config, 'wpa_key_mgmt', 'WPA-PSK');
		if (config.ieee80211w)
			append_value(config, 'wpa_key_mgmt', 'WPA-PSK-SHA256');
		if (config.ieee80211r)
			append_value(config, 'wpa_key_mgmt', 'FT-PSK');
		break;

	case 'psk-sae-compat':
		if (band == '6g') {
			append_value(config, 'wpa_key_mgmt', 'SAE');
			if (config.ieee80211r)
				append_value(config, 'wpa_key_mgmt', 'FT-SAE');

			if (config.sae_ext_key) {
				append_value(config, 'rsn_override_key_mgmt_2', 'SAE-EXT-KEY');
				if (config.ieee80211r)
					append_value(config, 'rsn_override_key_mgmt_2', 'FT-SAE-EXT-KEY');
			}
		} else {
			append_value(config, 'wpa_key_mgmt', 'WPA-PSK');
			if (config.ieee80211r)
				append_value(config, 'wpa_key_mgmt', 'FT-PSK');

			append_value(config, 'rsn_override_key_mgmt', 'SAE');
			if (config.ieee80211r)
				append_value(config, 'rsn_override_key_mgmt', 'FT-SAE');

			if (config.sae_ext_key) {
				append_value(config, 'rsn_override_key_mgmt_2', 'SAE-EXT-KEY');
				if (config.ieee80211r)
					append_value(config, 'rsn_override_key_mgmt_2', 'FT-SAE-EXT-KEY');
			}
		}
		break;

	case 'owe':
		append_value(config, 'wpa_key_mgmt', 'OWE');
		break;

	case 'dpp':
		append_value(config, 'wpa_key_mgmt', 'DPP');
		break;
	}

	if (config.rsno2_sae) {
		append_value(config, 'rsn_override_key_mgmt_2', 'SAE-EXT-KEY');
		if (config.ieee80211r)
			append_value(config, 'rsn_override_key_mgmt_2', 'FT-SAE-EXT-KEY');
	}

	if (config.dpp && config.auth_type != 'dpp')
		append_value(config, 'wpa_key_mgmt', 'DPP');

	if (config.fils) {
		switch(config.auth_type) {
		case 'eap192':
			append_value(config, 'wpa_key_mgmt', 'FILS-SHA384');
			if (config.ieee80211r)
				append_value(config, 'wpa_key_mgmt', 'FT-FILS-SHA384');
			break;

		case 'eap-eap2':
		case 'eap2':
		case 'eap':
			append_value(config, 'wpa_key_mgmt', 'FILS-SHA256');
			if (config.ieee80211r)
				append_value(config, 'wpa_key_mgmt', 'FT-FILS-SHA256');
			break;
		}
	}

	config.key_mgmt = config.wpa_key_mgmt;
};

function macaddr_random() {
	let f = fs.open("/dev/urandom", "r");
	let addr = f.read(6);

	addr = map(split(addr, ""), (v) => ord(v));
	addr[0] &= ~1;
	addr[0] |= 2;

	return join(":", map(addr, (v) => sprintf("%02x", v)));
}

export function prepare(data) {
	if (!data.macaddr) {
		data.default_macaddr = true;
	} else if (data.macaddr == 'random') {
		data.macaddr = macaddr_random();
		data.random_macaddr = true;
	}

	log(`Preparing interface: ${data.ifname}` + (data.macaddr ? ` with MAC: ${data.macaddr}` : ""));
};
