let libubus = require("ubus");
import * as uloop from "uloop";
import { open, readfile, access } from "fs";
import { wdev_remove, is_equal, vlist_new, phy_is_fullmac, phy_open, wdev_set_radio_mask, wdev_set_up, macaddr_keep, macaddr_sync, macaddr_sync_defer, macaddr_sync_entry, macaddr_release_defer, mld_prev_match } from "common";

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
hostapd.data.apsta_freq = {};

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
hostapd.data.dpp_hooks = {};

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

	/*
	 * 6 GHz has no HT Operation IE, so the secondary channel offset cannot
	 * be derived the usual way. For wide channels pass a null offset so the
	 * C helper auto-derives it and computes the segment centre frequency; a
	 * 0 offset would make it skip the centre calculation.
	 */
	if (freq > 5925 && width > 0 && sec_offset == 0)
		sec_offset = null;

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

function bss_macaddr_name(group, bss)
{
	return group + "/" + bss.ifname;
}

// A link carries the MLD address where the configuration names it as its
// BSSID. A generated MLD address comes from radio 0 (mld_add_bss()).
function bss_mld_share(phydev, bss)
{
	if (!bss.mld_ap)
		return null;
	if (bss.bssid && bss.bssid == bss.mld_bssid)
		return bss.ifname;
	if (!phydev.radio && bss.default_macaddr &&
	    hostapd.data.mld[bss.ifname]?.default_macaddr)
		return bss.ifname;

	return null;
}

function bss_macaddr_args(phydev, config, bss)
{
	let data = {
		group: phydev.name,
		ifname: bss.ifname,
		share: bss_mld_share(phydev, bss),
		num_global: config.num_global_macaddr,
		macaddr_base: config.macaddr_base,
		mbssid: config.mbssid,
	};

	if (!bss.default_macaddr) {
		data.macaddr = bss.bssid;
		data.static = true;
	} else if (data.share) {
		data.macaddr = bss.mld_bssid;
	}

	return data;
}

function bss_macaddr_get(phydev, config, bss, prev)
{
	let ret = phydev.macaddr_get(ubus, "hostapd", bss_macaddr_name(phydev.name, bss),
				     bss_macaddr_args(phydev, config, bss));
	if (ret.macaddr)
		return ret.macaddr;

	let addr = ret.transport ? (bss.default_macaddr ? prev : bss.bssid) : null;
	hostapd.printf(`No MAC address from netifd for ${bss.ifname} on phy ${phydev.name}: ${ret.error}${addr ? ", using " + addr : ""}`);

	return addr;
}

function bss_macaddr_entry(phydev, bss, from)
{
	return phydev.macaddr_sync_entry(bss.bssid, {
		share: bss_mld_share(phydev, bss),
		static: !bss.default_macaddr,
		from,
	});
}

function iface_macaddr_options(config)
{
	return {
		num_global: config.num_global_macaddr,
		macaddr_base: config.macaddr_base,
		mbssid: config.mbssid,
	};
}

function iface_macaddr_prune(name, config)
{
	let names = map(config?.bss ?? [], (bss) => bss_macaddr_name(name, bss));

	macaddr_sync_defer(ubus, "hostapd", name, macaddr_keep(names),
			   iface_macaddr_options(config ?? {}));
}

function iface_macaddr_sync(phydev, config)
{
	let list = {};

	for (let bss in config.bss)
		list[bss_macaddr_name(phydev.name, bss)] = bss_macaddr_entry(phydev, bss);

	macaddr_sync_defer(ubus, "hostapd", phydev.name, list,
			   iface_macaddr_options(config));
}

function iface_supplicant_start(phydev)
{
	ubus.defer("wpa_supplicant", "phy_set_state", {
		phy: phydev.phy,
		radio: phydev.radio ?? -1,
		stop: false,
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
	if (pending.timer)
		pending.timer.cancel();
	delete pending.timer;
	switch (state) {
	case "init":
		return "create_bss";
	case "create_bss":
		if (!bss.mld_ap) {
			let err = phydev.wdev_add(bss.ifname, {
				mode: "ap",
				macaddr: bss.bssid,
				radio: phydev.radio,
			});
			if (err) {
				hostapd.printf(`Failed to create ${bss.ifname} on phy ${phy}: ${err}`);
				iface_supplicant_start(phydev);
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

	/* libubus can run this from inside a synchronous ubus call, so continue
	 * from the event loop */
	this.defer = ubus.defer(obj, method, arg, (ret, data) => {
		delete pending.defer;
		pending.timer = uloop.timer(0, () => {
			delete pending.timer;
			pending.next(ret, data);
		});
	});
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

function csa_timer_cancel(name)
{
	let timers = hostapd.data.csa_timer;
	if (timers && timers[name]) {
		timers[name].cancel();
		delete timers[name];
	}
}

function iface_restart(phydev, config, old_config)
{
	let phy = phydev.name;
	let pending = hostapd.data.pending_config[phy];

	csa_timer_cancel(phy);

	if (pending)
		pending.abort();

	hostapd.remove_iface(phy);

	let prev_bssid = {};
	for (let bss in old_config?.bss ?? [])
		if (bss.ifname && bss.bssid)
			prev_bssid[bss.ifname] = bss.bssid;

	iface_remove(old_config);
	iface_remove(config);

	// an aborted setup can have stopped wpa_supplicant
	if (!config.bss || !config.bss[0]) {
		hostapd.printf(`No bss for phy ${phy}`);
		if (pending)
			iface_supplicant_start(phydev);
		return;
	}

	iface_macaddr_prune(phydev.name, config);
	for (let bss in config.bss) {
		let addr = bss_macaddr_get(phydev, config, bss, prev_bssid[bss.ifname]);
		if (addr) {
			bss.bssid = addr;
			continue;
		}

		hostapd.printf(`Failed to get a MAC address for phy ${phy}`);
		for (let cur in config.bss)
			if (cur.default_macaddr)
				delete cur.bssid;
		if (pending)
			iface_supplicant_start(phydev);
		return;
	}
	iface_macaddr_sync(phydev, config);

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

		// An MLD BSS is identified by its netdev name, which is shared with
		// all radios that contribute a link. A rename onto the name of
		// another MLD fails with EEXIST.
		if (config.mld_ap && prev_config.bss[i].ifname != config.ifname)
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

function bss_remove(name, bss, config)
{
	hostapd.printf(`Remove bss '${config.ifname}' on phy '${name}'`);
	bss.delete();
	if (!config.mld_ap)
		wdev_remove(config.ifname);
}

const radio_chan_fields = [
	"op_class",
	"vht_oper_chwidth", "vht_oper_centr_freq_seg0_idx",
	"he_oper_chwidth", "he_oper_centr_freq_seg0_idx",
	"eht_oper_chwidth", "eht_oper_centr_freq_seg0_idx",
];

function radio_line_is_chan(line)
{
	return index(radio_chan_fields, split(line, "=", 2)[0]) >= 0;
}

// The HT40+/- direction in ht_capab is channel-derived and re-applied through
// the CSA secondary-channel offset, so strip it when comparing the base config;
// the remaining ht_capab bits are device capabilities that require a restart.
function radio_base(radio)
{
	return map(filter(radio.data, (line) => !radio_line_is_chan(line)), (line) => {
		if (substr(line, 0, 9) != "ht_capab=")
			return line;
		return replace(replace(line, "[HT40+]", ""), "[HT40-]", "");
	});
}

function radio_reload_class(old_radio, new_radio)
{
	if (is_equal(old_radio, new_radio))
		return "same";

	// anything beyond channel/width, or the channel-follow flag flipping,
	// needs a full restart
	if (!is_equal(radio_base(old_radio), radio_base(new_radio)) ||
	    (!old_radio.channel_follow) != (!new_radio.channel_follow))
		return "restart";

	// base identical and apsta flag unchanged: only treat this as a channel
	// switch if the channel or its derived width lines actually differ (the
	// derived frequency field alone appearing is not a real change)
	if (old_radio.channel == new_radio.channel &&
	    is_equal(filter(old_radio.data, radio_line_is_chan),
	             filter(new_radio.data, radio_line_is_chan)))
		return "same";

	return "channel";
}

function iface_channel_is_dfs(radio)
{
	let freq = radio.frequency;

	/* 5 GHz DFS sub-bands; 2.4 GHz and 6 GHz have no radar channels */
	return freq && ((freq >= 5250 && freq <= 5330) ||
	                (freq >= 5490 && freq <= 5730));
}

function iface_csa_check(name)
{
	if (hostapd.data.csa_timer)
		delete hostapd.data.csa_timer[name];

	let config = hostapd.data.config[name];
	let iface = hostapd.interfaces[name];
	if (!config || !iface || !iface.csa_in_progress())
		return;

	hostapd.printf(`Config channel switch on phy ${name} did not complete, restarting`);
	let phydev = phy_open(config.phy, config.radio_idx);
	if (phydev)
		iface_restart(phydev, config, config);
}

function iface_channel_switch(name, config)
{
	let radio = config.radio;

	/*
	 * The runtime channel is followed from a co-located supplicant
	 * interface (STA/mesh/adhoc) via apsta_state; the AP never picks its
	 * own channel here. Adopt the new fallback channel without touching the
	 * running BSSes.
	 *
	 * A station MLD marks every radio it spans as channel following, yet it
	 * only drives the channel of the radios it holds a link on. Radios
	 * without a link have no channel to follow, so let them apply their own
	 * configuration instead of deferring to a station that never reports one.
	 */
	if (radio.channel_follow && hostapd.data.apsta_freq[name])
		return true;

	/*
	 * A single iface-level CSA cannot coordinate the links of an MLD AP
	 * that span multiple radios, and DFS targets need CAC before use.
	 * Fall back to a full restart for both.
	 */
	for (let bss in config.bss)
		if (bss.mld_ap)
			return false;

	if (!radio.frequency || iface_channel_is_dfs(radio))
		return false;

	let iface = hostapd.interfaces[name];
	if (!iface || iface.state() != "ENABLED")
		return false;

	let freq_info = iface_freq_info(iface, config, { frequency: radio.frequency });
	if (!freq_info)
		return false;

	freq_info.csa_count = 10;
	if (!iface.switch_channel(freq_info))
		return false;

	hostapd.printf(`Channel switch to ${radio.channel} on phy ${name}`);

	/*
	 * Verify the switch actually completes; if the driver never finishes it,
	 * fall back to a full restart. Armed only here, so ubus- or apsta-
	 * triggered channel switches are left to their own recovery.
	 */
	hostapd.data.csa_timer ??= {};
	csa_timer_cancel(name);

	let beacon_int = 100;
	for (let line in config.radio.data) {
		let m = match(line, /^beacon_int=([0-9]+)/);
		if (m) {
			beacon_int = int(m[1]);
			break;
		}
	}

	hostapd.data.csa_timer[name] = uloop.timer(freq_info.csa_count * beacon_int + 2000,
		() => iface_csa_check(name));

	return true;
}

function bss_list_prev_macaddr(bss_list, old_bss_list)
{
	let old = {};
	for (let bss in old_bss_list)
		old[bss.ifname] = bss;

	return map(bss_list, (bss) => {
		let prev = old[bss.ifname];
		if (!prev ||
		    prev.default_macaddr != bss.default_macaddr ||
		    prev.random_macaddr != bss.random_macaddr)
			return bss;
		if (!bss.default_macaddr && !bss.random_macaddr)
			return bss;

		return { ...bss, bssid: prev.bssid };
	});
}

// Move the addresses in one step, so that a rename or a swap of names
// does not give an address to another BSS.
function iface_macaddr_move(phydev, config, old_config, bss_list_cfg)
{
	let list = {};

	if (!length(bss_list_cfg))
		list[bss_macaddr_name(phydev.name, old_config.bss[0])] = "";

	for (let pass = 0; pass < 2; pass++) {
		for (let i = 0; i < length(config.bss); i++) {
			let bss = config.bss[i];
			let configured = !bss.default_macaddr;
			if (configured != (pass == 0))
				continue;

			let name = bss_macaddr_name(phydev.name, bss);
			let prev = bss_list_cfg[i];
			if (!bss.bssid) {
				list[name] = "";
				continue;
			}

			list[name] = bss_macaddr_entry(phydev, bss,
				prev ? bss_macaddr_name(phydev.name, prev) : null);
		}
	}

	let ret = macaddr_sync(ubus, "hostapd", phydev.name, list,
			       iface_macaddr_options(config));
	if (!ret) {
		hostapd.printf(`Could not move the MAC addresses on phy ${phydev.name}: ${ubus.error()}`);
		return true;
	}

	if (length(ret.refused)) {
		hostapd.printf(`MAC addresses refused on phy ${phydev.name}: ${join(", ", ret.refused)}`);
		return false;
	}

	return true;
}

function iface_reload_config(name, phydev, config, old_config)
{
	let phy = phydev.name;

	if (!old_config)
		return false;

	switch (radio_reload_class(old_config.radio, config.radio)) {
	case "restart":
		return false;
	case "channel":
		if (!iface_channel_switch(name, config))
			return false;
		break;
	}

	// A configuration whose restart failed has no interface. Only a
	// running or pending one is up to date.
	let prev_macaddr_bss = bss_list_prev_macaddr(config.bss, old_config.bss);
	if (is_equal(old_config.bss, prev_macaddr_bss) &&
	    (hostapd.interfaces[name] || hostapd.data.pending_config[name])) {
		for (let i = 0; i < length(config.bss); i++)
			config.bss[i].bssid = prev_macaddr_bss[i].bssid;
		return true;
	}

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
		if (!i && phy_is_fullmac(phydev.phy)) {
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

		if ((cur_config.default_macaddr || cur_config.random_macaddr) &&
		    cur_config.random_macaddr == prev_config.random_macaddr &&
		    cur_config.default_macaddr == prev_config.default_macaddr)
			cur_config.bssid = prev_config.bssid;

		if (!cur_config.bssid)
			return false;

		bss_list[i] = prev_bss;
		bss_list_cfg[i] = old_config.bss[prev];
	}

	if (config.mbssid && !bss_list_cfg[0]) {
		hostapd.printf("First BSS changed with MBSSID enabled");
		return false;
	}

	// A BSS cannot change between plain and MLD link in place, because
	// hostapd_bss_setup_multi_link() only runs when a BSS is allocated. A
	// rename to an MLD name fails as well, since bss_check_mld() already
	// created that netdev, and a rename of an MLD link renames the netdev of
	// all radios. Delete and recreate the first BSS instead. If no other BSS
	// is kept, keep the old one until the first new BSS exists, so that the
	// PHY never runs without a BSS, but no longer: its netdev and its
	// control socket keep its name, which a later new BSS can reuse.
	let deferred_bss, deferred_cfg;
	const first_bss_converts =
		(config.bss[0].mld_ap || old_config.bss[0].mld_ap) &&
		old_config.bss[0].ifname != config.bss[0].ifname;

	// Step 2: if none were found, rename and preserve the first one
	if (length(bss_list) == 0 && !first_bss_converts) {
		// can't change the bssid of the first bss
		let mld_addr = config.bss[0].default_macaddr &&
			bss_mld_share(phydev, config.bss[0]) &&
			config.bss[0].mld_bssid;
		if (mld_addr && old_config.bss[0].bssid != mld_addr) {
			hostapd.printf(`MLD address of first interface changed: ${lc(old_config.bss[0].bssid)} -> ${lc(mld_addr)}`);
			return false;
		}

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

		bss_list[0] = prev_bss;
		bss_list_cfg[0] = old_config.bss[0];
		prev_bss_hash[0] = null;
	}

	if (!iface_macaddr_move(phydev, config, old_config, bss_list_cfg))
		return false;

	// Step 3: delete all unused old interfaces
	for (let i = 0; i < length(prev_bss_hash); i++) {
		if (!prev_bss_hash[i])
			continue;

		let prev_bss = get_config_bss(name, old_config, i);
		if (!prev_bss)
			return false;

		if (!i && !length(bss_list)) {
			deferred_bss = prev_bss;
			deferred_cfg = old_config.bss[0];
			continue;
		}

		bss_remove(name, prev_bss, old_config.bss[i]);
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
	for (let i = 0; i < length(config.bss); i++) {
		let bsscfg = config.bss[i];
		if (bsscfg.bssid)
			continue;

		let addr = bss_macaddr_get(phydev, config, bsscfg);
		if (!addr) {
			hostapd.printf(`Failed to get a MAC address for phy ${name}`);
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

		if (deferred_bss) {
			bss_remove(name, deferred_bss, deferred_cfg);
			deferred_bss = null;
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

	iface_macaddr_sync(phydev, config);

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
	if (err) {
		hostapd.printf(`Failed to create MLD ${bss.ifname} on phy ${phydev.name}: ${err}`);
		delete mld_data.iface[iface_name];
		return;
	}

	err = wdev_set_up(bss.ifname, true);
	if (err) {
		hostapd.printf(`Failed to bring up MLD ${bss.ifname} on phy ${phydev.name}: ${err}`);
		wdev_remove(bss.ifname);
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
	let pending = hostapd.data.pending_config[name];

	if (pending)
		pending.abort();

	delete hostapd.data.apsta_freq[name];
	hostapd.remove_iface(name);
	return iface_remove(old_config);
}

// No fallback to a PHY restart, which would take the other BSSes down.
function mld_link_bss_remove(phy, name)
{
	let config = hostapd.data.config[phy];
	let bss = hostapd.bss[phy]?.[name];
	if (!config || !bss)
		return;

	let idx = index(map(config.bss, (b) => b.ifname), name);
	if (idx < 0)
		return;

	hostapd.printf(`Remove expired link of MLD ${name} on phy '${phy}'`);

	let mld_data = hostapd.data.mld[name];
	if (mld_data?.iface)
		delete mld_data.iface[phy];
	if (mld_data?.config?.radios)
		mld_data.config = {
			...mld_data.config,
			radios: filter(mld_data.config.radios, (r) => r != config.radio_idx),
		};

	config.orig_bss = filter(config.orig_bss ?? config.bss, (b) => b.ifname != name);

	if (length(config.bss) == 1) {
		delete hostapd.data.config[phy];
		iface_macaddr_prune(phy, null);
		iface_config_remove(phy, config);
		return;
	}

	let bss_config = config.bss[idx];
	splice(config.bss, idx, 1);
	bss_remove(phy, bss, bss_config);
	iface_macaddr_prune(phy, config);
}

function iface_set_config(name, config)
{
	let old_config = hostapd.data.config[name];

	if (!config) {
		delete hostapd.data.config[name];
		return iface_config_remove(name, old_config);
	}

	hostapd.data.config[name] = config;

	let phy = config.phy;
	let phydev = phy_open(phy, config.radio_idx);
	if (!phydev) {
		hostapd.printf(`Failed to open phy ${phy}`);
		return false;
	}

	config.orig_bss = [ ...config.bss ];
	iface_check_mld(phydev, name, config);
	if (!length(config.bss)) {
		iface_macaddr_prune(name, config);
		return iface_config_remove(name, old_config);
	}

	try {
		let ret = iface_reload_config(name, phydev, config, old_config);
		if (ret) {
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

		if (substr(line, 0, 2) == "# ")
			continue;

		if (val[0] == "interface") {
			bss = config_add_bss(config, val[1]);
			break;
		}

		if (val[0] == "channel") {
			config.radio.channel = val[1];
			continue;
		}

		if (val[0] == "#frequency") {
			config.radio.frequency = int(val[1]);
			continue;
		}

		if (val[0] == "#channel_follow") {
			config.radio.channel_follow = int(val[1]) == 1;
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

		if (substr(line, 0, 2) == "# ")
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

	for (let bss in config.bss)
		if (!bss.bssid)
			bss.default_macaddr = true;

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

function mld_radio_mask(radios)
{
	let radio_mask = 0;
	for (let r in radios)
		if (r != null)
			radio_mask |= 1 << r;

	return radio_mask;
}

function mld_macaddr_get(phydev, name, config, prev)
{
	let ret = phydev.macaddr_get(ubus, "hostapd", name, {
		group: "mld",
		ifname: name,
		macaddr: config.macaddr,
		static: !!config.macaddr,
		any_radio: true,
	});
	if (ret.macaddr)
		return ret.macaddr;

	let addr = ret.transport ? (config.macaddr ?? prev) : null;
	hostapd.printf(`No MAC address from netifd for MLD ${name}: ${ret.error}${addr ? ", using " + addr : ""}`);

	return addr;
}

function mld_add_bss(name, data, phy_list, prev)
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

		phy_list[config.phy] = phydev;
	}

	data.macaddr = mld_macaddr_get(phydev, name, config, prev?.macaddr);
	if (!data.macaddr)
		return;

	data.default_macaddr = !config.macaddr;

	data.radio_mask = mld_radio_mask(config.radios);
	data.ifname = name;
}

// An MLD netdev is defined by its name, wiphy, radio mask and address, i.e. the
// config fields used by mld_add_bss(). The caller looks up the previous entry by
// name. All remaining fields are BSS level and reach hostapd through the per PHY
// config file, so a change to them must not destroy the netdev and all BSSes
// attached to it.
//
// A radio that leaves the MLD removes only its own link. A radio that joins
// gets a new netdev, as mt7996 keeps the state of a removed link until the
// netdev goes.
function mld_config_matches(data, config)
{
	if (!data.ifname || !data.config)
		return false;

	if (data.config.phy != config.phy)
		return false;

	if (mld_radio_mask(config.radios) & ~mld_radio_mask(data.config.radios))
		return false;

	// cfg80211 refuses a link with another SSID while a link beacons
	if (data.config.ssid != config.ssid)
		return false;

	if (config.macaddr)
		return data.macaddr == config.macaddr;

	return !!data.default_macaddr;
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

function mld_prev_set(new_mld, prev_mld)
{
	let news = {};
	let prevs = {};

	for (let name, data in new_mld)
		if (!data.ifname && !data.config.macaddr)
			news[name] = data.config;
	for (let name, data in prev_mld)
		if (data.default_macaddr && data.macaddr)
			prevs[name] = data.config;

	for (let name, from in mld_prev_match(news, prevs))
		new_mld[name].prev_name = from;
}

// The links of a removed MLD give up their entries on every radio, and
// those of a continued MLD move to the links of the new MLD. A move needs
// a target name without a live entry; in a cycle of renames the entries
// stay stale and the links claim them by name.
function mld_links_release(new_mld, prev_mld)
{
	let next = {};
	for (let name, data in new_mld)
		if (data.prev_name)
			next[data.prev_name] = name;

	let order = [];
	let seen = {};
	let visit;
	visit = function(name) {
		if (seen[name])
			return;

		seen[name] = true;
		if (next[name] && prev_mld[next[name]])
			visit(next[name]);
		push(order, name);
	};
	for (let name in prev_mld)
		visit(name);

	for (let group, config in hostapd.data.config) {
		if (!config)
			continue;

		for (let name in order)
			macaddr_release_defer(ubus, "hostapd", bss_macaddr_name(group, { ifname: name }),
				next[name] && next[name] != name ? {
					to: bss_macaddr_name(group, { ifname: next[name] }),
					ifname: next[name],
					share: next[name],
					replace: !prev_mld[next[name]],
				} : null);
	}
}

function mld_macaddr_list(new_mld, prev_mld)
{
	let list = {};

	for (let name, data in new_mld) {
		let prev = prev_mld[data.prev_name];
		list[name] = "";
		if (data.ifname || !prev || data.prev_name == name)
			continue;

		list[name] = macaddr_sync_entry(data.config.phy, -1, prev.macaddr, {
			any_radio: true,
		});
	}

	return list;
}

function mld_set_config(config)
{
	let prev_mld = { ...hostapd.data.mld };
	let new_mld = {};
	let phy_list = {};
	let new_config = !length(prev_mld) && length(config);

	hostapd.printf(`Set MLD config: ${keys(config)}`);

	// find kept/new interfaces
	for (let name, data in config) {
		let prev = prev_mld[name];
		if (prev && mld_config_matches(prev, data)) {
			let mask = mld_radio_mask(data.radios);
			if (mask != mld_radio_mask(prev.config.radios))
				hostapd.printf(`Keep MLD interface ${name}, radios ${mask} of mask ${prev.radio_mask}`);
			if (!prev.has_wdev)
				prev.radio_mask = mask;
			prev.config = data;
			new_mld[name] = prev;
			delete prev_mld[name];
			continue;
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

	mld_prev_set(new_mld, prev_mld);
	mld_links_release(new_mld, prev_mld);

	// add new interfaces
	hostapd.data.mld = new_mld;
	macaddr_sync_defer(ubus, "hostapd", "mld", mld_macaddr_list(new_mld, prev_mld));
	for (let name, data in new_mld)
		if (!data.ifname)
			mld_add_bss(name, data, phy_list, prev_mld[data.prev_name ?? name]);

	if (!new_config)
		return;

	hostapd.printf(`Reload all interfaces`);
	for (let name in hostapd.data.config)
		mld_reload_interface(name);
}

function dpp_find_bss(ifname)
{
	for (let phy, bss_list in hostapd.bss) {
		if (bss_list[ifname])
			return bss_list[ifname];
	}
	return null;
}

function dpp_channel_handle_request(channel, req)
{
	let data = req.args ?? {};
	let bss;

	switch (req.type) {
	case "start":
		if (!data.ifname)
			return libubus.STATUS_INVALID_ARGUMENT;
		let old_hook = hostapd.data.dpp_hooks[data.ifname];
		if (old_hook && old_hook.channel != channel)
			old_hook.channel.disconnect();
		hostapd.data.dpp_hooks[data.ifname] = {
			channel: channel,
			timeout_count: 0,
		};
		return 0;

	case "stop":
		if (!data.ifname)
			return libubus.STATUS_INVALID_ARGUMENT;
		let hook = hostapd.data.dpp_hooks[data.ifname];
		if (hook && hook.channel == channel)
			delete hostapd.data.dpp_hooks[data.ifname];
		return 0;

	case "tx_action":
		bss = dpp_find_bss(data.ifname);
		if (!bss)
			return libubus.STATUS_NOT_FOUND;
		if (!bss.dpp_send_action(data.dst, data.freq ?? 0, data.frame))
			return libubus.STATUS_UNKNOWN_ERROR;
		return 0;

	case "tx_gas_resp":
		bss = dpp_find_bss(data.ifname);
		if (!bss)
			return libubus.STATUS_NOT_FOUND;
		if (!bss.dpp_send_gas_resp(data.dst, data.dialog_token, data.data, data.freq ?? 0))
			return libubus.STATUS_UNKNOWN_ERROR;
		return 0;

	case "set_cce":
		bss = dpp_find_bss(data.ifname);
		if (!bss)
			return libubus.STATUS_NOT_FOUND;
		let val = data.enable ? "dd04506f9a1e" : "";
		bss.ctrl("SET vendor_elements " + val);
		bss.ctrl("UPDATE_BEACON");
		return 0;

	default:
		return libubus.STATUS_METHOD_NOT_FOUND;
	}
}

function dpp_channel_handle_disconnect(channel)
{
	for (let ifname, hook in hostapd.data.dpp_hooks) {
		if (hook.channel == channel)
			delete hostapd.data.dpp_hooks[ifname];
	}
}

function dpp_rx_via_channel(ifname, method, data)
{
	let hook = hostapd.data.dpp_hooks[ifname];
	if (!hook)
		return null;

	let response = hook.channel.request({
		method: method,
		data: data,
	});
	if (hook.channel.error(true) == libubus.STATUS_TIMEOUT) {
		hook.timeout_count++;
		if (hook.timeout_count >= 3) {
			hostapd.printf(`DPP channel timeout for ${ifname}, disconnecting`);
			hook.channel.disconnect();
			delete hostapd.data.dpp_hooks[ifname];
		}
		return null;
	}

	hook.timeout_count = 0;
	return response;
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
			no_link: true,
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
				delete hostapd.data.apsta_freq[phy];
				iface.stop();
				return 0;
			}

			if (req.args.frequency)
				hostapd.data.apsta_freq[phy] = req.args.frequency;
			else if (req.args.no_link && hostapd.data.apsta_freq[phy]) {
				/*
				 * The station gave up the link that dictated this radio's
				 * channel. Hand the radio back to its own configuration
				 * rather than leaving the AP on the channel the station
				 * happened to use last.
				 */
				delete hostapd.data.apsta_freq[phy];

				if (iface_channel_switch(phy, config))
					return 0;

				let phydev = phy_open(config.phy, config.radio_idx);
				if (phydev)
					iface_restart(phydev, config, config);

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
	mbo_assoc_disallow: {
		args: {
			iface: "",
			reason: 0,
		},
		call: function(req) {
			if (!req.args.iface)
				return libubus.STATUS_INVALID_ARGUMENT;

			// Every link of an AP MLD is a BSS of its own, and each builds
			// its own Beacon.
			let found = false;
			for (let phy, bss_list in hostapd.bss) {
				let bss = bss_list[req.args.iface];
				if (!bss)
					continue;

				found = true;
				if (bss.ctrl(`SET mbo_assoc_disallow ${+req.args.reason}`) != "OK")
					return libubus.STATUS_UNKNOWN_ERROR;
			}

			return found ? 0 : libubus.STATUS_NOT_FOUND;
		}
	},
	mld_link_remove: {
		args: {
			phy: "",
			radio: 0,
			iface: "",
			count: 0,
		},
		call: function(req) {
			let phy = phy_name(req.args.phy, req.args.radio);
			if (!phy || !req.args.iface || !req.args.count ||
			    req.args.count < 0 || req.args.count > 0xffff)
				return libubus.STATUS_INVALID_ARGUMENT;

			let bss = hostapd.bss[phy]?.[req.args.iface];
			if (!bss)
				return libubus.STATUS_NOT_FOUND;

			let removal = bss.link_remove?.(req.args.count);
			if (removal == null)
				return libubus.STATUS_NOT_SUPPORTED;

			return removal;
		}
	},
	status: {
		args: {},
		call: function(req) {
			let interfaces = {};

			for (let phy_name, config in hostapd.data.config) {
				if (!config || !config.bss)
					continue;

				let is_pending = !!hostapd.data.pending_config[phy_name];
				let iface = hostapd.interfaces[phy_name];
				let state = iface?.state();
				let is_running = iface && state == "ENABLED" && !is_pending;

				for (let bss in config.bss) {
					let ifname = bss.ifname;
					let entry = interfaces[ifname];

					if (bss.mld_ap) {
						if (!entry) {
							let mld = hostapd.data.mld[ifname];
							entry = interfaces[ifname] = {
								wiphy: config.phy,
								macaddr: mld ? mld.macaddr : bss.mld_bssid,
								links: {},
							};
						}
						entry.links[config.radio_idx ?? 0] = {
							radio: config.radio_idx ?? 0,
							macaddr: bss.bssid,
							running: is_running,
							pending: is_pending,
							state,
						};
					} else {
						entry = {
							wiphy: config.phy,
							macaddr: bss.bssid,
							running: is_running,
							pending: is_pending,
							state,
						};
						if (config.radio_idx != null && config.radio_idx >= 0)
							entry.radio = config.radio_idx;
						interfaces[ifname] = entry;
					}
				}
			}

			return { interfaces };
		}
	},
	dpp_channel: {
		args: {},
		call: function(req) {
			let channel;
			let on_request = (chan_req) => dpp_channel_handle_request(channel, chan_req);
			let on_disconnect = () => dpp_channel_handle_disconnect(channel);

			channel = req.new_channel(on_request, on_disconnect, 1);
			if (!channel)
				return libubus.STATUS_UNKNOWN_ERROR;

			return 0;
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
		hostapd.data.ubus.disconnect();
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
		delete hostapd.data.dpp_hooks[name];
		bss_event("remove", name);
	},
	bss_link_removed: function(phy, name, obj) {
		mld_link_bss_remove(phy, name);
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
	dpp_rx_action: function(iface, src, frame_type, freq, frame) {
		let response = dpp_rx_via_channel(iface, "rx_action", {
			ifname: iface, src, frame_type, freq, frame,
		});
		if (response && response.handled)
			return true;
		return false;
	},
	dpp_rx_gas: function(iface, src, dialog_token, query, freq) {
		let response = dpp_rx_via_channel(iface, "rx_gas", {
			ifname: iface, src, dialog_token, query, freq,
		});
		if (response && response.response)
			return response.response;
		return null;
	},
	wps_m7_rx: function(ifname, addr, data) {
		let response = dpp_rx_via_channel(ifname, "wps_m7_rx", {
			ifname, addr, data,
		});
		if (!response)
			return null;
		return response;
	},
};
