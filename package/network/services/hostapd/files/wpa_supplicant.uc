let libubus = require("ubus");
import { open, readfile } from "fs";
import { wdev_create, wdev_set_mesh_params, wdev_remove, is_equal, wdev_set_up, vlist_new, phy_open } from "common";

let ubus = libubus.connect();

function ex_handler(e)
{
	e = split(`${e}\n${e.stacktrace[0].context}`, '\n');
	for (let line in e)
		wpas.printf(line);
	return libubus.STATUS_UNKNOWN_ERROR;
}
libubus.guard(ex_handler);

wpas.data.config = {};
wpas.data.iface_phy = {};
wpas.data.macaddr_list = {};

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

function iface_start(phydev, iface, macaddr_list)
{
	let phy = phydev.name;

	if (iface.running)
		return;

	let ifname = iface.config.iface;
	let wdev_config = {};
	for (let field in iface.config)
		wdev_config[field] = iface.config[field];
	if (!wdev_config.macaddr)
		wdev_config.macaddr = phydev.macaddr_next();

	wpas.data.iface_phy[ifname] = phy;
	wdev_remove(ifname);
	let ret = phydev.wdev_add(ifname, wdev_config);
	if (ret)
		wpas.printf(`Failed to create device ${ifname}: ${ret}`);
	wdev_set_up(ifname, true);
	wpas.add_iface(iface.config);
	iface.running = true;
}

function iface_cb(new_if, old_if)
{
	if (old_if && new_if && is_equal(old_if.config, new_if.config)) {
		new_if.running = old_if.running;
		return;
	}

	if (new_if && old_if)
		wpas.printf(`Update configuration for interface ${old_if.config.iface}`);
	else if (old_if)
		wpas.printf(`Remove interface ${old_if.config.iface}`);

	if (old_if)
		iface_stop(old_if);
}

function prepare_config(config, radio)
{
	config.config_data = readfile(config.config);

	return { config };
}

function set_config(config_name, phy_name, radio, num_global_macaddr, macaddr_base, config_list)
{
	let phy = wpas.data.config[config_name];

	if (radio < 0)
		radio = null;

	if (!phy) {
		phy = vlist_new(iface_cb, false);
		phy.name = phy_name;
		wpas.data.config[config_name] = phy;
	}

	phy.radio = radio;
	phy.num_global_macaddr = num_global_macaddr;
	phy.macaddr_base = macaddr_base;

	let values = [];
	for (let config in config_list)
		push(values, [ config.iface, prepare_config(config) ]);

	phy.update(values);
}

function start_pending(phy_name)
{
	let phy = wpas.data.config[phy_name];
	let ubus = wpas.data.ubus;

	if (!phy || !phy.data)
		return;

	let phydev = phy_open(phy.name, phy.radio);
	if (!phydev) {
		wpas.printf(`Could not open phy ${phy_name}`);
		return;
	}

	let macaddr_list = wpas.data.macaddr_list[phy_name];
	phydev.macaddr_init(macaddr_list, {
		num_global: phy.num_global_macaddr,
		macaddr_base: phy.macaddr_base,
	});

	for (let ifname in phy.data)
		iface_start(phydev, phy.data[ifname]);
}

function phy_name(phy, radio)
{
	if (!phy)
		return null;

	if (radio != null && radio >= 0)
		phy += "." + radio;

	return phy;
}

let main_obj = {
	phy_set_state: {
		args: {
			phy: "",
			radio: 0,
			stop: true,
		},
		call: function(req) {
			let name = phy_name(req.args.phy, req.args.radio);
			if (!name || req.args.stop == null)
				return libubus.STATUS_INVALID_ARGUMENT;

			let phy = wpas.data.config[name];
			if (!phy)
				return libubus.STATUS_NOT_FOUND;

			if (req.args.stop) {
				for (let ifname in phy.data)
					iface_stop(phy.data[ifname]);
			} else {
				start_pending(name);
			}

			return 0;
		}
	},
	phy_set_macaddr_list: {
		args: {
			phy: "",
			radio: 0,
			macaddr: [],
		},
		call: function(req) {
			let phy = phy_name(req.args.phy, req.args.radio);
			if (!phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			wpas.data.macaddr_list[phy] = req.args.macaddr;
			return 0;
		}
	},
	phy_status: {
		args: {
			phy: "",
			radio: 0,
		},
		call: function(req) {
			let phy = phy_name(req.args.phy, req.args.radio);
			if (!phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			phy = wpas.data.config[phy];
			if (!phy)
				return libubus.STATUS_NOT_FOUND;

			for (let ifname in phy.data) {
				try {
					let iface = wpas.interfaces[ifname];
					if (!iface)
						continue;

					let status = iface.status();
					if (!status)
						continue;

					if (status.state == "INTERFACE_DISABLED")
						continue;

					status.ifname = ifname;
					return status;
				} catch (e) {
					continue;
				}
			}

			return libubus.STATUS_NOT_FOUND;
		}
	},
	config_set: {
		args: {
			phy: "",
			radio: 0,
			num_global_macaddr: 0,
			macaddr_base: "",
			config: [],
			defer: true,
		},
		call: function(req) {
			let phy = phy_name(req.args.phy, req.args.radio);
			if (!phy)
				return libubus.STATUS_INVALID_ARGUMENT;

			wpas.printf(`Set new config for phy ${phy}`);
			if (req.args.config)
				set_config(phy, req.args.phy, req.args.radio, req.args.num_global_macaddr, req.args.macaddr_base, req.args.config);

			if (!req.args.defer)
				start_pending(phy);

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
	bss_info: {
		args: {
			iface: "",
		},
		call: function(req) {
			let ifname = req.args.iface;
			if (!ifname)
				return libubus.STATUS_INVALID_ARGUMENT;

			let iface = wpas.interfaces[ifname];
			if (!iface)
				return libubus.STATUS_NOT_FOUND;

			let status = iface.ctrl("STATUS");
			if (!status)
				return libubus.STATUS_NOT_FOUND;

			let ret = {};
			status = split(status, "\n");
			for (let line in status) {
				line = split(line, "=", 2);
				ret[line[0]] = line[1];
			}

			return ret;
		}
	},
};

wpas.data.ubus = ubus;
wpas.data.obj = ubus.publish("wpa_supplicant", main_obj);
wpas.udebug_set("wpa_supplicant", wpas.data.ubus);

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

		if (state != "COMPLETED")
			return;

		let phy_data = wpas.data.config[phy];
		if (!phy_data)
			return;

		let iface_data = phy_data.data[ifname];
		if (!iface_data)
			return;

		let wdev_config = iface_data.config;
		if (!wdev_config || wdev_config.mode != "mesh")
			return;

		wdev_set_mesh_params(ifname, wdev_config);
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
