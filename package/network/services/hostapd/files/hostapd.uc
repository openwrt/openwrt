let libubus = require("ubus");
import { open, readfile, access } from "fs";
import { wdev_remove, is_equal, vlist_new, phy_is_fullmac, phy_open, wdev_set_radio_mask, wdev_set_up } from "common";

let ubus = libubus.connect(null, 60);

function ex_handler(e)
{
	e = split(`${e}\n${e.stacktrace[0].context}`, '\n');
	for (let line in e)
		hostapd.printf(line);
	return libubus.STATUS_UNKNOWN_ERROR;
}
libubus.guard(ex_handler);

hostapd.data.config = {};
hostapd.data.pending_config = {};

hostapd.data.file_fields = {
	vlan_file: true,
	wpa_psk_file: true,
	sae_password_file: true,
	rxkh_file: true,
	accept_mac_file: true,
	deny_mac_file: true,
	eap_user_file: true,
	ca_cert: true,
	server_cert: true,
	server_cert2: true,
	private_key: true,
	private_key2: true,
	dh_file: true,
	eap_sim_db: true,
};

hostapd.data.iface_fields = {
	ft_iface: true,
	upnp_iface: true,
	snoop_iface: true,
	bridge: true,
	iapp_interface: true,
};

hostapd.data.bss_info_fields = {
	// radio
	hw_mode: true,
	channel: true,
	ieee80211ac: true,
	ieee80211ax: true,

	// bss
	bssid: true,
	ssid: true,
	wpa: true,
	wpa_key_mgmt: true,
	wpa_pairwise: true,
	auth_algs: true,
	ieee80211w: true,
	owe_transition_ifname: true,
};

hostapd.data.mld = {};

function iface_remove(cfg)
{
	if (!cfg || !cfg.bss || !cfg.bss[0] || !cfg.bss[0].ifname)
		return;

	for (let bss in cfg.bss)
		if (!bss.mld_ap)
			wdev_remove(bss.ifname);
}

function iface_gen_config(config, start_disabled)
{
	let str = `data:
${join("\n", config.radio.data)}
channel=${config.radio.channel}
`;

	for (let i = 0; i < length(config.bss); i++) {
		let bss = config.bss[i];
		let type = i > 0 ? "bss" : "interface";
		let nasid = bss.nasid ?? replace(bss.bssid, ":", "");
		let bssid = bss.bssid;
		if (bss.mld_ap)
			bssid += "\nmld_addr=" + bss.mld_bssid;
		str += `
${type}=${bss.ifname}
bssid=${bssid}
${join("\n", bss.data)}
nas_identifier=${nasid}
`;
		if (start_disabled)
			str += `
start_disabled=1
`;
	}

	return str;
}

function iface_freq_info(iface, config, params)
{
	let freq = params.frequency;
	if (!freq)
		return null;

	let sec_offset = params.sec_chan_offset;
	if (sec_offset != -1 && sec_offset != 1)
		sec_offset = 0;

	let width = 0;
	for (let line in config.radio.data) {
		if (!sec_offset && match(line, /^ht_capab=.*HT40/)) {
			sec_offset = null; // auto-detect
			continue;
		}

		let val = match(line, /^(vht_oper_chwidth|he_oper_chwidth)=(\d+)/);
		if (!val)
			continue;

		val = int(val[2]);
		if (val > width)
			width = val;
	}

	if (freq < 4000)
		width = 0;

	return hostapd.freq_info(freq, sec_offset, width);
}

function iface_add(phy, config, phy_status)
{
	let config_inline = iface_gen_config(config, !!phy_status);

	let bss = config.bss[0];
	let ret = hostapd.add_iface(`bss_config=${phy}:${config_inline}`);
	if (ret < 0)
		return false;

	if (!phy_status)
		return true;

	let iface = hostapd.interfaces[phy];
	if (!iface)
		return false;

	let freq_info = iface_freq_info(iface, config, phy_status);

	return iface.start(freq_info) >= 0;
}

function iface_config_macaddr_list(config)
{
	let macaddr_list = {};
	for (let name, mld in hostapd.data.mld)
		if (mld.macaddr)
			macaddr_list[mld.macaddr] = -1;
	for (let i = 0; i < length(config.bss); i++) {
		let bss = config.bss[i];
		if (!bss.default_macaddr)
			macaddr_list[bss.bssid] = i;
	}

	return macaddr_list;
}

function iface_update_supplicant_macaddr(phydev, config)
{
	let macaddr_list = [];
	for (let name, mld in hostapd.data.mld)
		if (mld.macaddr)
			push(macaddr_list, mld.macaddr);
	for (let bss in config.bss)
		push(macaddr_list, bss.bssid);
	ubus.defer("wpa_supplicant", "phy_set_macaddr_list", {
		phy: phydev.phy,
		radio: phydev.radio ?? -1,
		macaddr: macaddr_list
	});
}

function __iface_pending_next(pending, state, ret, data)
{
	let config = pending.config;
	let phydev = pending.phydev;
	let phy = pending.phy;
	let bss = config.bss[0];

	if (pending.defer)
		pending.defer.abort();
	delete pending.defer;
	switch (state) {
	case "init":
		iface_update_supplicant_macaddr(phydev, config);
		return "create_bss";
	case "create_bss":
		if (!bss.mld_ap) {
			let err = phydev.wdev_add(bss.ifname, {
				mode: "ap",
				radio: phydev.radio,
			});
			if (err) {
				hostapd.printf(`Failed to create ${bss.ifname} on phy ${phy}: ${err}`);
				return null;
			}
		}

		pending.call("wpa_supplicant", "phy_status", {
			phy: phydev.phy,
			radio: phydev.radio ?? -1,
		});
		return "check_phy";
	case "check_phy":
		let phy_status = data;
		if (phy_status && phy_status.state == "COMPLETED") {
			if (iface_add(phy, config, phy_status))
				return "done";

			hostapd.printf(`Failed to bring up phy ${phy} ifname=${bss.ifname} with supplicant provided frequency`);
		}
		pending.call("wpa_supplicant", "phy_set_state", {
			phy: phydev.phy,
			radio: phydev.radio ?? -1,
			stop: true
		});
		return "wpas_stopped";
	case "wpas_stopped":
		if (!iface_add(phy, config))
			hostapd.printf(`hostapd.add_iface failed for phy ${phy} ifname=${bss.ifname}`);
		pending.call("wpa_supplicant", "phy_set_state", {
			phy: phydev.phy,
			radio: phydev.radio ?? -1,
			stop: false
		});
		return null;
	case "done":
	default:
		delete hostapd.data.pending_config[phy];
		break;
	}
}

function iface_pending_next(ret, data)
{
	let pending = true;
	let cfg = this;

	while (pending) {
		try {
			this.next_state = __iface_pending_next(cfg, this.next_state, ret, data);
			if (!this.next_state) {
				__iface_pending_next(cfg, "done");
				return;
			}
		} catch(e) {
			hostapd.printf(`Exception: ${e}\n${e.stacktrace[0].context}`);
			return;
		}
		pending = !this.defer;
	}
}

function iface_pending_abort()
{
	this.next_state = "done";
	this.next();
}

function iface_pending_ubus_call(obj, method, arg)
{
	let ubus = hostapd.data.ubus;
	let pending = this;
	this.defer = ubus.defer(obj, method, arg, (ret, data) => { delete pending.defer; pending.next(ret, data) });
}

const iface_pending_proto = {
	next: iface_pending_next,
	call: iface_pending_ubus_call,
	abort: iface_pending_abort,
};

function iface_pending_init(phydev, config)
{
	let phy = phydev.name;

	let pending = proto({
		next_state: "init",
		phydev: phydev,
		phy: phy,
		config: config,
		next: iface_pending_next,
	}, iface_pending_proto);

	hostapd.data.pending_config[phy] = pending;
	pending.next();
}

function iface_macaddr_init(phydev, config, macaddr_list)
{
	let macaddr_data = {
		num_global: config.num_global_macaddr ?? 1,
		macaddr_base: config.macaddr_base,
		mbssid: config.mbssid ?? 0,
	};

	return phydev.macaddr_init(macaddr_list, macaddr_data);
}

function iface_restart(phydev, config, old_config)
{
	let phy = phydev.name;
	let pending = hostapd.data.pending_config[phy];

	if (pending)
		pending.abort();

	hostapd.remove_iface(phy);
	iface_remove(old_config);
	iface_remove(config);

	if (!config.bss || !config.bss[0]) {
		hostapd.printf(`No bss for phy ${phy}`);
		return;
	}

	iface_macaddr_init(phydev, config, iface_config_macaddr_list(config));
	for (let i = 0; i < length(config.bss); i++) {
		let bss = config.bss[i];
		if (bss.default_macaddr)
			bss.bssid = phydev.macaddr_next();
	}

	iface_pending_init(phydev, config);
}

function array_to_obj(arr, key, start)
{
	let obj = {};

	start ??= 0;
	for (let i = start; i < length(arr); i++) {
		let cur = arr[i];
		obj[cur[key]] = cur;
	}

	return obj;
}

function find_array_idx(arr, key, val)
{
	for (let i = 0; i < length(arr); i++)
		if (arr[i][key] == val)
			return i;

	return -1;
}

function bss_reload_psk(bss, config, old_config)
{
	if (is_equal(old_config.hash.wpa_psk_file, config.hash.wpa_psk_file))
		return;

	old_config.hash.wpa_psk_file = config.hash.wpa_psk_file;
	if (!is_equal(old_config, config))
		return;

	let ret = bss.ctrl("RELOAD_WPA_PSK");
	ret ??= "failed";

	hostapd.printf(`Reload WPA PSK file for bss ${config.ifname}: ${ret}`);
}

function normalize_rxkhs(txt)
{
	const pat = {
		sep: "\x20",
		mac: "([[:xdigit:]]{2}:?){5}[[:xdigit:]]{2}",
		r0kh_id: "[\x21-\x7e]{1,48}",
		r1kh_id: "([[:xdigit:]]{2}:?){5}[[:xdigit:]]{2}",
		key: "[[:xdigit:]]{32,}",
		r0kh: function() {
			return "r0kh=" + this.mac + this.sep + this.r0kh_id;
		},
		r1kh: function() {
			return "r1kh=" + this.mac + this.sep + this.r1kh_id;
		},
		rxkh: function() {
			return "(" + this.r0kh() + "|" + this.r1kh() + ")" + this.sep + this.key;
		},
	};

	let rxkhs = filter(
		split(txt, "\n"), (line) => match(line, regexp("^" + pat.rxkh() + "$"))
	) ?? [];

	rxkhs = map(rxkhs, function(k) {
		k = split(k, " ", 3);
		k[0] = lc(k[0]);
		if(match(k[0], /^r1kh/)) {
			k[1] = lc(k[1]);
		}
		if(!k[2] = hostapd.rkh_derive_key(k[2])) {
			return;
		}
		return join(" ", k);
	});

	return join("\n", sort(filter(rxkhs, length)));
}

function bss_reload_rxkhs(bss, config, old_config)
{
	let bss_rxkhs = join("\n", sort(split(bss.ctrl("GET_RXKHS"), "\n")));
	let bss_rxkhs_hash = hostapd.sha1(bss_rxkhs);

	if (is_equal(config.hash.rxkh_file, bss_rxkhs_hash)) {
		if (is_equal(old_config.hash.rxkh_file, config.hash.rxkh_file))
			return;
	}

	old_config.hash.rxkh_file = config.hash.rxkh_file;
	if (!is_equal(old_config, config))
		return;

	let ret = bss.ctrl("RELOAD_RXKHS");
	ret ??= "failed";

	hostapd.printf(`Reload RxKH file for bss ${config.ifname}: ${ret}`);
}

function remove_file_fields(config)
{
	return filter(config, (line) =>
		!match(line, /^\s*$/) &&
		!match(line, /^\s*#/) &&
		!hostapd.data.file_fields[split(line, "=")[0]]
	);
}

function bss_remove_file_fields(config)
{
	let new_cfg = {};

	for (let key in config)
		new_cfg[key] = config[key];
	new_cfg.data = remove_file_fields(new_cfg.data);
	new_cfg.hash = {};
	for (let key in config.hash)
		new_cfg.hash[key] = config.hash[key];
	delete new_cfg.hash.wpa_psk_file;
	delete new_cfg.hash.sae_password_file;
	delete new_cfg.hash.vlan_file;

	return new_cfg;
}

function bss_ifindex_list(config)
{
	config = filter(config, (line) => !!hostapd.data.iface_fields[split(line, "=")[0]]);

	return join(",", map(config, (line) => {
		try {
			let file = "/sys/class/net/" + split(line, "=")[1] + "/ifindex";
			let val = trim(readfile(file));
			return val;
		} catch (e) {
			return "";
		}
	}));
}

function bss_config_hash(config)
{
	return hostapd.sha1(remove_file_fields(config) + bss_ifindex_list(config));
}

function bss_find_existing(config, prev_config, prev_hash)
{
	let hash = bss_config_hash(config.data);

	for (let i = 0; i < length(prev_config.bss); i++) {
		if (!prev_hash[i] || hash != prev_hash[i])
			continue;

		prev_hash[i] = null;
		return i;
	}

	return -1;
}

function get_config_bss(name, config, idx)
{
	if (!config.bss[idx]) {
		hostapd.printf(`Invalid bss index ${idx}`);
		return;
	}

	let ifname = config.bss[idx].ifname;
	if (!ifname) {
		hostapd.printf(`Could not find bss ${config.bss[idx].ifname}`);
		return;
	}

	let if_bss = hostapd.bss[name];
	if (!if_bss) {
		hostapd.printf(`Could not find interface ${name} bss list`);
		return;
	}

	return if_bss[ifname];
}

function iface_reload_config(name, phydev, config, old_config)
{
	let phy = phydev.name;

	if (!old_config || !is_equal(old_config.radio, config.radio))
		return false;

	if (is_equal(old_config.bss, config.bss))
		return true;

	if (hostapd.data.pending_config[name])
		return false;

	if (!old_config.bss || !old_config.bss[0])
		return false;

	let iface = hostapd.interfaces[name];
	let iface_name = old_config.bss[0].ifname;
	if (!iface) {
		hostapd.printf(`Could not find previous interface ${iface_name}`);
		return false;
	}

	if (iface.state() != "ENABLED") {
		hostapd.printf(`Interface ${iface_name} is not fully configured`);
		return false;
	}

	let first_bss = get_config_bss(name, old_config, 0);
	if (!first_bss) {
		hostapd.printf(`Could not find bss of previous interface ${iface_name}`);
		return false;
	}

	let macaddr_list = iface_config_macaddr_list(config);
	let bss_list = [];
	let bss_list_cfg = [];
	let prev_bss_hash = [];

	for (let bss in old_config.bss) {
		let hash = bss_config_hash(bss.data);
		push(prev_bss_hash, bss_config_hash(bss.data));
	}

	// Step 1: find (possibly renamed) interfaces with the same config
	// and store them in the new order (with gaps)
	for (let i = 0; i < length(config.bss); i++) {
		let prev;

		// For fullmac devices, the first interface needs to be preserved,
		// since it's treated as the master
		if (!i && phy_is_fullmac(phy)) {
			prev = 0;
			prev_bss_hash[0] = null;
		} else {
			prev = bss_find_existing(config.bss[i], old_config, prev_bss_hash);
		}
		if (prev < 0)
			continue;

		let cur_config = config.bss[i];
		let prev_config = old_config.bss[prev];
		if (prev_config.force_reload) {
			delete prev_config.force_reload;
			continue;
		}

		let prev_bss = get_config_bss(name, old_config, prev);
		if (!prev_bss)
			return false;

		// try to preserve MAC address of this BSS by reassigning another
		// BSS if necessary
		if ((cur_config.default_macaddr || cur_config.random_macaddr) &&
		    cur_config.random_macaddr == prev_config.random_macaddr &&
		    cur_config.default_macaddr == prev_config.default_macaddr &&
		    !macaddr_list[prev_config.bssid]) {
			macaddr_list[prev_config.bssid] = i;
			cur_config.bssid = prev_config.bssid;
		}

		bss_list[i] = prev_bss;
		bss_list_cfg[i] = old_config.bss[prev];
	}

	if (config.mbssid && !bss_list_cfg[0]) {
		hostapd.printf("First BSS changed with MBSSID enabled");
		return false;
	}

	// Step 2: if none were found, rename and preserve the first one
	if (length(bss_list) == 0) {
		// can't change the bssid of the first bss
		if (config.bss[0].bssid != old_config.bss[0].bssid) {
			if (!config.bss[0].default_macaddr) {
				hostapd.printf(`BSSID of first interface changed: ${lc(old_config.bss[0].bssid)} -> ${lc(config.bss[0].bssid)}`);
				return false;
			}

			config.bss[0].bssid = old_config.bss[0].bssid;
		}

		let prev_bss = get_config_bss(name, old_config, 0);
		if (!prev_bss)
			return false;

		macaddr_list[config.bss[0].bssid] = 0;
		bss_list[0] = prev_bss;
		bss_list_cfg[0] = old_config.bss[0];
		prev_bss_hash[0] = null;
	}

	// Step 3: delete all unused old interfaces
	for (let i = 0; i < length(prev_bss_hash); i++) {
		if (!prev_bss_hash[i])
			continue;

		let prev_bss = get_config_bss(name, old_config, i);
		if (!prev_bss)
			return false;

		let ifname = old_config.bss[i].ifname;
		hostapd.printf(`Remove bss '${ifname}' on phy '${name}'`);
		prev_bss.delete();
		if (!old_config.bss[i].mld_ap)
			wdev_remove(ifname);
	}

	// Step 4: rename preserved interfaces, use temporary name on duplicates
	let rename_list = [];
	for (let i = 0; i < length(bss_list); i++) {
		if (!bss_list[i])
			continue;

		let old_ifname = bss_list_cfg[i].ifname;
		let new_ifname = config.bss[i].ifname;
		if (old_ifname == new_ifname)
			continue;

		if (hostapd.bss[name][new_ifname]) {
			new_ifname = "tmp_" + substr(hostapd.sha1(new_ifname), 0, 8);
			push(rename_list, i);
		}

		hostapd.printf(`Rename bss ${old_ifname} to ${new_ifname}`);
		if (!bss_list[i].rename(new_ifname)) {
			hostapd.printf(`Failed to rename bss ${old_ifname} to ${new_ifname}`);
			return false;
		}

		bss_list_cfg[i].ifname = new_ifname;
	}

	// Step 5: rename interfaces with temporary names
	for (let i in rename_list) {
		let new_ifname = config.bss[i].ifname;
		if (!bss_list[i].rename(new_ifname)) {
			hostapd.printf(`Failed to rename bss to ${new_ifname}`);
			return false;
		}
		bss_list_cfg[i].ifname = new_ifname;
	}

	// Step 6: assign BSSID for newly created interfaces
	macaddr_list = iface_macaddr_init(phydev, config, macaddr_list);
	for (let i = 0; i < length(config.bss); i++) {
		if (bss_list[i])
			continue;
		let bsscfg = config.bss[i];

		let mac_idx = macaddr_list[bsscfg.bssid];
		if (mac_idx < 0)
			macaddr_list[bsscfg.bssid] = i;
		if (mac_idx == i)
			continue;

		// statically assigned bssid of the new interface is in conflict
		// with the bssid of a reused interface. reassign the reused interface
		if (!bsscfg.default_macaddr) {
			// can't update bssid of the first BSS, need to restart
			if (!mac_idx < 0)
				return false;

			bsscfg = config.bss[mac_idx];
		}

		let addr = phydev.macaddr_next(i);
		if (!addr) {
			hostapd.printf(`Failed to generate mac address for phy ${name}`);
			return false;
		}
		bsscfg.bssid = addr;
	}

	let config_inline = iface_gen_config(config);

	// Step 7: fill in the gaps with new interfaces
	for (let i = 0; i < length(config.bss); i++) {
		let ifname = config.bss[i].ifname;
		let bss = bss_list[i];

		if (bss)
			continue;

		hostapd.printf(`Add bss ${ifname} on phy ${name}`);
		bss_list[i] = iface.add_bss(config_inline, i);
		if (!bss_list[i]) {
			hostapd.printf(`Failed to add new bss ${ifname} on phy ${name}`);
			return false;
		}
	}

	// Step 8: update interface bss order
	if (!iface.set_bss_order(bss_list)) {
		hostapd.printf(`Failed to update BSS order on phy '${name}'`);
		return false;
	}

	// Step 9: update config
	for (let i = 0; i < length(config.bss); i++) {
		if (!bss_list_cfg[i])
			continue;

		let ifname = config.bss[i].ifname;
		let bss = bss_list[i];

		if (is_equal(config.bss[i], bss_list_cfg[i]))
			continue;

		if (is_equal(bss_remove_file_fields(config.bss[i]),
		             bss_remove_file_fields(bss_list_cfg[i]))) {
			hostapd.printf(`Update config data files for bss ${ifname}`);
			if (bss.set_config(config_inline, i, true) < 0) {
				hostapd.printf(`Could not update config data files for bss ${ifname}`);
				return false;
			} else {
				bss.ctrl("RELOAD_WPA_PSK");
				continue;
			}
		}

		bss_reload_psk(bss, config.bss[i], bss_list_cfg[i]);
		bss_reload_rxkhs(bss, config.bss[i], bss_list_cfg[i]);
		if (is_equal(config.bss[i], bss_list_cfg[i]))
			continue;

		hostapd.printf(`Reload config for bss '${config.bss[0].ifname}' on phy '${name}'`);
		if (bss.set_config(config_inline, i) < 0) {
			hostapd.printf(`Failed to set config for bss ${ifname}`);
			return false;
		}
	}

	return true;
}

function bss_check_mld(phydev, iface_name, bss)
{
	if (!bss.ifname)
		return;

	let mld_data = hostapd.data.mld[bss.ifname];
	if (!mld_data || !mld_data.ifname || !mld_data.macaddr)
		return;

	bss.mld_bssid = mld_data.macaddr;
	mld_data.iface[iface_name] = true;

	if (!access('/sys/class/net/' + bss.ifname, 'x'))
		mld_data.has_wdev = false;

	if (mld_data.has_wdev)
		return true;

	hostapd.printf(`Create MLD interface ${bss.ifname} on phy ${phydev.name}, radio mask: ${mld_data.radio_mask}`);
	let err = phydev.wdev_add(bss.ifname, {
		mode: "ap",
		macaddr: mld_data.macaddr,
		radio_mask: mld_data.radio_mask,
	});
	wdev_set_up(bss.ifname, true);
	if (err) {
		hostapd.printf(`Failed to create MLD ${bss.ifname} on phy ${phydev.name}: ${err}`);
		delete mld_data.iface[iface_name];
		return;
	}

	mld_data.has_wdev = true;

	return true;
}

function iface_check_mld(phydev, name, config)
{
	phydev = phy_open(phydev.phy);

	for (let mld_name, mld_data in hostapd.data.mld)
		delete mld_data.iface[name];

	for (let i = 0; i < length(config.bss); i++) {
		let bss = config.bss[i];
		if (!bss.mld_ap)
			continue;

		if (!bss_check_mld(phydev, name, bss)) {
			hostapd.printf(`Skip MLD interface ${name} on phy ${phydev.name}`);
			splice(config.bss, i--, 1);
		}
	}

	for (let mld_name, mld_data in hostapd.data.mld) {
		if (length(mld_data.iface) > 0)
			continue;

		hostapd.printf(`Remove MLD interface ${mld_name}`);
		wdev_remove(mld_name);
		delete mld_data.has_wdev;
	}
}

function iface_config_remove(name, old_config)
{
	hostapd.remove_iface(name);
	return iface_remove(old_config);
}

function iface_set_config(name, config)
{
	let old_config = hostapd.data.config[name];

	hostapd.data.config[name] = config;

	let phy = config.phy;
	let phydev = phy_open(phy, config.radio_idx);
	if (!phydev) {
		hostapd.printf(`Failed to open phy ${phy}`);
		return false;
	}

	config.orig_bss = [ ...config.bss ];
	iface_check_mld(phydev, name, config);
	if (!length(config.bss))
		return iface_config_remove(name, old_config);

	try {
		let ret = iface_reload_config(name, phydev, config, old_config);
		if (ret) {
			iface_update_supplicant_macaddr(phydev, config);
			hostapd.printf(`Reloaded settings for phy ${name}`);
			return 0;
		}
	} catch (e) {
		hostapd.printf(`Error reloading config: ${e}\n${e.stacktrace[0].context}`);
	}

	hostapd.printf(`Restart interface for phy ${name}`);
	let ret = iface_restart(phydev, config, old_config);

	return ret;
}

function config_add_bss(config, name)
{
	let bss = {
		ifname: name,
		data: [],
		hash: {}
	};

	push(config.bss, bss);

	return bss;
}

function iface_load_config(phy, radio, filename)
{
	if (radio < 0)
		radio = null;

	let config = {
		phy,
		radio_idx: radio,
		radio: {
			data: []
		},
		bss: [],
		orig_file: filename,
	};

	let f = open(filename, "r");
	if (!f)
		return config;

	let bss;
	let line;
	while ((line = rtrim(f.read("line"), "\n")) != null) {
		let val = split(line, "=", 2);
		if (!val[0])
			continue;

		if (val[0] == "interface") {
			bss = config_add_bss(config, val[1]);
			break;
		}

		if (val[0] == "channel") {
			config.radio.channel = val[1];
			continue;
		}

		if (val[0] == "#num_global_macaddr")
			config[substr(val[0], 1)] = int(val[1]);
		else if (val[0] == "#macaddr_base")
			config[substr(val[0], 1)] = val[1];
		else if (val[0] == "mbssid")
			config[val[0]] = int(val[1]);

		push(config.radio.data, line);
	}

	while ((line = rtrim(f.read("line"), "\n")) != null) {
		if (line == "#default_macaddr")
			bss.default_macaddr = true;
		if (line == "#random_macaddr")
			bss.random_macaddr = true;

		let val = split(line, "=", 2);
		if (!val[0])
			continue;

		if (val[0] == "bssid") {
			bss.bssid = lc(val[1]);
			continue;
		}

		if (val[0] == "nas_identifier")
			bss.nasid = val[1];

		if (val[0] == "mld_ap")
			bss[val[0]] = int(val[1]);

		if (val[0] == "bss") {
			bss = config_add_bss(config, val[1]);
			continue;
		}

		if (hostapd.data.file_fields[val[0]]) {
			if (val[0] == "rxkh_file") {
				bss.hash[val[0]] = hostapd.sha1(normalize_rxkhs(readfile(val[1])));
			} else {
				bss.hash[val[0]] = hostapd.sha1(readfile(val[1]));
			}
		}

		push(bss.data, line);
	}
	f.close();

	return config;
}

function phy_name(phy, radio)
{
	if (!phy)
		return null;

	if (radio != null && radio >= 0)
		phy += "." + radio;

	return phy;
}

function bss_config(bss_name) {
	for (let phy, config in hostapd.data.config) {
		if (!config)
			continue;

		for (let bss in config.bss)
			if (bss.ifname == bss_name)
				return [ config, bss ];
	}
}

function mld_rename_bss(data, name)
{
	if (data.ifname == name)
		return true;

	// TODO: handle rename gracefully
	return false;
}

function mld_add_bss(name, data, phy_list, i)
{
	let config = data.config;
	if (!config.phy)
		return;

	hostapd.printf(`Add MLD interface ${name}`);
	wdev_remove(name);
	let phydev = phy_list[config.phy];
	if (!phydev) {
		phydev = phy_open(config.phy, 0);
		if (!phydev)
			return;

		let macaddr_list = {};
		let phy_config = hostapd.data.config[phy_name(config.phy, 0)];
		if (phy_config)
			macaddr_list = iface_config_macaddr_list(phy_config);
		iface_macaddr_init(phydev, data.config, macaddr_list);

		phy_list[config.phy] = phydev;
	}

	data.macaddr = config.macaddr;
	if (!data.macaddr) {
		data.macaddr = phydev.macaddr_next();
		data.default_macaddr = true;
	}

	let radio_mask = 0;
	for (let r in config.radios)
		if (r != null)
			radio_mask |= 1 << r;

	data.radio_mask = radio_mask;
	data.ifname = name;
}

function mld_find_matching_config(list, config)
{
	for (let name, data in list)
		if (is_equal(data.config, config))
			return name;
}

function mld_reload_interface(name)
{
	let config = hostapd.data.config[name];
	if (!config)
		return;

	config = { ...config };
	config.bss = config.orig_bss;

	iface_set_config(name, config);
}

function mld_set_config(config)
{
	let prev_mld = { ...hostapd.data.mld };
	let new_mld = {};
	let phy_list = {};
	let new_config = !length(prev_mld);

	hostapd.printf(`Set MLD config: ${keys(config)}`);

	// find renamed/new interfaces
	for (let name, data in config) {
		let prev = mld_find_matching_config(prev_mld, data);
		if (prev) {
			let data = prev_mld[prev];
			if (mld_rename_bss(data, name)) {
				new_mld[name] = data;
				delete prev_mld[prev];
				continue;
			}
		}

		new_mld[name] = {
			config: data,
			iface: {},
		};
	}

	let reload_iface = {};
	for (let name, data in prev_mld) {
		delete hostapd.data.mld[name];

		if (!data.ifname)
			continue;

		for (let iface, bss_list in hostapd.bss) {
			if (!bss_list[name])
				continue;
			reload_iface[iface] = true;
		}
	}

	for (let name in reload_iface)
		mld_reload_interface(name);

	for (let name, data in prev_mld) {
		if (data.ifname)
			hostapd.printf(`Remove MLD interface ${name}`);
		wdev_remove(name);
	}

	// add new interfaces
	hostapd.data.mld = new_mld;
	for (let name, data in new_mld)
		if (!data.ifname)
			mld_add_bss(name, data, phy_list);

	if (!new_config)
		return;

	hostapd.printf(`Reload all interfaces`);
	for (let name in hostapd.data.config)
		mld_reload_interface(name);
}

let main_obj = {
	reload: {
		args: {
			phy: "",
			radio: 0,
		},
		call: function(req) {
			let phy_list = req.args.phy ? [ phy_name(req.args.phy, req.args.radio) ] : keys(hostapd.data.config);
			for (let phy_name in phy_list) {
				let phy = hostapd.data.config[phy_name];
				let config = iface_load_config(phy.phy, phy.radio_idx, phy.orig_file);
				iface_set_config(phy_name, config);
			}

			return 0;
		}
	},
	apsta_state: {
		args: {
			phy: "",
			radio: 0,
			up: true,
			frequency: 0,
			sec_chan_offset: 0,
			csa: true,
			csa_count: 0,
		},
		call: function(req) {
			let phy = phy_name(req.args.phy, req.args.radio);
			if (req.args.up == null || !phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			let config = hostapd.data.config[phy];
			if (!config || !config.bss || !config.bss[0] || !config.bss[0].ifname)
				return 0;

			let iface = hostapd.interfaces[phy];
			if (!iface)
				return 0;

			if (!req.args.up) {
				iface.stop();
				return 0;
			}

			let freq_info;
			if (req.args.frequency) {
				freq_info = iface_freq_info(iface, config, req.args);
				if (!freq_info)
					return libubus.STATUS_UNKNOWN_ERROR;

				if (req.args.csa) {
					freq_info.csa_count = req.args.csa_count ?? 10;
					let ret = iface.switch_channel(freq_info);
					if (!ret)
						return libubus.STATUS_UNKNOWN_ERROR;
					return 0;
				}
			}

			let ret = iface.start(freq_info);
			if (!ret)
				return libubus.STATUS_UNKNOWN_ERROR;

			return 0;
		}
	},
	config_get_macaddr_list: {
		args: {
			phy: "",
			radio: 0,
		},
		call: function(req) {
			let phy = phy_name(req.args.phy, req.args.radio);
			if (!phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			let ret = {
				macaddr: [],
			};

			let config = hostapd.data.config[phy];
			if (!config)
				return ret;

			ret.macaddr = map(config.bss, (bss) => bss.bssid);
			return ret;
		}
	},
	switch_channel: {
		args: {
			phy: "",
			radio: 0,
			csa_count: 0,
			sec_channel: 0,
			oper_chwidth: 0,
			frequency: 0,
			center_freq1: 0,
			center_freq2: 0,
		},
		call: function(req) {
			let phy = phy_name(req.args.phy, req.args.radio);
			if (!req.args.frequency || !phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			let iface = hostapd.interfaces[phy];
			if (!iface)
				return libubus.STATUS_NOT_FOUND;

			req.args.csa_count ??= 10;
			let ret = iface.switch_channel(req.args);
			if (!ret)
				return libubus.STATUS_UNKNOWN_ERROR;

			return 0;
		},
	},
	mld_set: {
		args: {
			config: {}
		},
		call: function(req) {
			if (!req.args.config)
				return libubus.STATUS_INVALID_ARGUMENT;

			mld_set_config(req.args.config);

			return {
				pid: hostapd.getpid()
			};
		}
	},
	config_reset: {
		args: {
		},
		call: function(req) {
			for (let name in hostapd.data.config)
				iface_set_config(name);
			mld_set_config({});
			return 0;
		}
	},
	config_set: {
		args: {
			phy: "",
			radio: 0,
			config: "",
			prev_config: "",
		},
		call: function(req) {
			let phy = req.args.phy;
			let radio = req.args.radio;
			let name = phy_name(phy, radio);
			let file = req.args.config;
			let prev_file = req.args.prev_config;

			if (!phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			if (prev_file && !hostapd.data.config[name]) {
				let config = iface_load_config(phy, radio, prev_file);
				if (config)
					config.radio.data = [];
				hostapd.data.config[name] = config;
			}

			let config = iface_load_config(phy, radio, file);

			hostapd.printf(`Set new config for phy ${name}: ${file}`);
			iface_set_config(name, config);

			if (hostapd.data.auth_obj)
				hostapd.data.auth_obj.notify("reload", { phy, radio });

			return {
				pid: hostapd.getpid()
			};
		}
	},
	config_add: {
		args: {
			iface: "",
			config: "",
		},
		call: function(req) {
			if (!req.args.iface || !req.args.config)
				return libubus.STATUS_INVALID_ARGUMENT;

			if (hostapd.add_iface(`bss_config=${req.args.iface}:${req.args.config}`) < 0)
				return libubus.STATUS_INVALID_ARGUMENT;

			return {
				pid: hostapd.getpid()
			};
		}
	},
	config_remove: {
		args: {
			iface: ""
		},
		call: function(req) {
			if (!req.args.iface)
				return libubus.STATUS_INVALID_ARGUMENT;

			hostapd.remove_iface(req.args.iface);
			return 0;
		}
	},
	bss_info: {
		args: {
			iface: ""
		},
		call: function(req) {
			if (!req.args.iface)
				return libubus.STATUS_INVALID_ARGUMENT;

			let config = bss_config(req.args.iface);
			if (!config)
				return libubus.STATUS_NOT_FOUND;

			let bss = config[1];
			config = config[0];
			let ret = {};

			for (let line in [ ...config.radio.data, ...bss.data ]) {
				let fields = split(line, "=", 2);
				let name = fields[0];
				if (hostapd.data.bss_info_fields[name])
					ret[name] = fields[1];
			}

			return ret;
		}
	},
};

hostapd.data.ubus = ubus;
hostapd.data.obj = ubus.publish("hostapd", main_obj);


let auth_obj = {};
hostapd.data.auth_obj = ubus.publish("hostapd-auth", auth_obj);
hostapd.udebug_set("hostapd", hostapd.data.ubus);

function bss_event(type, name, data) {
	let ubus = hostapd.data.ubus;

	data ??= {};
	data.name = name;
	hostapd.data.obj.notify(`bss.${type}`, data, null, null, null, -1);
	ubus.call("service", "event", { type: `hostapd.${name}.${type}`, data: {} });
}

return {
	shutdown: function() {
		for (let phy in hostapd.data.config)
			iface_set_config(phy);
		hostapd.udebug_set(null);
		hostapd.ubus.disconnect();
	},
	bss_create: function(phy, name, obj) {
		phy = hostapd.data.config[phy];
		if (!phy)
			return;

		if (phy.radio_idx != null && phy.radio_idx >= 0)
			wdev_set_radio_mask(name, 1 << phy.radio_idx);
	},
	bss_add: function(phy, name, obj) {
		bss_event("add", name);
	},
	bss_reload: function(phy, name, obj, reconf) {
		bss_event("reload", name, { reconf: reconf != 0 });
	},
	bss_remove: function(phy, name, obj) {
		bss_event("remove", name);
	},
	sta_auth: function(iface, sta) {
		let msg = { iface, sta };
		let ret = {};
		let data_cb = (type, data) => {
			ret = { ...ret, ...data };
		};
		if (hostapd.data.auth_obj)
			hostapd.data.auth_obj.notify("sta_auth", msg, data_cb, null, null, 1000);
		return ret;
	},
	sta_connected: function(iface, sta, data) {
		let msg = { iface, sta, ...data };
		let ret = {};
		let data_cb = (type, data) => {
			ret = { ...ret, ...data };
		};
		if (hostapd.data.auth_obj)
			hostapd.data.auth_obj.notify("sta_connected", msg, data_cb, null, null, 1000);
		return ret;
	},
};
