let libubus = require("ubus");
import { open, readfile } from "fs";
import { wdev_create, wdev_remove, is_equal, vlist_new } from "common";

let ubus = libubus.connect();

wpas.data.config = {};
wpas.data.iface_phy = {};

function iface_stop(iface)
{
	let ifname = iface.config.iface;

	if (!iface.running)
		return;

	delete wpas.data.iface_phy[ifname];
	wpas.remove_iface(ifname);
	wdev_remove(ifname);
	iface.running = false;
}

function iface_start(phy, iface)
{
	if (iface.running)
		return;

	let ifname = iface.config.iface;

	wpas.data.iface_phy[ifname] = phy;
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

	if (old_if)
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
	phy_set_state: {
		args: {
			phy: "",
			stop: true,
		},
		call: function(req) {
			if (!req.args.phy || req.args.stop == null)
				return libubus.STATUS_INVALID_ARGUMENT;

			let phy = wpas.data.config[req.args.phy];
			if (!phy)
				return libubus.STATUS_NOT_FOUND;

			try {
				if (req.args.stop) {
					for (let ifname in phy.data)
						iface_stop(phy.data[ifname]);
				} else {
					start_pending(req.args.phy);
				}
			} catch (e) {
				wpas.printf(`Error chaging state: ${e}\n${e.stacktrace[0].context}`);
				return libubus.STATUS_INVALID_ARGUMENT;
			}
			return 0;
		}
	},
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

function iface_hostapd_notify(phy, ifname, iface, state)
{
	let ubus = wpas.data.ubus;
	let status = iface.status();
	let msg = { phy: phy };

	switch (state) {
	case "DISCONNECTED":
	case "AUTHENTICATING":
	case "SCANNING":
		msg.up = false;
		break;
	case "INTERFACE_DISABLED":
	case "INACTIVE":
		msg.up = true;
		break;
	case "COMPLETED":
		msg.up = true;
		msg.frequency = status.frequency;
		msg.sec_chan_offset = status.sec_chan_offset;
		break;
	default:
		return;
	}

	ubus.call("hostapd", "apsta_state", msg);
}

function iface_channel_switch(phy, ifname, iface, info)
{
	let msg = {
		phy: phy,
		up: true,
		csa: true,
		csa_count: info.csa_count ? info.csa_count - 1 : 0,
		frequency: info.frequency,
		sec_chan_offset: info.sec_chan_offset,
	};
	ubus.call("hostapd", "apsta_state", msg);
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
	},
	state: function(ifname, iface, state) {
		let phy = wpas.data.iface_phy[ifname];
		if (!phy) {
			wpas.printf(`no PHY for ifname ${ifname}`);
			return;
		}

		iface_hostapd_notify(phy, ifname, iface, state);
	},
	event: function(ifname, iface, ev, info) {
		let phy = wpas.data.iface_phy[ifname];
		if (!phy) {
			wpas.printf(`no PHY for ifname ${ifname}`);
			return;
		}

		if (ev == "CH_SWITCH_STARTED")
			iface_channel_switch(phy, ifname, iface, info);
	}
};
