'use strict';

import * as libubus from "ubus";
import { realpath } from "fs";
import {
	ex_wrap, handler_load, handler_attributes,
	parse_attribute_list, parse_bool, parse_array,
	TYPE_ARRAY, TYPE_STRING, TYPE_INT, TYPE_BOOL
} from "./utils.uc";
import * as wdev from "./wireless-device.uc";

let ubus = netifd.ubus;
let wireless = netifd.wireless = {
	handlers: {},
	devices: {},
	path: realpath(netifd.main_path + "/wireless"),
};

function update_config(new_devices)
{
	for (let name, dev in wireless.devices)
		if (!new_devices[name])
			dev.destroy();

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

	let sections = {
		device: {},
		iface: {},
		vlan: {},
		station: {},
	};

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
		for (let dev_name in dev_names) {
			let dev = devices[dev_name];
			if (!dev)
				continue;

			let handler = handlers[dev_name];
			if (!handler)
				continue;

			let config = parse_attribute_list(data, handler.iface);

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
		if (!data.iface || !vifs[data.iface])
			continue;

		for (let vif in vifs[data.iface]) {
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

	for (let name, data in sections.station) {
		if (!data.iface || !vifs[data.iface])
			continue;

		for (let vif in vifs[data.iface]) {
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

	update_config(devices);
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

function hotplug(name, add)
{
	for (let name, dev in wireless.devices)
		if (dev.autostart)
			dev.hotplug(name, add);
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
			return libubus.STATUS_NOT_FOUND;

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
		call: ex_wrap(function(req) {
			return wdev_call(req, (dev) => {
				dev.start();
				return 0;
			});
		})
	},
	down: {
		args: wdev_args,
		call: ex_wrap(function(req) {
			return wdev_call(req, (dev) => {
				dev.stop();
				return 0;
			});
		})
	},
	reconf: {
		args: wdev_args,
		call: ex_wrap(function(req) {
			return wdev_call(req, (dev) => {
				dev.update();
				return 0;
			});
		})
	},
	status: {
		args: wdev_args,
		call: ex_wrap(function(req) {
			let ret = {};
			let err = wdev_call(req, (dev) => {
				ret[dev.data.name] = dev.status();
				return 0;
			});
			if (err != 0)
				return err;

			return ret;
		})
	},
	notify: {
		args: {
			...wdev_args,
			command: 0,
			interface: "",
			vlan: "",
			data: {},
		},
		call: ex_wrap(function(req) {
			let dev = req.args.device;
			if (!dev)
				return libubus.STATUS_INVALID_ARGUMENT;

			dev = wireless.devices[dev];
			if (!dev)
				return libubus.STATUS_NOT_FOUND;

			return dev.notify(req);
		})
	},
	get_validate: {
		args: wdev_args,
		call: ex_wrap(function(req) {
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
		})
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

wireless.obj = ubus.publish("network.wireless", ubus_obj);

return {
	hotplug,
	config_init,
	config_start,
	check_interfaces,
};
