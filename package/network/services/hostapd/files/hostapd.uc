let libubus = require("ubus");
import { open, readfile } from "fs";
import { wdev_create, wdev_remove, is_equal, vlist_new, phy_is_fullmac } from "common";

let ubus = libubus.connect();

hostapd.data.config = {};

hostapd.data.file_fields = {
	vlan_file: true,
	wpa_psk_file: true,
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

function iface_remove(cfg)
{
	if (!cfg || !cfg.bss || !cfg.bss[0] || !cfg.bss[0].ifname)
		return;

	hostapd.remove_iface(cfg.bss[0].ifname);
	for (let bss in cfg.bss)
		wdev_remove(bss.ifname);
}

function iface_gen_config(phy, config, start_disabled)
{
	let str = `data:
${join("\n", config.radio.data)}
channel=${config.radio.channel}
`;

	for (let i = 0; i < length(config.bss); i++) {
		let bss = config.bss[i];
		let type = i > 0 ? "bss" : "interface";

		str += `
${type}=${bss.ifname}
${join("\n", bss.data)}
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
	let config_inline = iface_gen_config(phy, config, !!phy_status);

	let bss = config.bss[0];
	let ret = hostapd.add_iface(`bss_config=${bss.ifname}:${config_inline}`);
	if (ret < 0)
		return false;

	if (!phy_status)
		return true;

	let iface = hostapd.interfaces[bss.ifname];
	if (!iface)
		return false;

	let freq_info = iface_freq_info(iface, config, phy_status);

	return iface.start(freq_info) >= 0;
}

function iface_restart(phy, config, old_config)
{
	iface_remove(old_config);
	iface_remove(config);

	if (!config.bss || !config.bss[0]) {
		hostapd.printf(`No bss for phy ${phy}`);
		return;
	}

	let bss = config.bss[0];
	let err = wdev_create(phy, bss.ifname, { mode: "ap" });
	if (err)
		hostapd.printf(`Failed to create ${bss.ifname} on phy ${phy}: ${err}`);

	let ubus = hostapd.data.ubus;
	let phy_status = ubus.call("wpa_supplicant", "phy_status", { phy: phy });
	if (phy_status && phy_status.state == "COMPLETED") {
		if (iface_add(phy, config, phy_status))
			return;

		hostapd.printf(`Failed to bring up phy ${phy} ifname=${bss.ifname} with supplicant provided frequency`);
	}

	ubus.call("wpa_supplicant", "phy_set_state", { phy: phy, stop: true });
	if (!iface_add(phy, config))
		hostapd.printf(`hostapd.add_iface failed for phy ${phy} ifname=${bss.ifname}`);
	ubus.call("wpa_supplicant", "phy_set_state", { phy: phy, stop: false });
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

function iface_reload_config(phy, config, old_config)
{
	if (!old_config || !is_equal(old_config.radio, config.radio))
		return false;

	if (is_equal(old_config.bss, config.bss))
		return true;

	if (!old_config.bss || !old_config.bss[0])
		return false;

	if (config.bss[0].ifname != old_config.bss[0].ifname)
		return false;

	let iface_name = config.bss[0].ifname;
	let iface = hostapd.interfaces[iface_name];
	if (!iface)
		return false;

	let first_bss = hostapd.bss[iface_name];
	if (!first_bss)
		return false;

	let config_inline = iface_gen_config(phy, config);

	bss_reload_psk(first_bss, config.bss[0], old_config.bss[0]);
	if (!is_equal(config.bss[0], old_config.bss[0])) {
		if (phy_is_fullmac(phy))
			return false;

		if (config.bss[0].bssid != old_config.bss[0].bssid)
			return false;

		hostapd.printf(`Reload config for bss '${config.bss[0].ifname}' on phy '${phy}'`);
		if (first_bss.set_config(config_inline, 0) < 0) {
			hostapd.printf(`Failed to set config`);
			return false;
		}
	}

	let new_cfg = array_to_obj(config.bss, "ifname", 1);
	let old_cfg = array_to_obj(old_config.bss, "ifname", 1);

	for (let name in old_cfg) {
		let bss = hostapd.bss[name];
		if (!bss) {
			hostapd.printf(`bss '${name}' not found`);
			return false;
		}

		if (!new_cfg[name]) {
			hostapd.printf(`Remove bss '${name}' on phy '${phy}'`);
			bss.delete();
			wdev_remove(name);
			continue;
		}

		let new_cfg_data = new_cfg[name];
		delete new_cfg[name];

		if (is_equal(old_cfg[name], new_cfg_data))
			continue;

		hostapd.printf(`Reload config for bss '${name}' on phy '${phy}'`);
		let idx = find_array_idx(config.bss, "ifname", name);
		if (idx < 0) {
			hostapd.printf(`bss index not found`);
			return false;
		}

		if (bss.set_config(config_inline, idx) < 0) {
			hostapd.printf(`Failed to set config`);
			return false;
		}
	}

	for (let name in new_cfg) {
		hostapd.printf(`Add bss '${name}' on phy '${phy}'`);

		let idx = find_array_idx(config.bss, "ifname", name);
		if (idx < 0) {
			hostapd.printf(`bss index not found`);
			return false;
		}

		if (iface.add_bss(config_inline, idx) < 0) {
			hostapd.printf(`Failed to add bss`);
			return false;
		}
	}

	return true;
}

function iface_set_config(phy, config)
{
	let old_config = hostapd.data.config[phy];

	hostapd.data.config[phy] = config;

	if (!config)
		return iface_remove(old_config);

	let ret = iface_reload_config(phy, config, old_config);
	if (ret) {
		hostapd.printf(`Reloaded settings for phy ${phy}`);
		return 0;
	}

	hostapd.printf(`Restart interface for phy ${phy}`);
	return iface_restart(phy, config, old_config);
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

function iface_load_config(filename)
{
	let f = open(filename, "r");
	if (!f)
		return null;

	let config = {
		radio: {
			data: []
		},
		bss: [],
		orig_file: filename,
	};

	let bss;
	let line;
	while ((line = trim(f.read("line"))) != null) {
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

		push(config.radio.data, line);
	}

	while ((line = trim(f.read("line"))) != null) {
		let val = split(line, "=", 2);
		if (!val[0])
			continue;

		if (val[0] == "bssid")
			bss.bssid = val[1];

		if (val[0] == "bss") {
			bss = config_add_bss(config, val[1]);
			continue;
		}

		if (hostapd.data.file_fields[val[0]])
			bss.hash[val[0]] = hostapd.sha1(readfile(val[1]));

		push(bss.data, line);
	}
	f.close();

	return config;
}


let main_obj = {
	reload: {
		args: {
			phy: "",
		},
		call: function(req) {
			try {
				let phy_list = req.args.phy ? [ req.args.phy ] : keys(hostapd.data.config);
				for (let phy_name in phy_list) {
					let phy = hostapd.data.config[phy_name];
					let config = iface_load_config(phy.orig_file);
					iface_set_config(phy_name, config);
				}
			} catch(e) {
				hostapd.printf(`Error reloading config: ${e}\n${e.stacktrace[0].context}`);
				return libubus.STATUS_INVALID_ARGUMENT;
			}

			return 0;
		}
	},
	apsta_state: {
		args: {
			phy: "",
			up: true,
			frequency: 0,
			sec_chan_offset: 0,
			csa: true,
			csa_count: 0,
		},
		call: function(req) {
			if (req.args.up == null || !req.args.phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			let phy = req.args.phy;
			let config = hostapd.data.config[phy];
			if (!config || !config.bss || !config.bss[0] || !config.bss[0].ifname)
				return 0;

			let iface = hostapd.interfaces[config.bss[0].ifname];
			if (!iface)
				return 0;

			if (!req.args.up) {
				iface.stop();
				return 0;
			}

			if (!req.args.frequency)
				return libubus.STATUS_INVALID_ARGUMENT;

			let freq_info = iface_freq_info(iface, config, req.args);
			if (!freq_info)
				return libubus.STATUS_UNKNOWN_ERROR;

			let ret;
			if (req.args.csa) {
				freq_info.csa_count = req.args.csa_count ?? 10;
				ret = iface.switch_channel(freq_info);
			} else {
				iface.stop();
				ret = iface.start(freq_info);
			}
			if (!ret)
				return libubus.STATUS_UNKNOWN_ERROR;

			return 0;
		}
	},
	config_set: {
		args: {
			phy: "",
			config: "",
			prev_config: "",
		},
		call: function(req) {
			let phy = req.args.phy;
			let file = req.args.config;
			let prev_file = req.args.prev_config;

			if (!phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			try {
				if (prev_file && !hostapd.data.config[phy]) {
					let config = iface_load_config(prev_file);
					if (config)
						config.radio.data = [];
					hostapd.data.config[phy] = config;
				}

				let config = iface_load_config(file);

				hostapd.printf(`Set new config for phy ${phy}: ${file}`);
				iface_set_config(phy, config);
			} catch(e) {
				hostapd.printf(`Error loading config: ${e}\n${e.stacktrace[0].context}`);
				return libubus.STATUS_INVALID_ARGUMENT;
			}

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
};

hostapd.data.ubus = ubus;
hostapd.data.obj = ubus.publish("hostapd", main_obj);

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
			iface_set_config(phy, null);
		hostapd.ubus.disconnect();
	},
	bss_add: function(name, obj) {
		bss_event("add", name);
	},
	bss_reload: function(name, obj, reconf) {
		bss_event("reload", name, { reconf: reconf != 0 });
	},
	bss_remove: function(name, obj) {
		bss_event("remove", name);
	}
};
