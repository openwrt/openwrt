'use strict';

import * as ubus from "ubus";
import { realpath, lsdir } from "fs";
import {
	handler_load, handler_attributes,
	parse_attribute_list, parse_bool, parse_array,
	TYPE_ARRAY, TYPE_STRING, TYPE_INT, TYPE_BOOL
} from "./utils.uc";
import { find_phy } from "wifi.utils";
import * as wdev from "./wireless-device.uc";
import * as macaddr from "./wireless-macaddr.uc";

let wireless = netifd.wireless = {
	handlers: {},
	devices: {},
	mlo: {},
	path: realpath(netifd.main_path + "/wireless"),
};

function wpad_update_mlo(service, mode)
{
	let config = {};

	for (let ifname, data in wireless.mlo) {
		if (data.mode != mode)
			continue;

		data.phy = find_phy(data.radio_config[0], true);
		if (!data.phy)
			continue;

		let entry = { ...data };
		delete entry.radio_config;
		config[ifname] = entry;
	}

	ubus.call({
		object: service,
		method: "mld_set",
		return: "ignore",
		data: { config },
	});
}

function hostapd_update_mlo()
{
	wpad_update_mlo("hostapd", "ap");
}

function supplicant_update_mlo()
{
	wpad_update_mlo("wpa_supplicant", "sta");
}

// the security mode of a link depends on the bands of the other links
function mlo_bands(radio_config)
{
	let enabled = filter(radio_config, (v) => v && !v.disabled);
	return uniq(filter(map(enabled, (v) => v.band), (v) => v != null));
}

function mlo_vif_create(config, radio_config, vif_idx, mlo_vifs)
{
	let mlo_config = { ...config };

	if (config.wds)
		mlo_config['4addr'] = config.wds;
	mlo_config.radio_config = radio_config;

	let ifname = config.ifname;
	if (!ifname) {
		let idx = vif_idx[config.mode] ?? 0;
		vif_idx[config.mode] = idx + 1;
		ifname = config.mode + "-mld" + idx;
	}

	mlo_vifs[ifname] = mlo_config;
	return ifname;
}

function mlo_vif_macaddr(config, dev_names, dev_name)
{
	if (dev_name != dev_names[0])
		delete config.macaddr;
	if (!config.radio_macaddr)
		return;

	let idx = index(dev_names, dev_name);
	let macaddr = idx >= 0 ? config.radio_macaddr[idx] : null;
	if (macaddr)
		config.macaddr = macaddr;
}

// hostapd and wpa_supplicant compare lower-case addresses
function config_macaddr_lc(config)
{
	if (type(config) != "object")
		return;

	for (let field in [ "macaddr", "macaddr_base" ])
		if (type(config[field]) == "string")
			config[field] = lc(config[field]);

	if (type(config.radio_macaddr) == "array")
		config.radio_macaddr = map(config.radio_macaddr,
			(addr) => type(addr) == "string" ? lc(addr) : addr);
}

function config_macaddr_list(list, config)
{
	push(list, config.macaddr);
	if (type(config.radio_macaddr) == "array")
		push(list, ...config.radio_macaddr);
}

function macaddr_reserved_update(new_devices, mlo_vifs)
{
	let list = [];

	for (let name, dev in new_devices)
		for (let vif in dev.vif)
			config_macaddr_list(list, vif.config);

	for (let name, mlo_vif in mlo_vifs)
		config_macaddr_list(list, mlo_vif);

	macaddr.reserved_set(list);
}

function update_config(new_devices, mlo_vifs)
{
	for (let name, dev in new_devices) {
		config_macaddr_lc(dev.config);
		for (let vif in dev.vif)
			config_macaddr_lc(vif.config);
	}
	for (let name, mlo_vif in mlo_vifs)
		config_macaddr_lc(mlo_vif);

	macaddr_reserved_update(new_devices, mlo_vifs);

	wireless.mlo = mlo_vifs;
	hostapd_update_mlo();
	supplicant_update_mlo();

	for (let name, dev in wireless.devices) {
		if (new_devices[name])
			continue;

		dev.destroy();
	}

	for (let name, dev in new_devices) {
		let cur_dev = wireless.devices[name];
		if (cur_dev) {
			cur_dev.update(dev);
			continue;
		}

		let handler = wireless.handlers[dev.config.type];
		cur_dev = wdev.new(dev, handler.script);
		if (!cur_dev)
			continue;

		wireless.devices[name] = cur_dev;
	}
}

function config_init(uci)
{
	let config = uci.get_all("wireless");

	let handlers = {};
	let devices = {};
	let vifs = {};
	let mlo_vifs = {};

	let sections = {
		device: {},
		iface: {},
		vlan: {},
		station: {},
	};
	let radio_idx = {};
	let vif_idx = {};

	for (let name, data in config) {
		let type = data[".type"];
		if (parse_bool(data.disabled) && type != "wifi-device")
			continue;

		if (substr(type, 0, 5) != "wifi-")
			continue;

		let list = sections[substr(type, 5)];
		if (list)
			list[name] = data;
	}

	for (let name, data in sections.device) {
		if (!data.type)
			continue;

		let handler = wireless.handlers[data.type];
		if (!handler)
			continue;

		if (data.radio != null)
			radio_idx[name] = +data.radio;

		let config = parse_attribute_list(data, handler.device);
		devices[name] = {
			name,
			config,

			vif: [],
		};
		handlers[name] = handler;
	}

	for (let name, data in sections.iface) {
		let dev_names = parse_array(data.device);
		let mlo_vif = parse_bool(data.mlo);
		let radios = map(dev_names, (v) => radio_idx[v]);
		radios = filter(radios, (v) => v != null);
		let radio_config = map(dev_names, (v) => devices[v]?.config);
		let ifname;
		let mlo_created = false;

		for (let dev_name in dev_names) {
			let dev = devices[dev_name];
			if (!dev || dev.config.disabled)
				continue;

			let handler = handlers[dev_name];
			if (!handler)
				continue;

			let config = parse_attribute_list(data, handler.iface);
			config.radios = radios;
			if (mlo_vif)
				config.mlo_bands = mlo_bands(radio_config);

			if (mlo_vif && !mlo_created) {
				ifname = mlo_vif_create(config, radio_config, vif_idx, mlo_vifs);
				mlo_created = true;
			}

			if (ifname)
				config.ifname = ifname;
			mlo_vif_macaddr(config, dev_names, dev_name);

			let vif = {
				name, config,
				device: dev_name,

				vlan: [],
				sta: [],
			};
			push(dev.vif, vif);

			vifs[name] ??= [];
			push(vifs[name], vif);
		}
	}

	for (let name, data in sections.vlan) {
		let ifaces = parse_array(data.iface);
		for (let iface, iface_vifs in vifs) {
			if (length(ifaces) && index(ifaces, iface) < 0)
				continue;

			for (let vif in iface_vifs) {
				let dev = devices[vif.device];
				let handler = handlers[vif.device];
				if (!dev || !handler)
					continue;

				let config = parse_attribute_list(data, handler.vlan);

				let vlan = {
					name,
					config
				};
				push(vif.vlan, vlan);
			}
		}
	}

	for (let name, data in sections.station) {
		let ifaces = parse_array(data.iface);
		for (let iface, iface_vifs in vifs) {
			if (length(ifaces) && index(ifaces, iface) < 0)
				continue;

			for (let vif in iface_vifs) {
				let dev = devices[vif.device];
				let handler = handlers[vif.device];
				if (!dev || !handler)
					continue;

				let config = parse_attribute_list(data, handler.station);

				let sta = {
					name,
					config
				};
				push(vif.sta, sta);
			}
		}
	}

	let udata = ubus.call({
		object: "service",
		method: "get_data",
		data: {
			type: "wifi-device"
		},
	});
	for (let svcname, svc in udata) {
		for (let insname, ins in svc) {
			for (let typename, data in ins) {
				for (let radio, config in data) {
					if (type(config) != "object")
						continue;

					let dev = devices[radio];
					if (dev) {
						dev.config = { ...dev.config, ...config };
						continue;
					}

					let handler = wireless.handlers[config.type];
					if (!handler)
						continue;

					dev = devices[radio] = {
						name: radio,
						config,

						vif: [],
					};
					handlers[radio] = handler;
				}
			}
		}
	}


	udata = ubus.call({
		object: "service",
		method: "get_data",
		data: {
			type: "wifi-iface"
		},
	});

	for (let svcname, svc in udata) {
		for (let insname, ins in svc) {
			for (let typename, data in ins) {
				for (let radio, vifs in data) {
					if (type(vifs) != "object")
						continue;

					for (let name, vif in vifs) {
						let devs = vif.device;
						if (type(devs) != "array")
							devs = [ devs ];
						let config = vif.config;
						if (!config)
							continue;

						let mlo_vif = parse_bool(config.mlo);
						let radios = map(devs, (v) => radio_idx[v]);
						radios = filter(radios, (v) => v != null);
						let radio_config = map(devs, (v) => devices[v]?.config);
						radio_config = filter(radio_config, (v) => v != null);
						let ifname;

						if (mlo_vif) {
							ifname = mlo_vif_create(config, radio_config, vif_idx, mlo_vifs);
							mlo_vifs[ifname].radios = radios;
						}

						for (let device in devs) {
							let dev = devices[device];
							if (!dev)
								continue;

							let vif_config = ifname ? { ...config, ifname, radios, mlo_bands: mlo_bands(radio_config) } : config;
							if (ifname)
								mlo_vif_macaddr(vif_config, devs, device);

							let vif_data = {
								name, device, config: vif_config,
								vlan: [],
								sta: []
							};
							if (vif.vlans)
								vif_data.vlan = vif.vlans;
							if (vif.stations)
								vif_data.sta = vif.stations;
							push(dev.vif, vif_data);
						}
					}
				}
			}
		}
	}

	update_config(devices, mlo_vifs);
}

function config_start()
{
	for (let name, dev in wireless.devices)
		if (dev.autostart)
			dev.start();

}

function check_interfaces()
{
	for (let name, dev in wireless.devices)
		if (dev.autostart)
			dev.check();
}

function hotplug(ifname, add)
{
	for (let name, dev in wireless.devices)
		if (dev.autostart)
			dev.hotplug(ifname, add);
}

const network_config_attr = {
	network: TYPE_ARRAY,
	network_vlan: TYPE_ARRAY,
	bridge_isolate: TYPE_BOOL,
	isolate: TYPE_BOOL,
	proxy_arp: TYPE_BOOL,
	multicast_to_unicast: TYPE_BOOL,
};

const default_config_attr = {
	device: {
		disabled: TYPE_BOOL,
		type: TYPE_STRING,
	},
	iface: {
		...network_config_attr,
		device: TYPE_STRING,
		mode: TYPE_STRING,
		radio_macaddr: TYPE_ARRAY,
	},
	station: {
		iface: TYPE_STRING,

		mac: TYPE_STRING,
		key: TYPE_STRING,
		vid: TYPE_STRING,
	},
	vlan: {
		...network_config_attr,
		iface: TYPE_STRING,
		name: TYPE_STRING,
		vid: TYPE_STRING,
	},
};

const wdev_args = {
	device: ""
};

function wdev_call(req, cb)
{
	let dev = req.args.device;
	if (dev) {
		dev = wireless.devices[dev];
		if (!dev)
			return ubus.STATUS_NOT_FOUND;

		return cb(dev);
	}

	for (let name, dev in wireless.devices)
		cb(dev);

	return 0;
}

function attr_validate(attr_type, validate)
{
	if (validate)
		return validate;
	switch (attr_type) {
	case TYPE_STRING:
		return "string";
	case TYPE_ARRAY:
		return "list(string)";
	case TYPE_INT:
		return "uinteger";
	case TYPE_BOOL:
		return "bool";
	}
}

function get_validate_info(ret, handler)
{
	for (let kind in default_config_attr) {
		let cur = ret[kind == "iface" ? "interface" : kind] = {};
		let validate = handler[kind + "_validate"];

		for (let attr, attr_type in handler[kind]) {
			let val = attr_validate(attr_type, validate[attr]);
			if (val != null)
				cur[attr] = val;
		}
	}

	return ret;
}

const ubus_obj = {
	up: {
		args: wdev_args,
		call: function(req) {
			hostapd_update_mlo();

			return wdev_call(req, (dev) => {
				dev.start();
				return 0;
			});
		}
	},
	down: {
		args: wdev_args,
		call: function(req) {
			return wdev_call(req, (dev) => {
				dev.stop();
				return 0;
			});
		}
	},
	retry: {
		args: wdev_args,
		call: function(req) {
			hostapd_update_mlo();

			return wdev_call(req, (dev) => {
				dev.retry_setup();
				return 0;
			});
		}
	},
	reconf: {
		args: wdev_args,
		call: function(req) {
			hostapd_update_mlo();

			return wdev_call(req, (dev) => {
				dev.update();
				return 0;
			});
		}
	},
	status: {
		args: wdev_args,
		call: function(req) {
			let ret = {};
			let err = wdev_call(req, (dev) => {
				ret[dev.data.name] = dev.status();
				return 0;
			});
			if (err != 0)
				return err;

			return ret;
		}
	},
	notify: {
		args: {
			...wdev_args,
			command: 0,
			interface: "",
			vlan: "",
			data: {},
		},
		call: function(req) {
			let dev = req.args.device;
			if (!dev)
				return ubus.STATUS_INVALID_ARGUMENT;

			dev = wireless.devices[dev];
			if (!dev)
				return ubus.STATUS_NOT_FOUND;

			return dev.notify(req);
		}
	},
	get_validate: {
		args: wdev_args,
		call: function(req) {
			let ret = {};
			let err = wdev_call(req, (dev) => {
				let dev_type = dev.data.config.type;
				let cur = ret[dev.data.name] = {};
				get_validate_info(cur, wireless.handlers[dev_type]);
				return 0;
			});
			if (err != 0)
				return err;

			return ret;
		}
	},
	macaddr_get: {
		args: {
			phy: "",
			radio: 0,
			owner: "",
			name: "",
			group: "",
			ifname: "",
			macaddr: "",
			static: false,
			share: "",
			any_radio: false,
			num_global: 0,
			macaddr_base: "",
			mbssid: 0,
		},
		call: function(req) {
			return macaddr.macaddr_get(req.args) ?? ubus.STATUS_INVALID_ARGUMENT;
		}
	},
	macaddr_sync: {
		args: {
			owner: "",
			group: "",
			macaddr: {},
			num_global: 0,
			macaddr_base: "",
			mbssid: 0,
		},
		call: function(req) {
			return macaddr.macaddr_sync(req.args) ?? ubus.STATUS_INVALID_ARGUMENT;
		}
	},
	macaddr_release: {
		args: {
			owner: "",
			name: "",
			to: "",
			ifname: "",
			share: "",
			replace: false,
		},
		call: function(req) {
			return macaddr.macaddr_release(req.args) ?? ubus.STATUS_INVALID_ARGUMENT;
		}
	},
	macaddr_list: {
		args: {
			phy: "",
		},
		call: function(req) {
			return macaddr.macaddr_list(req.args);
		}
	},
};


handler_load(wireless.path, (script, data) => {
	if (!data.name)
		return;

	let handler = wireless.handlers[data.name] = {
		script,
	};
	for (let kind, attr in default_config_attr) {
		let validate = handler[kind + "_validate"] = {};
		handler[kind] = handler_attributes(data[kind], attr, validate);
	}
});

function wpad_reset()
{
	let conn = ubus.connect(null, 3);
	if (!conn) {
		netifd.log(netifd.L_WARNING, `wireless: no ubus connection for the reset: ${ubus.error()}\n`);
		return;
	}

	for (let obj in [ "hostapd", "wpa_supplicant" ]) {
		conn.call(obj, "config_reset", {});
		let err = conn.error(true);
		if (err && err != ubus.STATUS_NOT_FOUND)
			netifd.log(netifd.L_WARNING, `wireless: reset of ${obj} failed: ubus status ${err}\n`);
	}
	conn.disconnect();
}

// The allocation table does not survive a restart of netifd.
function wifi_reset()
{
	wpad_reset();

	for (let file in lsdir("/var/run", /^wdev-.*\.json$/)) {
		let phy = match(file, /^wdev-(.*)\.json$/)[1];
		if (!match(phy, /\.id$/))
			system([ "ucode", "/usr/share/hostap/wdev.uc", phy, "reset" ]);
	}
}

wifi_reset();

wireless.obj = ubus.publish("network.wireless", ubus_obj);
wireless.listener = ubus.listener("ubus.object.add", (event, msg) => {
	if (msg.path == "hostapd")
		hostapd_update_mlo();
	else if (msg.path == "wpa_supplicant")
		supplicant_update_mlo();
});
wireless.owner_listener = ubus.listener("ubus.object.remove", (event, msg) => {
	if (msg.path == "hostapd" || msg.path == "wpa_supplicant")
		macaddr.owner_gone(msg.path);
});

return {
	hotplug,
	config_init,
	config_start,
	check_interfaces,
};
