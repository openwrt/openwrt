let libubus = require("ubus");
import { open, readfile } from "fs";
import { wdev_create, wdev_remove, is_equal, vlist_new } from "common";

let ubus = libubus.connect();

wpas.data.config = {};

function iface_stop(iface)
{
	let ifname = iface.config.iface;

	wpas.remove_iface(ifname);
	wdev_remove(ifname);
	iface.running = false;
}

function iface_start(phy, iface)
{
	if (iface.running)
		return;

	let ifname = iface.config.iface;

	wdev_remove(ifname);
	let ret = wdev_create(phy, ifname, iface.config);
	if (ret)
		wpas.printf(`Failed to create device ${ifname}: ${ret}`);
	wpas.add_iface(iface.config);
	iface.running = true;
}

function iface_cb(new_if, old_if)
{
	if (old_if && new_if && is_equal(old_if.config, new_if.config)) {
		new_if.running = old_if.running;
		return;
	}

	if (old_if && old_if.running)
		iface_stop(old_if);
}

function prepare_config(config)
{
	config.config_data = readfile(config.config);

	return { config: config };
}

function set_config(phy_name, config_list)
{
	let phy = wpas.data.config[phy_name];

	if (!phy) {
		phy = vlist_new(iface_cb, false);
		wpas.data.config[phy_name] = phy;
	}

	let values = [];
	for (let config in config_list)
		push(values, [ config.iface, prepare_config(config) ]);

	phy.update(values);
}

function start_pending(phy_name)
{
	let phy = wpas.data.config[phy_name];

	for (let ifname in phy.data)
		iface_start(phy_name, phy.data[ifname]);
}

let main_obj = {
	config_set: {
		args: {
			phy: "",
			config: [],
			defer: true,
		},
		call: function(req) {
			if (!req.args.phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			try {
				if (req.args.config)
					set_config(req.args.phy, req.args.config);

				if (!req.args.defer)
					start_pending(req.args.phy);
			} catch (e) {
				wpas.printf(`Error loading config: ${e}\n${e.stacktrace[0].context}`);
				return libubus.STATUS_INVALID_ARGUMENT;
			}

			return {
				pid: wpas.getpid()
			};
		}
	},
	config_add: {
		args: {
			driver: "",
			iface: "",
			bridge: "",
			hostapd_ctrl: "",
			ctrl: "",
			config: "",
		},
		call: function(req) {
			if (!req.args.iface || !req.args.config)
				return libubus.STATUS_INVALID_ARGUMENT;

			if (wpas.add_iface(req.args) < 0)
				return libubus.STATUS_INVALID_ARGUMENT;

			return {
				pid: wpas.getpid()
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

			wpas.remove_iface(req.args.iface);
			return 0;
		}
	},
};

wpas.data.ubus = ubus;
wpas.data.obj = ubus.publish("wpa_supplicant", main_obj);

function iface_event(type, name, data) {
	let ubus = wpas.data.ubus;

	data ??= {};
	data.name = name;
	wpas.data.obj.notify(`iface.${type}`, data, null, null, null, -1);
	ubus.call("service", "event", { type: `wpa_supplicant.${name}.${type}`, data: {} });
}

return {
	shutdown: function() {
		for (let phy in wpas.data.config)
			set_config(phy, []);
		wpas.ubus.disconnect();
	},
	iface_add: function(name, obj) {
		iface_event("add", name);
	},
	iface_remove: function(name, obj) {
		iface_event("remove", name);
	}
};
