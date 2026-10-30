import * as nl80211 from "nl80211";
import * as rtnl from "rtnl";
import { readfile, glob, basename, readlink, open } from "fs";

const iftypes = {
	ap: nl80211.const.NL80211_IFTYPE_AP,
	mesh: nl80211.const.NL80211_IFTYPE_MESH_POINT,
	sta: nl80211.const.NL80211_IFTYPE_STATION,
	adhoc: nl80211.const.NL80211_IFTYPE_ADHOC,
	monitor: nl80211.const.NL80211_IFTYPE_MONITOR,
};

const mesh_params = {
	mesh_retry_timeout: "retry_timeout",
	mesh_confirm_timeout: "confirm_timeout",
	mesh_holding_timeout: "holding_timeout",
	mesh_max_peer_links: "max_peer_links",
	mesh_max_retries: "max_retries",
	mesh_ttl: "ttl",
	mesh_element_ttl: "element_ttl",
	mesh_auto_open_plinks: "auto_open_plinks",
	mesh_hwmp_max_preq_retries: "hwmp_max_preq_retries",
	mesh_path_refresh_time: "path_refresh_time",
	mesh_min_discovery_timeout: "min_discovery_timeout",
	mesh_hwmp_active_path_timeout: "hwmp_active_path_timeout",
	mesh_hwmp_preq_min_interval: "hwmp_preq_min_interval",
	mesh_hwmp_net_diameter_traversal_time: "hwmp_net_diam_trvs_time",
	mesh_hwmp_rootmode: "hwmp_rootmode",
	mesh_hwmp_rann_interval: "hwmp_rann_interval",
	mesh_gate_announcements: "gate_announcements",
	mesh_sync_offset_max_neighor: "sync_offset_max_neighbor",
	mesh_rssi_threshold: "rssi_threshold",
	mesh_hwmp_active_path_to_root_timeout: "hwmp_path_to_root_timeout",
	mesh_hwmp_root_interval: "hwmp_root_interval",
	mesh_hwmp_confirmation_interval: "hwmp_confirmation_interval",
	mesh_awake_window: "awake_window",
	mesh_plink_timeout: "plink_timeout",
	mesh_fwding: "forwarding",
	mesh_power_mode: "power_mode",
	mesh_nolearn: "nolearn"
};

function wdev_remove(name)
{
	nl80211.request(nl80211.const.NL80211_CMD_DEL_INTERFACE, 0, { dev: name });
}

function __phy_is_fullmac(phyidx)
{
	let data = nl80211.request(nl80211.const.NL80211_CMD_GET_WIPHY, 0, { wiphy: phyidx });

	return !data.software_iftypes.monitor;
}

function phy_is_fullmac(phy)
{
	let phyidx = int(trim(readfile(`/sys/class/ieee80211/${phy}/index`)));

	return __phy_is_fullmac(phyidx);
}

function find_reusable_wdev(phyidx)
{
	if (!__phy_is_fullmac(phyidx))
		return null;

	let data = nl80211.request(
		nl80211.const.NL80211_CMD_GET_INTERFACE,
		nl80211.const.NLM_F_DUMP,
		{ wiphy: phyidx });
	for (let res in data)
		if (trim(readfile(`/sys/class/net/${res.ifname}/operstate`)) == "down")
			return res.ifname;
	return null;
}

function wdev_set_radio_mask(name, mask)
{
	nl80211.request(nl80211.const.NL80211_CMD_SET_INTERFACE, 0, {
		dev: name,
		vif_radio_mask: mask
	});
}

// a driver without ndo_set_mac_address keeps the old address
function netdev_macaddr_set(name, macaddr)
{
	let cur = readfile(`/sys/class/net/${name}/address`);
	if (cur && lc(trim(cur)) == lc(macaddr))
		return;

	if (!rtnl.request(rtnl.const.RTM_SETLINK, 0, { dev: name, change: 1, flags: 0 }) ||
	    !rtnl.request(rtnl.const.RTM_SETLINK, 0, { dev: name, address: macaddr }))
		warn(`Could not set MAC address ${macaddr} on ${name}: ${rtnl.error()}\n`);
}

function wdev_create(phy, name, data)
{
	let phyidx = int(readfile(`/sys/class/ieee80211/${phy}/index`));

	wdev_remove(name);

	if (!iftypes[data.mode])
		return `Invalid mode: ${data.mode}`;

	if (!data.macaddr && data.mode != "monitor")
		return `No MAC address for ${name}`;

	let req = {
		wiphy: phyidx,
		ifname: name,
		iftype: iftypes[data.mode],
	};

	if (data["4addr"])
		req["4addr"] = data["4addr"];
	if (data.macaddr)
		req.mac = data.macaddr;
	if (data.radio_mask > 0)
		req.vif_radio_mask = data.radio_mask;
	else if (data.radio != null && data.radio >= 0)
		req.vif_radio_mask = 1 << data.radio;

	nl80211.error();

	let reused;
	let reuse_ifname = find_reusable_wdev(phyidx);
	if (reuse_ifname &&
	    (reuse_ifname == name ||
	     rtnl.request(rtnl.const.RTM_SETLINK, 0, { dev: reuse_ifname, ifname: name}) != false)) {
		req.dev = req.ifname;
		delete req.ifname;
		nl80211.request(nl80211.const.NL80211_CMD_SET_INTERFACE, 0, req);
		reused = true;
	} else {
		nl80211.request(
			nl80211.const.NL80211_CMD_NEW_INTERFACE,
			nl80211.const.NLM_F_CREATE,
			req);
	}

	let error = nl80211.error();
	if (error)
		return error;

	// nl80211_set_interface() ignores NL80211_ATTR_MAC
	if (reused && data.macaddr)
		netdev_macaddr_set(name, data.macaddr);

	if (data.powersave != null) {
		nl80211.request(nl80211.const.NL80211_CMD_SET_POWER_SAVE, 0,
			{ dev: name, ps_state: data.powersave ? 1 : 0});
	}

	return null;
}

function wdev_set_mesh_params(name, data)
{
	let mesh_cfg = {};

	for (let key in mesh_params) {
		let val = data[key];
		if (val == null)
			continue;
		mesh_cfg[mesh_params[key]] = int(val);
	}

	if (!length(mesh_cfg))
		return null;

	nl80211.request(nl80211.const.NL80211_CMD_SET_MESH_CONFIG, 0,
		{ dev: name, mesh_params: mesh_cfg });

	return nl80211.error();
}

function wdev_set_up(name, up)
{
	let ret = rtnl.request(rtnl.const.RTM_SETLINK, 0, { dev: name, change: 1, flags: up ? 1 : 0 });
	if (!ret)
		return rtnl.error() ?? "Could not set the interface flags";

	return null;
}

// netifd rejects null fields and fields of a mismatched type
function macaddr_args(data)
{
	let args = {};

	for (let key, val in data) {
		if (val == null)
			continue;

		switch (key) {
		case "radio":
		case "num_global":
		case "mbssid":
			val = int(val);
			if (val != val)
				continue;
			break;
		case "static":
		case "any_radio":
		case "replace":
			val = !!val;
			break;
		default:
			val = "" + val;
			break;
		}

		args[key] = val;
	}

	return args;
}

function macaddr_keep(names)
{
	let list = {};

	for (let name in names)
		list[name] = "";

	return list;
}

function macaddr_sync_args(owner, group, list, options)
{
	return {
		...macaddr_args(options ?? {}),
		owner, group,
		macaddr: list,
	};
}

function macaddr_sync(ubus, owner, group, list, options)
{
	return ubus.call("network.wireless", "macaddr_sync",
			 macaddr_sync_args(owner, group, list, options));
}

function macaddr_sync_defer(ubus, owner, group, list, options)
{
	ubus.defer("network.wireless", "macaddr_sync",
		   macaddr_sync_args(owner, group, list, options));
}

function macaddr_sync_entry(phy, radio, macaddr, data)
{
	return macaddr_args({
		...data,
		macaddr, phy,
		radio: radio ?? -1,
	});
}

function macaddr_release_defer(ubus, owner, name, move)
{
	ubus.defer("network.wireless", "macaddr_release",
		   macaddr_args({ ...(move ?? {}), owner, name }));
}

function radio_list_mask(radios)
{
	let mask = 0;

	for (let radio in radios)
		if (radio != null)
			mask |= 1 << radio;

	return mask;
}

function mld_prev_pass(ret, news, free, match)
{
	for (let name in sort(keys(news))) {
		if (ret[name])
			continue;

		let cur = filter(free, (prev) => match(name, prev))[0];
		if (!cur)
			continue;

		ret[name] = cur;
		splice(free, index(free, cur), 1);
	}
}

// Map each new MLD that continues a removed one, first by SSID and radios,
// then by SSID, to the previous name. The MLD takes over its address.
function mld_prev_match(news, prevs)
{
	let ret = {};
	let free = sort(keys(prevs));
	let same_phy = (name, prev) => prevs[prev].phy == news[name].phy;
	let same_ssid = (name, prev) => news[name].ssid != null &&
					prevs[prev].ssid == news[name].ssid;
	let same_radios = (name, prev) =>
		radio_list_mask(prevs[prev].radios) == radio_list_mask(news[name].radios);

	mld_prev_pass(ret, news, free, (name, prev) =>
		same_phy(name, prev) && same_ssid(name, prev) && same_radios(name, prev));
	mld_prev_pass(ret, news, free, (name, prev) =>
		same_phy(name, prev) && same_ssid(name, prev));

	return ret;
}

const phy_proto = {
	macaddr_get: function(ubus, owner, name, data) {
		let args = macaddr_args({
			...data,
			phy: this.phy,
			radio: this.radio ?? -1,
			owner, name,
		});

		let ret = ubus.call("network.wireless", "macaddr_get", args);
		if (type(ret) == "object")
			return ret;

		return {
			error: ubus.error() ?? "no reply",
			transport: true,
		};
	},

	macaddr_sync_entry: function(macaddr, data) {
		return macaddr_sync_entry(this.phy, this.radio, macaddr, data);
	},

	wdev_add: function(name, data) {
		return wdev_create(this.phy, name, {
			...data,
			radio: this.radio,
		});
	},

	wdev_list: function() {
		return nl80211.request(
			nl80211.const.NL80211_CMD_GET_INTERFACE,
			nl80211.const.NLM_F_DUMP,
			{ wiphy: this.idx }
		) ?? [];
	},
};

function phy_open(phy, radio)
{
	let phyidx = readfile(`/sys/class/ieee80211/${phy}/index`);
	if (!phyidx)
		return null;

	let name = phy;
	if (radio === "" || radio < 0)
		radio = null;
	if (radio != null) {
		radio = int(radio);
		name += "." + radio;
	}

	return proto({
		phy, name, radio,
		idx: int(phyidx),
	}, phy_proto);
}

const vlist_proto = {
	update: function(values, arg) {
		let data = this.data;
		let cb = this.cb;
		let seq = { };
		let new_data = {};
		let old_data = {};

		this.data = new_data;

		if (type(values) == "object") {
			for (let key in values) {
				old_data[key] = data[key];
				new_data[key] = values[key];
				delete data[key];
			}
		} else {
			for (let val in values) {
				let cur_key = val[0];
				let cur_obj = val[1];

				old_data[cur_key] = data[cur_key];
				new_data[cur_key] = val[1];
				delete data[cur_key];
			}
		}

		for (let key in data) {
			cb(null, data[key], arg);
			delete data[key];
		}
		for (let key in new_data)
			cb(new_data[key], old_data[key], arg);
	}
};

function is_equal(val1, val2) {
	let t1 = type(val1);

	if (t1 != type(val2))
		return false;

	if (t1 == "array") {
		if (length(val1) != length(val2))
			return false;

		for (let i = 0; i < length(val1); i++)
			if (!is_equal(val1[i], val2[i]))
				return false;

		return true;
	} else if (t1 == "object") {
		for (let key in val1)
			if (!is_equal(val1[key], val2[key]))
				return false;
		for (let key in val2)
			if (val1[key] == null)
				return false;
		return true;
	} else {
		return val1 == val2;
	}
}

function vlist_new(cb) {
	return proto({
		cb: cb,
		data: {}
	}, vlist_proto);
}

export { wdev_remove, wdev_create, wdev_set_mesh_params, wdev_set_radio_mask, wdev_set_up, is_equal, vlist_new, phy_is_fullmac, phy_open, macaddr_keep, macaddr_sync, macaddr_sync_defer, macaddr_sync_entry, macaddr_release_defer, mld_prev_match };
