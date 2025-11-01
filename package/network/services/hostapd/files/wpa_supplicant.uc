let libubus = require("ubus");
import * as uloop from "uloop";
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

wpas.data.mld = {};
wpas.data.config = {};
wpas.data.iface_phy = {};
wpas.data.iface_ubus = {};
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

function phy_dev_open(phy_name)
{
	let phy = wpas.data.config[phy_name];
	if (!phy) {
		warn(`Missing phy config for ${phy_name}\n`);
		return;
	}

	let phydev = phy_open(phy.name, phy.radio);
	if (!phydev)
		return;

	let macaddr_list = wpas.data.macaddr_list[phy_name];
	phydev.macaddr_init(macaddr_list, {
		num_global: phy.num_global_macaddr,
		macaddr_base: phy.macaddr_base,
	});

	return phydev;
}

function start_pending(phy_name)
{
	let phy = wpas.data.config[phy_name];
	if (!phy || !phy.data)
		return;

	let phydev = phy_dev_open(phy_name);
	if (!phydev) {
		wpas.printf(`Could not open phy ${phy_name}`);
		return;
	}

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

function mld_remove(data)
{
	if (!data.radio_mask_up)
		return;

	let name = data.name;
	wpas.printf(`Remove MLD interface ${name}`);
	wpas.remove_iface(name);
	wdev_remove(name);
	data.radio_mask_up = 0;
}

function mld_first_phy(data)
{
	let mask = data.radio_mask_present;

	for (let i = 0; mask; i++, mask >>= 1)
		if (mask & 1)
			return i;
}

function mld_radio_index(data, freq)
{
	let phys = data.phy_config;
	for (let i = 0; i < length(phys); i++)
		if (phys[i] && index(phys[i].freq_list, freq) >= 0)
			return i;
}

function mld_add(data, phy_list)
{
	let name = data.name;
	phy_list ??= [];

	wpas.printf(`Add MLD interface ${name}`);

	let radio = mld_first_phy(data);
	if (radio == null)
		return;

	let phy_name = data.phy + '.' + radio;
	let phydev = phy_list[phy_name];
	if (!phydev) {
		phydev = phy_dev_open(phy_name);
		if (!phydev)
			return;

		phy_list[phy_name] = phydev;
	}

	let wdev_config = { ...data.config, radio_mask: data.radio_mask };
	let ret = phydev.wdev_add(name, wdev_config);
	if (ret)
		wpas.printf(`Failed to create device ${name}: ${ret}`);

	let first_config = data.phy_config[radio];

	wdev_set_up(name, true);
	wpas.add_iface(first_config);

	let iface = wpas.interfaces[name];
	if (!iface) {
		wpas.printf(`Interface ${name} not found after adding\n`);
		wpas.remove_iface(name);
		wdev_remove(name);
		return;
	}

	if (length(data.freq_list) > 0)
		iface.config('freq_list', data.freq_list);

	data.radio_mask_up = data.radio_mask_present;
}

function mld_remove_links(data)
{
	// TODO
	mld_remove(data);
}

function mld_add_links(data)
{
	// TODO: incremental update
	mld_remove(data);
	mld_add(data);
}

function mld_set_config(config)
{
	let prev_mld = { ...wpas.data.mld };
	let new_mld = {};
	let phy_list = {};
	let new_config = !length(prev_mld);

	wpas.printf(`Set MLD config: ${keys(config)}`);

	for (let name, data in config) {
		let prev = prev_mld[name];
		if (prev && is_equal(prev.config, data)) {
			new_mld[name] = prev;
			delete prev_mld[name];
			continue;
		}

		let radio_mask = 0;
		for (let r in data.radios)
			if (r != null)
				radio_mask |= 1 << r;

		new_mld[name] = {
			name,
			config: data,
			phy: data.phy,
			phy_config: [],
			radio_mask,
			radio_mask_up: 0,
			radio_mask_present: 0,
		};
	}

	for (let name, data in prev_mld)
		mld_remove(data);

	wpas.data.mld = new_mld;

}

function mld_set_iface_config(name, data, radio, config)
{
	wpas.printf(`Set MLD interface ${name} radio ${radio} config: ${keys(config)}`);

	data.phy_config[radio] = config;
	if (config)
		data.radio_mask_present |= 1 << radio;
	else
		data.radio_mask_present &= ~(1 << radio);

	let freq_list;
	for (let config in data.phy_config) {
		if (!config || !config.freq_list)
			continue;
		if (!freq_list)
			freq_list = [ ...config.freq_list ];
		else
			push(freq_list, ...config.freq_list);
	}

	data.freq_list = freq_list;
}

function mld_update_iface(name, data) {
	if (!data.radio_mask_up)
		return;

	if (!data.radio_mask_present) {
		mld_remove(data);
		return;
	}

	let mask = data.radio_mask_up & ~data.radio_mask_present;
	if (!mask)
		return;

	mld_remove_links(data);
}

function mld_update_phy(phy, ifaces) {
	for (let name, data in wpas.data.mld) {
		if (data.phy != phy.name)
			continue;

		mld_set_iface_config(name, data, phy.radio, ifaces[name]);
		mld_update_iface(name, data);
	}
}

function mld_start() {
	if (wpas.data.mld_pending)
		return;

	wpas.printf(`Start pending MLD interfaces\n`);

	let phy_list = {};
	for (let name, data in wpas.data.mld) {
		wpas.printf(`MLD interface ${name} present=${data.radio_mask_present} up=${data.radio_mask_up}`);
		let add_mask = data.radio_mask_present & ~data.radio_mask_up;
		if (!add_mask)
			continue;

		if (!data.radio_mask_up)
			mld_add(data, phy_list);
		else
			mld_add_links(data);
	}
}

function mld_bss_allowed(data, bss) {
	if (!data.freq_list)
		return true;

	return index(data.freq_list, bss.freq) >= 0;
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
	let mlo_ifaces = {};
	for (let config in config_list)
		if (config.mlo)
			mlo_ifaces[config.iface] = config;
		else
			push(values, [ config.iface, prepare_config(config) ]);

	mld_update_phy(phy, mlo_ifaces);
	phy.update(values);
}

function iface_status_fill_radio_link(mld, radio, msg, link)
{
	let config = mld.phy_config[radio];
	if (!config)
		return;

	let freq_list = config.freq_list;
	if (!freq_list)
		return;

	if (!link || index(freq_list, link.frequency) < 0)
		return;

	msg.frequency = link.frequency;
	msg.sec_chan_offset = link.sec_chan_offset;
}

function iface_status_fill_radio(mld, radio, msg, status)
{
	if (status.links)
		for (let link in status.links)
			iface_status_fill_radio_link(mld, radio, msg, link);
	else
		iface_status_fill_radio_link(mld, radio, msg, status);
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

			let radio_mask = phy.radio != null ? (1 << phy.radio) : 0;
			if (req.args.stop) {
				for (let ifname in phy.data)
					iface_stop(phy.data[ifname]);
				for (let name, data in wpas.data.mld) {
					data.radio_mask_present &= ~radio_mask;
					if (data.radio_mask_up & radio_mask)
						mld_update_iface(name, data);
				}
			} else {
				start_pending(name);

				let found;
				for (let name, data in wpas.data.mld) {
					if (!(data.radio_mask & radio_mask))
						continue;
					data.radio_mask_present |= radio_mask;
					found = true;
				}
				if (found)
					mld_start();
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

			let ifnames = keys(phy.data);
			let radio_mask = phy.radio != null ? (1 << phy.radio) : 0;
			for (let name, data in wpas.data.mld)
				if (data.radio_mask_up & radio_mask)
					push(ifnames, name);

			for (let ifname in ifnames) {
				try {
					let iface = wpas.interfaces[ifname];
					if (!iface)
						continue;

					let status = iface.status();
					if (!status)
						continue;

					if (status.state == "INTERFACE_DISABLED")
						continue;

					if (phy.data[ifname]) {
						status.ifname = ifname;
						return status;
					}

					let ret = {
						ifname,
						state: status.state,
					};

					let mld = wpas.data.mld[ifname];
					iface_status_fill_radio(mld, phy.radio, ret, status);
					return ret;
				} catch (e) {
					ex_handler(e);
					continue;
				}
			}

			return libubus.STATUS_NOT_FOUND;
		}
	},
	iface_status: {
		args: {
			name: ""
		},
		call: function(req) {
			let iface = wpas.interfaces[req.args.name];
			if (!iface)
				return libubus.STATUS_NOT_FOUND;

			return iface.status();
		},
	},
	mld_set: {
		args: {
			config: {}
		},
		call: function(req) {
			if (!req.args.config)
				return libubus.STATUS_INVALID_ARGUMENT;

			wpas.data.mld_pending = true;
			mld_set_config(req.args.config);
			return 0;
		}
	},
	mld_start: {
		args: {},
		call: function(req) {
			wpas.data.mld_pending = false;
			mld_start();
			return 0;
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
				uloop.timer(100, () => start_pending(phy));

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
	let req = wpas.data.obj.notify(`iface.${type}`, data, null, null, null, -1);
	if (req)
		req.abort();

	ubus.call("service", "event", { type: `wpa_supplicant.${name}.${type}`, data: {} });
}

function iface_hostapd_notify(ifname, iface, state)
{
	let status = iface.status();
	let ubus = wpas.data.ubus;
	let msg = {};

	let mld = wpas.data.mld[ifname];
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
		if (!mld) {
			msg.frequency = status.frequency;
			msg.sec_chan_offset = status.sec_chan_offset;
		}
		break;
	default:
		return;
	}

	if (!mld) {
		msg.phy = wpas.data.iface_phy[ifname];
		if (!msg.phy) {
			wpas.printf(`no PHY for ifname ${ifname}`);
			return;
		}
		ubus.call("hostapd", "apsta_state", msg);
		return;
	}

	let radio_mask = mld.radio_mask;
	for (let i = 0; radio_mask; i++, radio_mask >>= 1) {
		if (!(radio_mask & 1)) {
			wpas.printf(`skip radio ${i}`);
			continue;
		}

		let radio_msg = {
			...msg,
			phy: mld.phy,
			radio: i,
		};

		if (state == "COMPLETED")
			iface_status_fill_radio(mld, i, radio_msg, status);

		ubus.call("hostapd", "apsta_state", radio_msg);
	}
}

function iface_channel_switch(ifname, iface, info)
{
	let msg = {
		up: true,
		csa: true,
		csa_count: info.csa_count ? info.csa_count - 1 : 0,
		frequency: info.frequency,
		sec_chan_offset: info.sec_chan_offset,
	};

	let mld = wpas.data.mld[ifname];
	if (mld) {
		msg.phy = mld.phy;
		msg.radio = mld_radio_index(mld, info.frequency);
		if (msg.radio == null) {
			wpas.printf(`PHY ${mld.phy} radio for frequency ${info.frequency} not found`);
			return;
		}
	} else {
		msg.phy = wpas.data.iface_phy[ifname];
		if (!msg.phy) {
			wpas.printf(`no PHY for ifname ${ifname}`);
			return;
		}
	}

	ubus.call("hostapd", "apsta_state", msg);
}

function iface_ubus_remove(ifname)
{
	let obj = wpas.data.iface_ubus[ifname];
	if (!obj)
		return;

	obj.remove();
	delete wpas.data.iface_ubus[ifname];
}

function iface_ubus_notify(ifname, event)
{
	let obj = wpas.data.iface_ubus[ifname];
	if (!obj)
		return;

	obj.notify('ctrl-event', { event }, null, null, null, -1);
}

function iface_ubus_add(ifname)
{
	let ubus = wpas.data.ubus;

	iface_ubus_remove(ifname);

	let obj = ubus.publish(`wpa_supplicant.${ifname}`, {
		reload: {
			args: {},
			call: (req) => {
				let iface = wpas.interfaces[ifname];
				if (!iface)
					return libubus.STATUS_NOT_FOUND;

				iface.ctrl("RECONFIGURE");
				return 0;
			},
		},
		wps_start: {
			args: {
				multi_ap: true
			},
			call: (req) => {
				let iface = wpas.interfaces[ifname];
				if (!iface)
					return libubus.STATUS_NOT_FOUND;

				iface.ctrl(`WPS_PBC multi_ap=${+req.args.multi_ap}`);
				return 0;
			},
		},
		wps_cancel: {
			args: {},
			call: (req) => {
				let iface = wpas.interfaces[ifname];
				if (!iface)
					return libubus.STATUS_NOT_FOUND;

				iface.ctrl("WPS_CANCEL");
				return 0;
			},
		},
		control: {
			args: {
				command: ""
			},
			call: (req) => {
				let iface = wpas.interfaces[ifname];
				if (!iface)
					return libubus.STATUS_NOT_FOUND;

				return {
					result: iface.ctrl(req.args.command)
				};
			},
		},
	});
	wpas.data.iface_ubus[ifname] = obj;
}

return {
	shutdown: function() {
		for (let phy in wpas.data.config)
			set_config(phy, []);
		wpas.ubus.disconnect();
	},
	bss_allowed: function(ifname, bss) {
		let mld = wpas.data.mld[ifname];
		if (!mld)
			return true;

		return mld_bss_allowed(mld, bss);
	},
	iface_add: function(name, obj) {
		iface_ubus_add(name);
		iface_event("add", name);
	},
	iface_remove: function(name, obj) {
		iface_event("remove", name);
		iface_ubus_remove(name);
	},
	ctrl_event: function(name, iface, ev) {
		iface_ubus_notify(name, ev);
	},
	state: function(ifname, iface, state) {
		let event_data = iface.status();
		event_data.name = ifname;
		iface_event("state", ifname, event_data);
		try {
			iface_hostapd_notify(ifname, iface, state);

			if (state != "COMPLETED")
				return;

			let phy = wpas.data.iface_phy[ifname];
			if (!phy)
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
		} catch (e) {
			ex_handler(e);
		}
	},
	event: function(ifname, iface, ev, info) {
		if (ev == "CH_SWITCH_STARTED")
			iface_channel_switch(ifname, iface, info);
	},
	wps_credentials: function(ifname, iface, cred) {
		cred.ifname = ifname;
		ubus.event("wps_credentials", cred);
	}
};
